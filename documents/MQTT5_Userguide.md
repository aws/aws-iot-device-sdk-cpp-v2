# Table of Contents
* [Introduction](#introduction)
* [What's Different? (relative to the MQTT311 implementation)](#whats-different-relative-to-the-mqtt311-implementation)
    + [Major changes](#major-changes)
    + [Minor changes](#minor-changes)
    + [Not Supported](#not-supported)
* [Getting Started with MQTT5](#getting-started-with-mqtt5)
    + [How to Create Mqtt5 Client](#how-to-create-mqtt5-client)
    + [Client lifecycle management](#client-lifecycle-management)
    + [How to Process Message](#how-to-process-message)
    + [How to Start and Stop](#how-to-start-and-stop)
        - [Start](#start)
        - [Stop](#stop)
    + [Connecting To AWS IoT Core](#connecting-to-aws-iot-core)
        - [Direct MQTT with X509-based mutual TLS](#direct-mqtt-with-x509-based-mutual-tls)
        - [MQTT over Websockets with Sigv4 authentication](#mqtt-over-websockets-with-sigv4-authentication)
        - [Direct MQTT with Custom Authentication](#direct-mqtt-with-custom-authentication)
        - [MQTT over Websockets with Cognito](#mqtt-over-websockets-with-cognito)
        - [Direct MQTT with Windows Certificate Store Method](#direct-mqtt-with-windows-certificate-store-method)
        - [Direct MQTT with PKCS11 Method](#direct-mqtt-with-pkcs11-method)
        - [Direct MQTT with pkcs12 method](#direct-mqtt-with-pkcs12-method)
    + [Adding an HTTP Proxy](#adding-an-http-proxy)
    + [Client Operations](#client-operations)
        - [Subscribe](#subscribe)
        - [Unsubscribe](#unsubscribe)
        - [Publish](#publish)
* [MQTT5 Best Practices](#mqtt5-best-practices)

# Introduction

This user guide is designed to act as a reference and guide for how to use MQTT5 with the CPP SDK, covering what MQTT5 is and some essential knowledge required to effectively use of MQTT5 within the CPP SDK.

This guide also includes code snippets for how to make a MQTT5 client with proper configuration, how to connect to AWS IoT Core, how to perform operations and interact with AWS IoT Core through MQTT5, and some best practices for MQTT5.

The goal of this guide is to provide a basic understanding of MQTT5 and how to leverage the CPP SDK to use MQTT5 for your IoT application.

If you are completely new to MQTT, it is highly recommended to read through the [concepts](./MQTT_CONCEPT.md) documentation to learn more about what MQTT is and what the terms used throughout this document mean.


# What's Different? (relative to the MQTT311 implementation)

SDK MQTT5 support comes from a separate client implementation.  In doing so, we took the opportunity to incorporate feedback about the 311 client that we could not apply without making breaking changes.  If you're used to the 311 client's API contract, there are a number of differences.


## Major changes

* The MQTT5 client does not treat initial connection failures differently.  With the 311 implementation, a failure during initial connect would halt reconnects completely.

* The set of client lifecycle events is expanded and contains more detailed information whenever possible.  All protocol data is exposed to the user.

* MQTT operations are completed with the full associated ACK packet when possible.

* New behavioral configuration options:

    * IoT Core specific validation - will validate and fail operations that break IoT Core specific restrictions

    * IoT Core specific flow control - will apply flow control to honor IoT Core specific per-connection limits and quotas

    * Flexible queue control - provides a number of options to control what happens to incomplete operations on a disconnection event

* A new API has been added to query the internal state of the client's operation queue.  This API allows the user to make more informed flow control decisions before submitting operations to the client.

* Data can no longer back up on the socket.  At most one frame of data is ever pending-write on the socket.

* The MQTT5 client has a single message-received callback.  Per-subscription callbacks are not supported.



## Minor changes

* Public API terminology has changed.  You *start* or *stop* the MQTT5 client.  This removes the semantic confusion with connect/disconnect as client-level controls vs. internal recurrent networking events.

* With the 311 implementation, there were two separate objects, a client and a connection.  With MQTT5, there is only the client.

* The user callbacks in the Mqtt5 do not provide a client reference in the way the Mqtt3 API does.
Example:
    ```
    // Client reference
    std::shared_ptr<Mqtt5Client> client = nullptr;

    // Create Mqtt5Client Builder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(...);

    // Setup lifecycle callbacks
    builder->WithClientConnectionSuccessCallback(
        [&client](const Mqtt5::OnConnectionSuccessEventData &eventData) {
            // Do mqtt5 client operations
            client->Publish(...);
        });

    // Build Mqtt5Client
    client = builder->Build();
    ```



## Not Supported

Not all parts of the MQTT5 spec are supported by the implementation.  We currently do not support:

* AUTH packets and the authentication fields in the CONNECT packet

* QoS 2



# Getting Started with MQTT5


## How to Create Mqtt5 Client
Once a MQTT5 client builder has been created, it is ready to make a [MQTT5 client](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_mqtt5_client.html). Something important to note is that once a MQTT5 client is built and finalized, the client configuration is immutable. Further modifications to the MQTT5 client builder will not change the settings of already created the MQTT5 clients. Before building a MQTT5 client from a MQTT5 client builder, make sure to have everything fully setup.

```cpp

    // Create Mqtt5Client Builder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(...);

    // Build Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5Client> client = builder->Build();

    if (mqtt5Client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }

    // Start mqtt5 connection session
    if (!mqtt5Client->Start())
    {
        fprintf("Failed start Mqtt5 client");
        return -1;
    }

```


## Client lifecycle management
The MQTT5 client emits a set of events related to state and network status changes. The lifecycle events callback should be set in Mqtt5ClientBuilder before the client builds.

```cpp

    // Create Mqtt5Client Builder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(...);


    /* setup lifecycle event callbacks */
    std::promise<bool> connectionPromise;
    std::promise<void> stoppedPromise;

    // Setup lifecycle callbacks
    builder->WithClientConnectionSuccessCallback(
        [&connectionPromise](const Mqtt5::OnConnectionSuccessEventData &eventData) {
            fprintf(
                stdout,
                "Mqtt5 Client connection succeed, clientid: %s.\n",
                eventData.negotiatedSettings->getClientId().c_str());
            connectionPromise.set_value(true);
        });

    builder->WithClientConnectionFailureCallback(
        [&connectionPromise](const Mqtt5::OnConnectionFailureEventData &eventData) {
            fprintf(
                stdout, "Mqtt5 Client connection failed with error: %s.\n", aws_error_debug_str(eventData.errorCode));
            connectionPromise.set_value(false);
        });

    builder->WithClientStoppedCallback([&stoppedPromise](const Mqtt5::OnStoppedEventData &) {
        fprintf(stdout, "Mqtt5 Client stopped.\n");
        stoppedPromise.set_value();
    });

    builder->WithClientAttemptingConnectCallback([](Mqtt5::OnAttemptingConnectEventData &) {
        fprintf(stdout, "Mqtt5 Client attempting connection...\n");
    });

    builder->WithClientDisconnectionCallback(
        [](const Mqtt5::OnDisconnectionEventData &eventData) {
            fprintf(stdout, "Mqtt5 Client disconnection with reason: %s.\n", aws_error_debug_str(eventData.errorCode));
        });

    // Build Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5Client> client = builder->Build();

    if (mqtt5Client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }

    // Start mqtt5 connection session
    if (!mqtt5Client->Start())
    {
        fprintf("Failed start Mqtt5 client");
        return -1;
    }

    if(connectionPromise.get_future().get() == true)
    {
        // Client started successfully
    }
    else
    {
        // Client Start failed.
    }

    if (client->Stop())
    {
        // Waiting for client stopped callback
        stoppedPromise.get_future().get();
    }
    else
    {
        fprintf(stdout, "Failed to stop the mqtt connection session. Exiting..\n");
        return -1;
    }

```

**AttemptingConnect**

Emitted when the client begins to make a connection attempt.



**ConnectionSuccess**

Emitted when a connection attempt succeeds based on receipt of an affirmative CONNACK packet from the MQTT broker.  A ConnectionSuccess event includes the MQTT broker's CONNACK packet, as well as a structure -- the NegotiatedSettings -- which contains the final values for all variable MQTT session settings (based on protocol defaults, client wishes, and server response).



**ConnectionFailure**

Emitted when a connection attempt fails at any point between DNS resolution and CONNACK receipt.  In addition to an error code, additional data may be present in the event based on the context.  For example, if the remote endpoint sent a CONNACK with a failing reason code, the CONNACK packet will be included in the event data.



**Disconnect**

Emitted when the client's network connection is shut down, either by a local action, event, or a remote close or reset.  Only emitted after a ConnectionSuccess event: a network connection that is shut down during the connecting process manifests as a ConnectionFailure event.
A Disconnect event will always include an error code.  If the Disconnect event is due to the receipt of a server-sent DISCONNECT packet, the packet will be included with the event data.



**Stopped**

Emitted once the client has shutdown any associated network connection and entered an idle state where it will no longer attempt to reconnect.  Only emitted after an invocation of stop() on the client.  A stopped client may always be started again.


## How to Process Message
[`withPublishReceivedCallback`](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_iot_1_1_mqtt5_client_builder.html#a178bd62d671ea2f273841e2e097744e8) will get involved when a publish is received. The callback should be set before building the client. Please note, once a MQTT5 client is built and finalized, the client configuration is immutable.

```cpp
    // Create Mqtt5Client Builder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(...);

    builder->WithPublishReceivedCallback([](const Mqtt5::PublishReceivedEventData &eventData) {
        if (eventData.publishPacket == nullptr)
            return;
        fprintf(stdout, "Publish received on topic %s:", eventData.publishPacket->getTopic().c_str());
        fwrite(eventData.publishPacket->getPayload().ptr, 1, eventData.publishPacket->getPayload().len, stdout);
        fprintf(stdout, "\n");
    });

    std::shared_ptr<Aws::Crt::Mqtt5Client> client = builder->Build();
```


## How to Start and Stop

### **Start**

Invoking `start()` on the client will put it into an active state where it recurrently establishes a connection to the configured remote endpoint.  Reconnecting continues until you invoke stop().


```cpp

    // Create Mqtt5Client Builder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(...);

    // Build Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5Client> client = builder->Build();

    if (mqtt5Client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }

    // Start mqtt5 connection session
    if (!mqtt5Client->Start())
    {
        fprintf("Failed start Mqtt5 client");
        return -1;
    }

```

### **Stop**

The `Stop()` API supports a DISCONNECT packet as an optional parameter.  If supplied, the DISCONNECT packet will be sent to the server prior to closing the socket.  You may listen for the 'Stopped' lifecycle event on the client for the result. Invoking `stop()` breaks the current connection (if any) and moves the client into an idle state. When waiting for finished with an MQTT5 client,

```cpp
    if (!client->Stop())
    {
        fprintf(stdout, "Failed to stop the mqtt connection session. Exiting..\n");
    }

```

## Connecting To AWS IoT Core

We strongly recommend using the Mqtt5ClientBuilder class to configure MQTT5 clients when connecting to AWS IoT Core.  The builder simplifies configuration for all authentication methods supported by AWS IoT Core.

This section shows samples for all of the authentication possibilities.


### Direct MQTT with X509-based mutual TLS

For X509 based mutual TLS, you can create a client where the certificate and private key are configured by path:

```cpp
    // Create a Client using Mqtt5ClientBuilder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
        "<clientEndpoint>", "<certificateFilePath>", "<privateKeyFilePath>");

    /* You can setup other client options and lifecycle event callbacks before call builder->Build().
    ** Once the the client get built, you could no longer update the client options or connection options
    ** on the created client.
    */

    // Build Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5Client> client = builder->Build();

    if (client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }

    // start use the mqtt5 client

```



### MQTT over Websockets with Sigv4 authentication

Sigv4-based authentication requires a credentials provider capable of sourcing valid AWS credentials. Sourced credentials will sign the websocket upgrade request made by the client while connecting. The default credentials provider chain supported by the SDK is capable of resolving credentials in a variety of environments according to a chain of priorities:


```Environment -> Profile (local file system) -> STS Web Identity -> IMDS (ec2) or ECS```



If the default credentials provider chain and AWS region are specified, you do not need to specify any additional configuration. Alternatively, if you're connecting to a special region for which standard pattern matching does not work, or if you need a specific credentials provider, you can specify advanced websocket configuration options.


```cpp
    // Create websocket configuration
    Aws::Crt::Auth::CredentialsProviderChainDefaultConfig defaultConfig;
    std::shared_ptr<Aws::Crt::Auth::ICredentialsProvider> provider = Aws::Crt::Auth::CredentialsProvider::CreateCredentialsProviderChainDefault(defaultConfig);
    if (!provider)
    {
        fprintf(stderr, "Failure to create credentials provider!\n");
        exit(-1);
    }
    Aws::Iot::WebsocketConfig websocketConfig(<signing region>, provider);

    // Create a Client using Mqtt5ClientBuilder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithWebsocket(
        "<clientEndpoint>", websocketConfig);

    /* You can setup other client options and lifecycle event callbacks before call builder->Build().
    ** Once the the client get built, you could no longer update the client options or connection options
    ** on the created client.
    */

    // Build Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5Client> mqtt5Client = builder->Build();

    if (mqtt5Client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }


```


### Direct MQTT with Custom Authentication

AWS IoT Core Custom Authentication allows you to use a lambda to gate access to IoT Core resources.  For this authentication method,you must supply an additional configuration structure containing fields relevant to AWS IoT Core Custom Authentication.



If your custom authenticator does not use signing, you don't specify anything related to the token signature:

```cpp
    // Setup custom authorization config
    Mqtt5CustomAuthConfig customAuth;
    customAuth.WithAuthorizerName("<Name of your custom authorizer>");
    customAuth.WithUsername("<Value of the username field that should be passed to the authorizer's lambda>");
    customAuth.WithPassword(<Binary data value of the password field to be passed to the authorizer lambda>);

    // Create a Client using Mqtt5ClientBuilder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithCustomCustomAuthorizer(
        "<clientEndpoint>", customAuth);

    /* You can setup other client options and lifecycle event callbacks before call builder->Build().
    ** Once the the client get built, you could no longer update the client options or connection options
    ** on the created client.
    */

    // Build Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5Client> mqtt5Client = builder->Build();

    if (mqtt5Client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }

```


If your custom authorizer uses signing, you must specify the three signed token properties as well. It is your responsibility to URI-encode the Username, AuthorizerName, and TokenKeyName parameters.

```cpp
    // Setup custom authorization config
    Mqtt5CustomAuthConfig customAuth;
    customAuth.WithAuthrizaerName("<Name of your custom authorizer>");
    customAuth.WithUsername("<Value of the username field that should be passed to the authorizer's lambda>");
    customAuth.WithPassword(<Binary data value of the password field to be passed to the authorizer lambda>);
    customAuth.WithTokenSignature("<The signature of the custom authorizer>")

    // Create a Client using Mqtt5ClientBuilder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithCustomCustomAuthorizer(
        "<clientEndpoint>", customAuth);

    /* You can setup other client options and lifecycle event callbacks before call builder->Build().
    ** Once the the client get built, you could no longer update the client options or connection options
    ** on the created client.
    */

    // Build Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5Client> mqtt5Client = builder->Build();

    if (mqtt5Client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }

```

In both cases, the builder will construct a final CONNECT packet username field value for you based on the values configured.  Do not add the token-signing fields to the value of the username that you assign within the custom authentication config structure.  Similarly, do not add any custom authentication related values to the username in the CONNECT configuration optionally attached to the client configuration. The builder will do everything for you.


### MQTT over Websockets with Cognito

A MQTT5 websocket connection can be made using Cognito to authenticate rather than the AWS credentials located on the
device or via key and certificate. Instead, Cognito can authenticate the connection using a valid Cognito identity ID.
This requires a valid Cognito identity ID, which can be retrieved from a Cognito identity pool.
A Cognito identity pool can be created from the AWS console.

**Note** Please note, the difference between, authenticated vs. unauthenticated identities: Authenticated identities
belong to users who are authenticated by any supported identity provider. Unauthenticated identities typically belong to
guest users.
For more information, see [Cognitor Identity Pools](https://docs.aws.amazon.com/cognito/latest/developerguide/identity-pools.html).


To create a MQTT5 builder configured for this connection, see the following code:

```cpp
    // Create websocket configuration
    Aws::Crt::Auth::CredentialsProviderChainDefaultConfig defaultConfig;
    std::shared_ptr<Aws::Crt::Auth::ICredentialsProvider> provider = Aws::Crt::Auth::CredentialsProvider::CreateCredentialsProviderChainDefault(defaultConfig);

    Aws::Crt::Auth::CredentialsProviderCognitoConfig cognitoConfig;
    // See https://docs.aws.amazon.com/general/latest/gr/cognito_identity.html for Cognito endpoints
    cognitoConfig.Endpoint = "cognito-identity.<region>.amazonaws.com";
    cognitoConfig.Identity = "<Cognito Identity ID>";
    Aws::Crt::Io::TlsContextOptions tlsCtxOptions = Aws::Crt::Io::TlsContextOptions::InitDefaultClient();
    cognitoConfig.TlsCtx = Aws::Crt::Io::TlsContext(tlsCtxOptions, Aws::Crt::Io::TlsMode::CLIENT);
    std::shared_ptr<Aws::Crt::Auth::ICredentialsProvider> provider = Aws::Crt::Auth::CredentialsProvider::CreateCredentialsProviderCognito(cognitoConfig);

    if (!provider)
    {
        fprintf(stderr, "Failure to create credentials provider!\n");
        exit(-1);
    }
    Aws::Iot::WebsocketConfig websocketConfig(<signing region>, provider);

    // Create a Client using Mqtt5ClientBuilder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithWebsocket(
        "<clientEndpoint>", websocketConfig);

    /* You can setup other client options and lifecycle event callbacks before call builder->Build().
    ** Once the the client get built, you could no longer update the client options or connection options
    ** on the created client.
    */

    // Build Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5Client> mqtt5Client = builder->Build();

    if (mqtt5Client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }

```
**Note**: A Cognito identity ID is different from a Cognito identity pool ID and trying to connect with a Cognito identity pool ID will not work. If you are unable to connect, make sure you are passing a Cognito identity ID rather than a Cognito identity pool ID.

### Direct MQTT with Windows Certificate Store Method

A MQTT5 direct connection can be made with mutual TLS with the certificate and private key in the Windows certificate
store, rather than simply being files on disk. To create a MQTT5 builder configured for this connection, see the following code:


```cpp
    String windowsCertPath = "CurrentUser\\MY\\A11F8A9B5DF5B98BA3508FBCA575D09570E0D2C6";

    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithWindowsCertStorePath(
            "<clientEndpoint>", windowsCertPath);

    // Build Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> mqtt5Client = builder->Build();

    /* You can setup other client options and lifecycle event callbacks before call builder->Build().
    ** Once the the client get built, you could no longer update the client options or connection options
    ** on the created client.
    */

    if (mqtt5Client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }

```
Note: This is the primary way to use HSM/TPMs on Windows.
Note: Windows Certificate Store connection support is only available on Windows devices.

### Direct MQTT with PKCS11 Method

A MQTT5 direct connection can be made using a PKCS11 device rather than using a PEM encoded private key,
the private key for mutual TLS is stored on a PKCS#11 compatible smart card or Hardware Security Module (HSM).
 To create a MQTT5 builder configured for this connection, see the following code:
```cpp
    std::shared_ptr<Aws::Crt::Io::Pkcs11Lib> pkcs11Lib = Aws::Crt::Io::Pkcs11Lib::Create(
        "<pkcs11_lib_filename>", Aws::Crt::Io::Pkcs11Lib::InitializeFinalizeBehavior::Strict);
    if (!pkcs11Lib)
    {
        fprintf(stderr, "Pkcs11Lib failed: %s\n", Aws::Crt::ErrorDebugString(Aws::Crt::LastError()));
        ASSERT_TRUE(false);
    }
    Aws::Crt::Io::TlsContextPkcs11Options pkcs11Options(pkcs11Lib);
    pkcs11Options.SetCertificateFilePath("<pkcs11_cert>");
    pkcs11Options.SetUserPin("<pkcs11_userPin>");
    pkcs11Options.SetTokenLabel("<pkcs11_tokenLabel>");
    pkcs11Options.SetPrivateKeyObjectLabel("<pkcs11_privateKeyLabel>");

    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsPkcs11(
			"<endpoint>", pkcs11Options);

    builder->WithPort(8883);
    builder->WithCertificateAuthority("<pkcs11_ca>");

    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> mqtt5Client = builder->Build();

    if (mqtt5Client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }

```
Note: Currently, TLS integration with PKCS#11 is only available on Unix devices.

### Direct MQTT with PKCS12 Method
A MQTT5 direct connection can be made using a PKCS12 file rather than using a PEM encoded private key.
To create a MQTT5 builder configured for this connection, see the following code:
```cpp
    Aws::Iot::Pkcs12Options testPkcs12Options;
    testPkcs12Options.pkcs12_file = "<pkcs12_key>";
    testPkcs12Options.pkcs12_password = "<pkcs12_password>";

    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsPkcs12(
        "<endpoint>", testPkcs12Options);

    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> mqtt5Client = builder->Build();

    if (mqtt5Client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }

```
Note: Currently, TLS integration with PKCS#12 is only available on MacOS devices.

## Adding an HTTP Proxy

No matter what your connection transport or authentication method is, you may connect through an HTTP proxy by applying proxy configuration to the builder:

```cpp
    // Create a Client using Mqtt5ClientBuilder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithXXXXX( ... );

    Http::HttpClientConnectionProxyOptions proxyOptions;
    proxyOptions.HostName = "<proxyHost>";
    proxyOptions.Port = <proxyPort>;
    builder->WithHttpProxyOptions(proxyOptions);

    /* You can setup other client options and lifecycle event callbacks before call builder->Build().
    ** Once the the client get built, you could no longer update the client options or connection options
    ** on the created client.
    */

    // Build Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5Client> mqtt5Client = builder->Build();

    if (mqtt5Client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }

```

SDK Proxy support also includes support for basic authentication and TLS-to-proxy. SDK proxy support does not include any additional proxy authentication methods (kerberos, NTLM, etc...) nor does it include non-HTTP proxies (SOCKS5, for example).

## Client Operations

There are three basic MQTT operations you can perform with the MQTT5 client.


### Subscribe

The Subscribe operation takes a description of the SUBSCRIBE packet you wish to send and return false if the operation goes wrong. The operation takes in a subscribe completion callback which returns the corresponding SubAckPacket returned by the broker.


```cpp

    // Create multiple subscription data. With the Mqtt5 API, we can subscribe multiple topics at once.
    Subscription data1();
    data1.withNoLocal(false).withTopicFilter("test/topic/test1");
    Subscription data2();
    data2.withTopicFilter("test/topic/test2");
    Subscription data3();
    data3.withTopicFilter("test/topic/test3");

    Vector<Subscription> subscriptionList;
    subscriptionList.push_back(data1);
    subscriptionList.push_back(data2);
    subscriptionList.push_back(data3);

    // Creaet a SubscribePacket with the subscription list. You can also use packet->WithSubscription(subscription) to push_back a single subscription data.
    std::shared_ptr<Mqtt5::SubscribePacket> packet = std::make_shared<SubscribePacket>();
    packet->WithSubscriptions(subscriptionList);

    bool subSuccess = mqtt5Client->Subscribe(
        packet,
        [](std::shared_ptr<Mqtt5::Mqtt5Client>, int, std::shared_ptr<Mqtt5::SubAckPacket> suback){
            for (auto code : suback->getReasonCodes())
            {
                fprintf(stdout, "Get suback from server with code: %d \n", code );
            }
        });

    if (!subSuccess){
        fprintf(stdout, "Failed to perform subscribe operation on client." );
    }



```


### Unsubscribe

The Unsubscribe operation takes a description of the UNSUBSCRIBE packet you wish to send and return false if the operation goes wrong. The operation takes in a subscribe completion callback which returns the corresponding UnSubAckPacket returned by the broker.



```cpp

    String topic1 = "test/topic/test1";
    String topic2 = "test/topic/test2";
    Vector<String> topics;
    topics.push_back(topic1);
    topics.push_back(topic2);
    std::shared_ptr<UnsubscribePacket> unsub = std::make_shared<UnsubscribePacket>();
    unsub->WithTopicFilters(topics);
    bool unsubSuccess = mqtt5Client->Unsubscribe(
        packet,
        [](std::shared_ptr<Mqtt5::Mqtt5Client>, int, std::shared_ptr<Mqtt5::UnSubAckPacket> unsuback){
            for (auto code : unsuback->getReasonCodes())
            {
                fprintf(stdout, "Get unsuback from server with code: %d \n", code );
            }
        });

    if (!unsubSuccess){
        fprintf(stdout, "Failed to perform unsubscribe operation on client." );
    }


```


### Publish

The Publish operation takes a description of the PUBLISH packet you wish to send and return false if the operation goes wrong.
The publish completion callback will return a PublishResult, which is a polymorphic value, as soon as the packet has been written to the socket.
If the PUBLISH was a QoS 0 publish, then the completion callback returns `nullptr` for PublishResult.
If the PUBLISH was a QoS 1 publish, then the completion callback returns a PubAckPacket.


```cpp

    Crt::String testTopic = "my/own/topic";
    Crt::String message_string = "any payload";

    ByteCursor payload = ByteCursorFromString(message_string);

    // Create PublishPacket.
    std::shared_ptr<PublishPacket> publish = std::make_shared<PublishPacket>(testTopic, payload, QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE);

    // Setup publish completion callback. The callback will get triggered when the pulbish completes and publish result returned from the server
    OnPublishCompletionHandler callback = [](int, std::shared_ptr<PublishResult> result){
        if(!result->wasSuccessful())
        {
            fprintf(stdout, "Publish failed with error_code: %d", result->getErrorCode());
        }
        else
        {
            fprintf(stdout, "Publish Succeed.");
        }
    };

    if(!mqtt5Client->Publish(publish, std::move(callback)))
    {
        fprintf(stdout, "Publish Operation Failed.\n");
        return -1;
    }

```


# MQTT5 Best Practices

Below are some best practices for the MQTT5 client that are recommended to follow for the best development experience:

* When creating MQTT5 clients, make sure to use ClientIDs that are unique! If you connect two MQTT5 clients with the same ClientID, they will Disconnect each other! If you do not configure a ClientID, the MQTT5 server will automatically assign one.
* Use the minimum QoS you can get away with for the lowest latency and bandwidth costs. For example, if you are sending data consistently multiple times per second and do not have to have a guarantee the server got each and every publish, using QoS 0 may be ideal compared to QoS 1. Of course, this heavily depends on your use case but generally it is recommended to use the lowest QoS possible.
* If you are getting unexpected disconnects when trying to connect to AWS IoT Core, make sure to check your IoT Core Thing’s policy and permissions to make sure your device is has the permissions it needs to connect!
* For **Publish**, **Subscribe**, and **Unsubscribe**, you can check the reason codes in the CompletionCallbacks to see if the operation actually succeeded.
* You MUST NOT perform blocking operations on any callback, or you will cause a deadlock. For example: in the on_publish_received callback, do not send a publish, and then wait for the future to complete within the callback. The Client cannot do work until your callback returns, so the thread will be stuck.
* You can use `LastError()` and `ErrorDebugString(error_code)` to get the error code and error message.

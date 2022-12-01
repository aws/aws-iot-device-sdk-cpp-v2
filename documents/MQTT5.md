# MQTT 5

## Table of Contents
* [Developer Preview Disclaimer](#developer-preview-disclaimer)
* [Introduction](#introduction)
* [What's Different? (relative to the MQTT311 implementation)](#what-s-different---relative-to-the-mqtt311-implementation-)
    + [Major changes](#major-changes)
    + [Minor changes](#minor-changes)
    + [Not Supported](#not-supported)
* [[Getting Started with MQTT5]](#-getting-started-with-mqtt5-)
    + [How to Create Mqtt5 Client](#how-to-create-mqtt5-client)
    + [How to Start and Stop](#how-to-start-and-stop)
        - [**Start**](#--start--)
        - [**Stop**](#--stop--)
    + [Client lifecycle management](#client-lifecycle-management)
    + [Connecting To AWS IoT Core](#connecting-to-aws-iot-core)
        - [Direct MQTT with X509-based mutual TLS](#direct-mqtt-with-x509-based-mutual-tls)
        - [MQTT over Websockets with Sigv4 authentication](#mqtt-over-websockets-with-sigv4-authentication)
        - [Direct MQTT with Custom Authentication](#direct-mqtt-with-custom-authentication)
        - [HTTP Proxy](#http-proxy)
    + [Client Operations](#client-operations)
        - [Subscribe](#subscribe)
        - [Unsubscribe](#unsubscribe)
        - [Publish](#publish)
* [MQTT5 Best Practices](#mqtt5-best-practices)

## Developer Preview Disclaimer

MQTT5 support is currently in **developer preview**.  We encourage feedback at all times, but feedback during the preview window is especially valuable in shaping the final product.  During the preview period we may make backwards-incompatible changes to the public API, but in general, this is something we will try our best to avoid.


The MQTT5 client cannot yet be used with the AWS IoT MQTT services (Shadow, Jobs, Identity).  This is a shortcoming that we hope to address in the near future.

## Introduction

This user guide is designed to act as a reference and guide for how to use MQTT5 with the CPP SDK, covering what MQTT5 is and some essential knowledge required to effectively use of MQTT5 within the CPP SDK.

This guide also includes code snippets for how to make a MQTT5 client with proper configuration, how to connect to AWS IoT Core, how to perform operations and interact with AWS IoT Core through MQTT5, and some best practices for MQTT5.

The goal of this guide is to provide a basic understanding of MQTT5 and how to leverage the CPP SDK to use MQTT5 for your IoT application.

If you are completely new to MQTT, it is highly recommended to read through the [concepts](./MQTT_CONCEPT.md) documentation to learn more about what MQTT is and what the terms used throughout this document mean.


## What's Different? (relative to the MQTT311 implementation)

SDK MQTT5 support comes from a separate client implementation.  In doing so, we took the opportunity to incorporate feedback about the 311 client that we could not apply without making breaking changes.  If you're used to the 311 client's API contract, there are a number of differences.


### Major changes

* The MQTT5 client does not treat initial connection failures differently.  With the 311 implementation, a failure during initial connect would halt reconnects completely.

* The set of client lifecycle events is expanded and contains more detailed information whenever possible.  All protocol data is exposed to the user.

* MQTT operations are completed with the full associated ACK packet when possible.

* New behavioral configuration options:

    * IoT Core specific validation - will validate and fail operations that break IoT Core specific restrictions

    * IoT Core specific flow control - will apply flow control to honor IoT Core specific per-connection limits and quotas

    * Flexible queue control - provides a number of options to control what happens to incomplete operations on a disconnection event

* A new API has been added to query the internal state of the client's operation queue.  This API allows the user to make more informed flow control decisions before submitting operatons to the client.

* Data can no longer back up on the socket.  At most one frame of data is ever pending-write on the socket.

* The MQTT5 client has a single message-received callback.  Per-subscription callbacks are not supported.



### Minor changes

* Public API terminology has changed.  You *start* or *stop* the MQTT5 client.  This removes the semantic confusion with connect/disconnect as client-level controls vs. internal recurrent networking events.

* With the 311 implementation, there were two separate objects, a client and a connection.  With MQTT5, there is only the client.




### Not Supported

Not all parts of the MQTT5 spec are supported by the implementation.  We currently do not support:

* AUTH packets and the authentication fields in the CONNECT packet

* QoS 2



## [Getting Started with MQTT5]


### How to Create Mqtt5 Client
Once a MQTT5 client builder has been created, it is ready to make a [MQTT5 client](https://awslabs.github.io/aws-crt-java/software/amazon/awssdk/crt/mqtt5/Mqtt5Client.html). Something important to note is that once a MQTT5 client is built and finalized, the client configuration is immutable. Further modifications to the MQTT5 client builder will not change the settings of already created the MQTT5 clients. Before building a MQTT5 client from a MQTT5 client builder, make sure to have everything fully setup.

```

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

### How to Start and Stop

#### **Start**

Invoking start() on the client will put it into an active state where it recurrently establishes a connection to the configured remote endpoint.  Reconnecting continues until you invoke stop().


```

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

#### **Stop**

The Stop() API supports a DISCONNECT packet as an optional parameter.  If supplied, the DISCONNECT packet will be sent to the server prior to closing the socket.  You may listen for the 'Stopped' lifecycle event on the client for the result. Invoking stop() breaks the current connection (if any) and moves the client into an idle state. When waiting for finished with an MQTT5 client,

```
    if (!client->Stop())
    {
        fprintf(stdout, "Failed to stop the mqtt connection session. Exiting..\n");
    }

```

### Client lifecycle management
The MQTT5 client emits a set of events related to state and network status changes. The lifecycle events callback should be set in Mqtt5ClientBuilder before the client builds.

```

    // Create Mqtt5Client Builder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(...);


    /* setup lifecycle event callbacks */
    std::promise<bool> connectionPromise;
    std::promise<void> stoppedPromise;

    // Setup lifecycle callbacks
    builder->withClientConnectionSuccessCallback(
        [&connectionPromise](Mqtt5::Mqtt5Client &, const Mqtt5::OnConnectionSuccessEventData &eventData) {
            fprintf(
                stdout,
                "Mqtt5 Client connection succeed, clientid: %s.\n",
                eventData.negotiatedSettings->getClientId().c_str());
            connectionPromise.set_value(true);
        });

    builder->withClientConnectionFailureCallback(
        [&connectionPromise](Mqtt5::Mqtt5Client &, const Mqtt5::OnConnectionFailureEventData &eventData) {
            fprintf(
                stdout, "Mqtt5 Client connection failed with error: %s.\n", aws_error_debug_str(eventData.errorCode));
            connectionPromise.set_value(false);
        });

    builder->withClientStoppedCallback([&stoppedPromise](Mqtt5::Mqtt5Client &, const Mqtt5::OnStoppedEventData &) {
        fprintf(stdout, "Mqtt5 Client stopped.\n");
        stoppedPromise.set_value();
    });

    builder->withClientAttemptingConnectCallback([](Mqtt5::Mqtt5Client &, const Mqtt5::OnAttemptingConnectEventData &) {
        fprintf(stdout, "Mqtt5 Client attempting connection...\n");
    });

    builder->withClientDisconnectionCallback(
        [](Mqtt5::Mqtt5Client &, const Mqtt5::OnDisconnectionEventData &eventData) {
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
        return -1'
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





### Connecting To AWS IoT Core

We strongly recommend using the Mqtt5ClientBuilder class to configure MQTT5 clients when connecting to AWS IoT Core.  The builder simplifies configuration for all authentication methods supported by AWS IoT Core.

This section shows samples for all of the authentication possibilities.


#### Direct MQTT with X509-based mutual TLS

For X509 based mutual TLS, you can create a client where the certificate and private key are configured by path:

```
    // Create a Client using Mqtt5ClientBuilder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
        <clientEndpoint>, <certificateFilePath>, <privateKeyFilePath>);

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



#### MQTT over Websockets with Sigv4 authentication

Sigv4-based authentication requires a credentials provider capable of sourcing valid AWS credentials. Sourced credentials will sign the websocket upgrade request made by the client while connecting. The default credentials provider chain supported by the SDK is capable of resolving credentials in a variety of environments according to a chain of priorities:


```Environment -> Profile (local file system) -> STS Web Identity -> IMDS (ec2) or ECS```



If the default credentials provider chain and AWS region are specified, you do not need to specify any additional configuration, Alternatively, if you're connecting to a special region for which standard pattern matching does not work, or if you need a specific credentials provider, you can specify advanced websocket configuration options.


```
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
        <clientEndpoint>, websocketConfig);

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


#### Direct MQTT with Custom Authentication

AWS IoT Core Custom Authentication allows you to use a lambda to gate access to IoT Core resources.  For this authentication method,you must supply an additional configuration structure containing fields relevant to AWS IoT Core Custom Authentication.



If your custom authenticator does not use signing, you don't specify anything related to the token signature:

```
    // Setup custom authorization config
    Mqtt5CustomAuthConfig customAuth;
    customAuth.WithAuthrizaerName(<Name of your custom authorizer>);
    customAuth.WithUsername(<Value of the username field that should be passed to the authorizer's lambda>);
    customAuth.WithPassword(<Binary data value of the password field that should be passed to the authorizer's lambda>);

    // Create a Client using Mqtt5ClientBuilder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithCustomCustomAuthorizer(
        <clientEndpoint>, customAuth);

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


If your custom authorizer uses signing, you must specify the three signed token properties as well.  The token signature must be the URI-encoding of the base64 encoding of the digital signature of the token value via the private key associated with the public key that was registered with the custom authorizer.  It is your responsibility to URI-encode the token signature.

```
    // Setup custom authorization config
    Mqtt5CustomAuthConfig customAuth;
    customAuth.WithAuthrizaerName(<Name of your custom authorizer>);
    customAuth.WithUsername(<Value of the username field that should be passed to the authorizer's lambda>);
    customAuth.WithPassword(<Binary data value of the password field that should be passed to the authorizer's lambda>);
    customAuth.WithTokenSignature(<The signature of the custom authorizer>)

    // Create a Client using Mqtt5ClientBuilder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithCustomCustomAuthorizer(
        <clientEndpoint>, customAuth);

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



#### HTTP Proxy

No matter what your connection transport or authentication method is, you may connect through an HTTP proxy by applying proxy configuration to the builder:

```

    // Create a Client using Mqtt5ClientBuilder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithXXXXX( ... );

    Http::HttpClientConnectionProxyOptions proxyOptions;
    proxyOptions.HostName = <proxyHost>;
    proxyOptions.Port = <proxyPort>;
    builder->withHttpProxyOptions(proxyOptions);

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

### Client Operations

There are three basic MQTT operations you can perform with the MQTT5 client.


#### Subscribe

The Subscribe operation takes a description of the SUBSCRIBE packet you wish to send and return false if the operation goes wrong. The operation takes in a subscribe completion callback which returns ponding SubAckPacket returned by the broker.


```

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

    // Creaet a SubscribePacket with the subscription list. You can also use packet->withSubscription(subscription) to push_back a single subscription data.
    std::shared_ptr<Mqtt5::SubscribePacket> packet = std::make_shared<SubscribePacket>();
    packet->withSubscriptions(subscriptionList);

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


#### Unsubscribe

The Unsubscribe operation takes a description of the UNSUBSCRIBE packet you wish to send and return false if the operation goes wrong. The operation takes in a subscribe completion callback which returns ponding UnSubAckPacket returned by the broker.



```

    String topic1 = "test/topic/test1";
    String topic2 = "test/topic/test2";
    Vector<String> topics;
    topics.push_back(topic1);
    topics.push_back(topic2);
    std::shared_ptr<UnsubscribePacket> unsub = std::make_shared<UnsubscribePacket>();
    unsub->withTopicFilters(topics);
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


#### Publish

The Publish operation takes a description of the PUBLISH packet you wish to send and return false if the operation goes wrong.
The publish completion callback will return a PublishResult, which is a polymorphic value, as soon as the packet has been written to the socket.
If the PUBLISH was a QoS 0 publish, then the completion callback returns nullptr for PublishResult.
If the PUBLISH was a QoS 1 publish, then the completion callback returns a PubAckPacket.


```

    Crt::String testTopic = "my/own/topic";
    Crt::String message_string = "any payload";

    ByteCursor payload = ByteCursorFromString(message_string);

    // Create PublishPacket.
    std::shared_ptr<PublishPacket> publish = std::make_shared<PublishPacket>(testTopic, payload, QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE);

    // Setup publish completion callback. The callback will get triggered when the pulbish completes and publish result returned from the server
    OnPublishCompletionHandler callback = [](std::shared_ptr<Mqtt5Client> client, int, std::shared_ptr<PublishResult> result){
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


## MQTT5 Best Practices

Below are some best practices for the MQTT5 client that are recommended to follow for the best development experience:

* When creating MQTT5 clients, make sure to use ClientIDs that are unique! If you connect two MQTT5 clients with the same ClientID, they will Disconnect each other! If you do not configure a ClientID, the MQTT5 server will automatically assign one.
* Use the minimum QoS you can get away with for the lowest latency and bandwidth costs. For example, if you are sending data consistently multiple times per second and do not have to have a guarantee the server got each and every publish, using QoS 0 may be ideal compared to QoS 1. Of course, this heavily depends on your use case but generally it is recommended to use the lowest QoS possible.
* If you are getting unexpected disconnects when trying to connect to AWS IoT Core, make sure to check your IoT Core Thing’s policy and permissions to make sure your device is has the permissions it needs to connect!
* For **Publish**, **Subscribe**, and **Unsubscribe**, you can check the reason codes in the CompletionCallbacks to see if the operation actually succeeded.

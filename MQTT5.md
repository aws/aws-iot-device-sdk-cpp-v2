# MQTT 5


## Developer Preview Disclaimer

MQTT5 support is currently in **developer preview**.  We encourage feedback at all times, but feedback during the preview window is especially valuable in shaping the final product.  During the preview period we may make backwards-incompatible changes to the public API, but in general, this is something we will try our best to avoid.


The MQTT5 client cannot yet be used with the AWS IoT MQTT services (Shadow, Jobs, Identity).  This is a shortcoming that we hope to address in the near future.

## Introduction

This user guide is designed to act as a reference and guide for how to use MQTT5 with the CPP SDK, covering what MQTT5 is and some essential knowledge required to effectively use of MQTT5 within the CPP SDK.

This guide also includes code snippets for how to make a MQTT5 client with proper configuration, how to connect to AWS IoT Core, how to perform operations and interact with AWS IoT Core through MQTT5, and some best practices for MQTT5.

The goal of this guide is to provide a basic understanding of MQTT5 and how to leverage the CPP SDK to use MQTT5 for your IoT application.

If you are completely new to MQTT, it is highly recommended to read through the <concepts> section to learn more about what MQTT is and what the terms used throughout this document mean. If you are experienced with MQTT, feel free to skip ahead to the <Using MQTT5 in the SDK> section to jump right into the implementation specifics with code snippets showing how to use MQTT5 within the CPP SDK.


## Concepts

### What is MQTT

MQTT is a message protocol often used for IoT devices thanks to its smaller bandwidth cost, its defined spec, and relatively low complexity compared to something like HTTP. MQTT5 is the latest version of MQTT protocol with additional features, like user properties, and developer enhancements, like better error logging, that make developing a MQTT application easier and more powerful.

*Please note that this document does not include the whole MQTT protocol* and only covers the basic essential MQTT concepts to aid in the understanding and usage of the CPP SDK API. In the Further Reading section there are links to documentation that covers the MQTT protocol in more detail.

### MQTT Knowledge Essentials

MQTT is a message protocol, but what does that mean? Simply put, MQTT is a way for a device on one network to send information to a device on another networking using an agreed upon set of rules and protocols. This information is sent through what is called *Packets*, which is data formatted in a specific way so the receiver knows how to parse it and extract the data contained within. The data within a packet is often referred to as the *payload*.

The concepts section will cover the most important aspects of MQTT relative to its usage in the CPP SDK in the sections below. The first thing that needs to be covered are the two main types of devices on the internet with MQTT:

**Servers** and **Clients**:

* **Server** (interchangeably referred to as *Brokers*): where MQTT packets are sent and processed. A single server can process many packets and have many different MQTT clients (see below) at once. A server may both send and receive packets from one or more clients.
    * Servers can perform many different operations with packets depending on the data within, the Quality of Service level (also known as “QoS”), and the topic the packets are being sent to. The server will process incoming packets to determine whether to forward the packet to subscribers (clients who have subscribed to the topic in the packet) and/or responding to the client that sent the packet with an acknowledgement packet.
    * AWS IoT Core is a service that provides a MQTT5 server, which devices can connect to and send packets to and from. AWS IoT Core also has services that, when packets are sent to, will invoke certain operations and other services.
* **Client**: what sends and receives packets from the server. A client connects by sending a Connect Packet with information on who the client is, as well as often including authorization data. Upon connecting successfully, an MQTT client can subscribe and unsubscribe from topics, send and receive Publish Packets to topics and from topics it is subscribed to, as well as disconnect and reconnect.

In general, you will be working on writing code that makes a MQTT5 client and sends data to the server when certain operations happen, to report data from the device, to respond to data sent from the server or another client whose topic the client is subscribed to (more on subscriptions below), or otherwise react based on the packets being sent and received. Because of this, this document will be focusing the majority of the information on the client side of things, rather than the server.

(*Implementer note*: Link “subscriptions below” to the correct section in the Markdown when adding to GitHub.)


**Terminology and Relationship**

The connection process for MQTT is different than connection process for a *Client/Server* paradigm. The process to connect in MQTT requires both a *Connection* and a *Session*, in addition to a *Client*. This can be confusing as they have similar functionalities. Below are the rough definitions for each concept:

* **Client**: Manages the Connection and the Session. A client contains all the information needed to connect to a MQTT server, as well as the interface to use for performing operations.
    * Operations are described further below in the operations section.
    * (**Implementer note**: Link “operations section” to the section in the Markdown when adding to GitHub.)
* **Connection**: Directly handles packets and communicating with the MQTT server via sockets. The connection is where the MQTT operations are directly performed at. The connection also handles keeping the client connected by responding to PING requests from the server, as well as tracking operations and their timeouts.
    * The MQTT client creates a connection. Connections cannot exist without a client.
* **Session**: The period of time where a client is connected to a MQTT server and is able to perform operations. A session ends when the MQTT client is disconnected, shutdown, or otherwise destroyed.
    * Sessions exist only when there is a client and connection, and the connection is actively connected to a MQTT server.

In the CPP SDK, the terminology isn’t quite as important as some MQTT implementations because *in MQTT5 a client can only have a single Connection and Session.* However, it is important to know the terms and what each part means when MQTT5 is discussed. All of your code will interact solely with the MQTT5 client, so if you are unsure, just remember the MQTT5 client is what your code interacts with.

* Note that in MQTT 3.1.1, often referred to simply as “MQTT” in the CPP SDK code, there are some fundamental differences compared to MQTT5. A few of these differences are listed below:
    * In the MQTT 3.1.1 client, the MQTT connection is where your code interacts to perform MQTT operations. In MQTT5, the code instead performs all operations through the MQTT5 client.
    * In the MQTT 3.1.1 client, you can have multiple MQTT connections (and by extension, sessions) per MQTT 3.1.1 client. In MQTT5, each MQTT5 client has only a single connection and session. In MQTT5 you can use multiple clients each with their own connection.
    * In the MQTT 3.1.1 client, when you subscribe to a topic it adds it to a topic tree so when a publish packet comes in, per-topic callbacks will be invoked. In MQTT5, there is not a topic tree, and instead all subscriptions will invoke the same callback when a publish packet comes in.


Additionally, there is some further terms that are important to define in relation to making a connection with MQTT:

* **Endpoint**: The server address or URL of the MQTT server to connect to.
* **Port**: The network port to use for sending packets to and from the device to the server. In CPP SDK, this can often be left alone as the SDK will automatically set the port based on the connection type.

**What is a Topic**

In MQTT, one of the important functionalities of the MQTT server is to handle sending packets to the proper *topics*. A topic is a string that tells the server where to send a packet when it receives one, so it can properly process or forward the packet on to other clients who are listening for packets on that topic. Topics can be specific or can use wildcards via the asterisk character (*).

With wildcards, this means a client can make a subscription to a topic of “test/hello-*” for example, and then that client will receive all packets sent to “test” that start with “hello-”. For example, a packet sent to “test/hello-world” and a packet sent to “test/hello-/world” would both be received by a client subscribed to “test/hello-*”.

It should be noted that not every packet has a topic, like connection and disconnect packets do not include and a topic.

It should also be noted that topics are extremely important for both the MQTT client and server. The MQTT client needs to know of topics to know where to send data to when performing operations, while the MQTT server needs to process topics to make sure each MQTT client connected gets the proper data as soon as new data is sent to the MQTT server.


**What is Quality Of Service (QOS)**

Another important term in MQTT is *Quality Of Service*, or *QOS*. QOS dictates how reliable the MQTT packet is and what steps will be taken when it seems data may not be received by the MQTT server. This is important if a client is sending information in a packet that has important data and it is critical to know whether it has to be received by the server or not. There are three levels of QOS and each increasing level means more mechanisms to ensure the server has received the packet.

* **QOS 0**: This level is often referred to as “*at most once*” and means that the MQTT packet will be sent once and that is it, there will be no additional processing to determine if the packet was received.
* **QOS 1**: This level is often referred to as “*at least once*” and means that the MQTT packet will be sent and then the sender (client or server) will wait for an acknowledgement (ACK) to come back. If an ACK is not received within a certain time frame, then the packet will be resent.
    * Packets that have been sent but an ACK not yet received are referred to as “in-flight” packets.
    * QOS 1 does not guarantee that the packet will not be sent more than once, as it will continue to resend the packet at a set interval until an ACK is received.
* **QOS 2**: This level is often referred to as “*exactly once*” and means that the MQTT packet will be sent exactly once and no more. This is done through a series of ACKs on both the sender and receiver’s side to confirm that the packet was sent, arrived at the server, and was successfully processed.
    * **Note**: **QOS 2 is not supported in the CPP SDK at this time.**

The higher the QOS level, the more complicated and slower the complete operation will be due to the extra packets. Packets with higher QOS take longer to be fully processed and require more bandwidth than those with a lower QOS. In general, clients should send packets with the minimum QOS that required for the data being sent for faster speeds and less bandwidth.


**Allowed Operations**

MQTT clients and servers send packets through *operations*. A MQTT client can perform many different operations, each with a different functionality and *associated* *packet*:

* **Start**: Sends a packet requesting a connection to a server. A connect packet contains information that tells the server who the client is, what settings to use for sending MQTT packets, and what the options the client supports. This packet often also contains authentication information that verifies the client is authorized to connect. This packet performs a handshake with the server that allows the client to perform all other operations describe herein.
    * A connect packet can also include something called a **Will**. A will is a publish sent to a topic when the client disconnects and/or the server feels the client has become disconnected due to not responding to PING requests within the agreed upon time.
    * The client is the only one that can send a connect packet. However, the server will send an acknowledgement packet, often referred to as the ConnAck packet, with information on whether the connection was successful or not. If successful, the ConnAck packet will also include Negotiated-Settings, which detail the settings the server supports and the client can use to communication.
    * **Note**: In the MQTT 3.1.1 client, the Start operation is called **Connect**. Also note that Negotiated-Settings are only sent in MQTT5.
* **Publish**: Sends a packet containing whatever data the client wishes to send to a given topic. A publish can be made with different QOS levels, can be set to be retained (persist on the server).
    * Publish packets can have payloads containing string or binary data.
* **Subscribe**: Sends a packet telling the server that the client wants to get any publish packets sent to a specific topic. A subscribe can be made with different QOS levels and can be configured to get retained messages.
    * **Note**: Publish packets can be received that are sent from both the client and the server, as well as other clients who are also connected to the server.
* **Unsubscribe**: Sends a packet telling the server that the client no longer wants to get publish packet on a specific topic anymore.
* **Stop**: Sends a packet telling the server to disconnect the client from the server. The disconnect packet can contain a reason for the disconnect and some additional options, like whether to send a will.
    * **Note**: In the MQTT 3.1.1 client SDK, the Stop operation is called *Disconnect*.


Operations on QOS1 or higher will cause the server to send an acknowledgement packet, often referred to as an ACK packet, when the client performs an operation. For example, if a client performs a publish operation, the client will send a publish packet to the MQTT server and the server will send back a PubAck packet when the publish has been received. These ACK packets often contain information on whether the operation was successful, and if it was unsuccessful, information on why.

It should also be noted that in MQTT5 most operations and their packets can also include *user properties*. User properties are new to MQTT5 and are key-value pairs that can be set in the packet when a client performs an operation. User properties can be helpful for passing along additional information or context to other MQTT5 clients.


### Further Reading:

Hopefully that has helped give an overview of MQTT and what it is within the CPP SDK. If you are curious to learn more about MQTT, how it works, and what it can do, please see the following links for more information:

* MQTT 5 standard: https://docs.oasis-open.org/mqtt/mqtt/v5.0/os/mqtt-v5.0-os.html
* MQTT 3.1.1 standard: https://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.html




## Using MQTT5 in the SDK


### What's Different? (relative to the MQTT311 implementation)

SDK MQTT5 support comes from a separate client implementation.  In doing so, we took the opportunity to incorporate feedback about the 311 client that we could not apply without making breaking changes.  If you're used to the 311 client's API contract, there are a number of differences.


**Major changes**

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




**Minor changes**

* Public API terminology has changed.  You *start* or *stop* the MQTT5 client.  This removes the semantic confusion with connect/disconnect as client-level controls vs. internal recurrent networking events.

* With the 311 implementation, there were two separate objects, a client and a connection.  With MQTT5, there is only the client.




**Not Supported**

Not all parts of the MQTT5 spec are supported by the implementation.  We currently do not support:

* AUTH packets and the authentication fields in the CONNECT packet

* QoS 2




## Client lifecycle management

Once created, an MQTT5 client's configuration is immutable.  Invoking start() on the client will put it into an active state where it recurrently establishes a connection to the configured remote endpoint.  Reconnecting continues until you invoke stop().




```

    // Create Mqtt5Client Builder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(...);


    /* setup lifecycle event callbacks */
    std::promise<bool> connectionPromise;
    std::promise<void> stoppedPromise;

    builder->withClientConnectionSuccessCallback([&connectionPromise](
                                                Mqtt5::Mqtt5Client&,
                                                std::shared_ptr<Aws::Crt::Mqtt5::ConnAckPacket>,
                                                std::shared_ptr<Aws::Crt::Mqtt5::NegotiatedSettings> settings) {
        fprintf(stdout, "Mqtt5 Client connection succeed, clientid: %s.\n", settings->getClientId().c_str());
        connectionPromise.set_value(true);
    });

    builder->withClientConnectionFailureCallback(
        [&connectionPromise](Mqtt5::Mqtt5Client &, int error_code, std::shared_ptr<Aws::Crt::Mqtt5::ConnAckPacket>) {
            fprintf(stdout, "Mqtt5 Client connection failed with error: %s.\n", aws_error_debug_str(error_code));
            connectionPromise.set_value(false);
        });

    builder->withClientStoppedCallback(
        [&stoppedPromise](Mqtt5::Mqtt5Client &) {
        fprintf(stdout, "Mqtt5 Client stopped.\n");
        stoppedPromise.set_value();
    });

    builder->withPublishReceivedCallback(
    [](std::shared_ptr<Mqtt5::Mqtt5Client>, std::shared_ptr<Mqtt5::PublishPacket> publish) {
        fprintf(stdout, "Message received on Topic: %s.\n", publish->getTopic());
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

```




Invoking stop() breaks the current connection (if any) and moves the client into an idle state. When waiting for finished with an MQTT5 client,



```
    if (!client->Stop())
    {
        fprintf(stdout, "Failed to stop the mqtt connection session. Exiting..\n");
        return -1;
    }
    stoppedPromise.get_future().wait();

```


The MQTT5 client emits a set of events related to state and network status changes.

### AttemptingConnect

Emitted when the client begins to make a connection attempt.



### ConnectionSuccess

Emitted when a connection attempt succeeds based on receipt of an affirmative CONNACK packet from the MQTT broker.  A ConnectionSuccess event includes the MQTT broker's CONNACK packet, as well as a structure -- the NegotiatedSettings -- which contains the final values for all variable MQTT session settings (based on protocol defaults, client wishes, and server response).



### ConnectionFailure

Emitted when a connection attempt fails at any point between DNS resolution and CONNACK receipt.  In addition to an error code, additional data may be present in the event based on the context.  For example, if the remote endpoint sent a CONNACK with a failing reason code, the CONNACK packet will be included in the event data.



### Disconnect

Emitted when the client's network connection is shut down, either by a local action, event, or a remote close or reset.  Only emitted after a ConnectionSuccess event: a network connection that is shut down during the connecting process manifests as a ConnectionFailure event.
A Disconnect event will always include an error code.  If the Disconnect event is due to the receipt of a server-sent DISCONNECT packet, the packet will be included with the event data.



### Stopped

Emitted once the client has shutdown any associated network connection and entered an idle state where it will no longer attempt to reconnect.  Only emitted after an invocation of stop() on the client.  A stopped client may always be started again.





## Connecting To AWS IoT Core

We strongly recommend using the Mqtt5ClientBuilder class to configure MQTT5 clients when connecting to AWS IoT Core.  The builder simplifies configuration for all authentication methods supported by AWS IoT Core.

This section shows samples for all of the authentication possibilities.


### Direct MQTT with X509-based mutual TLS

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

## Client Operations

There are four basic MQTT operations you can perform with the MQTT5 client.



### Subscribe

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


### Unsubscribe

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


### Publish

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



##### Disconnect

The stop() API supports a DISCONNECT packet as an optional parameter.  If supplied, the DISCONNECT packet will be sent to the server prior to closing the socket.  You may listen for the 'stopped' event on the client for the result.


```
if (!mqtt5Client->Stop())
{
    fprintf(stdout, "Failed to disconnect from the mqtt connection. Exiting..\n");
    return -1;
}
// Waiting for closing the client

```

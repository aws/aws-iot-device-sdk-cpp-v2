# Migrate from V1 to V2 of the AWS IoT SDK for C++

The V2 AWS IoT SDK for C++ is a major rewrite of the V1 code base. It includes many updates, such as improved consistency, ease of use, more detailed information about client status, an offline operation queue control, etc. This guide describes the major features that are new in V2, and provides guidance on how to migrate your code to V2 from V1.

## What’s new in V2 SDK

* V2 SDK client is truly async. Operations take callback functions/lambdas, that is called when the operation is registered with the server
    Blocking calls can be emulated by setting an `std::promise<>` in the callback by calling `promise.set_value() `and then waiting for the returned `std::future<>` object to be resolved after the function call by calling `future.wait()`
* V2 SDK provides implementation for MQTT5 protocol, the next step in evolution of the MQTT protocol.
* Public API terminology has changed. You `S``tart()` or `S``top()` the MQTT5 client rather than `Connect` or `Disconnect` like in V1. This removes the semantic confusion with the connect/disconnect as the client-level controls vs. internal recurrent networking events.
* Support for Fleet Provisioning AWS IoT Core service.

Public API for almost all actions and operations has changed significantly. For more details about the new features and to see specific code examples, refer to the other sections of this guide.


## How To Get Started with V2 SDK

There are differences between IoT C++ V1 SDK and IoT C++ V2 SDK. Below are changes you need to make to use IoT C++ V2 SDK features. For more information about MQTT5, visit [MQTT5 User Guide](https://github.com/aws/aws-iot-device-sdk-cpp-v2/blob/main/documents/MQTT5_Userguide.md).


### MQTT Protocol

V1 SDK uses an MQTT version 3.1.1 client under the hood.

V2 SDK provides MQTT version 3.1.1 and MQTT version 5.0 client implementations. This guide focuses on the MQTT5 since this version is significant improvement over MQTT3. See MQTT5 features section.


### Client Builder

To access the AWS IoT service, you must initialize an MQTT client.

In V1 SDK, the [awsiotsdk::MqttClient](http://aws-iot-device-sdk-cpp-docs.s3-website-us-east-1.amazonaws.com/latest/classawsiotsdk_1_1_mqtt_client.html) class represents an MQTT client. You instantiate the client directly passing all the required parameters to the class constructor. It’s possible to change client settings after its creation using `set*` methods, e.g. `SetAutoReconnectEnabled` or `SetMaxReconnectBackoffTimeout`.

In V2 SDK, the [Aws::Iot::MqttClient](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_iot_1_1_mqtt_client.html) class represents an MQTT client, specifically MQTT5 protocol. V2 SDK provides an [Aws::Iot::Mqtt5ClientBuilder](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_iot_1_1_mqtt5_client_builder.html) designed to easily create common configuration types such as direct MQTT or WebSocket connections.
Once an MQTT5 client is built and finalized, the resulting MQTT5 client cannot have its settings modified.

_Example of creating a client in V1_

```
#include "OpenSSLConnection.hpp"

 util::String clientEndpoint = "<prefix>-ats.iot.<region>.amazonaws.com";
uint16_t clientPort = <port number>
 util::String clientId = "<unique client id>";
 util::String rootCaPath = "<root certificate path>";
 util::String certificateFile = "<certificate file>";  // X.509 based certificate file
 util::String privateKeyFile = "<private key file>";   // PEM encoded private key file

 std::shared_ptr<network::OpenSSLConnection> p_network_connection =
    std::make_shared<network::OpenSSLConnection>(
        clientEndpoint,
        clientPort,
        rootCaPath,
        certificateFile,
        privateKeyFile);
ResponseCode rc = p_network_connection->Initialize();
```

_Example of creating a client in V2_

V2 SDK supports different connection types. Given the same input parameters as in the V1 example above, the most suitable method to create an MQTT5 client will [beNewMqtt5ClientBuilderWithMtlsFromPath](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_iot_1_1_mqtt5_client_builder.html#ab595bbc50e08b9d2f78f62e9efeafd65).

```
util::String clientEndpoint = "<prefix>-ats.iot.<region>.amazonaws.com";
util::String certificateFile = "<certificate file>";  // X.509 based certificate file
util::String privateKeyFile = "<private key file>";   // PEM encoded private key file
uint32_t clientPort = <port number>

std::shared_ptr<Aws::Iot::Mqtt5ClientBuilder>builder(
    Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
        clientEndpoint,
        certificateFie,
        privateKeyFile));
std::shared_ptr<Mqtt5::ConnectPacket> connectOptions =
        std::make_shared<Mqtt5::ConnectPacket>();
util::String clientId = "<unique client id>";
connectOptions->WithClientId(clientId);
builder->WithConnectOptions(connectOptions);
builder->WithPort(clientPort);
std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client = builder->Build();
```

Refer to the [Connection Types and Features](https://quip-amazon.com/bbcuA13anxJa#temp:C:YWR0b9b69e08b72487dbe620342f) section for other connection types supported by V2 SDK.

### Connection Types and Features

V1 SDK supports two types of connections to connect to the AWS IoT service: MQTT with X.509 certificate and MQTT over Secure WebSocket with SigV4 authentication.

V2 SDK adds a collection of connection types and cryptography formats (e.g. [PKCS #11](https://en.wikipedia.org/wiki/PKCS_11) and [Custom Authorizer](https://docs.aws.amazon.com/iot/latest/developerguide/custom-authentication.html)), credential providers (e.g. [Amazon Cognito](https://aws.amazon.com/cognito/) and [Windows Certificate Store](https://learn.microsoft.com/en-us/windows-hardware/drivers/install/certificate-stores)), and other connection-related features.
Refer to the “[Connecting To AWS IoT Core](https://github.com/aws/aws-iot-device-sdk-cpp-v2/blob/main/documents/MQTT5_Userguide.md#connecting-to-aws-iot-core)” section of the MQTT5 user guide for detailed information and code snippets on each connection type and connection feature.

|Connection Type/Feature	|V1 SDK	|V2 SDK	|User guide section	|	|
|---	|---	|---	|---	|---	|
|	|	|	|	|	|
|MQTT over Secure WebSocket with AWS SigV4 authentication	|✔	|✔	|	|	|
|MQTT (over TLS 1.2) with X.509 certificate based mutual authentication	|✘	|✔	|	|X.509	|
|MQTT with PKCS12 Method	|✔*	|✔	|	|Container for X.509	|
|MQTT with Custom Key Operation Method	|✔*	|✔	|	|X.509	|
|MQTT with Custom Authorizer Method	|✔*	|✔	|	|	|
|MQTT with Windows Certificate Store Method	|✔*	|✔	|	|X.509	|
|MQTT with PKCS11 Method	|✘	|✔	|	|X.509 plus other formats	|
|Websocket Connection with Cognito Authentication Method	|✘	|✔	|	|	|
|HTTP Proxy	|✔**	|✔	|	|	|

✔*   - In order to get this connection type work in V1 SDK, you need to implement the [Custom Authentication workflow](https://docs.aws.amazon.com/iot/latest/developerguide/custom-authorizer.html).
✔**  - Though V1 does not allow to specify HTTP proxy, it is possible to configure systemwide proxy.

### Lifecycle Events

Both V1 and V2 SDKs provide lifecycle events for the MQTT clients.

V1 SDK provides 3 lifecycle events: “ClientCoreState::ApplicationResubscribeCallbackPt”, “ClientCoreState::ApplicationDisconnectCallbackPtr”, and “ClientCoreState::ApplicationReconnectCallbackPtr”. You can supply a custom callback function via the function `Create`. It is recommended to use lifecycle events callbacks to help determine the state of the MQTT client during operation.

V2 SDK add 3 new lifecycle events and removes one(Resubscribe Callback), providing 5 lifecycle events in total: “WithClientConnectionSuccessCallback”, “WithClientConnectionFailureCallback”, “WithClientDisconnectionCallback”, “WithClientStoppedCallback”, and “WithClientAttemptingConnectCallback”.
Refer to the [MQTT5 user guide](https://github.com/aws/aws-iot-device-sdk-cpp-v2/blob/main/documents/MQTT5_Userguide.md#client-lifecycle-management) for the details.

_Example of setting lifecycle events in V1_

```
std::shared_ptr<MqttClient> p_iot_client_;
ResponseCode DisconnectCallback(
        util::String client_id,
        std::shared_ptr<DisconnectCallbackContextData> p_app_handler_data) {
    ...
    return ResponseCode::SUCCESS;
}
ResponseCode JobsSample::ReconnectCallback(
        util::String client_id,
        std::shared_ptr<ReconnectCallbackContextData> p_app_handler_data,
        ResponseCode reconnect_result) {
     ...
    return ResponseCode::SUCCESS;
}
ResponseCode ResubscribeCallback(util::String client_id,
        std::shared_ptr<ResubscribeCallbackContextData> p_app_handler_data,
        ResponseCode resubscribe_result) {
    ...
    return ResponseCode::SUCCESS;
        }
ClientCoreState::ApplicationDisconnectCallbackPtr p_disconnect_handler =
        std::bind(&DisconnectCallback,
                  this,
                  std::placeholders::_1,
                  std::placeholders::_2);

ClientCoreState::ApplicationReconnectCallbackPtr p_reconnect_handler =
        std::bind(&ReconnectCallback,
                  this,
                  std::placeholders::_1,
                  std::placeholders::_2,
                  std::placeholders::_3);

ClientCoreState::ApplicationResubscribeCallbackPtr p_resubscribe_handler =
        std::bind(&ResubscribeCallback,
                  this,
                  std::placeholders::_1,
                  std::placeholders::_2,
                  std::placeholders::_3);

p_iot_client_ = std::shared_ptr<MqttClient>(
        MqttClient::Create(p_network_connection_,
                           ConfigCommon::mqtt_command_timeout_,
                           p_disconnect_handler, nullptr,
                           p_reconnect_handler, nullptr,
                           p_resubscribe_handler, nullptr));

rc = p_iot_client_->Connect(/*...*/);

```

_Example of setting lifecycle events in V2_

```
builder->WithClientConnectionSuccessCallback(
        [&](const Mqtt5::OnConnectionSuccessEventData &eventData) { });

builder->WithClientConnectionFailureCallback([&](
        const Mqtt5::OnConnectionFailureEventData &eventData) { });

builder->WithClientStoppedCallback([&](
        const Mqtt5::OnStoppedEventData & eventData) { });

builder->WithClientAttemptingConnectCallback([&](
        OnAttemptingConnectEventData &eventData)) { });

builder->WithClientDisconnectionCallback([&](
        OnDisconnectionEventData &eventData) { });

std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client = builder->Build();

```

### Publish

V1 SDK provides two API calls for publishing: blocking and non-blocking. For the non-blocking version, the result of the publish operation is reported via a set of callbacks. If you try to publish to a topic that is not allowed by a policy, AWS IoT Core service will close the connection.

V2 SDK provides only asynchronous non-blocking API. A  [PublishPacket](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_publish_packet.html) object containing a description of the PUBLISH packet. The [publish](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_mqtt5_client.html#a5f1214d3a574d91e1db7c97f8636de96) operation takes a `PublishPacket` instance  and a [Aws::Crt::Mqtt5::OnPublishCompletionHandler](https://aws.github.io/aws-iot-device-sdk-cpp-v2/namespace_aws_1_1_crt_1_1_mqtt5.html#a6c8e5bc5d3a6eb7f4767f3c1ecd8524c) that contains a returned [`PublishResult`](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_publish_result.html) in its parameter that will contain different data depending on the `QoS` used in the publish.

* For QoS 0 (AT_MOST_ONCE): Calling `getValue` will return `null` and the promise will be complete as soon as the packet has been written to the socket.
* For QoS 1 (AT_LEAST_ONCE): Calling `getValue` will return a [PubAckPacket](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_pub_ack_packet.html) and the promise will be complete as soon as the PUBACK is received from the broker.

If the operation fails for any reason before these respective completion events, the promise is rejected with a descriptive error. You should always check the reason code of a [PubAckPacket](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_pub_ack_packet.html) completion to determine if a QoS 1 publish operation actually succeeded.

_Example of publishing in V1_

```
// Blocking API.
std::chrono::milliseconds action_response_timeout = 20;
bool is_retained = false;
bool is_duplicate = false;

ResponseCode rc = p_iot_client_->Publish(Utf8String::Create("my/topic"),
                                         is_retained,
                                         is_duplicate,
                                         awsiotsdk::mqtt::QoS::QOS1,
                                         "hello",
                                         action_response_timeout);
```

```
// Non-blocking API.
std::chrono::milliseconds action_response_timeout = 20;
bool is_retained = false;
bool is_duplicate = false;
uint16_t packet_id = 0;
void PublishActionTester::AsyncAckHandler(uint16_t action_id, ResponseCode rc) {
    /* callback function */
}
ActionData::AsyncAckNotificationHandlerPtr p_async_ack_handler =
        std::bind(&PublishActionTester::AsyncAckHandler,
                  this,
                  std::placeholders::_1,
                  std::placeholders::_2);
uint16_t &packet_id_out;

rc = p_iot_client_->PublishAsync(Utf8String::Create("my/topic"),
                                 is_retained,
                                 is_duplicate,
                                 mqtt::QoS::QOS0,
                                 "hello",
                                 p_async_ack_handler,
                                 packet_id);
```

_Example of publishing in V2_

```
std::shared_ptr<Mqtt5::PublishPacket> publish =
        std::make_shared<Mqtt5::PublishPacket>(
                "my topic",
                "hello",
                Mqtt5::QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE);

auto onPublishComplete =
        [](int, std::shared_ptr<Aws::Crt::Mqtt5::PublishResult> result)
        { /* publish callback */ };

bool rc = client->Publish(publish, onPublishComplete);

```

### Subscribe

V1 provides blocking and non-blocking API for subscribing. To subscribe to topic in V1, you should provide an instance of [awsiotsdk::mqtt::Subscription](http://aws-iot-device-sdk-cpp-docs.s3-website-us-east-1.amazonaws.com/latest/classawsiotsdk_1_1mqtt_1_1_subscription.html) to the [subscribe](http://aws-iot-device-sdk-cpp-docs.s3-website-us-east-1.amazonaws.com/latest/classawsiotsdk_1_1_mqtt_client.html#a9702f18a7d663aae5b76bae70d3999c7) operation. awsiotsdk::mqtt::Subscription object (or, usually, an object of a children class) implements `ApplicationCallbackHandlerPtr` method which will be called on receiving a new message. If you try to subscribe to a topic that is not allowed by a policy, AWS IoT Core service will close the connection.

V2 SDK provides only asynchronous non-blocking API. First, you need to create a [SubscribePacket](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_subscribe_packet.html) object. If you specify multiple topics in the WithSubscription member function, V2 SDK will subscribe to all of these topics using one request. The [Subscribe](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_mqtt5_client.html#aa6c3bfc3cbd99b17957148ac1e8c34c4) operation takes a description of the `SubscribePacket` you wish to send and sends back a callback that resolves with success or failure with the corresponding [SubAckPacket](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_sub_ack_packet.html) returned by the broker; You should always check the reason codes of a `SubAckPacket` completion to determine if the subscribe operation actually succeeded.

In V2 SDK, if the MQTT5 client is going to subscribe and receive packets from the MQTT broker, it is important to also setup the `builder.WithPublishReceivedCallback` callback. This callback is invoked whenever the client receives a message from the server on a topic the client is subscribed to. With this callback, you can process messages made to subscribed topics.

_Example of subscribing in V1_

```
ResponseCode PubSub::SubscribeCallback(
            util::String topic_name,
            util::String payload,
            std::shared_ptr<mqtt::SubscriptionHandlerContextData> p_app_handler_data)
{
    /* subscribe callback */
    return ResponseCode::SUCCESS;
}

std::unique_ptr<Utf8String> p_topic_name = Utf8String::Create("my/own/topic");
mqtt::Subscription::ApplicationCallbackHandlerPtr p_sub_handler =
        std::bind(&PubSub::SubscribeCallback,
                  this,
                  std::placeholders::_1,
                  std::placeholders::_2,
                  std::placeholders::_3);


std::shared_ptr<mqtt::Subscription> p_subscription =
        mqtt::Subscription::Create(std::move(p_topic_name),
                                   mqtt::QoS::QOS0,
                                   p_sub_handler,
                                   nullptr); // handler data

util::Vector<std::shared_ptr<mqtt::Subscription>> topic_vector;
topic_vector.push_back(p_subscription);
std::chrono::milliseconds action_response_timeout = 10;

// Subscribe to topic
ResponseCode rc = p_iot_client_->Subscribe(topic_vector,
                                           action_response_timeout);

```

_Example of subscribing in V2_

```
builder->WithPublishReceivedCallback(
        [&](const Mqtt5::PublishReceivedEventData &eventData) {
            /*Called when a message is received by one of the active subscriptions.*/
        });

std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client = builder->Build();

Mqtt5::Subscription sub1("my/own/topic",
                         Mqtt5::QOS::AWS_MQTT5_QOS_AT_LEAST_ONCE);
std::shared_ptr<Mqtt5::SubscribePacket> subPacket =
        std::make_shared<Mqtt5::SubscribePacket>();
subPacket->WithSubscription(std::move(sub1));

auto onSubAck = [&](int error_code,
                    std::shared_ptr<Mqtt5::SubAckPacket> suback)
{ /* acknowledge callback */ };

// Subscribe to topic
bool rc = client->Subscribe(subPacket, onSubAck);

```



### Unsubscribe

V1 SDK provides blocking and non-blocking API for unsubscribing. To unsubscribe from topic in V1, you should provide a `std::vector` of `std::unique_ptr` of `Utf8String` to the [Unsubscribe](http://aws-iot-device-sdk-cpp-docs.s3-website-us-east-1.amazonaws.com/latest/classawsiotsdk_1_1_mqtt_client.html#a8dc5fa8e8c1522219e6df33cbaa7e376) operation.
For asynchronous operations use [UnsubscribeAsync](http://aws-iot-device-sdk-cpp-docs.s3-website-us-east-1.amazonaws.com/latest/classawsiotsdk_1_1_mqtt_client.html#a4577dd3e720dea692755c640d4c638ed)  success and failure results are sent through the callback [ActionData::AsyncAckNotificationHandlerPtr](http://aws-iot-device-sdk-cpp-docs.s3-website-us-east-1.amazonaws.com/latest/classawsiotsdk_1_1_action_data.html#a5d1d7452e081205b414e4df985d82f60) 

V2 SDK provides only asynchronous non-blocking API. First, you need to create an [UnsubscribePacket](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_unsubscribe_packet.html) object with the help of [UnsubscribePacketBuilder](https://awslabs.github.io/aws-crt-java/software/amazon/awssdk/crt/mqtt5/packets/UnsubscribePacket.UnsubscribePacketBuilder.html). The [Unsubscribe](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_mqtt5_client.html#a96a931b49893d54712062722c5ab7d1a) operation takes a description of the [UnsubscribePacket](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_unsubscribe_packet.html) you wish to send and returns a promise that resolves successfully with the corresponding [UnsubAckPacket](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_un_sub_ack_packet.html) returned by the broker; the promise is rejected with an error if anything goes wrong before the [UnsubAckPacket](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_un_sub_ack_packet.html) is received. You should always check the reason codes of a [UnsubAckPacket](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_un_sub_ack_packet.html) completion to determine if the unsubscribe operation actually succeeded.
Similar to subscribing, you can unsubscribe from multiple topics in one request: just call [WithTopicFilter](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_unsubscribe_packet.html#a822c4f630d69ce3d1ba6ce8db021ab2a) for each topic you wish to unsubscribe from.

_Example of unsubscribing in V1_

```
// Blocking API.
std::chrono::milliseconds action_response_timeout = 20;
std::unique_ptr<Utf8String> p_topic_name = Utf8String::Create("my/topic");
util::Vector<std::unique_ptr<Utf8String>> topic_vector;
topic_vector.push_back(std::move(p_topic_name));
ResponseCode rc = p_iot_client_->Unsubscribe(std::move(topic_vector),
                                            action_response_timeout);

```

```
// Non-blocking API.
void unsubAck(uint16_t action_id, ResponseCode rc)
{
}
uint16_t packet_id = 0;
util::Vector<std::unique_ptr<Utf8String>> topic_vector;
p_topic_name = "my/topic";
topic_vector.push_back(std::move(p_topic_name));
ActionData::AsyncAckNotificationHandlerPtr p_async_ack_handler
auto unsubAck = [&](uint16_t action_id, ResponseCode rc) { };
ResponseCode rc = p_iot_client_->UnsubscribeAsync(std::move(topic_vector),
                                                  unsubAck,
                                                  packet_id);
```

_Example of unsubscribing in V2_

```
std::shared_ptr<Mqtt5::UnsubscribePacket> unsub =
        std::make_shared<Mqtt5::UnsubscribePacket>();
unsub->WithTopicFilter("my/topic");
auto unsubAck = [&](int, std::shared_ptr<Mqtt5::UnSubAckPacket>) {
    /* callback */
    };
bool rc = client->Unsubscribe(unsub, unsubAck);

```



### Client Stop

In V1 SDK, the `disconnect` method in the `AWSIotMqttClient` class disconnects the client. Once disconnected, the client can connect again by calling `connect`.

In V2 SDK, an MQTT5 client can stop a session by calling the [stop](https://awslabs.github.io/aws-crt-java/software/amazon/awssdk/crt/mqtt5/Mqtt5Client.html#stop(software.amazon.awssdk.crt.mqtt5.packets.DisconnectPacket)) method. You can provide an optional [DisconnectPacket](https://awslabs.github.io/aws-crt-java/software/amazon/awssdk/crt/mqtt5/packets/DisconnectPacket.html) parameter. A closed client can be started again by calling [start](https://awslabs.github.io/aws-crt-java/software/amazon/awssdk/crt/mqtt5/Mqtt5Client.html#start()).

_Example of disconnecting a client in V1_

```
std::chrono::milliseconds action_response_timeout = 20;
client.Disconnect(action_response_timeout);
```

_Example of disconnecting a client in V2_

```
builder->WithClientStoppedCallback([](const Mqtt5::OnStoppedEventData &event) {
    /* on stop callback */
});
Mqtt5Client client = builder->Build();
client->Stop();
```



### Reconnects

V1 attempts to reconnect automatically until connection succeeds or `client.Disconnect()` is called

V2 attempts to reconnect automatically until connection succeeds or `client.stop()` is called, either the initial `client→Start()` succeeds or fails. The reconnection parameters, such as min/max delays and [jitter modes](https://aws.github.io/aws-iot-device-sdk-cpp-v2/namespace_aws_1_1_crt_1_1_mqtt5.html#ab88e42f90f56a82b1af57320ffadbafd), are configurable through [Aws::Crt::Mqtt5::ReconnectOptions](https://aws.github.io/aws-iot-device-sdk-cpp-v2/struct_aws_1_1_crt_1_1_mqtt5_1_1_reconnect_options.html).


_Example of tweaking reconnection settings in V1_

```
std::chrono::seconds min_reconnect_backoff_timeout = 20;
std::chrono::seconds max_reconnect_backoff_timeout = 30;
client.SetMinReconnectBackoffTimeout(min_reconnect_backoff_timeout);
client.SetMaxReconnectBackoffTimeout(max_reconnect_backoff_timeout);
```

_Example of tweaking reconnection settings in V2_

```
Aws::Crt::Mqtt5::ReconnectOptions reconnectOptions = {
        Mqtt5::JitterMode::AWS_EXPONENTIAL_BACKOFF_JITTER_FULL, // reconnect mode
        1000,  // min reconnect delay ms
        1000,  // max reconnect delay ms
        1000   // min connected time to reset reconnect delay ms
};
builder.WithReconnectOptions();
Mqtt5Client client = builder->Build();

```



### Offline Operations Queue

V1 doesn’t set a limit on the number on in-flight publish

V2 provides a way to configure which kind of packets will be placed into the offline queue when the client is in the disconnected state. The following code snippet demonstrates how to enable storing all packets except QOS0 publish packets in the offline queue on disconnect:

_Example of configuring the offline queue in V2_

```
AwsIotMqtt5ClientBuilder builder;
std::shared_ptr<Aws::Iot::Mqtt5ClientBuilder>builder(
    Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(/* ... */));
builder.WithOfflineQueueBehavior(
        Mqtt5::ClientOperationQueueBehaviorType::
            AWS_MQTT5_COQBT_FAIL_QOS0_PUBLISH_ON_DISCONNECT);
Mqtt5Client client = builder->Build();

```

Note that AWS IoT Core [limits the number of allowed operations per second](https://docs.aws.amazon.com/general/latest/gr/iot-core.html#message-broker-limits). The [`getOperationStatistics`](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_mqtt5_client.html#aa9bf915cfbcfc80b4dc47bbda3529f72) method returns  the current state of an `Mqtt5Client` object’s queue of operations, which may help with tracking the number of in-flight messages.

```
Mqtt5::Mqtt5ClientOperationStatistics statistics =
        mqtt5Client->GetOperationStatistics();
std::cout<<"Client operations queue statistics\n"
         <<"\tgetUnackedOperationCoount: " <<statistics.unackedOperationCount<<"\n"
         <<"\tgetUnackedOperationSize: "<<statistics.unackedOperationSize<<"\n"
         <<"\tgetIncompleteOperationCount: "<<statistics.incompleteOperationCount<<"\n"
         <<"\tgetIncompleteOperationSize: "<<statistics.incompleteOperationSize<<std::endl;

```

See [withOfflineQueueBehavior documentation](https://aws.github.io/aws-iot-device-sdk-cpp-v2/namespace_aws_1_1_crt_1_1_mqtt5.html#a1eb626870603eab906714e2b86d79816) for more details.
See [ClientOfflineQueueBehavior documentation](https://aws.github.io/aws-iot-device-sdk-cpp-v2/namespace_aws_1_1_crt_1_1_mqtt5.html#a1eb626870603eab906714e2b86d79816) to find the list of the supported offline queue behaviors and their description.


### Operation Timeouts

In V1 SDK, all operations (*publish*, *subscribe*, *unsubscribe*) will not timeout unless you define a timeout for them. If no timeout is defined, there is a possibility that an operation will wait forever for the server to respond and block the calling thread indefinitely.

In V2 SDK, operations timeout is set for the MQTT5 client with the builder method [withAckTimeoutSeconds](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_iot_1_1_mqtt5_client_builder.html#a2769eb658b3809c5bd3d28724b936a67). The default value is no timeout. As in V1 SDK, failing to set a timeout can cause an operation to stuck forever, but it won’t block the client.
The [`getOperationStatistics`](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_mqtt5_client.html#aa9bf915cfbcfc80b4dc47bbda3529f72) method returns  the current state of an `Mqtt5Client` object’s queue of operations, which may help with tracking operations.

_Example of timeouts in V1_

```
std::chrono::milliseconds connectTimeout = 200;
std::chrono::seconds keepAliveTimeout = 200;

util::String client_id_tagged = "clID"
std::unique_ptr<Utf8String> client_id = Utf8String::Create(client_id_tagged);

rc = p_iot_client_->Connect(connectTimeout,
                            true
                            mqtt::Version::MQTT_3_1_1,
                            keepAliveTimeout,
                            std::move(client_id),
                            nullptr,
                            nullptr,
                            nullptr);

std::chrono::seconds publishTimeout = 10;
rc = p_iot_client_->Publish(Utf8String::Create("my/topic"),
                            false,
                            false,
                            awsiotsdk::mqtt::QoS::QOS1,
                            "hello",
                            publishTimeout);
```

_Example of timeouts in V2_

```
builder.WithAckTimeoutSeconds(10);
Mqtt5Client client = builder->Build();
```



### Logging

V1 and V2 SDK uses a custom logger allowing to control the logging process simultaneously for all layers of the SDK.

_Example of enabling logging in V1_
To enable logging on V1 follow the next example:

```
#include "util/logging/Logging.hpp"
#include "util/logging/LogMacros.hpp"
#include "util/logging/ConsoleLogSystem.hpp"

std::shared_ptr<awsiotsdk::util::Logging::ConsoleLogSystem> p_log_system =
        std::make_shared<awsiotsdk::util::Logging::ConsoleLogSystem>(
                awsiotsdk::util::Logging::LogLevel::Info);
awsiotsdk::util::Logging::InitializeAWSLogging(p_log_system);

AWS_LOG_INFO(INTEG_TEST_RUNNER_LOG_TAG,
             "Initialize Test Config Failed with rc : %d",
             32);
```

_Example of enabling logging in V2_
You can enable logging by passing the folowing properties:

```
ApiHandle apiHandle;

apiHandle.InitializeLogging(Aws::Crt::LogLevel::Info,
                            "logFile");
/* or */
apiHandle.InitializeLogging(logLevel, stderr);

AWS_LOGF_ERROR("log location", "Invalid operation");

```

_Example of shutting down logging_

```
awsiotsdk::util::Logging::ShutdownAWSLogging();
```

In V2 logging is shutdown automatically with ApiHandle destruction when it goes out of scope

### Client for Device Shadow Service

V1 SDK is built with [AWS IoT device shadow support](http://docs.aws.amazon.com/iot/latest/developerguide/iot-thing-shadows.html), providing access to thing shadows (sometimes referred to as device shadows). It also supports a simplified shadow access model, which allows developers to exchange data with their shadows by just using getter and setter methods without having to serialize or deserialize any JSON documents.

V2 SDK supports device shadow service as well, but with completely different API.
First, you subscribe to special topics to get data and feedback from a service. The service client provides API for that. For example, `SubscribeToGetShadow``Accepted`  subscribes to a topic to which AWS IoT Core will publish a shadow document; and via the `SubscribeToGetShadowRejected` the server will notify you if it cannot send you a requested document.
After subscribing to all the required topics, the service client can start interacting with the server, for example update the status or request for data. These actions are also performed via client API calls. For example, `PublishGetShadow`  sends a request to AWS IoT Core to get a shadow document. The requested Shadow document will be received in a callback specified in the `SubscribeToGetShadowAccepted` call.

AWS IoT Core [documentation for Device Shadow](https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html) service provides detailed descriptions for the topics used to interact with the service.

_Example of creating a Device Shadow service client in V1_

```
#include "shadow/Shadow.hpp"
// Blocking and non-blocking API.
String thingName = "<thing name>";
std::chrono::milliseconds mqtt_command_timeout = 200;
ResponseCode rc = p_iot_client_->Connect(/* ... */);
// 1st way
Shadow shadowClient(
        p_iot_client_,
        mqtt_command_timeout,
        thingNmae,
        clientTokenPrefix);

// 2nd way
 std::unique_ptr<Shadow> shadowClient = Shadow::Create(
        p_iot_client_,
        mqtt_command_timeout,
        thingName,
        clientTokenPrefix);

```



```
// Simplified shadow Access Model.
ResponseCode ShadowDelta::ActionResponseHandler(
        util::String thing_name,
        ShadowRequestType request_type,
        ShadowResponseType response_type,
        util::JsonDocument &payload)
{
}
Shadow::RequestHandlerPtr p_action_handler = std::bind(
        &ShadowDelta::ActionResponseHandler,
        this, std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4);
util::Map<ShadowRequestType, Shadow::RequestHandlerPtr> request_mapping;
request_mapping.insert(std::make_pair(ShadowRequestType::Get, p_action_handler));
request_mapping.insert(std::make_pair(ShadowRequestType::Update, p_action_handler));
request_mapping.insert(std::make_pair(ShadowRequestType::Delete, p_action_handler)); 
request_mapping.insert(std::make_pair(ShadowRequestType::Delta, p_action_handler));

my_shadow.AddShadowSubscription(request_mapping);

```


_Example of creating a Device Shadow service client in V2_
A thing name in V2 SDK shadow client is specified for the operations with shadow documents.

```
#include <aws/iotshadow/ErrorResponse.h>
#include <aws/iotshadow/GetShadowRequest.h>
#include <aws/iotshadow/GetShadowResponse.h>
#include <aws/iotshadow/GetShadowSubscriptionRequest.h>
#include <aws/iotshadow/IotShadowClient.h>
#include <aws/iotshadow/ShadowDeltaUpdatedEvent.h>
#include <aws/iotshadow/ShadowDeltaUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/UpdateShadowRequest.h>
#include <aws/iotshadow/UpdateShadowResponse.h>
#include <aws/iotshadow/UpdateShadowSubscriptionRequest.h>

std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client = builder->Build();
client->Start();
Aws::Iotshadow::IotShadowClient shadowClient(client);

```


_Example of getting a shadow document in V1_

```
// Blockig API.
std::condition_variable sync_action_response_wait_;
ResponseCode ShadowDelta::ActionResponseHandler(
        util::String thing_name,
        ShadowRequestType request_type,
        ShadowResponseType response_type,
        util::JsonDocument &payload)
{
    sync_action_response_wait_.notify_all();
    return 0;
}

std::unique_lock<std::mutex> block_handler_lock(sync_action_response_lock_);
{
    ResponseCode rc = my_shadow.performGetAsync();
    std::chrono::milliseconds shadow_action_timeout = 200;
    if (ResponseCode::SUCCESS == rc) {
        cv_status cv = sync_action_response_wait_.wait_for(
                block_handler_lock,
                shadow_action_timeout);
        if (cv = cv_status::no_timeout) {
            util::JsonDocumen doc = my_shadow.GetServerDocument();
        }
    }
}
```

```
 // Non-blocking API.
 ResponseCode rc = my_shadow.performGetAsync();
 util::JsonDocumen doc = my_shadow.GetServerDocument();

```


_Example of getting a shadow document in V2_

```
GetShadowSubscriptionRequest shadowSubscriptionRequest;
shadowSubscriptionRequest.ThingName = "<thing name>";

auto onGetShadowAccepted = [&](GetShadowResponse *response, int ioErr) {
        /* shadow document received. */
        /* The `response` object contains the shadow document. */
    };
auto onGetShadowUpdatedAcceptedSubAck = [&](int ioErr) { };
auto onGetShadowRejected = [&](ErrorResponse *error, int ioErr) {
        /* called when getting the shadow document failed. */
    };

shadowClient.SubscribeToGetShadowAccepted(
        shadowSubscriptionRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        onGetShadowAccepted,
        onGetShadowUpdatedAcceptedSubAck);

auto onGetShadowUpdatedRejectedSubAck = [&](int ioErr) { };
shadowClient.SubscribeToGetShadowRejected(
        shadowSubscriptionRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        onGetShadowRejected,
        onGetShadowUpdatedRejectedSubAck);

GetShadowRequest shadowGetRequest;
shadowGetRequest.ThingName = "<thing name>";
auto onGetShadowRequestSubAck = [&](int ioErr) { };

/* Send request for a shadow document.
   On success, the document will be received on `onGetShadowAccepted` callback.
   On failure, the `onGetShadowRejected` callback will be called. */
shadowClient.PublishGetShadow(
        shadowGetRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        onGetShadowRequestSubAck);

```


_Example of updating a shadow document in V1_

```
// Non-blocking API.
util::JsonDocument doc;
util::JsonParser::InitializeFromJsonString(
        doc,
        "{\"state\":{\"reported\":{\"sensor\":3.0}}}");
my_shadow.UpdateDeviceShadow(doc);
rc = my_shadow.PerformUpdateAsync();

/* for a blocking model look at the Example of getting a shadow document in V1 */
```

_Example of updating a shadow document in V2_

```
UpdateShadowRequest updateShadowRequest;
updateShadowRequest.ClientToken = uuid.ToString();
updateShadowRequest.ThingName = "<thing name">;

ShadowState state;
JsonObject desired;
desired.WithString("sensor", "2.9");
state.Desired = desired;
updateShadowRequest.State = state;


auto onUpdateShadowAccepted = [&](UpdateShadowResponse *response, int ioErr) {
        // Called when an update request succeeded.
    };
auto onUpdatedAcceptedSubAck = [&](int ioErr) { };

UpdateShadowSubscriptionRequest updateShadowSubscriptionRequest;
updateShadowSubscriptionRequest.ThingName = "<thing name>";
shadowClient.SubscribeToUpdateShadowAccepted(
        updateShadowSubscriptionRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        onUpdateShadowAccepted,
        onUpdatedAcceptedSubAck);

auto onUpdateShadowRejected = [&](ErrorResponse *error, int ioErr) {
        // Called when an update request failed.
    };
auto onUpdatedRejectedSubAck = [&](int ioErr) { };
shadowClient.SubscribeToUpdateShadowRejected(
        updateShadowSubscriptionRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        onUpdateShadowRejected,
        onUpdatedRejectedSubAck);


auto publishCompleted = [&](int ioErr) { };
shadowClient.PublishUpdateShadow(
        updateShadowRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        std::move(publishCompleted));

```

See API documentation for V2 SDK [Device Shadow](https://aws.github.io/aws-iot-device-sdk-cpp-v2/namespace_aws_1_1_iotshadow.html) service client for more details.
Refer to the V2 SDK [Device Shadow](https://github.com/aws/aws-iot-device-sdk-cpp-v2/tree/b065b818f955aef6181b2c89815425ea6c5b4194/samples/shadow) sample for code example.


### Client for Jobs Service

V1 and V2 SDK offer support of AWS IoT Core services implementing a service client for the [Jobs](https://docs.aws.amazon.com/iot/latest/developerguide/iot-jobs.html) service which helps with defining a set of remote operations that can be sent to and run on one or more devices connected to AWS IoT.

V1 IotJobs APIs are defined [here](https://aws.github.io/aws-iot-device-sdk-cpp-v2/namespace_aws_1_1_iotjobs.html), with its corresponding code [samples](https://github.com/aws/aws-iot-device-sdk-cpp/tree/master/samples/Jobs)

The Jobs service client provides API similar to API provided by [Client for Device Shadow Service](https://quip-amazon.com/bbcuA13anxJa#temp:C:YWR759cb689cfb8488da0d1d539f). First, you subscribe to special topics to get data and feedback from a service. The service client provides API for that. After subscribing to all the required topics, the service client can start interacting with the server, for example update the status or request for data. These actions are also performed via client API calls.

AWS IoT Core documentation for [Jobs](https://docs.aws.amazon.com/iot/latest/developerguide/jobs-mqtt-api.html) service provides detailed descriptions for the topics used to interact with the service.

See API documentation for V2 SDK [Jobs](https://aws.github.io/aws-iot-device-sdk-cpp-v2/namespace_aws_1_1_iotjobs.html) service clients for more details.
Refer to the V2 SDK [Jobs](https://github.com/aws/aws-iot-device-sdk-cpp-v2/tree/b065b818f955aef6181b2c89815425ea6c5b4194/samples/jobs) samples for code examples.

_V1 example executing jobs_

```
ResponseCode GetPendingCallback(
        util::String topic_name,
        util::String payload,
        std::shared_ptr<mqtt::SubscriptionHandlerContextData> p_app_handler_data)
{}
ResponseCode NextJobCallback(
        util::String topic_name,
        util::String payload,
        std::shared_ptr<mqtt::SubscriptionHandlerContextData> p_app_handler_data)
{}
ResponseCode UpdateAcceptedCallback(
        util::String topic_name,
        util::String payload,
        std::shared_ptr<mqtt::SubscriptionHandlerContextData> p_app_handler_data)
{}
ResponseCode UpdateRejectedCallback(
        util::String topic_name,
        util::String payload,
        std::shared_ptr<mqtt::SubscriptionHandlerContextData> p_app_handler_data)
{}
rc = p_iot_client_->Connect(/*...*/);


mqtt::Subscription::ApplicationCallbackHandlerPtr p_pending_handler =
        std::bind(
                &GetPendingCallback,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3);

mqtt::Subscription::ApplicationCallbackHandlerPtr p_next_handler =
        std::bind(
                &NextJobCallback,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3);

mqtt::Subscription::ApplicationCallbackHandlerPtr p_update_accepted_handler =
        std::bind(
                &UpdateAcceptedCallback,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3);

mqtt::Subscription::ApplicationCallbackHandlerPtr p_update_rejected_handler =
        std::bind(
                &UpdateRejectedCallback,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3);

util::Vector<std::shared_ptr<mqtt::Subscription>> topic_vector;
std::shared_ptr<mqtt::Subscription> p_subscription;

p_subscription = p_jobs_->CreateJobsSubscription(
        p_pending_handler,
        nullptr,
        Jobs::JOB_GET_PENDING_TOPIC,
        Jobs::JOB_ACCEPTED_REPLY_TYPE);
topic_vector.push_back(p_subscription);

p_subscription = p_jobs_->CreateJobsSubscription(
        p_next_handler,
        nullptr,
        Jobs::JOB_DESCRIBE_TOPIC,
        Jobs::JOB_ACCEPTED_REPLY_TYPE,
        "$next");
topic_vector.push_back(p_subscription);

p_subscription = p_jobs_->CreateJobsSubscription(
        p_next_handler,
        nullptr,
        Jobs::JOB_NOTIFY_NEXT_TOPIC);
topic_vector.push_back(p_subscription);

p_subscription = p_jobs_->CreateJobsSubscription(
        p_update_accepted_handler,
        nullptr,
        Jobs::JOB_UPDATE_TOPIC,
        Jobs::JOB_ACCEPTED_REPLY_TYPE,
        "+");
topic_vector.push_back(p_subscription);

p_subscription = p_jobs_->CreateJobsSubscription(
        p_update_rejected_handler,
        nullptr,
        Jobs::JOB_UPDATE_TOPIC,
        Jobs::JOB_REJECTED_REPLY_TYPE,
        "+");
topic_vector.push_back(p_subscription);

ResponseCode rc = p_iot_client_->Subscribe(
        topic_vector,
        ConfigCommon::mqtt_command_timeout_);

util::String client_id_tagged = ConfigCommon::base_client_id_;
client_id_tagged.append("_jobs_sample_");
client_id_tagged.append(std::to_string(rand()));

p_jobs_ = Jobs::Create(
        p_iot_client_,
        mqtt::QoS::QOS1,
        "<thing name>",
        client_id_tagged);
ResponseCode rc;
rc = p_jobs_->SendJobsQuery(Jobs::JOB_GET_PENDING_TOPIC);
rc = p_jobs_->SendJobsQuery(Jobs::JOB_DESCRIBE_TOPIC, "$next");

```

_V2 example executig a job_

```
IotJobsClient jobsClient(client);

auto subscriptionHandler = [&](DescribeJobExecutionResponse *response, int ioErr)
{ };
auto subAckHandler = [&](int)
 { };

// Get information about the job
DescribeJobExecutionSubscriptionRequest describeJobExecutionSubscriptionRequest;
describeJobExecutionSubscriptionRequest.ThingName = "<thing name>";
describeJobExecutionSubscriptionRequest.JobId = "<job id>";

jobsClient.SubscribeToDescribeJobExecutionAccepted(
        describeJobExecutionSubscriptionRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        subscriptionHandler,
        subAckHandler);

auto failureHandler = [&](RejectedError *rejectedError, int ioErr) { };

jobsClient.SubscribeToDescribeJobExecutionRejected(
        describeJobExecutionSubscriptionRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        failureHandler,
        subAckHandler);

DescribeJobExecutionRequest describeJobExecutionRequest;
describeJobExecutionRequest.ThingName = "<thing name>";
describeJobExecutionRequest.JobId = "<job id>";
describeJobExecutionRequest.IncludeJobDocument = true;
Aws::Crt::UUID uuid;
describeJobExecutionRequest.ClientToken = uuid.ToString();

auto publishHandler = [&](int ioErr) {  };

jobsClient.PublishDescribeJobExecution(
        QTT_QOS_AT_LEAST_ONCE,
        publishHandler);
auto OnSubscribeToStartNextPendingJobExecutionAcceptedResponse =
        [&](StartNextJobExecutionResponse *response, int ioErr) { };

// Gets and starts the next pending job execution for a thing (
StartNextPendingJobExecutionSubscriptionRequest subscriptionRequest;
subscriptionRequest.ThingName = "<thing name>";

jobsClient.SubscribeToStartNextPendingJobExecutionAccepted(
        subscriptionRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        OnSubscribeToStartNextPendingJobExecutionAcceptedResponse,
        subAckHandler);
jobsClient.SubscribeToStartNextPendingJobExecutionRejected(
        subscriptionRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        failureHandler,
        subAckHandler);

StartNextPendingJobExecutionRequest publishRequest;
publishRequest.ThingName = cmdData.input_thingName;
publishRequest.StepTimeoutInMinutes = 15L;

jobsClient.PublishStartNextPendingJobExecution(
        publishRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        publishHandler);


// Send an update about the status of the job
auto failureHandler = [&](RejectedError *rejectedError, int ioErr) { };

auto subscribeHandler = [&](UpdateJobExecutionResponse *response, int ioErr) { };

auto publishHandler = [&](int ioErr) { };

jobsClient.SubscribeToUpdateJobExecutionAccepted(
subscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, subscribeHandler, subAckHandler);

jobsClient.SubscribeToUpdateJobExecutionRejected(
subscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, failureHandler, subAckHandler);

UpdateJobExecutionRequest publishRequest;
publishRequest.ThingName = "<thing name>";
publishRequest.JobId = "<job id>";
publishRequest.ExecutionNumber = 12;
publishRequest.Status = JobStatus::IN_PROGRESS;
publishRequest.ExpectedVersion = 0

jobsClient.PublishUpdateJobExecution(
        publishRequest,
        AWS_MQTT_QOS_AT_LEAST_ONCE,
        publishHandler);

```



### Client for Fleet Provisioning Service

Another IoT service that V2 SDK provides access to is [Fleet Provisioning](https://docs.aws.amazon.com/iot/latest/developerguide/provision-wo-cert.html) (also known as Identity Service). By using AWS IoT fleet provisioning, AWS IoT can generate and securely deliver device certificates and private keys to your devices when they connect to AWS IoT for the first time.

The Fleet Provisioning service client provides API similar to API provided by [Client for Device Shadow Service](https://quip-amazon.com/bbcuA13anxJa#temp:C:YWR759cb689cfb8488da0d1d539f). First, you subscribe to special topics to get data and feedback from a service. The service client provides API for that. After subscribing to all the required topics, the service client can start interacting with the server, for example update the status or request for data. These actions are also performed via client API calls.

AWS IoT Core documentation for [Fleet Provisioning](https://docs.aws.amazon.com/iot/latest/developerguide/fleet-provision-api.html) service provides detailed descriptions for the topics used to interact with the service.

See API documentation for V2 SDK  [Fleet Provisioning](https://aws.github.io/aws-iot-device-sdk-cpp-v2/namespace_aws_1_1_iotidentity.html) service client for more details.
Refer to the V2 SDK [Fleet Provisioning](https://github.com/aws/aws-iot-device-sdk-cpp-v2/tree/main/samples/fleet_provisioning/mqtt5_fleet_provisioning) samples for code examples.


### Example

It’s always helpful to look at a working example to see how new functionality works, to be able to tweak different options, to compare with existing code. For that reasons, we implemented a [Publish/Subscribe example](https://github.com/aws/aws-iot-device-sdk-cpp-v2/tree/main/samples/mqtt5/mqtt5_pubsub) ([source code](https://github.com/aws/aws-iot-device-sdk-cpp-v2/blob/main/samples/mqtt5/mqtt5_pubsub/main.cpp)) in V2 SDK similar to a sample provided by V1 SDK (see a corresponding [readme section](https://github.com/aws/aws-iot-device-sdk-cpp/blob/master/samples/README.md) and [source code](https://github.com/aws/aws-iot-device-sdk-cpp/blob/master/samples/PubSub/PubSub.cpp)).


## How to get help

For any migration related questions or feedback, you can contact us at [discussion](https://github.com/aws/aws-iot-device-sdk-cpp-v2/discussions) by submitting an issue with a label `label:migration`.

## Appendix

### MQTT5 Features

**Clean Start and Session Expiry**
You can use Clean Start and Session Expiry to handle your persistent sessions with more flexibility.
Refer to [Mqtt5ClientOptions.ClientSessionBehavior](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_mqtt5_client_options.html#a61d6bedd2502d209db912838f74462bb) enum and [NegotiatedSettings.getSessionExpiryInterval](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_conn_ack_packet.html#aed6565927dcc2ecfb789f978f5a1aee4) method for details.

**Reason Code on all ACKs**
You can debug or process error messages more easily using the reason codes. Reason codes are returned by the message broker based on the type of interaction with the broker (Subscribe, Publish, Acknowledge).
See [PubAckReasonCode](https://aws.github.io/aws-iot-device-sdk-cpp-v2/namespace_aws_1_1_crt_1_1_mqtt5.html#a5901f1fc1e66ef0f859402b747630a02), [SubAckReasonCode](https://aws.github.io/aws-iot-device-sdk-cpp-v2/namespace_aws_1_1_crt_1_1_mqtt5.html#a272e5b89320326afd9e0de269100ccd3), [UnsubAckReasonCode](https://aws.github.io/aws-iot-device-sdk-cpp-v2/namespace_aws_1_1_crt_1_1_mqtt5.html#a0fece0c83f48d577ea7dfafe58f1261a), [ConnectReasonCode](https://aws.github.io/aws-iot-device-sdk-cpp-v2/namespace_aws_1_1_crt_1_1_mqtt5.html#a25d5cf0c9496d5002642c146bf0af9b2), [DisconnectReasonCode](https://aws.github.io/aws-iot-device-sdk-cpp-v2/namespace_aws_1_1_crt_1_1_mqtt5.html#ac305e4f9be3e3b06adfdb0abb4814163). 

**Topic Aliases**
You can substitute a topic name with a topic alias, which is a two-byte integer.
Use [withTopicAlias](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_publish_packet.html#a609e3b04a9c670e07f746da527d3bf17) method when creating a PUBLISH packet.

**Message Expiry**
You can add message expiry values to published messages. Use [withMessageExpiryIntervalSeconds](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_publish_packet.html#aa9e7f2887ab39b0c82a990119df7b941) method in PublishPacketBuilder class.

**Server disconnect**
When a disconnection happens, the server can proactively send the client a DISCONNECT to notify connection closure with a reason code for disconnection.
Refer to [DisconnectPacket](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_disconnect_packet.html) class for details.

**Request/Response**
Publishers can request a response be sent by the receiver to a publisher-specified topic upon reception. Use [withResponseTopic](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_publish_packet.html#acccc99a74512973210026a24f37c2db5) method in PublishPacketBuilder class.

**Maximum Packet Size**
Client and Server can independently specify the maximum packet size that they support. See [connectPacketBuilder.withMaximumPacketSizeBytes](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_connect_packet.html#a88ec9f83510875c5cd92277ecc439bad), [NegotiatedSettings.getMaximumPacketSizeToServer](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_connect_packet.html#a25670e9f1c004d93b3332cd432689b92), and [ConnAckPacket.getMaximumPacketSize](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_conn_ack_packet.html#a83a5f4aaa007bdf6dddc41c718d0bfd6) methods.

**Payload format and content type**
You can specify the payload format (binary, text) and content type when a message is published. These are forwarded to the receiver of the message. Use [withContentType](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_crt_1_1_mqtt5_1_1_publish_packet.html#a0a5e4d33a3c82cdf4d6ef5d490bd509f) method in PublishPacketBuilder class.

**Shared Subscriptions**
Shared Subscriptions allow multiple clients to share a subscription to a topic and only one client will receive messages published to that topic using a random distribution.
Refer to [shared subscription](https://github.com/aws/aws-iot-device-sdk-java-v2/blob/main/samples/Mqtt5/SharedSubscription/README.md) in V2 SDK.
**NOTE** AWS IoT Core provides this functionality for MQTT3 as well.

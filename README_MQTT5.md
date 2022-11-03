# MQTT5
The following sections provide some basic examples of using the SDK to access the AWS IoT service over MQTT5. For more details please refer to < MQTT5 sample link > and < API Documentation link >

## Initialize the Client

To access the AWS IoT service, you can create a `Mqtt5Client` usng `Mqtt5ClientBuilder` in which you initialize the client depends on the connection type (MQTT5 or MQTT5 over WebSocket) and authentication type you choose. The `Mqtt5ClientBuilder` will setup the client options and create a `Mqtt5Client` for you.

* Initialize the Client with MTLS:
For MQTT5 client, the AWS IoT service requires TLS mutual authentication.
The following examples showed how to create a client with a valid client certificate (X.509) and key. You can use the [AWS IoT console][aws-iot-console] or the AWS command line tools to generate certificates and keys.

```
// use value returned by describe-endpoint --endpoint-type "iot:Data-ATS"
Crt::String clientEndpoint = "<prefix>-ats.iot.<region>.amazonaws.com";
// X.509 based certificate file
const char* certificateFilePath = "<certificate file path>";
// PKCS#1 or PKCS#8 PEM encoded private key file
const char* privateKeyFilePath = "<private key file path>";

// Create a Client using Mqtt5ClientBuilder
Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
    clientEndpoint, certificateFilePath, privateKeyFilePath);

/* You can setup other client options and callbacks before call builder->Build(). Once the the client get created, you could no longer update the client options or connection options.

// Build Mqtt5Client
std::shared_ptr<Aws::Crt::Mqtt5Client> client = builder->Build();

if (client == nullptr)
{
    fprintf(stdout, "Client creation failed.\n");
    return -1;
}

```


* Initialize the Client with MQTT Over WebSocket:

```
// use value returned by describe-endpoint --endpoint-type "iot:Data-ATS"
Crt::String clientEndpoint = "<prefix>-ats.iot.<region>.amazonaws.com";
// X.509 based certificate file
const char* certificateFilePath = "<certificate file path>";
// PKCS#1 or PKCS#8 PEM encoded private key file
const char* privateKeyFilePath = "<private key file path>";
// The signing region. e.x.: 'us-east-1'
const char* signing_region = '<signing_region>';

// Create websocket configuration
Aws::Crt::Auth::CredentialsProviderChainDefaultConfig defaultConfig;
std::shared_ptr<Aws::Crt::Auth::ICredentialsProvider> provider = Aws::Crt::Auth::CredentialsProvider::CreateCredentialsProviderChainDefault(defaultConfig);
if (!provider)
{
    fprintf(stderr, "Failure to create credentials provider!\n");
    exit(-1);
}
Aws::Iot::WebsocketConfig websocketConfig(signing_region, provider);

// Create a Client using Mqtt5ClientBuilder
Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithWebsocket(
    clientEndpoint, websocketConfig);

/* You can setup other client options and callbacks before call builder->Build(). Once the the client get created, you could no longer update the client options or connection options.

// Build Mqtt5Client
std::shared_ptr<Aws::Crt::Mqtt5Client> mqtt5Client = builder->Build();

if (mqtt5Client == nullptr)
{
    fprintf(stdout, "Client creation failed.\n");
    return -1;
}
```

## Start & Stop Client connection Session
After build the Mqtt5Client, you can start/stop a MQTT5 connection session as you needed. Mqtt5Client was designed with a async manner. We use callbacks to track life cycle status of the Mqtt5Client.

```
// Create Mqtt5ClientBulder

std::promise<bool> connectionPromise;
std::promise<void> stoppedPromise;

// Setup lifecycle callbacks
builder->withClientConnectionSuccessCallback([&connectionPromise](
                                                    Mqtt5::Mqtt5Client&,
                                                    std::shared_ptr<Aws::Crt::Mqtt5::ConnAckPacket>,
                                                    std::shared_ptr<Aws::Crt::Mqtt5::NegotiatedSettings> settings) {
    fprintf(stdout, "Mqtt5 Client connection succeed, clientid: %s.\n", settings->getClientId().c_str());
    connectionPromise.set_value(true);
});

builder->withClientConnectionFailureCallback(
    [&connectionPromise](Mqtt5::Mqtt5Client &, int error_code, std::shared_ptr<Aws::Crt::Mqtt5::ConnAckPacket>)
    {
        fprintf(stdout, "Mqtt5 Client connection failed with error: %s.\n", aws_error_debug_str(error_code));
        connectionPromise.set_value(false);
    });

builder->withClientStoppedCallback(
    [&stoppedPromise](Mqtt5::Mqtt5Client &)
    {
    fprintf(stdout, "Mqtt5 Client stopped.\n");
    stoppedPromise.set_value();
});

// Build Mqtt5Client
std::shared_ptr<Aws::Crt::Mqtt5Client> client = builder->Build();

if (mqtt5Client == nullptr)
{
    fprintf(stdout, "Client creation failed.\n");
    return -1;
}

if (!mqtt5Client->Start())
{
    fprintf("Failed start Mqtt5 client");
    return -1;
}

// Waiting for ConnAck from the server
if (connectionPromise.get_future().get() == false)
{
    fprintf("Connection failed.");
}

if (!mqtt5Client->Stop())
{
    fprintf(stdout, "Failed to disconnect from the mqtt connection. Exiting..\n");
    return -1;
}
// Wait for client shutdown
disconnectPromise.get_future().wait();

```


## Publish and Subscribe

After the Mqtt5Client get setup and started, you can perform publish, subscribe and unsubscribe oprations.

* To publish a message:

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

* To subscribe to topics:

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

* To unsubscribe to topics:

    Similar to subscribe operation, you can unsubscribe from multiple topics.

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
    fprintf(stdout, "Failed to perform subscribe operation on client." );
}

```

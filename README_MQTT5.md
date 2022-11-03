# MQTT5
The following sections provide some basic examples of using the SDK to access the AWS IoT service over MQTT5. For more details please refer to <MQTT5 samples>

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
std::shared_ptr<Aws::Crt::Mqtt5Client> client = builder->Build();

if (client == nullptr)
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

if (client == nullptr)
{
    fprintf(stdout, "Client creation failed.\n");
    return -1;
}

if (!client->Start())
{
    fprintf("Failed start Mqtt5 client");
    return -1;
}

// Waiting for ConnAck from the server
if (connectionPromise.get_future().get() == false)
{
    fprintf("Connection failed.");
}

if (!client->Stop())
{
    fprintf(stdout, "Failed to disconnect from the mqtt connection. Exiting..\n");
    return -1;
}
// Wait for client shutdown
disconnectPromise.get_future().wait();

```


## Publish and Subscribe

After the Mqtt5Client setup and started, you can publish messages and subscribe
to topics.

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

if(!client->Publish(publish, std::move(callback)))
{
    fprintf(stdout, "Publish Operation Failed.\n");
    return -1;
}

```

* To subscribe to a topic:

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
    [](std::shared_ptr<Mqtt5::Mqtt5Client>, int, std::shared_ptr<Mqtt5::SubAckPacket> suback)
    {
        for (auto code : suback->getReasonCodes())
        {
            std::cout << "Get suback with codes: " << code << std::endl;
            if (code > Mqtt5::SubAckReasonCode::AWS_MQTT5_SARC_GRANTED_QOS_2)
            {
                std::cout << "Subscription Succeed." << std::endl;
            }
            else
            {
                std::cout << "Subscription Failed." << std::endl;
            }
        }
    });

if (!subSuccess)
{
    std::cout << "[ERROR]Subscription Failed." << std::endl;
    if (mqtt5Client->Stop())
    {
        stoppedPromise.get_future().get();
    }
    else
    {
        std::cout << "[ERROR]Failed to stop the client " << std::endl;
    }
    return -1;
}

```


*Note*: all operations (publish, subscribe, unsubscribe) will not timeout unless
you define a timeout when performing the operation. If no timeout is defined, then
a value of 0 is used, meaning the operation will never timeout and, in rare cases,
wait forever for the server to respond and block the calling thread indefinitely.
It is recommended to set a timeout for QoS1 operations if your application expects
responses within a fixed duration or if there is the possibility the server you are
communicating with may not respond.

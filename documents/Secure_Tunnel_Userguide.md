# Introduction
When devices are deployed behind restricted firewalls at remote sites, you need a way to gain access to those devices for troubleshooting, configuration updates, and other operational tasks. Use secure tunneling to establish bidirectional communication to remote devices over a secure connection that is managed by AWS IoT. Secure tunneling does not require updates to your existing inbound firewall rules, so you can keep the same security level provided by firewall rules at a remote site.

More information on the service and how to open, close, and manage secure tunnels can be found here: https://docs.aws.amazon.com/iot/latest/developerguide/secure-tunneling.html

A sample is also provided and can be found here: https://github.com/aws/aws-iot-device-sdk-cpp-v2/tree/main/samples#secure-tunnel



# Getting started with Secure Tunnels
## How to Create a Secure Tunnel Client
Once a Secure Tunnel builder has been created, it is ready to make a Secure Tunnel client. Something important to note is that once a Secure Tunnel client is built and finalized, the configuration is immutable. Further modifications to the Secure Tunnel builder will not change the settings of already created Secure Tunnel clients.

```
// Create Secure Tunnel Builder
SecureTunnelBuilder builder = SecureTunnelBuilder(...);

// Build Secure Tunnel Client
std::shared_ptr<SecureTunnel> secureTunnel = builder.Build();

if (secureTunnel == nullptr)
{
    fprintf(stdout, "Secure Tunnel creation failed.\n");
    return -1;
}

// Start the secure tunnel connection
if (!secureTunnel->Start())
{
    fprintf("Failed to start Secure Tunnel\n");
    return -1;
}
```
## Callbacks

### OnConnectionSuccess
When the Secure Tunnel Client successfully connects with the Secure Tunnel service, this callback will return the available (if any) service ids.

### OnConnectionFailure
When a WebSocket upgrade request fails, this callback will return an error code.

### OnConnectionShutdown
When the WebSocket connection shuts down, this callback will be invoked.

### OnSendDataComplete
When a message has been completely written to the socket, this callback will be invoked.

### OnMessageReceived
When a message is received on an open Secure Tunnel stream, this callback will return the message.

### OnStreamStarted
When a stream is started by a Source connected to the Destination, the Destination will invoke this callback and return the stream information.

### OnStreamStopped
When an open stream is closed, this callback will be invoked and return the stopped stream's information.

### OnSessionReset
When the Secure Tunnel Service requests the Secure Tunnel client fully reset, this callback is invoked.

### OnStopped
When the Secure Tunnel has reached a fully stopped state this callback is invoked.

## Setting Secure Tunnel Callbacks
The Secure Tunnel client uses callbacks to keep the user updated on its status and pass along relavant information. These can be set up using the Secure Tunnel builder's With functions.

```
// Create Secure Tunnel Builder
SecureTunnelBuilder builder = SecureTunnelBuilder(...);

// Setting the onMessageReceived callback using the builder
builder.WithOnMessageReceived([&](SecureTunnel *secureTunnel, const MessageReceivedEventData &eventData) {
        {
            std::shared_ptr<Message> message = eventData.message;
            if (message->getServiceId().has_value()){
                fprintf(
                    stdout,
                    "Message received on service id:'" PRInSTR "'\n",
                    AWS_BYTE_CURSOR_PRI(message->getServiceId().value()));
            }

            if(message->getPayload().has_value()){
                fprintf(
                    stdout,
                    "Message has payload:'" PRInSTR "'\n",
                    AWS_BYTE_CURSOR_PRI(message->getPayload().value()));
            }
        }
    });

// Build Secure Tunnel Client
std::shared_ptr<SecureTunnel> secureTunnel = builder.Build();

if (secureTunnel == nullptr)
{
    fprintf(stdout, "Secure Tunnel creation failed.\n");
    return -1;
}

// Start the secure tunnel connection
if (!secureTunnel->Start())
{
    fprintf("Failed to start Secure Tunnel\n");
    return -1;
}

// Messages received on a stream will now be printed to stdout.
```

# How to Start and Stop

## Start
Invoking Start() on the Secure Tunnel Client will put it into an active state where it recurrently establishes a connection to the configured Secure Tunnel endpoint using the provided [Client Access Token](https://docs.aws.amazon.com/iot/latest/developerguide/secure-tunneling-concepts.html). If a [Client Token](https://docs.aws.amazon.com/iot/latest/developerguide/secure-tunneling-concepts.html) is provided, the Secure Tunnel Client will use it but in the event one is not provided, the Secure Tunnel Client will automatically generate one for use on a reconnection attempts. The Client Token for any initial connection to the Secure Tunnel service MUST be unique. Reusing a Client Token from a previous connection will result in a failed connection to the Secure Tunnel Service.
```
// Create Secure Tunnel Builder
SecureTunnelBuilder builder = SecureTunnelBuilder(...);

// Adding a client token to the builder
String clientToken;
builder.WithClientToken(clientToken.c_str());

// Build Secure Tunnel Client
std::shared_ptr<SecureTunnel> secureTunnel = builder.Build();

if (secureTunnel == nullptr)
{
    fprintf(stdout, "Secure Tunnel creation failed.\n");
    return -1;
}

// Start the secure tunnel connection
if (!secureTunnel->Start())
{
    fprintf("Failed to start Secure Tunnel\n");
    return -1;
}
```

## Stop
Invoking Stop() on the Secure Tunnel Client breaks the current connection (if any) and moves the client into an idle state.
```
if(!secureTunnel->Stop()){
    fprintf(stdout, "Failed to stop the Secure Tunnel connection session. Exiting..\n");
}
```

# Secure Tunnel Operations

## Send Message
The SendMessage operation takes a description of the Message you wish to send and returns a success/failure in the synchronous logic that kicks off the Send Message operation. When the message is fully written to the socket, the OnSendDataComplete callback will be invoked.

```
Crt::String serviceId_string = "ssh";
Crt::String message_string = "any payload";

ByteCursor serviceId = ByteCursorFromString(serviceId_string);
ByteCursor payload = ByteCursorFromString(message_string);

// Create Message
std::shared_ptr<Message> message = std::make_shared<Message>();
message->withServiceId(serviceId);
message->withPayload(payload);

// Send Message
secureTunnel->SendMessage(message);
```

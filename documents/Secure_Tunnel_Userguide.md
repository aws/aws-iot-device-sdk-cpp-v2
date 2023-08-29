# Secure Tunnel Guide
#Table of Contents
* [Introduction](#introduction)
* [Getting Started with Secure Tunnels](#getting-started-with-secure-tunnels)
    + [How to Create a Secure Tunnel Client](#how-to-create-a-secure-tunnel-client)
* [Callbacks](#callbacks)
    + [Setting Secure Tunnel Callbacks](#setting-secure-tunnel-callbacks)
    + [Available Callbacks](#available-callbacks)
* [Connecting and Disconnecting the Secure Tunnel Client](#connecting-and-disconnecting-the-secure-tunnel-client)
    + [Start (Connect)](#start)
    + [Stop (Disconnect)](#stop)
* [Multiplexing and Simultaneous TCP Connections](#multiplexing-and-simultaneous-tcp-connections)
    + [Opening a Secure Tunnel with Multiplexing](#opening-a-secure-tunnel-with-multiplexing)
    + [Service Ids Available on the Secure Tunnel](#service-ids-available-on-the-secure-tunnel)
    + [Using Service Ids](#using-service-ids)
    + [Using Connection Ids](#using-connection-ids)
* [Send Message](#send-message)
* [Secure Tunnel Best Practices/FAQ](#secure-tunnel-best-practices)

## Introduction
When devices are deployed behind restricted firewalls at remote sites, you need a way to gain access to those devices for troubleshooting, configuration updates, and other operational tasks. Use secure tunneling to establish bidirectional communication to remote devices over a secure connection that is managed by AWS IoT. Secure tunneling does not require updates to your existing inbound firewall rules, so you can keep the same security level provided by firewall rules at a remote site.

More information on the service and how to open, close, and manage secure tunnels can be found here: https://docs.aws.amazon.com/iot/latest/developerguide/secure-tunneling.html

A sample is also provided and can be found here: https://github.com/aws/aws-iot-device-sdk-cpp-v2/tree/main/samples#secure-tunnel



# Getting started with Secure Tunnels
## How to Create a Secure Tunnel Client
Once a Secure Tunnel builder has been created, it is ready to make a Secure Tunnel client. Something important to note is that once a Secure Tunnel client is built and finalized, the configuration is immutable and cannot be changed. Further modifications to the Secure Tunnel builder will not change the settings of already created Secure Tunnel clients.

```cpp
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
# Callbacks
## Setting Secure Tunnel Callbacks
The Secure Tunnel client uses callbacks to keep the user updated on its status and pass along relavant information. These can be set up using the Secure Tunnel builder's With functions.

```cpp
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

// Messages received on a stream will now be printed to stdout.
```
## Available Callbacks
### OnConnectionSuccess
When the Secure Tunnel Client successfully connects with the Secure Tunnel service, this callback will return the available (if any) service ids.

### OnConnectionFailure
When a WebSocket upgrade request fails to connect, this callback will return an error code.

### OnConnectionShutdown
When the WebSocket connection shuts down, this callback will be invoked.

### OnSendMessageComplete

This callback is invoked when either a message has been completely written to the socket or an error occured.
The callback is invoked for the following operations:
- SendMessage
- SendStreamStart
- SendConnectionStart

See [Send Message](#send-message) for additional details.

### OnMessageReceived
When a message is received on an open Secure Tunnel stream, this callback will return the message.

### OnStreamStarted
When a stream is started by a Source connected to the Destination, the Destination will invoke this callback and return the stream information.

### OnStreamStopped
When an open stream is closed, this callback will be invoked and return the stopped stream's information.

### OnConnectionStarted
When a connection start message is received and a new active connection is established, the Destination will invoke this callback and return the connection information.

### OnConnectionReset
When a connection has ended either in error or closed intentionally by the secure tunnel peer, the client will invoke this callback and return the connection information.

### OnSessionReset
When the Secure Tunnel service requests the Secure Tunnel client fully reset, this callback is invoked.

### OnStopped
When the Secure Tunnel has reached a fully stopped state this callback is invoked.

# Connecting and Disconnecting the Secure Tunnel Client

## Start
Invoking `Start()` on the Secure Tunnel Client will put it into an active state where it recurrently establishes a connection to the configured Secure Tunnel endpoint using the provided [Client Access Token](https://docs.aws.amazon.com/iot/latest/developerguide/secure-tunneling-concepts.html). If a [Client Token](https://docs.aws.amazon.com/iot/latest/developerguide/secure-tunneling-concepts.html) is provided, the Secure Tunnel Client will use it. If a Client Token is not provided, the Secure Tunnel Client will automatically generate one for use on a reconnection attempts. The Client Token for any initial connection to the Secure Tunnel service **MUST** be unique. Reusing a Client Token from a previous connection will result in a failed connection to the Secure Tunnel Service.
```cpp
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
### Reconnecting
A Secure Tunnel Client that has been started will attempt to reconnect upon a failed connection attempt or disconnection until `Stop()` is called. If the secure tunnel closes or the access tokens change, the Secure Tunnel Client will become disconnected and continue to attempt a reconnection until `Stop()` is called. The Secure Tunnel Client implements a Full Jitter Backoff Algorithm along with an exponential back off timer. More information on both can be found here: https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/

## Stop
Invoking `Stop()` on the Secure Tunnel Client breaks the current connection (if any) and moves the client into an idle state. A Secure Tunnel Client that has been stopped will no longer attempt to reconnect and is ready to be cleaned up.
```cpp
if(!secureTunnel->Stop()){
    fprintf(stdout, "Failed to stop the Secure Tunnel connection session. Exiting..\n");
}
```
# Multiplexing and Simultaneous TCP Connections
You can use multiple data streams per Secure Tunnel by using the [Multiplexing and Simultaneous TCP Connections](https://docs.aws.amazon.com/iot/latest/developerguide/multiplexing.html) features.


## Opening a Secure Tunnel with Multiplexing
To use Multiplexing, a Secure Tunnel must be created with one to three "services". A Secure Tunnel can be opened through the AWS IoT console [Secure Tunnel Hub](https://console.aws.amazon.com/iot/home#/tunnelhub) or by using the [OpenTunnel API](https://docs.aws.amazon.com/iot/latest/apireference/API_Operations_AWS_IoT_Secure_Tunneling.html). Both of these methods allow you to add services with whichever names suit your needs.
## Service Ids Available on the Secure Tunnel
On a successfull connection to a Secure Tunnel, the Secure Tunnel Client will invoke the `OnConnectionSuccess` callback. This callback will return `ConnectionSuccessEventData` which contains any available Service Ids that can be used for multiplexing. Below is an example of how to set the callback using the Secure Tunnel Builder and check whether a Service Id is available.
```cpp
// Create Secure Tunnel Builder
SecureTunnelBuilder builder = SecureTunnelBuilder(...);

// Add a callback function to the builder for OnConnectionSuccess
builder.WithOnConnectionSuccess([&](SecureTunnel *secureTunnel, const ConnectionSuccessEventData &eventData) {
        //Check if a Service Id is available
        if (eventData.connectionData->getServiceId1().has_value())
        {
            // This Secure Tunnel IS using Service Ids and ServiceId1 is set.
            fprintf(
                stdout,
                "Secure Tunnel connected with  Service IDs '" PRInSTR "'",
                AWS_BYTE_CURSOR_PRI(eventData.connectionData->getServiceId1().value()));

            // Service Ids are set in the connectionData in order of availability. ServiceId2 will only be set if ServiceId1 is set.
            if (eventData.connectionData->getServiceId2().has_value())
            {
                fprintf(
                    stdout, ", '" PRInSTR "'", AWS_BYTE_CURSOR_PRI(eventData.connectionData->getServiceId2().value()));

                // ServiceId3 will only be set if ServiceId2 is set.
                if (eventData.connectionData->getServiceId3().has_value())
                {
                    fprintf(
                        stdout,
                        ", '" PRInSTR "'",
                        AWS_BYTE_CURSOR_PRI(eventData.connectionData->getServiceId3().value()));
                }
            }
            fprintf(stdout, "\n");
        }
        else
        {
            // This Secure Tunnel is NOT using Service Ids and Multiplexing
            fprintf(stdout, "Secure Tunnel is not using Service Ids.\n");
        }
    });
```
## Using Service Ids
Service Ids can be added to outbound Messages as shown below in the Send Message example. If the Service Id is both available on the current Secure Tunnel and there is an open stream with a Source device on that Service Id, the message will be sent. If the Service Id does not exist on the current Secure Tunnel or there is no currently active stream available on that Service Id, the Message will not be sent and a Warning will be logged. The `OnStreamStarted` callback is invoked when a stream is started and it returns a `StreamStartedEventData` which can be parsed to determine if a stream was started using a Service Id for Multiplexing. Incoming messages can also be parsed to determine if a Service Id has been set as shown above in the [Setting Secure Tunnel Callbacks](#setting-secure-tunnel-callbacks) code example.
## Using Connection Ids
Connection Ids can be added to outbound Messages as shown below in the Send Message example. If there is an active stream currently open using the combination of the Service Id and Connection Id, the message will be sent. If a Connection Id is not set on an outbound message, a Connecion Id of 1 is assumed and applied to the Message. When additional streams are activated, the `OnConnectionStarted` callback is invoked and returns a `ConnectionStartedEventData` which can be parsed to determine the Connection Id of the newly activated stream. A Connection Id will also be present in the `StreamStartedEventData` that is returned when the `OnStreamStarted` callback is invoked.

# Send Message

The `SendMessage()` operation takes a description of the Message you wish to send and enques the message for sending it asynchronously to the destination. The call returns a success/failure in the synchronous logic that kicks off the asynchronous operation. However, since the main validation actions are performed asynchronously, the user code should setup the `OnSendMessageComplete` callback for catching errors.

```cpp
builder.WithOnSendMessageComplete(
    [&](SecureTunnel *secureTunnel, int errorCode, const SendMessageCompleteEventData &eventData) {
        if (errorCode)
        {
            fprintf(stdout, "Send Message failed with error code %d(%s)\n", errorCode, ErrorDebugString(errorCode));
        }
    });

Crt::String serviceId_string = "ssh";
Crt::String message_string = "any payload";

ByteCursor serviceId = ByteCursorFromString(serviceId_string);
uint32_t connectionId = 1
ByteCursor payload = ByteCursorFromString(message_string);

// Create Message
std::shared_ptr<Message> message = std::make_shared<Message>();
// Add a Service Id
message->withServiceId(serviceId);
// Add a Connection Id
message->withConnectionId(connectionId);
// Add a payload
message->withPayload(payload);

// Send Message
secureTunnel->SendMessage(message);
```

# Secure Tunnel Best Practices
* You MUST NOT perform blocking operations on any callback, or you will cause a deadlock.
* If you do not provide a Client Token during creation of the Secure Tunnel, one will be automatically generated for you to use in reconnections. This token is not saved outside of the current Secure Tunnel Client. If the Client is destroyed, the original access tokens must be rotated to connect to the secure tunnel again. Information on rotating tokens can be found here: https://docs.aws.amazon.com/iot/latest/developerguide/iot-secure-tunneling-troubleshooting.html
* Client tokens MUST be unique. You cannot for example, pair a Client Token with an Access Token on one secure tunnel, and then use the same Client Token with a different Access Token on a separate secure tunnel. The Secure Tunnel Service will not allow a Client Token to be paired with more than one Access Token.
* A Secure Tunnel Client that has called `Start()` will continue to attempt to connect the Secure Tunnel Service until `Stop()` is called, even if the Secure Tunnel it is trying to connect with has been closed. You MUST call `Stop()` to cease future connection attempts.
* The [onStreamStarted](#onstreamstarted) and [onConnectionStarted](#onconnectionstarted) callbacks should be set to detect and store the service id and/or connection id of streams started by a source device for use with messages. The [Secure Tunnel sample](../samples/secure_tunneling/secure_tunnel/README.md) provides an basic example of how this can be done.
* Outgoing messages MUST be assigned a service id and/or a connection id if the established stream contains a service id or a connection id or the message will be rejected. e.g. If a stream is started using service id "ssh" and connection id (1), a message sent in response must also include the service id "ssh" and connection id (1) or it will not find an active stream to send it on. Refer to the [Send Message](#send-message) code block for instruction on adding a service id and/or connection id to your message.


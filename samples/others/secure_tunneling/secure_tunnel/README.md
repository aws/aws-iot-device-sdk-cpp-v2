# Secure Tunnel

[**Return to main sample list**](../../README.md)

This sample uses AWS IoT [Secure Tunneling](https://docs.aws.amazon.com/iot/latest/developerguide/secure-tunneling.html) Service to connect a destination or a source Secure Tunnel Client to an AWS Secure Tunnel endpoint using access tokens using the [V3WebSocketProtocol](https://github.com/aws-samples/aws-iot-securetunneling-localproxy/blob/main/V3WebSocketProtocolGuide.md). For more information, see the [Secure Tunnel Userguide](../../../documents/Secure_Tunnel_Userguide.md)

## How to run

Create a new secure tunnel in the AWS IoT console (https://console.aws.amazon.com/iot/) (AWS IoT/Manage/Tunnels/Create tunnel) and retrieve the destination and source access tokens. (https://docs.aws.amazon.com/iot/latest/developerguide/secure-tunneling-tutorial-open-tunnel.html). Once you have these tokens, you are ready to open a secure tunnel.

### Destination Mode

To run the sample with a destination access token in destination mode (default), you can use the following command:

``` sh
./secure-tunnel --signing_region <signing_region> --access_token_file <path to destination access token>
```

The sample will create a Secure Tunnel connection and remain connected in `DESTINATION MODE` and will echo any messages it receives through the Secure Tunnel back to the Source Device.

### Source Mode

While the focus of the Secure Tunnel Client for the IoT Device SDK is to connect with Secure Tunnels in `DESTINATION MODE` we also support connecting in `SOURCE MODE`. The token file should be the Source Token in this instance and you must add the `--local_proxy_mode_source` flag:

``` sh
./secure-tunnel --signing_region <signing_region> --access_token_file <path to source access token> --local_proxy_mode_source
```

Then two samples will connect to each other through the AWS Secure Tunnel endpoint and establish a stream through which data can be transmitted in either direction.
The sample will create a Secure Tunnel connection in `SOURCE MODE` and will open a stream using an available `Service Id`. It will then send n messages on the opened stream. It will then create a new simultaneous TCP connection on the stream and send an additional n messages on the new TCP connection. It will then exit.

### Proxy

Note that a proxy server may be used via the `--proxy_host` and `--proxy_port` argument. If the proxy server requires a user name and password to connect,  you can use `--proxy_user_name` and `--proxy_password` to in the sample to pass the required data to the sample.

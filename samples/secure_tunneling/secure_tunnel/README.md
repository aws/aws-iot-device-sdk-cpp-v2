## Secure Tunnel

[**Return to main sample list**](../../README.md)

This sample uses AWS IoT [Secure Tunneling](https://docs.aws.amazon.com/iot/latest/developerguide/secure-tunneling.html) Service to connect a destination and a source with each other through the AWS Secure Tunnel endpoint using access tokens using the [V2WebSocketProtocol](https://github.com/aws-samples/aws-iot-securetunneling-localproxy/blob/main/V2WebSocketProtocolGuide.md). For more information, see the [Secure Tunnel Userguide](../../../documents/Secure_Tunnel_Userguide.md)

Your IoT Core Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect, subscribe, publish, and receive. Below is a sample policy that can be used on your IoT Core Thing that will allow this sample to run as intended.

<details>
<summary>(see sample policy)</summary>
<pre>
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": [
        "iot:Publish",
        "iot:Receive"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<thing_name>/tunnels/notify"
      ]
    },
    {
      "Effect": "Allow",
      "Action": [
        "iot:Subscribe"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<thing_name>/tunnels/notify"
      ]
    },
    {
      "Effect": "Allow",
      "Action": [
        "iot:Connect"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:client/test-*"
      ]
    }
  ]
}
</pre>

Replace with the following with the data from your AWS account:
* `<region>`: The AWS IoT Core region where you created your AWS IoT Core thing you wish to use with this sample. For example `us-east-1`.
* `<account>`: Your AWS IoT Core account ID. This is the set of numbers in the top right next to your AWS account name when using the AWS IoT Core website.
* `<thingname>`: The name of your AWS IoT Core thing you want the device connection to be associated with

Note that in a real application, you may want to avoid the use of wildcards in your ClientID or use them selectively. Please follow best practices when working with AWS on production applications using the SDK. Also, for the purposes of this sample, please make sure your policy allows a client ID of `test-*` to connect or use `--client_id <client ID here>` to send the client ID your policy supports.

## How to run

Create a new secure tunnel in the AWS IoT console (https://console.aws.amazon.com/iot/) (AWS IoT/Manage/Tunnels/Create tunnel) and retrieve the destination and source access tokens. (https://docs.aws.amazon.com/iot/latest/developerguide/secure-tunneling-tutorial-open-tunnel.html). Once you have these tokens, you are ready to open a secure tunnel.

To run the sample with a destination access token in destination mode (default), you can use the following command:

``` sh
./secure_tunnel --region <region> --access_token_file <path to destination access token>
```

However, for this sample to work, it will also need another instance of the sample running in source mode. You can run another instance of the sample in source mode using the same command, but adding the `--localProxyModeSource` flag:

``` sh
./secure_tunnel --region <region> --access_token_file <path to source access token> --localProxyModeSource
```

Then two samples will then connect to each other through the AWS Secure Tunnel endpoint and establish a stream through which data can be transmitted in either direction.

Note that a proxy server may be used via the `--proxy_host` and `--proxy_port` argument. If the proxy server requires a user name and password to connect,  you can use `--proxy_user_name` and `--proxy_password` to in the sample to pass the required data to the sample.

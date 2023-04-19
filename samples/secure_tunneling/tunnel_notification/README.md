# Secure Tunnel Notification

[**Return to main sample list**](../../README.md)

This sample uses an MQTT Client and the AWS IoT [Secure Tunneling](https://docs.aws.amazon.com/iot/latest/developerguide/secure-tunneling.html) Service to receive a tunnel notification and then connects to the Secure Tunnel using a Secure Tunnel Client.

This sample requires you to create a Secure Tunnel for your thing. See [instructions here](https://docs.aws.amazon.com/iot/latest/developerguide/secure-tunneling-tutorial.html) on how to create a tunnel. You can also read more about secure tunneling in the [Secure Tunnel Userguide](../../../documents/Secure_Tunnel_Userguide.md).

This sample requires a certificate and key file using Mutual TLS (mTLS). On startup, the device connects to the server using the certificate and key files and subscribes to a specific topic generated along with the Thing Name to listen for any Secure Tunnels that are opened for that specific Thing. When a Secure Tunnel is created for the Thing, it will receive a publish with the Secure Tunnel information which it can then use to create a Secure Tunnel Client and connect to the Secure Tunnel Service in `DESTINATION MODE`. AWS IoT will publish the notification upon creation of a new tunnel and will retransmit a new notification if instructed to "Generate new access tokens".

Your IoT Core Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect, subscribe, and receive. Below is a sample policy that can be used on your IoT Core Thing that will allow this sample to run as intended.

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
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thing_name</b>/tunnels/notify"
      ]
    },
    {
      "Effect": "Allow",
      "Action": [
        "iot:Subscribe"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thing_name</b>/tunnels/notify"
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

</details>

## How to run
To Run this sample, use the following command:
``` sh
./tunnel-notification --endpoint <endpoint> --cert <path to the certificate> --key <path to the private key> --thing_name <thing name>
```
Once the MQTT Client is connected, create a new secure tunnel in the AWS IoT console (https://console.aws.amazon.com/iot/) (AWS IoT/Manage/Tunnels/Create tunnel) for the Thing.

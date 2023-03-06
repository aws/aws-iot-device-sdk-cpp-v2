# Secure Tunnel Notification

[**Return to main sample list**](../../README.md)

This sample uses the AWS IoT [Secure Tunneling](https://docs.aws.amazon.com/iot/latest/developerguide/secure-tunneling.html) Service to receive a tunnel notification.

This sample requires you to create a tunnel for your thing. See [instructions here](https://docs.aws.amazon.com/iot/latest/developerguide/secure-tunneling-tutorial.html) on how to create a tunnel. You can also read more about secure tunneling in the [Secure Tunnel Userguide](../../../documents/Secure_Tunnel_Userguide.md).

On startup, the sample will wait until it receives, and then displays the tunnel notification.

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

Create a new secure tunnel in the AWS IoT console (https://console.aws.amazon.com/iot/) (AWS IoT/Manage/Tunnels/Create tunnel). Once you have the tunnel open, you can run the sample with the following command:

``` sh
./tunnel-notification --endpoint <endpoint> --cert <path to the certificate> --key <path to the private key> --thing_name <thing name>
```

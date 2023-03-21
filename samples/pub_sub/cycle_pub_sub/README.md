# Cycle Pub-Sub

[**Return to main sample list**](../../README.md)

This sample will create a series of clients, and then randomly perform operations on them (*connect, disconnect, publish, subscribe, etc.*) every few seconds. Status updates on what each client in the sample are doing are continually printed to the console and the sample will run until the time duration set is complete.

This sample is primarily for testing stability over a period of time for testing and CI, but it also shows how to randomly perform operations across multiple clients.

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
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/test/topic"
      ]
    },
    {
      "Effect": "Allow",
      "Action": [
        "iot:Subscribe"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/test/topic"
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

Note that in a real application, you may want to avoid the use of wildcards in your ClientID or use them selectively. Please follow best practices when working with AWS on production applications using the SDK. Also, for the purposes of this sample, please make sure your policy allows a client ID of `test-*` to connect or use `--client_id <client ID here>` to send the client ID your policy supports.


## How to run

To Run this sample, use the following command:

``` sh
./cycle-pub-sub --endpoint <endpoint> --cert <path to the certificate> --key <path to the private key> --topic <topic name>
```

You can also pass a Certificate Authority file (CA) if your certificate and key combination requires it:

``` sh
./cycle-pub-sub --endpoint <endpoint> --cert <path to the certificate> --key <path to the private key> --topic <topic name> --ca_file <path to CA file>
```

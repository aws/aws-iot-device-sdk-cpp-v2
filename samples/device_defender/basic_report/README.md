# Device Defender

[**Return to main sample list**](../../README.md)

This sample uses the AWS IoT [Device Defender](https://aws.amazon.com/iot-device-defender/) Service to send on device metrics to AWS. Device Defender is an AWS IoT Core service that allows you to monitor the health of your IoT device through sending periodic updates containing device data to AWS IoT Core.

**Note**: Device Defender is **only supported on Unix (Linux).**

On startup, the sample will make a MQTT connection and a Device Defender task to send metrics every minute or at the time interval passed as a CLI argument. This sample shows how to send custom metrics in addition to the standard metrics that are always sent with Device Defender.

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
        "iot:Subscribe",
        "iot:RetainPublish"
      ],
      "Resource": "arn:aws:iot:<b>region</b>:<b>account</b>:*/$aws/things/*/defender/metrics/*"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Connect",
      "Resource": "arn:aws:iot:<b>region</b>:<b>account</b>:client-*"
    }
  ]
}
</pre>

Replace with the following with the data from your AWS account:
* `<region>`: The AWS IoT Core region where you created your AWS IoT Core thing you wish to use with this sample. For example `us-east-1`.
* `<account>`: Your AWS IoT Core account ID. This is the set of numbers in the top right next to your AWS account name when using the AWS IoT Core website.

Note that in a real application, you may want to avoid the use of wildcards in your ClientID or use them selectively. Please follow best practices when working with AWS on production applications using the SDK. Also, for the purposes of this sample, please make sure your policy allows a client ID of `test-*` to connect or use `--client_id <client ID here>` to send the client ID your policy supports.

</details>

## How to run

You will need to create a **Security Profile** to see the metric results in the AWS console. You can create a Security Profile by going to `Detect -> Security Profiles` from the AWS IOT Console. To see the custom metrics, you will need to add them in `Detect -> Metrics` and then press the `Create` button to make a new custom metric.

This sample expects and requires the following custom metrics:
* `CustomNumber`
  * type: `number`
  * info: always sends the number `10`.
* `CustomNumberTwo`
  * type `number`
  * info: sends a random number from `-50` to `50`.
* `CustomNumberList`
  * type `number-list`
  * info: sends a predefined list of numbers.
* `CustomStringList`
  * type `string-list`
  * info: sends a predefined list of strings.
* `CustomIPList`
  * type `ip-list`
  * info: sends a predefined list of documentation IP addresses.

To run the Device Defender sample, use the following command:

``` sh
./basic-report --endpoint <endpoint> --cert <path to the certificate> --key <path to the private key> --thing_name <thing name> --ca_file <path to root CA>
```

You can also pass a Certificate Authority file (CA) if your certificate and key combination requires it:

``` sh
./basic-report --endpoint <endpoint> --cert <path to the certificate> --key <path to the private key> --thing_name <thing name> --ca_file <path to root CA>
```

### Device Defender Data Requirements

The formatting and data requirements for custom metrics in Device Defender are listed below. If you input data that does not follow the requirements, it will be sent in the JSON packet but Device Defender will not show/retain the data! If your data is not showing up in Device Defender, please double check that your data fits into the requirements below.

* `number` and `number-list` types: Supports precision up to 15 digits. Supports both positive and negative values.
* `string-list` type: Supports letters A through Z (uppercase and lowercase) and the characters `:`, `_`, `-`, `/`, and `.
* `ip-list` type: Supports only valid IPV4 and IPV6 addresses.

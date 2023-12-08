# Websocket Connect with Custom Authentication

[**Return to main sample list**](../../README.md)

This sample makes an MQTT connection and connects through a [Custom Authorizer](https://docs.aws.amazon.com/iot/latest/developerguide/custom-authentication.html).
On startup, the device connects to the server and then disconnects.
This sample is for reference on connecting using a Custom Authorizer.
Using a Custom Authorizer allows you to perform your own authorization using an AWS Lambda function.
See [Custom Authorizer](https://docs.aws.amazon.com/iot/latest/developerguide/custom-authentication.html) for more information.
You will need to setup your Custom Authorizer so that the lambda function returns a policy document.
See [this page on the documentation](https://docs.aws.amazon.com/iot/latest/developerguide/config-custom-auth.html) for more details and example return result.
You can customize this lambda function as needed for your application to provide your own security measures based on the needs of your application.
Your IoT Core Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect.
Below is a sample policy that can be used on your IoT Core Thing that will allow this sample to run as intended.

If you want to use simple or custom auth (or static creds, or basic auth, etc) instead,
then you will need to replace part of the sample (connection\_setup function) with a code snippet we provided in its corresponding readme.

Your IoT Core Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect. Below is a sample policy that can be used on your IoT Core Thing that will allow this sample to run as intended.

<details>
<summary>(see sample policy)</summary>
<pre>
{
  "Version": "2012-10-17",
  "Statement": [
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

For this sample, using Websockets will attempt to connect using custom auth.

</details>

<details>
<summary> (code snipet to replace similar section)</summary>
<pre language="cpp">
<code>
void connection_setup(int argc, char \*argv[], ApiHandle &apiHandle, Utils::cmdData &cmdData,
    Aws::Iot::MqttClientConnectionConfigBuilder &clientConfigBuilder)
{
  cmdData = Utils::parseSampleInputCustomAuthorizerConnect(argc, argv, &apiHandle);
  // Create the MQTT builder and populate it with data from cmdData.
  Aws::Crt::Auth::CredentialsProviderChainDefaultConfig defaultConfig;
  std::shared_ptr<Aws::Crt::Auth::ICredentialsProvider> provider =
      Aws::Crt::Auth::CredentialsProvider::CreateCredentialsProviderChainDefault(defaultConfig);
  Aws::Iot::WebsocketConfig websocketConfig((cmdData.input_signingRegion), provider);
  clientConfigBuilder = Aws::Iot::MqttClientConnectionConfigBuilder(websocketConfig);
  clientConfigBuilder.WithEndpoint((cmdData.input_endpoint));
  clientConfigBuilder.WithCustomAuthorizer(
      (cmdData.input_customAuthUsername),
      (cmdData.input_customAuthorizerName),
      (cmdData.input_customAuthorizerSignature),
      (cmdData.input_customAuthPassword),
      (cmdData.input_customTokenKeyName),
      (cmdData.input_customTokenValue));
}

</code>
</pre>
</details>

## How to run

Options for custom auth
```
--custom_auth_username <str>
--custom_auth_authorizer_name <str>
--custom_auth_authorizer_signature <str>
--custom_auth_password <str>
--custom_auth_token_name <str>
--custom_auth_token_value <str>
```

To run the websocket connect use the following command:

```
./websocket-connect --endpoint <endpoint> --signing_region <signing region> --custom_auth_username <str> --custom_auth_authorizer_name <str> --custom_auth_authorizer_signature <str> --custom_auth_password <str> --custom_auth_token_name <str> --custom_auth_token_value <str>
```


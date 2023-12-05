# Websocket Connect with Proxy

[**Return to main sample list**](../../README.md)

This sample makes an MQTT connection via Websockets and then disconnects.
On startup, the device connects to the server via Websockets and then disconnects right after.
This sample is for reference on connecting via Websockets using a proxy.
This sample demonstrates conneting via Websockets using a proxy serverb.

If you want to use simple or custom auth (or static creds, or basic auth, etc) instead,
then you will need to replace part of the sample (connection\_setup function) with a code snippet we provided in its corresponding readme.

* [Websocket Connect/simple_auth](./README.md)
* [Websocket Connect/custom auth](./README_custom_auth.md)
* [Websocket Connect/username and password](./README_username_password.md)
* [Websocket Connect/static credentials](./README_static_credentials.md)

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

For this sample, using Websockets will attempt to fetch the AWS credentials to authorize the connection from a proxy server.

</details>

<details>
<summary> (code snipet to replace the connection_setup function)</summary>
<pre language="c++"> <code>
void connection_setup(int argc, char *argv[], ApiHandle &apiHandle, Utils::cmdData &cmdData,
             Aws::Iot::MqttClientConnectionConfigBuilder &clientConfigBuilder)
{
    Utils::cmdData cmdData = Utils::parseSampleInputWebsocketConnect(argc, argv, &apiHandle);

    std::shared_ptr<Aws::Crt::Auth::ICredentialsProvider> provider = nullptr;
    Aws::Crt::Auth::CredentialsProviderChainDefaultConfig defaultConfig;
    provider = Aws::Crt::Auth::CredentialsProvider::CreateCredentialsProviderChainDefault(defaultConfig);
    if (!provider)
    {
        fprintf(stderr, "Failure to create credentials provider!\n");
        exit(-1);
    }
    Aws::Iot::WebsocketConfig config(cmdData.input_signingRegion, provider);
    clientConfigBuilder = Aws::Iot::MqttClientConnectionConfigBuilder(config);
    if (cmdData.input_ca != "")
    {
        clientConfigBuilder.WithCertificateAuthority(cmdData.input_ca.c_str());
    }
    if (cmdData.input_proxyHost == "")
    {
        fprintf(stderr, "proxy address missing!\n");
        exit(-1);
    }
    Aws::Crt::Http::HttpClientConnectionProxyOptions proxyOptions;
    proxyOptions.HostName = cmdData.input_proxyHost;
    proxyOptions.Port = static_cast<uint16_t>(cmdData.input_proxyPort);
    proxyOptions.AuthType = Aws::Crt::Http::AwsHttpProxyAuthenticationType::None;
    clientConfigBuilder.WithHttpProxyOptions(proxyOptions);

    if (cmdData.input_port != 0)
    {
        clientConfigBuilder.WithPortOverride(static_cast<uint16_t>(cmdData.input_port));
    }
    clientConfigBuilder.WithEndpoint(cmdData.input_endpoint);
}
</code>
</pre>
</details>

## How to run

Options for connecting through an HTTP proxy
```
--proxy_host <str>
--proxy_port <int>
```

To run the websocket connect use the following command:

``` sh
./websocket-connect --endpoint <endpoint> --signing_region <signing region> --proxy_host <str> --proxy_port <int>
```





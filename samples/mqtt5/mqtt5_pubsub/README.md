# MQTT5 PubSub

[**Return to main sample list**](../../README.md)

This sample uses the
[Message Broker](https://docs.aws.amazon.com/iot/latest/developerguide/iot-message-broker.html)
for AWS IoT to send and receive messages through an MQTT connection using MQTT5.

MQTT5 introduces additional features and enhancements that improve the development experience with MQTT. You can read more about MQTT5 in the C++ V2 SDK by checking out the [MQTT5 user guide](../../../documents/MQTT5_Userguide.md).

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

</details>

## How to run

To Run this sample using a direct MQTT connection with a key and certificate, use the following command:

``` sh
./mqtt5_pubsub  --endpoint <endpoint> --cert <path to the certificate> --key <path to the private key> --topic <topic name>
```

You can also pass a Certificate Authority file (CA) if your certificate and key combination requires it:

``` sh
./mqtt5_pubsub  --endpoint <endpoint> --cert <path to the certificate> --key <path to the private key> --topic <topic name> --ca_file <path to root CA>
```
## Alternate Authentication Methods supported by AWS IoT Core
* [Direct MQTT with X509-based mutual TLS](#direct-mqtt-with-x509-based-mutual-tls)
* [MQTT over Websockets with Sigv4 authentication](#mqtt-over-websockets-with-sigv4-authentication)
* [Direct MQTT with Custom Authentication](#direct-mqtt-with-custom-authentication)
* [MQTT over Websockets with Cognito](#mqtt-over-websockets-with-cognito)

### Direct MQTT with X509-based mutual TLS

For X509 based mutual TLS, you can create a client where the certificate and private key are configured by path:

```cpp
    // Create a Client using Mqtt5ClientBuilder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithMtlsFromPath(
        "<clientEndpoint>", "<certificateFilePath>", "<privateKeyFilePath>");

    /* You can setup other client options and lifecycle event callbacks before call builder->Build().
    ** Once the the client get built, you could no longer update the client options or connection options
    ** on the created client.
    */

    // Build Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5Client> client = builder->Build();

    if (client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }

    // start use the mqtt5 client

```



### MQTT over Websockets with Sigv4 authentication

Sigv4-based authentication requires a credentials provider capable of sourcing valid AWS credentials. Sourced credentials will sign the websocket upgrade request made by the client while connecting. The default credentials provider chain supported by the SDK is capable of resolving credentials in a variety of environments according to a chain of priorities:


```Environment -> Profile (local file system) -> STS Web Identity -> IMDS (ec2) or ECS```



If the default credentials provider chain and AWS region are specified, you do not need to specify any additional configuration, Alternatively, if you're connecting to a special region for which standard pattern matching does not work, or if you need a specific credentials provider, you can specify advanced websocket configuration options.


```cpp
    // Create websocket configuration
    Aws::Crt::Auth::CredentialsProviderChainDefaultConfig defaultConfig;
    std::shared_ptr<Aws::Crt::Auth::ICredentialsProvider> provider = Aws::Crt::Auth::CredentialsProvider::CreateCredentialsProviderChainDefault(defaultConfig);
    if (!provider)
    {
        fprintf(stderr, "Failure to create credentials provider!\n");
        exit(-1);
    }
    Aws::Iot::WebsocketConfig websocketConfig(<signing region>, provider);

    // Create a Client using Mqtt5ClientBuilder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithWebsocket(
        "<clientEndpoint>", websocketConfig);

    /* You can setup other client options and lifecycle event callbacks before call builder->Build().
    ** Once the the client get built, you could no longer update the client options or connection options
    ** on the created client.
    */

    // Build Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5Client> mqtt5Client = builder->Build();

    if (mqtt5Client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }


```


### Direct MQTT with Custom Authentication

AWS IoT Core Custom Authentication allows you to use a lambda to gate access to IoT Core resources.  For this authentication method,you must supply an additional configuration structure containing fields relevant to AWS IoT Core Custom Authentication.



If your custom authenticator does not use signing, you don't specify anything related to the token signature:

```cpp
    // Setup custom authorization config
    Mqtt5CustomAuthConfig customAuth;
    customAuth.WithAuthrizaerName("<Name of your custom authorizer>");
    customAuth.WithUsername("<Value of the username field that should be passed to the authorizer's lambda>");
    customAuth.WithPassword(<Binary data value of the password field to be passed to the authorizer lambda>);

    // Create a Client using Mqtt5ClientBuilder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithCustomCustomAuthorizer(
        "<clientEndpoint>", customAuth);

    /* You can setup other client options and lifecycle event callbacks before call builder->Build().
    ** Once the the client get built, you could no longer update the client options or connection options
    ** on the created client.
    */

    // Build Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5Client> mqtt5Client = builder->Build();

    if (mqtt5Client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }

```


If your custom authorizer uses signing, you must specify the three signed token properties as well.  The token signature must be the URI-encoding of the base64 encoding of the digital signature of the token value via the private key associated with the public key that was registered with the custom authorizer.  It is your responsibility to URI-encode the token signature.

```cpp
    // Setup custom authorization config
    Mqtt5CustomAuthConfig customAuth;
    customAuth.WithAuthrizaerName("<Name of your custom authorizer>");
    customAuth.WithUsername("<Value of the username field that should be passed to the authorizer's lambda>");
    customAuth.WithPassword(<Binary data value of the password field to be passed to the authorizer lambda>);
    customAuth.WithTokenSignature("<The signature of the custom authorizer>")

    // Create a Client using Mqtt5ClientBuilder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithCustomCustomAuthorizer(
        "<clientEndpoint>", customAuth);

    /* You can setup other client options and lifecycle event callbacks before call builder->Build().
    ** Once the the client get built, you could no longer update the client options or connection options
    ** on the created client.
    */

    // Build Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5Client> mqtt5Client = builder->Build();

    if (mqtt5Client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }

```

In both cases, the builder will construct a final CONNECT packet username field value for you based on the values configured.  Do not add the token-signing fields to the value of the username that you assign within the custom authentication config structure.  Similarly, do not add any custom authentication related values to the username in the CONNECT configuration optionally attached to the client configuration. The builder will do everything for you.


### MQTT over Websockets with Cognito

A MQTT5 websocket connection can be made using Cognito to authenticate rather than the AWS credentials located on the device or via key and certificate. Instead, Cognito can authenticate the connection using a valid Cognito identity ID. This requires a valid Cognito identity ID, which can be retrieved from a Cognito identity pool. A Cognito identity pool can be created from the AWS console.

To create a MQTT5 builder configured for this connection, see the following code:

```cpp
    // Create websocket configuration
    Aws::Crt::Auth::CredentialsProviderChainDefaultConfig defaultConfig;
    std::shared_ptr<Aws::Crt::Auth::ICredentialsProvider> provider = Aws::Crt::Auth::CredentialsProvider::CreateCredentialsProviderChainDefault(defaultConfig);

    Aws::Crt::Auth::CredentialsProviderCognitoConfig cognitoConfig;
    // See https://docs.aws.amazon.com/general/latest/gr/cognito_identity.html for Cognito endpoints
    cognitoConfig.Endpoint = "cognito-identity.<region>.amazonaws.com";
    cognitoConfig.Identity = "<Cognito Identity ID>";
    Aws::Crt::Io::TlsContextOptions tlsCtxOptions = Aws::Crt::Io::TlsContextOptions::InitDefaultClient();
    cognitoConfig.TlsCtx = Aws::Crt::Io::TlsContext(tlsCtxOptions, Aws::Crt::Io::TlsMode::CLIENT);
    std::shared_ptr<Aws::Crt::Auth::ICredentialsProvider> provider = Aws::Crt::Auth::CredentialsProvider::CreateCredentialsProviderCognito(cognitoConfig);

    if (!provider)
    {
        fprintf(stderr, "Failure to create credentials provider!\n");
        exit(-1);
    }
    Aws::Iot::WebsocketConfig websocketConfig(<signing region>, provider);

    // Create a Client using Mqtt5ClientBuilder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithWebsocket(
        "<clientEndpoint>", websocketConfig);

    /* You can setup other client options and lifecycle event callbacks before call builder->Build().
    ** Once the the client get built, you could no longer update the client options or connection options
    ** on the created client.
    */

    // Build Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5Client> mqtt5Client = builder->Build();

    if (mqtt5Client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }


```

**Note**: A Cognito identity ID is different from a Cognito identity pool ID and trying to connect with a Cognito identity pool ID will not work. If you are unable to connect, make sure you are passing a Cognito identity ID rather than a Cognito identity pool ID.


### HTTP Proxy

No matter what your connection transport or authentication method is, you may connect through an HTTP proxy by applying proxy configuration to the builder:

```cpp
    // Create a Client using Mqtt5ClientBuilder
    Aws::Iot::Mqtt5ClientBuilder *builder = Aws::Iot::Mqtt5ClientBuilder::NewMqtt5ClientBuilderWithXXXXX( ... );

    Http::HttpClientConnectionProxyOptions proxyOptions;
    proxyOptions.HostName = "<proxyHost>";
    proxyOptions.Port = <proxyPort>;
    builder->WithHttpProxyOptions(proxyOptions);

    /* You can setup other client options and lifecycle event callbacks before call builder->Build().
    ** Once the the client get built, you could no longer update the client options or connection options
    ** on the created client.
    */

    // Build Mqtt5Client
    std::shared_ptr<Aws::Crt::Mqtt5Client> mqtt5Client = builder->Build();

    if (mqtt5Client == nullptr)
    {
        fprintf(stdout, "Client creation failed.\n");
        return -1;
    }

```

SDK Proxy support also includes support for basic authentication and TLS-to-proxy. SDK proxy support does not include any additional proxy authentication methods (kerberos, NTLM, etc...) nor does it include non-HTTP proxies (SOCKS5, for example).
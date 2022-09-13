# Sample apps for the AWS IoT Device SDK for C++ v2

* [Basic Pub-Sub](#basic-pub-sub)
* [Basic Connect](#basic-connect)
* [Websocket Connect](#websocket-connect)
* [PKCS#11 Connect](#pkcs11-connect)
* [Raw Connect](#raw-connect)
* [x509 Credentials Provider Connect](#x509-credentials-provider-connect)
* [Windows Certificate MQTT Connect](#windows-certificate-mqtt-connect)
* [Custom Authorizer Connect](#custom-authorizer-connect)
* [Fleet provisioning](#fleet-provisioning)
* [Secure Tunnel](#secure-tunnel)
* [Secure Tunnel Notification](#secure-tunnel-notification)
* [Shadow](#shadow)
* [Jobs](#jobs)
* [Greengrass discovery](#greengrass-discovery)
* [Greengrass IPC](#greengrass-ipc)
* [Device Defender](#device-defender)
* [Cycle Pub-Sub](#cycle-pub-sub)

## Build Instruction

Firstly, build and install aws-iot-devices-sdk-cpp-v2 with following instructions from [Installation](../README.md#Installation).

### Build samples

Change directory into one of the samples. Under the directory of the sample, run the following commands:

``` sh
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="<absolute path sdk-cpp-workspace dir>" -DCMAKE_BUILD_TYPE="<Release|RelWithDebInfo|Debug>" ..
cmake --build . --config "<Release|RelWithDebInfo|Debug>"
```

To view the commands for a given sample, run the compiled program and pass `--help`.

```
./basic-pub-sub --help
```

#### Note

* `-DCMAKE_PREFIX_PATH` needs to be set to the path aws-iot-device-sdk-cpp-v2 installed. Since [Installation](../README.md#Installation) takes sdk-cpp-workspace as an example, here takes that as an example too.

* `-DCMAKE_BUILD_TYPE` and `--config` needs to match the CMAKE_BUILD_TYPE when aws-iot-device-sdk-cpp-v2 built. `--config` is only REQUIRED for multi-configuration build tools.

## Basic Pub-Sub

This sample uses the
[Message Broker](https://docs.aws.amazon.com/iot/latest/developerguide/iot-message-broker.html)
for AWS IoT to send and receive messages through an MQTT connection.
On startup, the device connects to the server, subscribes to a topic, and begins publishing messages to that topic. The device should receive those same messages back from the message broker, since it is subscribed to that same topic. Status updates are continually printed to the console.

Source: `samples/pub_sub/basic_pub_sub/main.cpp`

Your Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect, subscribe, publish, and receive. Make sure your policy allows a client ID of `test-*` to connect or use `--client_id <client ID here>` to send the client ID your policy supports.

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
</details>

To run the basic MQTT Pub-Sub use the following command:

``` sh
./basic-pub-sub --endpoint <endpoint> --ca_file <path to root CA>
--cert <path to the certificate> --key <path to the private key>
--topic <topic name>
```

## Basic Connect

This sample makes an MQTT connection using a certificate and key file. On startup, the device connects to the server using the certificate and key files, and then disconnects. This sample is for reference on connecting via certificate and key files.

Source: `samples/mqtt/basic_connect/main.cpp`

Your Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect. Make sure your policy allows a client ID of `test-*` to connect or use `--client_id <client ID here>` to send the client ID your policy supports.

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
</details>

To run the basic connect sample use the following command:

``` sh
./basic-connect --endpoint <endpoint> --ca_file <path to root CA> --cert <path to the certificate> --key <path to the private key>
```

## Websocket Connect

This sample makes an MQTT connection via websockets and then disconnects. On startup, the device connects to the server via websockets and then disconnects. This sample is for reference on connecting via websockets.

Source: `samples/mqtt/websocket_connect/main.cpp`

Your Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect. Make sure your policy allows a client ID of `test-*` to connect or use `--client_id <client ID here>` to send the client ID your policy supports.

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
</details>

To run the websocket connect use the following command:

``` sh
./websocket-connect --endpoint <endpoint> --ca_file <path to root CA> --signing_region <signing region>
```

Note that using Websockets will attempt to fetch the AWS credentials from your enviornment variables or local files.
See the [authorizing direct AWS](https://docs.aws.amazon.com/iot/latest/developerguide/authorizing-direct-aws.html) page for documentation on how to get the AWS credentials, which then you can set to the `AWS_ACCESS_KEY_ID`, `AWS_SECRET_ACCESS`, and `AWS_SESSION_TOKEN` environment variables.

## PKCS#11 Connect

This sample is similar to the [Basic Connect](#basic-connect),
but the private key for mutual TLS is stored on a PKCS#11 compatible smart card or Hardware Security Module (HSM)

WARNING: Unix only. Currently, TLS integration with PKCS#11 is only available on Unix devices.

source: `samples/mqtt/pkcs11_connect/main.cpp`

To run this sample using [SoftHSM2](https://www.opendnssec.org/softhsm/) as the PKCS#11 device:

1)  Create an IoT Thing with a certificate and key if you haven't already.

2)  Convert the private key into PKCS#8 format
    ```sh
    openssl pkcs8 -topk8 -in <private.pem.key> -out <private.p8.key> -nocrypt
    ```

3)  Install [SoftHSM2](https://www.opendnssec.org/softhsm/):
    ```sh
    sudo apt install softhsm
    ```

    Check that it's working:
    ```sh
    softhsm2-util --show-slots
    ```

    If this spits out an error message, create a config file:
    *   Default location: `~/.config/softhsm2/softhsm2.conf`
    *   This file must specify a valid token directory:
        ```
        directories.tokendir = /path/for/my/softhsm/tokens/
        ```

4)  Create token and import private key.

    You can use any values for the labels, PINs, etc
    ```sh
    softhsm2-util --init-token --free --label <token-label> --pin <user-pin> --so-pin <so-pin>
    ```

    Note which slot the token ended up in

    ```sh
    softhsm2-util --import <private.p8.key> --slot <slot-with-token> --label <key-label> --id <hex-chars> --pin <user-pin>
    ```

5)  Now you can run the sample:
    ```sh
    ./pkcs11-connect --endpoint <endpoint> --ca_file <path to root CA> --cert <path to certificate> --pkcs11_lib <path to PKCS11 lib> --pin <user-pin> --token_label <token-label> --key_label <key-label>
    ```

Your Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect. Make sure your policy allows a client ID of `test-*` to connect or use `--client_id <client ID here>` to send the client ID your policy supports.

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
</details>

## Windows Certificate MQTT Connect

WARNING: Windows only

This sample is similar to the [Basic Connect](#basic-connect),
but your certificate and private key are in a
[Windows certificate store](https://docs.microsoft.com/en-us/windows-hardware/drivers/install/certificate-stores),
rather than simply being files on disk.

To run this sample you need the path to your certificate in the store,
which will look something like:
"CurrentUser\MY\A11F8A9B5DF5B98BA3508FBCA575D09570E0D2C6"
(where "CurrentUser\MY" is the store and "A11F8A9B5DF5B98BA3508FBCA575D09570E0D2C6" is the certificate's thumbprint)

If your certificate and private key are in a
[TPM](https://docs.microsoft.com/en-us/windows/security/information-protection/tpm/trusted-platform-module-overview),
you would use them by passing their certificate store path.

source: `samples/mqtt/windows_cert_connect/main.cpp`

Your Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect. Make sure your policy allows a client ID of `test-*` to connect or use `--client_id <client ID here>` to send the client ID your policy supports.

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
</details>

To run this sample with a basic certificate from AWS IoT Core:

1)  Create an IoT Thing with a certificate and key if you haven't already.

2)  Combine the certificate and private key into a single .pfx file.

    You will be prompted for a password while creating this file. Remember it for the next step.

    If you have OpenSSL installed:
    ```powershell
    openssl pkcs12 -in certificate.pem.crt -inkey private.pem.key -out certificate.pfx
    ```

    Otherwise use [CertUtil](https://docs.microsoft.com/en-us/windows-server/administration/windows-commands/certutil).
    ```powershell
    certutil -mergePFX certificate.pem.crt,private.pem.key certificate.pfx
    ```

3)  Add the .pfx file to a Windows certificate store using PowerShell's
    [Import-PfxCertificate](https://docs.microsoft.com/en-us/powershell/module/pki/import-pfxcertificate)

    In this example we're adding it to "CurrentUser\MY"

    ```powershell
    $mypwd = Get-Credential -UserName 'Enter password below' -Message 'Enter password below'
    Import-PfxCertificate -FilePath certificate.pfx -CertStoreLocation Cert:\CurrentUser\MY -Password $mypwd.Password
    ```

    Note the certificate thumbprint that is printed out:
    ```
    Thumbprint                                Subject
    ----------                                -------
    A11F8A9B5DF5B98BA3508FBCA575D09570E0D2C6  CN=AWS IoT Certificate
    ```

    So this certificate's path would be: "CurrentUser\MY\A11F8A9B5DF5B98BA3508FBCA575D09570E0D2C6"

4) Now you can run the sample:

    ```
    .\windows-cert-connect.exe --endpoint <endpoint> --ca_file <path to root CA> --cert <path to certificate>
    ```

## Raw Connect

This sample is similar to the [Basic Connect](#basic-connect), but the connection setup is more manual.
This is a starting point for using custom
[Configurable Endpoints](https://docs.aws.amazon.com/iot/latest/developerguide/iot-custom-endpoints-configurable.html) or [Custom Authentication](https://docs.aws.amazon.com/iot/latest/developerguide/custom-authentication.html).

source: `samples/mqtt/raw_connect/main.cpp`

To run the Raw MQTT Pub-Sub sample use the following command:

``` sh
./raw-connect --endpoint <endpoint> --ca_file <path to root CA>
--cert <path to the certificate> --key <path to the private key>
--user_name <user name to send on connect> --password <password to send on connect>
```

Your Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect. Make sure your policy allows a client ID of `test-*` to connect or use `--client_id <client ID here>` to send the client ID your policy supports.

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
</details>

## x509 Credentials Provider Connect

This sample is similar to the [Basic Pub-Sub](#basic-pub-sub), but the connection uses a X.509 certificate
to source the AWS credentials when connecting.

source: `samples/mqtt/x509_credentials_provider_connect/main.cpp`

To run the x509 Credentials Provider Connect sample use the following command:

``` sh
./x509-credentials-provider-connect --endpoint <endpoint> --ca_file <path to root CA>
--signing_region <signing region> --x509_ca_file <path to x509 CA>
--x509_cert <path to x509 cert> --x509_endpoint <x509 endpoint>
-- x509_key <path to x509 key> --x509_role_alias <alias> -x509_thing_name <thing name>
```

Your Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect. Make sure your policy allows a client ID of `test-*` to connect or use `--client_id <client ID here>` to send the client ID your policy supports.

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
</details>

## Custom Authorizer Connect

This sample makes an MQTT connection and connects through a [Custom Authorizer](https://docs.aws.amazon.com/iot/latest/developerguide/custom-authentication.html). On startup, the device connects to the server and then disconnects. This sample is for reference on connecting using a custom authorizer.

Your Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect. Make sure your policy allows a client ID of `test-*` to connect or use `--client_id <client ID here>` to send the client ID your policy supports.

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
</details>

To run the Custom Authorizer Connect sample use the following command:

``` sh
./custom-authorizer-connect --endpoint <endpoint> --ca_file <path to root CA> --custom_auth_authorizer_name <authorizer name>
```

You will need to setup your Custom Authorizer so that the lambda function returns a policy document. See [this page on the documentation](https://docs.aws.amazon.com/iot/latest/developerguide/config-custom-auth.html) for more details and example return result.


## Shadow

This sample uses the AWS IoT
[Device Shadow](https://docs.aws.amazon.com/iot/latest/developerguide/iot-device-shadows.html)
Service to keep a property in
sync between device and server. Imagine a light whose color may be changed
through an app, or set by a local user.

Once connected, type a value in the terminal and press Enter to update
the property's "reported" value. The sample also responds when the "desired"
value changes on the server. To observe this, edit the Shadow document in
the AWS Console and set a new "desired" value.

On startup, the sample requests the shadow document to learn the property's
initial state. The sample also subscribes to "delta" events from the server,
which are sent when a property's "desired" value differs from its "reported"
value. When the sample learns of a new desired value, that value is changed
on the device and an update is sent to the server with the new "reported"
value.

Source: `samples/shadow/shadow_sync/main.cpp`

Your Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect, subscribe, publish, and receive. Make sure your policy allows a client ID of `test-*` to connect or use `--client_id <client ID here>` to send the client ID your policy supports.

<details>
<summary>(see sample policy)</summary>
<pre>
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": [
        "iot:Publish"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/shadow/get",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/shadow/update"
      ]
    },
    {
      "Effect": "Allow",
      "Action": [
        "iot:Receive"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/shadow/get/accepted",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/shadow/get/rejected",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/shadow/update/accepted",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/shadow/update/rejected",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/shadow/update/delta"
      ]
    },
    {
      "Effect": "Allow",
      "Action": [
        "iot:Subscribe"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/shadow/get/accepted",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/shadow/get/rejected",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/shadow/update/accepted",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/shadow/update/rejected",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/shadow/update/delta"
      ]
    },
    {
      "Effect": "Allow",
      "Action": "iot:Connect",
      "Resource": "arn:aws:iot:<b>region</b>:<b>account</b>:client/test-*"
    }
  ]
}
</pre>
</details>

To run the Shadow sample use the following command:

``` sh
./shadow-sync --endpoint <endpoint> --ca_file <path to root CA>
--cert <path to the certificate> --key <path to the private key>
--thing_name <thing name> --shadow_property <shadow property name>
```

This will allow you to run the program and set the shadow property. To disconnect and exit the program, enter `quit`.

## Jobs

This sample uses the AWS IoT
[Jobs](https://docs.aws.amazon.com/iot/latest/developerguide/iot-jobs.html)
Service to describe jobs to execute.

This sample requires you to create jobs for your device to execute. See
[instructions here](https://docs.aws.amazon.com/iot/latest/developerguide/create-manage-jobs.html).

On startup, the sample describes a job that is pending execution.

Source: `samples/jobs/describe_job_execution/main.cpp`

Your Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect, subscribe, publish, and receive. Make sure your policy allows a client ID of `test-*` to connect or use `--client_id <client ID here>` to send the client ID your policy supports.

<details>
<summary>(see sample policy)</summary>
<pre>
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": "iot:Connect",
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:client/<b>thingname</b>",
        "arn:aws:iot:<b>region</b>:<b>account</b>:client/test-*"
      ]
    },
    {
      "Effect": "Allow",
      "Action": "iot:Publish",
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/test/dc/pubtopic",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/events/job/*",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/events/jobExecution/*",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/jobs/*"
      ]
    },
    {
      "Effect": "Allow",
      "Action": "iot:Subscribe",
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/test/dc/subtopic",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/events/jobExecution/*",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/jobs/*"
      ]
    },
    {
      "Effect": "Allow",
      "Action": "iot:Receive",
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/test/dc/subtopic",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/jobs/*"
      ]
    },
    {
      "Effect": "Allow",
      "Action": [
        "iot:DescribeJobExecution",
        "iot:GetPendingJobExecutions",
        "iot:StartNextPendingJobExecution",
        "iot:UpdateJobExecution"
      ],
      "Resource": "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>"
    }
  ]
}
</pre>

See the [Basic job policy example](https://docs.aws.amazon.com/iot/latest/developerguide/basic-jobs-example.html) page for another policy example.
</details>

To run the job sample use the following command:

``` sh
./describe-job-execution --endpoint <endpoint> --ca_file <path to root CA>
--cert <path to the certificate> --key <path to the private key>
--thing_name <thing name> --job_id <the job id>
```

Note that if you get a `Service Error 4 occurred` error, you may have incorrectly input the job id. The job id needs to exactly match the job id in the AWS console.

## Fleet provisioning

This sample uses the AWS IoT
[Fleet provisioning](https://docs.aws.amazon.com/iot/latest/developerguide/provision-wo-cert.html)
to provision devices using either a CSR or KeysAndcertificate and subsequently calls RegisterThing.

On startup, the script subscribes to topics based on the request type of either CSR or Keys topics,
publishes the request to corresponding topic and calls RegisterThing.

Source: `samples/identity/fleet_provisioning/main.cpp`

Run the sample like this to provision using CreateKeysAndCertificate:

``` sh
./fleet-provisioning --endpoint <endpoint> --ca_file <path to root CA>
--cert <path to the certificate> --key <path to the private key>
--template_name <template name> --template_parameters <template parameters json>
```

Run the sample like this to provision using Csr:

``` sh
./fleet-provisioning --endpoint <endpoint> --ca_file <path to root CA>
--cert <path to the certificate> --key <path to the private key>
--template_name <template name> --template_parameters <template parameters json> --csr <path to the CSR in PEM format>
```

Your Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect, subscribe, publish, and receive. Make sure your policy allows a client ID of `test-*` to connect or use `--client_id <client ID here>` to send the client ID your policy supports.

<details>
<summary>(see sample policy)</summary>
<pre>
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": [
        "iot:Publish"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/certificates/create/json",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/certificates/create-from-csr/json",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/provisioning-templates/<b>templatename</b>/provision/json"
      ]
    },
    {
      "Effect": "Allow",
      "Action": [
        "iot:Receive",
        "iot:Subscribe"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/certificates/create/json/accepted",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/certificates/create/json/rejected",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/certificates/create-from-csr/json/accepted",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/certificates/create-from-csr/json/rejected",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/provisioning-templates/<b>templatename</b>/provision/json/accepted",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/provisioning-templates/<b>templatename</b>/provision/json/rejected"
      ]
    },
    {
      "Effect": "Allow",
      "Action": "iot:Connect",
      "Resource": "arn:aws:iot:<b>region</b>:<b>account</b>:client/test-*"
    }
  ]
}
</pre>
</details>

### Fleet Provisioning Detailed Instructions

#### Aws Resource Setup

Fleet provisioning requires some additional AWS resources be set up first. This section documents the steps you need to take to
get the sample up and running. These steps assume you have the AWS CLI installed and the default user/credentials has
sufficient permission to perform all of the listed operations. You will also need python3 to be able to run parse_cert_set_result.py. These steps are based on provisioning setup steps
that can be found at [Embedded C SDK Setup](https://docs.aws.amazon.com/freertos/latest/lib-ref/c-sdk/provisioning/provisioning_tests.html#provisioning_system_tests_setup).

First, create the IAM role that will be needed by the fleet provisioning template. Replace `RoleName` with a name of the role you want to create.
``` sh
aws iam create-role \
    --role-name [RoleName] \
    --assume-role-policy-document '{"Version":"2012-10-17","Statement":[{"Action":"sts:AssumeRole","Effect":"Allow","Principal":{"Service":"iot.amazonaws.com"}}]}'
```
Next, attach a policy to the role created in the first step. Replace `RoleName` with the name of the role you created previously.
``` sh
aws iam attach-role-policy \
        --role-name [RoleName] \
        --policy-arn arn:aws:iam::aws:policy/service-role/AWSIoTThingsRegistration
```
Finally, create the template resource which will be used for provisioning by the demo application. This needs to be done only
once.  To create a template, the following AWS CLI command may be used. Replace `TemplateName` with the name of the fleet
provisioning template you want to create. Replace `RoleName` with the name of the role you created previously. Replace
`TemplateJSON` with the template body as a JSON string (containing escape characters). Replace `account` with your AWS
account number.
``` sh
aws iot create-provisioning-template \
        --template-name [TemplateName] \
        --provisioning-role-arn arn:aws:iam::[account]:role/[RoleName] \
        --template-body "[TemplateJSON]" \
        --enabled
```
The rest of the instructions assume you have used the following for the template body:
``` sh
{\"Parameters\":{\"DeviceLocation\":{\"Type\":\"String\"},\"AWS::IoT::Certificate::Id\":{\"Type\":\"String\"},\"SerialNumber\":{\"Type\":\"String\"}},\"Mappings\":{\"LocationTable\":{\"Seattle\":{\"LocationUrl\":\"https://example.aws\"}}},\"Resources\":{\"thing\":{\"Type\":\"AWS::IoT::Thing\",\"Properties\":{\"ThingName\":{\"Fn::Join\":[\"\",[\"ThingPrefix_\",{\"Ref\":\"SerialNumber\"}]]},\"AttributePayload\":{\"version\":\"v1\",\"serialNumber\":\"serialNumber\"}},\"OverrideSettings\":{\"AttributePayload\":\"MERGE\",\"ThingTypeName\":\"REPLACE\",\"ThingGroups\":\"DO_NOTHING\"}},\"certificate\":{\"Type\":\"AWS::IoT::Certificate\",\"Properties\":{\"CertificateId\":{\"Ref\":\"AWS::IoT::Certificate::Id\"},\"Status\":\"Active\"},\"OverrideSettings\":{\"Status\":\"REPLACE\"}},\"policy\":{\"Type\":\"AWS::IoT::Policy\",\"Properties\":{\"PolicyDocument\":{\"Version\":\"2012-10-17\",\"Statement\":[{\"Effect\":\"Allow\",\"Action\":[\"iot:Connect\",\"iot:Subscribe\",\"iot:Publish\",\"iot:Receive\"],\"Resource\":\"*\"}]}}}},\"DeviceConfiguration\":{\"FallbackUrl\":\"https://www.example.com/test-site\",\"LocationUrl\":{\"Fn::FindInMap\":[\"LocationTable\",{\"Ref\":\"DeviceLocation\"},\"LocationUrl\"]}}}
```
If you use a different body, you may need to pass in different template parameters.

#### Running the sample and provisioning using a certificate-key set from a provisioning claim

To run the provisioning sample, you'll need a certificate and key set with sufficient permissions. Provisioning certificates are normally
created ahead of time and placed on your device, but for this sample, we will just create them on the fly. You can also
use any certificate set you've already created if it has sufficient IoT permissions and in doing so, you can skip the step
that calls `create-provisioning-claim`.

We've included a script in the utils folder that creates certificate and key files from the response of calling
`create-provisioning-claim`. These dynamically sourced certificates are only valid for five minutes. When running the command,
you'll need to substitute the name of the template you previously created, and on Windows, replace the paths with something appropriate.

(Optional) Create a temporary provisioning claim certificate set. This command is executed in the debug folder(`aws-iot-device-sdk-cpp-v2-build\samples\identity\fleet_provisioning\Debug`):
``` sh
aws iot create-provisioning-claim \
        --template-name [TemplateName] \
        | python3 ../../../../../aws-iot-device-sdk-cpp-v2/utils/parse_cert_set_result.py \
        --path /tmp \
        --filename provision
```

The provisioning claim's cert and key set have been written to `/tmp/provision*`. Now you can use these temporary keys
to perform the actual provisioning. If you are not using the temporary provisioning certificate, replace the paths for `--cert`
and `--key` appropriately:

``` sh
./fleet-provisioning \
        --endpoint <endpoint> \
        --ca_file <path to CA file> \
        --cert <path to certificate> \
        --key <path to key> \
        --template_name <template name> \
        --template_parameters "{\"SerialNumber\":\"1\",\"DeviceLocation\":\"Seattle\"}"
```

Notice that we provided substitution values for the two parameters in the template body, `DeviceLocation` and `SerialNumber`.

#### Run the sample using the certificate signing request workflow

To run the sample with this workflow, you'll need to create a certificate signing request.

First create a certificate-key pair:
``` sh
openssl genrsa -out /tmp/deviceCert.key 2048
```

Next create a certificate signing request from it:
``` sh
openssl req -new -key /tmp/deviceCert.key -out /tmp/deviceCert.csr
```

(Optional) As with the previous workflow, we'll create a temporary certificate set from a provisioning claim. This step can
be skipped if you're using a certificate set capable of provisioning the device. This command is executed in the debug folder(`aws-iot-device-sdk-cpp-v2-build\samples\identity\fleet_provisioning\Debug`):

``` sh
aws iot create-provisioning-claim \
        --template-name [TemplateName] \
        | python3 ../../../../../aws-iot-device-sdk-cpp-v2/utils/parse_cert_set_result.py \
        --path /tmp \
        --filename provision
```

Finally, supply the certificate signing request while invoking the provisioning sample. As with the previous workflow, if
using a permanent certificate set, replace the paths specified in the `--cert` and `--key` arguments:
``` sh
./fleet-provisioning \
        --endpoint <endpoint> \
        --ca_file <path to root CA> \
        --cert <path to certificate> \
        --key <path to key> \
        --template_name <template name> \
        --template_parameters "{\"SerialNumber\":\"1\",\"DeviceLocation\":\"Seattle\"}" \
        --csr <path to csr file>
```

## Secure Tunnel

This sample uses AWS IoT [Secure Tunneling](https://docs.aws.amazon.com/iot/latest/developerguide/secure-tunneling.html) Service to connect a destination and a source with each other through the AWS Secure Tunnel endpoint using access tokens.

Source: `samples/secure_tunneling/secure_tunnel`

Create a new secure tunnel in the AWS IoT console (https://console.aws.amazon.com/iot/) (AWS IoT/Manage/Tunnels/Create tunnel) and retrieve the destination and source access tokens. (https://docs.aws.amazon.com/iot/latest/developerguide/secure-tunneling-tutorial-open-tunnel.html)

Provide the necessary arguments along with the destination access token and start the sample in destination mode (default).

``` sh
./secure_tunnel --endpoint <endpoint> --ca_file <path to root CA>
--cert <path to the certificate> --key <path to the private key>
--thing_name <thing name> --region <region> --access_token_file <path to destination access token> 
```

Provide the necessary arguments along with the source access token and start a second sample in source mode by using the flag --localProxyModeSource.

``` sh
./secure_tunnel --endpoint <endpoint> --ca_file <path to root CA>
--cert <path to the certificate> --key <path to the private key>
--thing_name <thing name> --region <region> --access_token_file <path to source access token> 
--localProxyModeSource
```

The two samples will then connect to each other through the AWS Secure Tunnel endpoint and establish a stream through which data can be trasmitted in either direction.

A proxy server may be used by providing the --proxy_host and --proxy_port arguments. If the proxy server requires a user name and password the --proxy_user_name and --proxy_password arguments should be used.

## Secure Tunnel Notification

This sample uses the AWS IoT [Secure Tunneling](https://docs.aws.amazon.com/iot/latest/developerguide/secure-tunneling.html) Service to receive a tunnel notification.

Source: `samples/secure_tunneling/tunnel_notification`

This sample requires you to create a tunnel for your thing. See [instructions here](https://docs.aws.amazon.com/iot/latest/developerguide/secure-tunneling-tutorial.html).

On startup, the sample will wait until it receives, and then displays the tunnel notification.

## Greengrass discovery

This sample is intended for use with the following tutorials in the AWS IoT Greengrass documentation:

* [Connect and test client devices](https://docs.aws.amazon.com/greengrass/v2/developerguide/client-devices-tutorial.html) (Greengrass V2)
* [Test client device communications](https://docs.aws.amazon.com/greengrass/v2/developerguide/test-client-device-communications.html) (Greengrass V2)
* [Getting Started with AWS IoT Greengrass](https://docs.aws.amazon.com/greengrass/latest/developerguide/gg-gs.html) (Greengrass V1)

## Greengrass IPC

This sample must be run from within a Greengrass V2 component using an appropriate policy to perform `aws.greengrass#PublishToIoTCore` and `aws.greengrass#SubscribeToIoTCore` operations on `test/topic` or any other topic passed as a CLI argument.

For more information about Greengrass interprocess communication (IPC), see [here](https://docs.aws.amazon.com/greengrass/v2/developerguide/interprocess-communication.html). This sample uses IPC to perform [IoT Core MQTT operations](https://docs.aws.amazon.com/greengrass/v2/developerguide/ipc-iot-core-mqtt.html).

## Device Defender

This sample uses the AWS IoT [Device Defender]() Service to send on device metrics to AWS.

Source: `samples/device_defender/basic_report/main.cpp`

On startup, the sample will make a MQTT connection and a Device Defender task to send metrics every minute or at the time interval passed as a CLI argument. This sample shows how to send custom metrics in addition to the standard metrics that are always sent with Device Defender.

You will need to create a **Security Profile** to see the metric results in the AWS console. You can create a Security Profile by going to `Detect -> Security Profiles` from the AWS IOT Console. To see the custom metrics, you will need to add them in `Detect -> Metrics` and then press the `Create` button to make a new custom metric.

This sample uses the following custom metrics:
* `CustomNumber` - type: `number` info: always sends the number `10`.
* `CustomNumberTwo` - type `number` info: sends a random number from `-50` to `50`.
* `CustomNumberList` - type `number-list` info: sends a predefined list of numbers.
* `CustomStringList` - type `string-list` info: sends a predefined list of strings.
* `CustomIPList` - type `ip-list` info: sends a predefined list of documentation IP addresses.

**Note:** This sample **only runs on Linux**. Device Defender is only supported on Linux.

To run the basic Device Defender use the following command:

``` sh
./basic-report --endpoint <endpoint> --ca_file <path to root CA>
--cert <path to the certificate> --key <path to the private key>
--thing_name <thing name>
```

Your Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect, publish, and receive. Make sure your policy allows a client ID of `test-*` to connect or use `--client_id <client ID here>` to send the client ID your policy supports.

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
      "Resource": "arn:aws:iot:<b>region</b>:<b>account</b>:client/*"
    }
  ]
}
</pre>
</details>

**Note:** The formatting and data requirements for custom metrics in Device Defender are listed below. If you input data that does not follow the requirements, it will be sent in the JSON packet but Device Defender will not show/retain the data! If your data is not showing up in Device Defender, please double check that your data fits into the requirements below.

* `number` and `number-list` types: Supports precision up to 15 digits. Supports both positive and negative values.
* `string-list` type: Supports letters A through Z (uppercase and lowercase) and the characters `:`, `_`, `-`, `/`, and `.
* `ip-list` type: Supports only valid IPV4 and IPV6 addresses.

## Cycle Pub-Sub

This sample will create a series of clients, and then randomly perform operations on them (*connect, disconnect, publish, subscribe, etc.*) every few seconds. Status updates on what each client in the sample are doing are continually printed to the console and the sample will run until the time duration set is complete.

This sample is primarily for testing stability over a period of time for testing and CI, but it also shows how to randomly perform operations across multiple clients.

Source: `samples/pub_sub/cycle_pub_sub/main.cpp`

Your Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect, subscribe, unsubscribe, publish, and receive. Make sure your policy allows a client ID of `test-client-*` to connect.

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
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/test/shared_topic",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/test-client-*"
      ]
    },
    {
      "Effect": "Allow",
      "Action": [
        "iot:Subscribe",
        "iot:Unsubscribe"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/test/shared_topic",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/test-client-*"
      ]
    },
    {
      "Effect": "Allow",
      "Action": [
        "iot:Connect"
      ],
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:client/test-client-*"
      ]
    }
  ]
}
</pre>
</details>

To run the Cycle Pub-Sub sample, use the following command:

``` sh
./cycle-pub-sub --endpoint <endpoint> --ca_file <path to root CA>
--cert <path to the certificate> --key <path to the private key>
```

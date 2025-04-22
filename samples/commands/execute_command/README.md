# Commands Execution

[**Return to main sample list**](../../README.md)

This is a semi-interactive sample that allows you to use the AWS IoT [Commands](https://docs.aws.amazon.com/iot/latest/developerguide/iot-remote-command.html)
service to receive and process remote commands.

> [!NOTE]
> The sample walkthrough assumes the [AWS CLI](https://aws.amazon.com/cli/) has been installed and configured in order
> to invoke control plane operations that are not possible with the device SDK.

> [!NOTE]
> In a real use case, control plane commands (the AWS CLI commands) would be issued by applications under control of
> the customer, while the data plane operations (the actions performed by the sample application) would be issued by
> software running on the IoT device itself.

Using the Commands service and this sample requires an understanding of two closely-related but different service terms:
* **Command** - metadata describing a task that the user would like one or more devices to run.
* **Command Execution** - metadata describing the state of a single device's attempt to execute a command.

In particular, you can define a **Command** and then send it multiple times to the same device. The device will try to
execute each received **Command Execution**.

### Interaction with sample application

Once connected, the sample supports the following commands:

* `help` - prints the set of supported commands
* `quit` - quits the sample application
* `list-streams` - list all open streaming operations
* `close-stream` - close a specified streaming operation

### Prerequisites
Your IoT Core Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect as well as subscribe, publish,
and receive as necessary to perform all of the data plane operations. Below is a sample policy that can be used on your
IoT Core Thing that will allow this sample to run as intended.

<details>
<summary>Sample Policy</summary>
<pre>
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": "iot:Publish",
      "Resource": [
        "arn:aws:iot:&lt;region&gt;:&lt;account&gt;:topic/$aws/commands/&lt;device_type&gt;/&lt;device_id&gt;/executions/*/response/json"
      ]
    },
    {
      "Effect": "Allow",
      "Action": "iot:Receive",
      "Resource": [
        "arn:aws:iot:&lt;region&gt;:&lt;account&gt;:topic/$aws/commands/&lt;device_type&gt;/&lt;device_id&gt;/executions/*/request/*",
        "arn:aws:iot:&lt;region&gt;:&lt;account&gt;:topic/$aws/commands/&lt;device_type&gt;/&lt;device_id&gt;/executions/*/response/accepted/json",
        "arn:aws:iot:&lt;region&gt;:&lt;account&gt;:topic/$aws/commands/&lt;device_type&gt;/&lt;device_id&gt;/executions/*/response/rejected/json"
      ]
    },
    {
      "Effect": "Allow",
      "Action": "iot:Subscribe",
      "Resource": [
        "arn:aws:iot:&lt;region&gt;:&lt;account&gt;:topicfilter/$aws/commands/&lt;device_type&gt;/&lt;device_id&gt;/executions/*/request/*",
        "arn:aws:iot:&lt;region&gt;:&lt;account&gt;:topicfilter/$aws/commands/&lt;device_type&gt;/&lt;device_id&gt;/executions/*/response/accepted/json",
        "arn:aws:iot:&lt;region&gt;:&lt;account&gt;:topicfilter/$aws/commands/&lt;device_type&gt;/&lt;device_id&gt;/executions/*/response/rejected/json"
      ]
    },
    {
      "Effect": "Allow",
      "Action": "iot:Connect",
      "Resource": "arn:aws:iot:&lt;region&gt;:&lt;account&gt;:client/test-*"
    }
  ]
}
</pre>

Replace with the following with the data from your AWS account:
* `<region>`: The AWS IoT Core region where you created your AWS IoT Core thing you wish to use with this sample. For example `us-east-1`.
* `<account>`: Your AWS IoT Core account ID. This is the set of numbers in the top right next to your AWS account name when using the AWS IoT Core website.
* `<device_type>`: Can be either `things` or `clients`.
* `<device_id>`: Depending on `<device_type>` value, this is either IoT Thing name or MQTT client ID.

Note that in a real application, you may want to avoid the use of wildcards in your ClientID or use them selectively.
Please follow best practices when working with AWS on production applications using the SDK. Also, for the purposes of
this sample, please make sure your policy allows a client ID of `test-*` to connect or use `--client_id <client ID here>`
to send the client ID your policy supports.

</details>

Additionally, the AWS CLI triggered control plane operations in the walkthrough require that AWS credentials with
appropriate permissions be sourceable. At a minimum, the following permissions must be granted:
<details>
<summary>Sample Policy</summary>
<pre>
{
    "Version": "2012-10-17",
    "Statement": 
    {
        "Action": "iot:CreateCommand",
        "Effect": "Allow",
        "Resource": "arn:aws:iot:&lt;region&gt;:&lt;account&gt;:command/&lt;command_name&gt;"          
    }
}
</pre>

Replace with the following with the data from your AWS account:
* `<region>`: The AWS IoT Core region where you created your AWS IoT Core thing you wish to use with this sample. 
  For example `us-east-1`.
* `<account>`: Your AWS IoT Core account ID. This is the set of numbers in the top right next to your AWS account name
  when using the AWS IoT Core website.
* `<command_name>`: The unique identifier for your AWS IoT command, such as `LockDoor`. If you want to use more than
  one command, you can use `*` or specify multiple commands under the Resource section in the IAM policy.

</details>

## Building and Running the Sample

Before building and running the sample, you must first build and install the SDK:

``` sh
cd <sdk-root-directory>
cmake -S . -B build/ -DCMAKE_INSTALL_PREFIX=<sdk_install_path>
cmake --build build/ --target install
```

Now build the sample:

``` sh
cd samples/commands/execute_command
cmake -S . -B build/ -DCMAKE_PREFIX_PATH=<sdk_install_path>
cmake --build build/
```

To run the sample:

``` sh
./build/execute-command \
    --endpoint <endpoint> \
    --cert <path to the certificate> \
    --key <path to the private key> \
    --thing_name <thing name> --client_id <client id>
```

The sample will automatically connect to IoT Core and then will subscribe to streams of Command Executions related to
the IoT Thing name and MQTT client ID you specified.

As soon as you create a new IoT command execution targeting your thing/client, the sample will receive and process it, and
then will update its status in IoT Core.

Once successfully connected, you can issue instructions to the sample.

## Walkthrough

The commands sample walkthrough involves a sequence of steps, some of which are instructions issued to the running sample,
and some of which are AWS CLI control plane commands for creating and deleting IoT commands.

> [!IMPORTANT]
> For the walkthrough to work, the CLI commands must use the same region that the sample is connected to.

### Command Creation

All commands in this section are control plane operations that use the AWS CLI and are not sample commands.  

First, you can examine the existing `AWS CLI` commands:

```shell
aws iot list-commands --namespace "AWS-IoT"
```

yields output like
```
{
    "commands": []
}
```
from which we can see that there are no existing IoT commands.

Next, we'll create a couple of IoT commands. When creating the command, you must provide a payload. The payload that you
provide is base64 encoded.

Create an IoT command with JSON payload:
```shell
aws iot create-command \
    --command-id MyJsonCommand \
    --display-name MyJsonCommand \
    --description "Switch the light using JSON" \
    --namespace AWS-IoT \
    --payload '{"content":"eyAibWVzc2FnZSI6ICJIZWxsbyBJb1QiIH0=","contentType":"application/json"}'
```

Create an IoT command with some arbitrary data format:
```shell
aws iot create-command \
    --command-id MyGenericCommand \
    --display-name MyGenericCommand \
    --description "Switch the light using an arbitrary payload" \
    --namespace AWS-IoT \
    --payload '{"content":"eyAibWVzc2FnZSI6ICJIZWxsbyBJb1QiIH0="}'
```
Even though we pass JSON payload, the created IoT command's payload will be marked as `application/octet-stream`
because we didn't explicitly specify that it's JSON.

IoT commands do not target any device. For that, you need to create IoT command execution.

Keep the sample running in the background to see IoT commands notifications arrive by the sample's streaming operations.

First, you'll need the full ARN of the IoT Thing in use:

```
aws iot describe-thing --thing-name <Thing Name>
```

With that in hand, create a new IoT command execution targeting your IoT thing using the AWS CLI:

```
aws iot-jobs-data start-command-execution \
  --command-arn "arn:aws:iot:<region>:<account>:command/MyJsonCommand" \
  --target-arn <thing ARN from previous command>\
  --endpoint <endpoint> \
  --execution-timeout-seconds 10
```

On success, the CLI command should give output like:

```
{
    "executionId": "12345678-1234-5678-9abc-123456789abc"
}
```

Meanwhile, your running jobs sample should receive notifications and output something similar to:

```
Received new command for 'json' payload format:
  execution ID: '12345678-1234-5678-9abc-123456789abc'
  payload format: 'application/json'
  execution timeout: 9
  payload size: 26
[12345678-1234-5678-9abc-123456789abc] Validating command
[12345678-1234-5678-9abc-123456789abc] Executing command
[12345678-1234-5678-9abc-123456789abc] Processing command... it'll take 7 seconds
[12345678-1234-5678-9abc-123456789abc] Updating command execution
[12345678-1234-5678-9abc-123456789abc] Successfully updated execution status
```

# Commands Execution

[**Return to main sample list**](../../README.md)

This is a semi-interactive sample that allows you to use the AWS IoT [Commands](https://docs.aws.amazon.com/iot/latest/developerguide/iot-remote-command.html)
service to receive and process remote instructions.

> [!NOTE]
> The sample walkthrough assumes the [AWS CLI](https://aws.amazon.com/cli/) has been installed and configured in order
> to invoke control plane operations that are not possible with the device SDK.

In a real use case, control plane commands (the AWS CLI commands) would be issued by applications under control of
the customer, while the data plane operations (the actions performed by the sample application) would be issued by
software running on the IoT device itself.

Using the IoT Commands service and this sample requires an understanding of two closely-related but different service terms:
* **AWS IoT Command** - metadata describing a task that the user would like one or more devices to run.
* **AWS IoT Command Execution** - metadata describing the state of a single device's attempt to execute an AWS IoT Command.

In particular, you can define an **AWS IoT Command** and then send it multiple times to the same device. The device will
try to execute each received **AWS IoT Command Execution**.

Another important feature of AWS IoT command service is using different MQTT topics for different payload formats. This
means that your device can choose to receive only AWS IoT commands with specified payload format.
AWS IoT Commands service distinguishes the following payload formats:
- JSON
- CBOR
- generic (i.e. everything else)

This also means that if your device wants to receive JSON **and** CBOR payloads, it needs to subscribe to both topics
using two separate API calls.

> [!NOTE]
> The word `command` is quite overloaded in this sample. It's supposed to be used with multiple meanings:
> - AWS IoT command - an event with instructions sent from the IoT Core to a device
> - AWS CLI command - a shell command for executing a control plane operation
> - sample command - an action that this sample application can perform, such as `open-thing-stream`
> - there is also AWS IoT command execution.
>
> To avoid confusion, the `command` word will allways be used with a context.

### Interaction with sample application

Once connected, the sample supports the following sample commands:

* open-thing-stream <payload-format> - subscribe to a stream of AWS IoT command executions with a specified payload format
targeting the IoT Thing set on the application startup
* open-client-stream <payload-format> - subscribe to a stream of AWS IoT command executions with a specified payload format
targeting the MQTT client ID set on the application startup
* update-command-execution <execution-id> \<status> \[\<reason-code>] \[\<reason-description>] - update status for specified
execution ID;
  * status can be one of the following: IN_PROGRESS, SUCCEEDED, REJECTED, FAILED, TIMED_OUT
  * reason-code and reason-description may be optionally provided for the REJECTED, FAILED, or TIMED_OUT statuses

Miscellaneous
* list-streams - list all open streaming operations
* close-stream <stream-id> - close a specified stream; <stream-id> is internal ID that can be found with 'list-streams'
* quit - quit the sample application

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

```shell
cd <sdk-root-directory>
cmake -S . -B build/ -DCMAKE_INSTALL_PREFIX=<sdk_install_path>
cmake --build build/ --target install
```

Now build the sample:

```shell
cd samples/commands/execute_command
cmake -S . -B build/ -DCMAKE_PREFIX_PATH=<sdk_install_path>
cmake --build build/
```

To run the sample:

```shell
./build/execute-command \
    --endpoint <endpoint> \
    --cert <path to the certificate> \
    --key <path to the private key> \
    --thing_name <thing name> \
    --client_id <client id>
```

The sample will automatically connect to IoT Core and then will subscribe to streams of AWs IoT command executions
related to the IoT Thing name and MQTT client ID you specified.

As soon as you create a new AWS IoT command execution targeting your thing/client, the sample will receive and process it, and
then will update its status in IoT Core.

Once successfully connected, you can issue instructions to the sample.

## Walkthrough

The commands sample walkthrough involves a sequence of steps, some of which are instructions issued to the running sample,
and some of which are AWS CLI control plane commands for creating and deleting AWS IoT commands.

> [!IMPORTANT]
> For the walkthrough to work, the AWS CLI commands must use the same region that the sample is connected to.

### Creating AWS IoT Commands

Creation of an AWS IoT Command is achieved by using control plane operations, i.e. AWS CLI.

We'll start with creation of a couple of AWS IoT commands. When creating the AWS IoT command, you must provide a payload.
The payload that you provide is base64 encoded.

The first AWS IoT command contains JSON payload:
```shell
aws iot create-command \
    --command-id MyJsonCommand \
    --display-name MyJsonCommand \
    --description "Switch the light using JSON" \
    --namespace AWS-IoT \
    --payload '{"content":"eyAibWVzc2FnZSI6ICJIZWxsbyBJb1QiIH0=","contentType":"application/json"}'
```

The second AWS IoT command contains some arbitrary data:
```shell
aws iot create-command \
    --command-id MyGenericCommand \
    --display-name MyGenericCommand \
    --description "Switch the light using an arbitrary payload" \
    --namespace AWS-IoT \
    --payload '{"content":"eyAibWVzc2FnZSI6ICJIZWxsbyBJb1QiIH0="}'
```
Even though we pass JSON payload, the created AWS IoT command's payload will be marked as `application/octet-stream`
because we didn't explicitly specify that it's JSON.

Now we can examine the newly created AWS IoT commands using the following AWS CLI command:

```shell
aws iot list-commands --namespace "AWS-IoT"
```

yields output like the following:
```json
{
    "commands": [
        {
            "commandArn": "arn:aws:iot:...:command/MyGenericCommand",
            "commandId": "MyGenericCommand",
            "displayName": "MyGenericCommand",
            "deprecated": false,
            "pendingDeletion": false
        },
        {
            "commandArn": "arn:aws:iot:...:command/MyJsonCommand",
            "commandId": "MyJsonCommand",
            "displayName": "MyJsonCommand",
            "deprecated": false,
            "pendingDeletion": false
        }
    ]
}
```
Take a notice of the `commandArn` field. It is used in creation of AWS IoT command executions.


### Running sample and subscribing to AWS IoT Command Executions

It's time to run the sample (unless you did it already) with the following shell command:
```shell
./build/execute-command \
    --endpoint <endpoint> \
    --cert <path to the certificate> \
    --key <path to the private key> \
    --thing_name <IoT thing name> \
    --client_id <MQTT client id>
```

First, let's subscribe to AWS IoT commands with JSON payloads using the following sample command:
```
open-thing-stream json
```

Let's open another stream, this time for generic payloads:
```
open-thing-stream generic
```

To examine the open streaming operations use the `list-streams` sample command:
```
list-streams
```

and you will see something like this
```
Streams:
  2: device type 'things', device ID 'MyIotThing', payload type 'generic'
  1: device type 'things', device ID 'MyIotThing', payload type 'json'
```

You can close a streaming operation using the `close-stream` sample command:
```
close-stream <stream-id>
```
, where `<stream-id>` is a sequence number of the operation. For our example, JSON operation has stream ID 1 and generic
operation has stream ID 2.

For example, to close `generic` stream, execute this sample command:
```
close-stream 2
```

### Sending AWS IoT Command Executions

AWS IoT command just defines a set of instructions. It cannot target any device. For sending AWS IoT command to a device,
you need to create AWS IoT command execution.

Keep the sample running in the background to see execution notifications arrive by the sample's streaming
operations.

First, you'll need the full ARN of the IoT Thing in use:

```shell
aws iot describe-thing --thing-name <Thing Name>
```

> [!TIP]
> In this document, all AWS IOT command executions target IoT Thing. If you want to create an AWS IoT command execution
> for MQTT client ID, you should use the "arn:aws:iot:\<region>:\<account>:client/\<client-id>" format for --target-arn option.

With that in hand, create a new AWS IoT command execution targeting your IoT thing using the AWS CLI command:

```shell
aws iot-jobs-data start-command-execution \
  --command-arn <command ARN for MyJsonCommand returned by 'aws iot list-commands --namespace "AWS-IoT"'> \
  --target-arn <thing ARN from previous command>\
  --endpoint <endpoint> \
  --execution-timeout-seconds 10
```

On success, the AWS CLI command should give output like:

```json
{
    "executionId": "11111111-1111-1111-1111-111111111111"
}
```

Meanwhile, your running sample should receive notifications and output something similar to:

```
Received new command for 'json' payload format:
  execution ID: '11111111-1111-1111-1111-111111111111'
  payload format: 'application/json'
  execution timeout: 9
  payload size: 26
```

> [!IMPORTANT]
> IoT C++ SDK v2 does not parse payload of the incoming AWS IoT commands. User code gets a structure containing byte buffer
> for payload and payload format if it was specified for the IoT command. User code is supposed to parse payload itself.

Your device has only 9-10 seconds to report back the execution status, which is not enough for an interactive application.
The AWS IoT command execution you sent will probably time out before you manage to send the status update.

You can check the AWS IoT command execution status using the following AWS CLI command:

```shell
aws iot get-command-execution \
    --execution-id <execution ID returned by start-command-execution> \
    --target-arn <thing ARN>
```

will yield something like this:

```json
{
    "executionId": "11111111-1111-1111-1111-111111111111",
    "commandArn": "arn:aws:iot:...:command/MyJsonCommand",
    "targetArn": "arn:aws:iot:...:thing/MyIotThing",
    "status": "TIMED_OUT",
    "statusReason": {
        "reasonCode": "$NO_RESPONSE_FROM_DEVICE"
    },
    "executionTimeoutSeconds": 10
}
```

Let's send another AWS IoT command execution, this time with a timeout more suitable for our sample. Notice that we use
the same AWS IoT command, the only thing that changed is the execution timeout value:

```shell
aws iot-jobs-data start-command-execution \
  --command-arn <command ARN for MyJsonCommand returned by 'aws iot list-commands --namespace "AWS-IoT"'> \
  --target-arn <thing ARN from previous command>\
  --endpoint <endpoint> \
  --execution-timeout-seconds 300
```

The running sample will receive another notification, with the new execution ID:
```
Received new command for 'json' payload format:
  execution ID: '22222222-2222-2222-2222-222222222222'
  payload format: 'application/json'
  execution timeout: 299
  payload size: 26
```

Let's proceed to the next section where we're going to update the status of an AWS IoT command execution.

### Updating and monitoring AWS IoT command execution status

The sample didn't yet update the status of the AWS IoT command execution, so the following AWS CLI command

```shell
aws iot get-command-execution --target-arn "<thing ARN>" --execution-id <AWS IoT command execution ID>
```

should return `CREATED` status:

```json
{
    "executionId": "22222222-2222-2222-2222-222222222222",
    "commandArn": "arn:aws:iot:...:command/MyJsonCommand",
    "targetArn": "arn:aws:iot:...:thing/MyIotThing",
    "status": "CREATED",
    "executionTimeoutSeconds": 300
}
```

To update the status of a received AWS IoT command execution, we should use the `update-command-execution` sample command.
Take an AWS IoT command execution ID your sample received at the end of the previous section and pass it to
`update-command-execution` along with the `IN_PROGRESS` status:
```
update-command-execution <execution-id> IN_PROGRESS
```

Then this AWS CLI command
```shell
aws iot get-command-execution --target-arn "<thing ARN>" --execution-id <IoT command execution ID>
```

should return something like

```json
{
    "executionId": "22222222-2222-2222-2222-222222222222",
    "commandArn": "arn:aws:iot:...:command/MyJsonCommand",
    "targetArn": "arn:aws:iot:...:thing/MyIotThing",
    "status": "IN_PROGRESS",
    "executionTimeoutSeconds": 300
}
```

`IN_PROGRESS` is an intermediary execution status, i.e. it's possible to change this status.
`SUCCEEDED`, `FAILED`, and `REJECTED` statuses are terminal - when you set the AWS IoT command execution status to one
of them, it's final.

There is also the `TIMED_OUT` status. Though it's supposed to be set by the service side when there is no response from
the device in `timeout` time, your application may provide additional info by setting the `statusReason` field in the update
event.

Let's set the AWS IoT command execution status to one of the terminal states with sample command:
```
update-command-execution <execution-id> SUCCEEDED
```
or
```
update-command-execution <execution-id> FAILED SHORT_FAILURE_CODE A longer description
```

will yield something like
```
Updating command execution '22222222-2222-2222-2222-222222222222'
Successfully updated execution status for '22222222-2222-2222-2222-222222222222'
```

If you try to update the status of the same AWS IoT command execution to something else, it'll fail:
```
update-command-execution <execution-id> REJECTED
```

will yield
```
Updating command execution '22222222-2222-2222-2222-222222222222'
Failed to update execution status: execution ID 22222222-2222-2222-2222-222222222222
Failed to update execution status: response code 5174
Failed to update execution status: error message Command Execution status cannot be updated to REJECTED since
                                   execution has already completed with status FAILED.
Failed to update execution status: error code 8 (TerminalStateReached)
```

Let's check one last time the execution status with AWS CLI command:

```shell
aws iot get-command-execution --target-arn "<thing ARN>" --execution-id <IoT command execution ID>
```

which will yield

```json
{
    "executionId": "22222222-2222-2222-2222-222222222222",
    "commandArn": "arn:aws:iot:...:command/MyJsonCommand",
    "targetArn": "arn:aws:iot:...:thing/MyIotThing",
    "status": "FAILED",
    "statusReason": {
        "reasonCode": "SHORT_FAILURE_CODE",
        "reasonDescription": "A longer description"
    },
    "executionTimeoutSeconds": 300
}
```

### Cleaning up

When all executions for a given AWS IoT command have reached a terminal state (`SUCCEEDED`, `FAILED`, `REJECTED`), you
can delete the AWS IoT command itself. This is a control plane operation that requires an HTTP-based SDK or the AWS CLI
to perform:

```shell
aws iot delete-command --command-id <command-id>
```

### Misc Topics

#### What is the proper generic architecture for a command-processing application running on a device?

1. On startup, create and open streaming operations for the needed AWS IoT command events using
   `IClientV2::CreateCommandExecutionsJsonPayloadStream`, `IClientV2::CreateCommandExecutionsCborPayloadStream`,
   and/or `IClientV2::CreateCommandExecutionsGenericPayloadStream` functions.
2. **DO NOT** process received AWS IoT commands right in the callback passed to `CreateCommandExecutions*PayloadStream`.
   As a general rule, **DO NOT** perform any time-consuming or blocking operations in the callback. One of possible
   approaches is to put incoming IoT commands into a shared queue, then the designated executor(s) will process them in
   separate thread(s).
3. If your application is expected to receive a lot of AWS IoT commands, monitor the number of them enqueued for processing.
   Consider introducing priorities based on AWS IoT command timeouts or some internal value.

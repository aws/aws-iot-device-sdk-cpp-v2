# Jobs Sandbox

[**Return to main sample list**](../../README.md)

This is an interactive sample that supports a set of commands that allow you to interact with the AWS IoT [Jobs](https://docs.aws.amazon.com/iot/latest/developerguide/iot-jobs.html) Service.   The sample walkthrough assumes the [AWS CLI](https://aws.amazon.com/cli/) has been installed and configured in order to invoke control plane operations that are not possible with the device SDK.
In a real use case, control plane commands would be issued by applications under control of the customer, while the data plane operations would be issue by software running on the
IoT device itself.

Using the Jobs service and this sample requires an understanding of two closely-related but different service terms:
* **Job** - metadata describing a task that the user would like one or more devices to run
* **Job Execution** - metadata describing the state of a single device's attempt to execute a job

In particular, you could have many IoT devices (things) that belong to a thing group.  You could create a **Job** that targets the thing group.  Each device/thing would
manage its own individual **Job Execution** that corresponded to its attempt to fulfill the overall job request.  In the sections that follow, notice that all of the data-plane (SDK)
commands use `job-execution` while all of the control plane operations (via the AWS CLI) use `job`.

### Commands

Once connected, the sample supports the following commands:

* `get-pending-job-executions` - gets the state of all incomplete job executions for this thing/device.
* `start-next-pending-job-execution` - if one or more pending job executions exist for this thing/device, attempts to transition the next one from QUEUED to IN_PROGRESS.  Returns information about the newly-in-progress job execution, if it exists.
* `describe-job-execution <jobId>` - gets the current state of this thing's execution of a particular job.
* `update-job-execution <jobId> <SUCCEEDED | IN_PROGRESS | FAILED | CANCELED>` - updates the status field of this thing's execution of a particular job.  SUCCEEDED, FAILED, and CANCELED are all terminal states.

Miscellaneous
* `help` - prints the set of supported commands
* `quit` - quits the sample application

### Prerequisites
Your IoT Core Thing's [Policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) must provide privileges for this sample to connect as well as subscribe, publish, and receive as necessary to perform all of the data plane operations. Below is a sample policy that can be used on your IoT Core Thing that will allow this sample to run as intended.

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
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/jobs/start-next",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/jobs/*/update",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/jobs/*/get",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/jobs/get"
      ]
    },
    {
      "Effect": "Allow",
      "Action": "iot:Receive",
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/jobs/notify",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/jobs/notify-next",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/jobs/start-next/*",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/jobs/*/update/*",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/jobs/get/*",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topic/$aws/things/<b>thingname</b>/jobs/*/get/*"
      ]
    },
    {
      "Effect": "Allow",
      "Action": "iot:Subscribe",
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/jobs/notify",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/jobs/notify-next",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/jobs/start-next/*",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/jobs/*/update/*",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/jobs/get/*",
        "arn:aws:iot:<b>region</b>:<b>account</b>:topicfilter/$aws/things/<b>thingname</b>/jobs/*/get/*"
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

Replace with the following with the data from your AWS account:
* `<region>`: The AWS IoT Core region where you created your AWS IoT Core thing you wish to use with this sample. For example `us-east-1`.
* `<account>`: Your AWS IoT Core account ID. This is the set of numbers in the top right next to your AWS account name when using the AWS IoT Core website.
* `<thingname>`: The name of your AWS IoT Core thing you want the device connection to be associated with

Note that in a real application, you may want to avoid the use of wildcards in your ClientID or use them selectively. Please follow best practices when working with AWS on production applications using the SDK. Also, for the purposes of this sample, please make sure your policy allows a client ID of `test-*` to connect or use `--client_id <client ID here>` to send the client ID your policy supports.

</details>

Additionally, the AWS CLI triggered control plane operations in the walkthrough require that AWS credentials with appropriate permissions be sourcable.  At a minimum, the following permissions must be granted:
<details>
<summary>Sample Policy</summary>
<pre>
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": "iot:CreateJob",
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:job/*",
        "arn:aws:iot:<b>region</b>:<b>account</b>:thing/<b>thingname</b>"
      ]
    },
    {
      "Effect": "Allow",
      "Action": "iot:DeleteJob",
      "Resource": [
        "arn:aws:iot:<b>region</b>:<b>account</b>:job/*",
        "arn:aws:iot:<b>region</b>:<b>account</b>:thing/<b>thingname</b>"
      ]
    }
  ]
}
</pre>

Replace with the following with the data from your AWS account:
* `<region>`: The AWS IoT Core region where you created your AWS IoT Core thing you wish to use with this sample. For example `us-east-1`.
* `<account>`: Your AWS IoT Core account ID. This is the set of numbers in the top right next to your AWS account name when using the AWS IoT Core website.
* `<thingname>`: The name of your AWS IoT Core thing you want the device connection to be associated with

Notice that you must provide `iot:CreateJob` permission to all things targeted by your jobs as well as the job itself.  In this example, we use a wildcard for the
job permission so that you can name the jobs whatever you would like.

</details>

## Building and Running the Sample

Before building and running the sample, you must first build and install the SDK:

``` sh
cd <sdk-root-directory>
mkdir _build
cd _build
cmake -DCMAKE_INSTALL_PREFIX=<sdk_install_path> ..
make && make install
```

Now build the sample:

``` sh
cd samples/jobs/jobs-sandbox
mkdir _build
cd _build
cmake -DCMAKE_PREFIX_PATH=<sdk_install_path> ..
make
```

To run the sample:

``` sh
./jobs-sandbox --endpoint <endpoint> --cert <path to the certificate> --key <path to the private key> --thing_name <thing name>
```

The sample also listens to a pair of event streams related the configured thing's job processing, so in addition to responses, you will occasionally see output from these streaming operations as they receive events from the jobs service.

Once successfully connected, you can issue commands.

## Walkthrough

The jobs sample walkthrough involves a sequence of steps, some of which are commands issued to the running sample, and some of which are AWS CLI control plane commands for
creating and deleting jobs.  For the walkthrough to work, the CLI commands must use the same region that the sample is connected to.

This walkthrough assumes a freshly-created IoT thing that has no pre-existing jobs targeting it.

### Job Creation
First, we check if there are any incomplete job executions for this thing.  Assuming the thing is freshly-created, we expect there to be nothing:

```
get-pending-job-executions
```
yields output like
```
get-pending-job-executions result: {"inProgressJobs":[],"queuedJobs":[],"timestamp":1730746099,"clientToken":"6c5828e4-9a6b-4b07-10d7-9193819b9c88"}
```
from which we can see that the device has no pending job executions and no in-progress job executions.

Next, we'll create a couple of jobs that target the device.  These are control plane operations that use the AWS CLI and are not sample commands.  Keep the sample running in the background to see job notifications arrive by the sample's streaming operations.

First, you'll need the full ARN of the IoT Thing in use:

```
aws iot describe-thing --thing-name <Thing Name>
```

With that in hand, create a new job targeting your IoT thing using the AWS CLI:

```
aws iot create-job --job-id TestJob --targets "<Thing ARN>" --document "{\"Todo\":\"Reboot\"}"
```

On success, the CLI command should give output like:

```
{
    "jobArn": "<Job ARN prefix>/TestJob",
    "jobId": "TestJob"
}
```

Meanwhile, your running jobs sample should receive notifications and output something similar to:

```
Received JobExecutionsChanged event:  {"jobs":{"QUEUED":[{"jobId":"TestJob","executionNumber":1,"versionNumber":1,"lastUpdatedAt":1730747114,"queuedAt":1730747114}]},"timestamp":1730747115}

Received NextJobExecutionChanged event:  {"execution":{"jobId":"TestJob","jobDocument":{"Todo":"Reboot"},"status":"QUEUED","queuedAt":1730747114,"lastUpdatedAt":1730747114,"versionNumber":1,"executionNumber":1},"timestamp":1730747115}
```

Creating the job via the AWS CLI triggered two (MQTT-based) events: a JobExecutionsChanged event and a
NextJobExecutionChanged event.  When the sample is run, it creates and opens two streaming operations that listen for these two different events, by using the
`CreateJobExecutionsChangedStream` and `CreateNextJobExecutionChangedStream` APIs in the Jobs service client.

A JobExecutionsChanged event is emitted every time either the queued or in-progress job execution sets change for the device.  A NextJobExecutionChanged event is emitted
only when the next job to be executed changes.  So if you create N jobs targeting a device, you'll get N JobExecutionsChanged events, but only (up to) one
NextJobExecutionChanged event (unless the device starts completing jobs, triggering additional NextJobExecutionChanged events).

Let's create a second job as well:

```
aws iot create-job --job-id QuestionableJob --targets "<Thing ARN>" --document "{\"Todo\":\"Delete Root User\"}"
```

whose output might look like

```
{
    "jobArn": "<Job ARN prefix>/QuestionableJob",
    "jobId": "QuestionableJob"
}
```
and the sample should output a single JobExecutionsChanged event that now shows the two jobs in the QUEUED state:

```
Received JobExecutionsChanged event:  {"jobs":{"QUEUED":[{"jobId":"TestJob","executionNumber":1,"versionNumber":1,"lastUpdatedAt":1730747114,"queuedAt":1730747114},{"jobId":"QuestionableJob","executionNumber":1,"versionNumber":1,"lastUpdatedAt":1730747777,"queuedAt":1730747777}]},"timestamp":1730747778}
```

Notice how this time, there is no NextJobExecutionChanged event because the second job is behind the first, and therefore the next job execution hasn't changed.  As we will
see below, a NextJobExecutionChanged event referencing the second job will be emitted when the first job (in progress) is completed.

### Job Execution
Our device now has two jobs queued that it needs to (pretend to) execute.  Let's see how to do that, and what happens when we do.

The easiest way to start a job execution is via the `startNextPendingJobExecution` API.  This API takes the job execution at the head of the QUEUED list and moves it
into the IN_PROGRESS state, returning its job document in the process.

```
start-next-pending-job-execution
```

should yield output similar to:

```
start-next-pending-job-execution result: {"clientToken":"686ccb4e-bb55-4032-68c9-5f810556d151","execution":{"jobId":"TestJob","jobDocument":{"Todo":"Reboot"},"status":"IN_PROGRESS","queuedAt":1730747114,"startedAt":1730748422,"lastUpdatedAt":1730748422,"versionNumber":2,"executionNumber":1},"timestamp":1730748422}
```
Note that the response includes the job's JSON document (`{"Todo":"Reboot"}`), which is what describes what the job actually entails.  The contents of the job document and its interpretation and
execution are the responsibility of the developer.  Notice also that no events were emitted from the action of moving a job from the QUEUED state to the IN_PROGRESS state.

If we run `getPendingJobExecutions` again, we see that Job1 is now in progress, while Job2 remains in the queued state:

```
get-pending-job-executions
```
```
get-pending-job-executions result: {"inProgressJobs":[{"jobId":"TestJob","executionNumber":1,"versionNumber":2,"lastUpdatedAt":1730748422,"queuedAt":1730747114,"startedAt":1730748422}],"queuedJobs":[{"jobId":"QuestionableJob","executionNumber":1,"versionNumber":1,"lastUpdatedAt":1730747777,"queuedAt":1730747777}],"timestamp":1730748515,"clientToken":"14e7a359-41d3-e5f1-0b37-353434eeee0f"}
```

A real device application would perform the job execution steps as needed.  Let's assume that has been done.  We need to tell the service the job has
completed:

```
update-job-execution TestJob SUCCEEDED
```
will trigger output similar to
```
update-job-execution result: {"clientToken":"47444947-7691-6c94-470f-878795f3462f","timestamp":1730748769}

Received JobExecutionsChanged event:  {"jobs":{"QUEUED":[{"jobId":"QuestionableJob","executionNumber":1,"versionNumber":1,"lastUpdatedAt":1730747777,"queuedAt":1730747777}]},"timestamp":1730748770}

Received NextJobExecutionChanged event:  {"execution":{"jobId":"QuestionableJob","jobDocument":{"Todo":"Delete Root User"},"status":"QUEUED","queuedAt":1730747777,"lastUpdatedAt":1730747777,"versionNumber":1,"executionNumber":1},"timestamp":1730748770}
```
Notice we get a response as well as two events, since both
1. The set of incomplete job executions set has changed.
1. The next job to be executed has changed.

As expected, we can move QuestionableJob's execution into IN_PROGRESS by invoking `startNextPendingJobExecution` again:

```
start-next-pending-job-execution
```
while should yield something like
```
start-next-pending-job-execution result: {"clientToken":"8eabdcf4-13dd-cbcd-fa45-fdab5c1921ed","execution":{"jobId":"QuestionableJob","jobDocument":{"Todo":"Delete Root User"},"status":"IN_PROGRESS","queuedAt":1730747777,"startedAt":1730748903,"lastUpdatedAt":1730748903,"versionNumber":2,"executionNumber":1},"timestamp":1730748903}
```

Let's pretend that the job execution failed.  An update variant can notify the Jobs service of this fact:

```
update-job-execution QuestionableJob FAILED
```
triggering
```
update-job-execution result: {"clientToken":"fd2cc005-4cd9-5f20-453e-1e69f87368cd","timestamp":1730748962}

Received JobExecutionsChanged event:  {"jobs":{},"timestamp":1730748963}

Received NextJobExecutionChanged event:  {"timestamp":1730748963}
```
At this point, no incomplete job executions remain.

### Job Cleanup
When all executions for a given job have reached a terminal state (SUCCEEDED, FAILED, CANCELED), you can delete the job itself.  This is a control plane operation
that requires an HTTP-based SDK or the AWS CLI to perform:

```
aws iot delete-job --job-id TestJob
aws iot delete-job --job-id QuestionableJob
```

Deleting a job fails if an incomplete (non success/failure) job execution exists for the job.

### Misc. Topics
#### What happens if I call `StartNextPendingJobExecution` and there are no jobs to execute?
The request will not fail, but the `execution` field of the response will be empty, indicating that there is nothing to do.

#### What happens if I call `StartNextPendingJobExecution` twice in a row (or while another job is in the IN_PROGRESS state)?
The service will return the execution information for the IN_PROGRESS job again.

#### What if I want my device to handle multiple job executions at once?
Since `startNextPendingJobExecution` does not help here, the device application can manually update a job execution from the QUEUED state to the IN_PROGRESS
state in the same manner that it completes a job execution: use `getPendingJobExecutions` to get the list of queued executions and use
`updateJobExecution` to move one or more job executions into the IN_PROGRESS state.

#### What is the proper generic architecture for a job-processing application running on a device?
A device's persistent job executor should:
1. On startup, create and open streaming operations for both the JobExecutionsChanged and NextJobExecutionChanged events
2. On startup, get and cache the set of incomplete job executions using `GetPendingJobExecutions`
3. Keep the cached job execution set up to date by reacting appropriately to JobExecutionsChanged and NextJobExecutionChanged events
4. While there are incomplete job executions, start and execute them one-at-a-time; otherwise wait for a new entry in the incomplete (queued) job executions set.
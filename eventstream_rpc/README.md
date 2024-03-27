# Eventstream RPC

This module implements client for eventstream RPC. The eventstream RPC client provides a base for defining custom RPC
operations and implements interaction with RPC server.

## Client Connection

`ClientConnection` class represents the connection to the RPC server. It's responsible for connection lifetime and
can send connection-level messages (e.g. sending PING or opening a new stream).

## Client Stream/Continuation

Application-level messages are supposed to be sent over streams. `ClientConnection::NewStream` creates a new stream which is represented
by the `ClientContinuation` class.  
The new stream must be activated with `ClientContinuation::Activate` method. This method initiates sending messages over the wire.

Application-level messages can be sent using `ClientContinuation::SendMessage` method.

## Client Operation

Based upon a client continuation, this entity represents an abstract operation that the client can perform. Concrete operations
should be implemented as classes inherited from the `ClientOperation` class.

Each operation has input data (referred as **Request**) and output data (referred as **Response**).  
Requests are sent by the `Activate` method.

### Example of a Client Operation

[DeleteThingShadowOperation](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_greengrass_1_1_delete_thing_shadow_operation.html)
defines an RPC operation for deleting a thing shadow.

[DeleteThingShadowOperation::Activate](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_greengrass_1_1_delete_thing_shadow_operation.html#a7aca4de69329780dfa9ff17315e68b23)
takes [DeleteThingShadowRequest](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_greengrass_1_1_delete_thing_shadow_request.html)
instance as an argument and sends it to the server.

After the request is done, you can use [DeleteThingShadowOperation::GetResult](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_greengrass_1_1_delete_thing_shadow_operation.html#a261fa97489a6e2015e11f7c25aa13ee9)
to get an instance of the [DeleteThingShadowResult](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_greengrass_1_1_delete_thing_shadow_result.html)
class, an auxiliary wrapper class returning a response in case of success or an error in case of failure. To obtain
an actual response, use [DeleteThingShadowResult::GetOperationResponse](https://aws.github.io/aws-iot-device-sdk-cpp-v2/class_aws_1_1_greengrass_1_1_delete_thing_shadow_result.html#a6f4c4987311e016d20cf8e82808181fd)
method.

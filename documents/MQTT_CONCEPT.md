# MQTT Concepts

## Table of Contents
* [What is MQTT](#what-is-mqtt)
* [MQTT Knowledge Essentials](#mqtt-knowledge-essentials)
    + [Servers and Clients](#--servers---and---clients---)
    + [Terminology and Relationship](#--terminology-and-relationship--)
    + [What is a Topic](#--what-is-a-topic--)
    + [What is Quality Of Service (QOS)](#--what-is-quality-of-service--qos---)
    + [Allowed Operations](#--allowed-operations--)
* [Further Reading](#further-reading-)


## What is MQTT

MQTT is a message protocol often used for IoT devices thanks to its smaller bandwidth cost, its defined spec, and relatively low complexity compared to something like HTTP. MQTT5 is the latest version of MQTT protocol with additional features, like user properties, and developer enhancements, like better error logging, that make developing a MQTT application easier and more powerful.

*Please note that this document does not include the whole MQTT protocol* and only covers the basic essential MQTT concepts to aid in the understanding and usage of the CPP SDK API. In the Further Reading section there are links to documentation that covers the MQTT protocol in more detail.

## MQTT Knowledge Essentials

MQTT is a message protocol, but what does that mean? Simply put, MQTT is a way for a device on one network to send information to a device on another networking using an agreed upon set of rules and protocols. This information is sent through what is called *Packets*, which is data formatted in a specific way so the receiver knows how to parse it and extract the data contained within. The data within a packet is often referred to as the *payload*.

The concepts section will cover the most important aspects of MQTT relative to its usage in the CPP SDK in the sections below. The first thing that needs to be covered are the two main types of devices on the internet with MQTT:

### **Servers** and **Clients**:

* **Server** (interchangeably referred to as *Brokers*): where MQTT packets are sent and processed. A single server can process many packets and have many different MQTT clients (see below) at once. A server may both send and receive packets from one or more clients.
    * Servers can perform many different operations with packets depending on the data within, the Quality of Service level (also known as “QoS”), and the topic the packets are being sent to. The server will process incoming packets to determine whether to forward the packet to subscribers (clients who have subscribed to the topic in the packet) and/or responding to the client that sent the packet with an acknowledgement packet.
    * AWS IoT Core is a service that provides a MQTT5 server, which devices can connect to and send packets to and from. AWS IoT Core also has services that, when packets are sent to, will invoke certain operations and other services.
* **Client**: what sends and receives packets from the server. A client connects by sending a Connect Packet with information on who the client is, as well as often including authorization data. Upon connecting successfully, an MQTT client can subscribe and unsubscribe from topics, send and receive Publish Packets to topics and from topics it is subscribed to, as well as disconnect and reconnect.

In general, you will be working on writing code that makes a MQTT5 client and sends data to the server when certain operations happen, to report data from the device, to respond to data sent from the server or another client whose topic the client is subscribed to (more on subscriptions below), or otherwise react based on the packets being sent and received. Because of this, this document will be focusing the majority of the information on the client side of things, rather than the server.

(*Implementer note*: Link “subscriptions below” to the correct section in the Markdown when adding to GitHub.)


### **Terminology and Relationship**

The connection process for MQTT is different than connection process for a *Client/Server* paradigm. The process to connect in MQTT requires both a *Connection* and a *Session*, in addition to a *Client*. This can be confusing as they have similar functionalities. Below are the rough definitions for each concept:

* **Client**: Manages the Connection and the Session. A client contains all the information needed to connect to a MQTT server, as well as the interface to use for performing operations.
    * Operations are described further below in the operations section.
    * (**Implementer note**: Link “operations section” to the section in the Markdown when adding to GitHub.)
* **Connection**: Directly handles packets and communicating with the MQTT server via sockets. The connection is where the MQTT operations are directly performed at. The connection also handles keeping the client connected by responding to PING requests from the server, as well as tracking operations and their timeouts.
    * The MQTT client creates a connection. Connections cannot exist without a client.
* **Session**: The period of time where a client is connected to a MQTT server and is able to perform operations. A session ends when the MQTT client is disconnected, shutdown, or otherwise destroyed.
    * Sessions exist only when there is a client and connection, and the connection is actively connected to a MQTT server.

In the CPP SDK, the terminology isn’t quite as important as some MQTT implementations because *in MQTT5 a client can only have a single Connection and Session.* However, it is important to know the terms and what each part means when MQTT5 is discussed. All of your code will interact solely with the MQTT5 client, so if you are unsure, just remember the MQTT5 client is what your code interacts with.

* Note that in MQTT 3.1.1, often referred to simply as “MQTT” in the CPP SDK code, there are some fundamental differences compared to MQTT5. A few of these differences are listed below:
    * In the MQTT 3.1.1 client, the MQTT connection is where your code interacts to perform MQTT operations. In MQTT5, the code instead performs all operations through the MQTT5 client.
    * In the MQTT 3.1.1 client, you can have multiple MQTT connections (and by extension, sessions) per MQTT 3.1.1 client. In MQTT5, each MQTT5 client has only a single connection and session. In MQTT5 you can use multiple clients each with their own connection.
    * In the MQTT 3.1.1 client, when you subscribe to a topic it adds it to a topic tree so when a publish packet comes in, per-topic callbacks will be invoked. In MQTT5, there is not a topic tree, and instead all subscriptions will invoke the same callback when a publish packet comes in.


Additionally, there is some further terms that are important to define in relation to making a connection with MQTT:

* **Endpoint**: The server address or URL of the MQTT server to connect to.
* **Port**: The network port to use for sending packets to and from the device to the server. In CPP SDK, this can often be left alone as the SDK will automatically set the port based on the connection type.

### **What is a Topic**

In MQTT, one of the important functionalities of the MQTT server is to handle sending packets to the proper *topics*. A topic is a string that tells the server where to send a packet when it receives one, so it can properly process or forward the packet on to other clients who are listening for packets on that topic. Topics can be specific or can use wildcards via the asterisk character (*).

With wildcards, this means a client can make a subscription to a topic of “test/hello-*” for example, and then that client will receive all packets sent to “test” that start with “hello-”. For example, a packet sent to “test/hello-world” and a packet sent to “test/hello-/world” would both be received by a client subscribed to “test/hello-*”.

It should be noted that not every packet has a topic, like connection and disconnect packets do not include and a topic.

It should also be noted that topics are extremely important for both the MQTT client and server. The MQTT client needs to know of topics to know where to send data to when performing operations, while the MQTT server needs to process topics to make sure each MQTT client connected gets the proper data as soon as new data is sent to the MQTT server.


### **What is Quality Of Service (QOS)**

Another important term in MQTT is *Quality Of Service*, or *QOS*. QOS dictates how reliable the MQTT packet is and what steps will be taken when it seems data may not be received by the MQTT server. This is important if a client is sending information in a packet that has important data and it is critical to know whether it has to be received by the server or not. There are three levels of QOS and each increasing level means more mechanisms to ensure the server has received the packet.

* **QOS 0**: This level is often referred to as “*at most once*” and means that the MQTT packet will be sent once and that is it, there will be no additional processing to determine if the packet was received.
* **QOS 1**: This level is often referred to as “*at least once*” and means that the MQTT packet will be sent and then the sender (client or server) will wait for an acknowledgement (ACK) to come back. If an ACK is not received within a certain time frame, then the packet will be resent.
    * Packets that have been sent but an ACK not yet received are referred to as “in-flight” packets.
    * QOS 1 does not guarantee that the packet will not be sent more than once, as it will continue to resend the packet at a set interval until an ACK is received.
* **QOS 2**: This level is often referred to as “*exactly once*” and means that the MQTT packet will be sent exactly once and no more. This is done through a series of ACKs on both the sender and receiver’s side to confirm that the packet was sent, arrived at the server, and was successfully processed.
    * **Note**: **QOS 2 is not supported in the CPP SDK at this time.**

The higher the QOS level, the more complicated and slower the complete operation will be due to the extra packets. Packets with higher QOS take longer to be fully processed and require more bandwidth than those with a lower QOS. In general, clients should send packets with the minimum QOS that required for the data being sent for faster speeds and less bandwidth.


### **Allowed Operations**

MQTT clients and servers send packets through *operations*. A MQTT client can perform many different operations, each with a different functionality and *associated* *packet*:

* **Start**: Sends a packet requesting a connection to a server. A connect packet contains information that tells the server who the client is, what settings to use for sending MQTT packets, and what the options the client supports. This packet often also contains authentication information that verifies the client is authorized to connect. This packet performs a handshake with the server that allows the client to perform all other operations describe herein.
    * A connect packet can also include something called a **Will**. A will is a publish sent to a topic when the client disconnects and/or the server feels the client has become disconnected due to not responding to PING requests within the agreed upon time.
    * The client is the only one that can send a connect packet. However, the server will send an acknowledgement packet, often referred to as the ConnAck packet, with information on whether the connection was successful or not. If successful, the ConnAck packet will also include Negotiated-Settings, which detail the settings the server supports and the client can use to communication.
    * **Note**: In the MQTT 3.1.1 client, the Start operation is called **Connect**. Also note that Negotiated-Settings are only sent in MQTT5.
* **Publish**: Sends a packet containing whatever data the client wishes to send to a given topic. A publish can be made with different QOS levels, can be set to be retained (persist on the server).
    * Publish packets can have payloads containing string or binary data.
* **Subscribe**: Sends a packet telling the server that the client wants to get any publish packets sent to a specific topic. A subscribe can be made with different QOS levels and can be configured to get retained messages.
    * **Note**: Publish packets can be received that are sent from both the client and the server, as well as other clients who are also connected to the server.
* **Unsubscribe**: Sends a packet telling the server that the client no longer wants to get publish packet on a specific topic anymore.
* **Stop**: Sends a packet telling the server to disconnect the client from the server. The disconnect packet can contain a reason for the disconnect and some additional options, like whether to send a will.
    * **Note**: In the MQTT 3.1.1 client SDK, the Stop operation is called *Disconnect*.


Operations on QOS1 or higher will cause the server to send an acknowledgement packet, often referred to as an ACK packet, when the client performs an operation. For example, if a client performs a publish operation, the client will send a publish packet to the MQTT server and the server will send back a PubAck packet when the publish has been received. These ACK packets often contain information on whether the operation was successful, and if it was unsuccessful, information on why.

It should also be noted that in MQTT5 most operations and their packets can also include *user properties*. User properties are new to MQTT5 and are key-value pairs that can be set in the packet when a client performs an operation. User properties can be helpful for passing along additional information or context to other MQTT5 clients.


## Further Reading:

Hopefully that has helped give an overview of MQTT and what it is within the CPP SDK. If you are curious to learn more about MQTT, how it works, and what it can do, please see the following links for more information:

* MQTT 5 standard: https://docs.oasis-open.org/mqtt/mqtt/v5.0/os/mqtt-v5.0-os.html
* MQTT 3.1.1 standard: https://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.html

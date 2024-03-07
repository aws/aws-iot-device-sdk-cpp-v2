/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <aws/common/environment.h>
#include <aws/crt/Api.h>
#include <aws/crt/http/HttpProxyStrategy.h>
#include <aws/iotdevicecommon/IotDevice.h>
#include <aws/iotsecuretunneling/SecureTunnel.h>
#include <thread>

using namespace Aws::Crt;
using namespace Aws::Iotsecuretunneling;
using namespace Aws::Crt::Io;
using namespace std::chrono_literals;

AWS_STATIC_STRING_FROM_LITERAL(SECTUN_ENDPOINT, "SECTUN_ENDPOINT");
AWS_STATIC_STRING_FROM_LITERAL(SECTUN_SOURCE_TOKEN, "SECTUN_SOURCE_TOKEN");
AWS_STATIC_STRING_FROM_LITERAL(SECTUN_DESTINATION_TOKEN, "SECTUN_DESTINATION_TOKEN");
AWS_STATIC_STRING_FROM_LITERAL(SECTUN_SOURCE_CLIENT_TOKEN, "SECTUN_SOURCE_CLIENT_TOKEN");
AWS_STATIC_STRING_FROM_LITERAL(SECTUN_DESTINATION_CLIENT_TOKEN, "SECTUN_DESTINATION_CLIENT_TOKEN");
AWS_STATIC_STRING_FROM_LITERAL(SECTUN_PAYLOAD_MESSAGE, "Payload Message");

void setEnvVariable(struct aws_allocator *allocator, const struct aws_string *variable_name, String &stringToSet)
{
    aws_string *awsStringToSet = NULL;
    aws_get_environment_value(allocator, variable_name, &awsStringToSet);
    stringToSet = awsStringToSet == nullptr ? "" : aws_string_c_str(awsStringToSet);
    aws_string_destroy(awsStringToSet);
}

int main(int argc, char *argv[])
{
    fprintf(stdout, "Secure Tunnel Test Starting\n");
    struct aws_allocator *allocator = aws_default_allocator();
    ApiHandle apiHandle;

    /* Uncomment to produce logs in codebuild */
    // apiHandle.InitializeLogging(Aws::Crt::LogLevel::Trace, stderr);

    aws_iotdevice_library_init(allocator);

    /* service id storage for use in test */
    Aws::Crt::ByteBuf m_serviceIdStorage;
    AWS_ZERO_STRUCT(m_serviceIdStorage);
    Aws::Crt::Optional<Aws::Crt::ByteCursor> m_serviceId;

    std::promise<void> promiseDestinationConnected;
    std::promise<void> promiseSourceConnected;

    std::promise<void> promiseDestinationStreamStarted;
    std::promise<void> promiseDestinationConnectionStarted;

    std::promise<void> promiseDestinationReceivedMessage;
    std::promise<void> promiseSourceReceivedMessage;

    std::promise<void> promiseDestinationStopped;
    std::promise<void> promiseSourceStopped;

    String endpoint;
    String destinationToken;
    String sourceToken;
    String destinationClientToken;
    String sourceClientToken;
    /* Connection Id is used for Simultaneous HTTP Connections (Protocl V3) */
    uint32_t connectionId = 1;
    uint32_t connectionId2 = 2;

    setEnvVariable(allocator, SECTUN_DESTINATION_TOKEN, destinationToken);
    setEnvVariable(allocator, SECTUN_SOURCE_TOKEN, sourceToken);
    setEnvVariable(allocator, SECTUN_DESTINATION_CLIENT_TOKEN, destinationClientToken);
    setEnvVariable(allocator, SECTUN_SOURCE_CLIENT_TOKEN, sourceClientToken);
    setEnvVariable(allocator, SECTUN_ENDPOINT, endpoint);

    if (apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError() != AWS_ERROR_SUCCESS)
    {
        fprintf(
            stderr,
            "ClientBootstrap failed with error %s\n",
            ErrorDebugString(apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError()));
        exit(-1);
    }

    /* Use a SecureTunnelBuilder to set up and build the secure tunnel clients */
    SecureTunnelBuilder builderDestination = SecureTunnelBuilder(
        allocator, destinationToken.c_str(), AWS_SECURE_TUNNELING_DESTINATION_MODE, endpoint.c_str());
    SecureTunnelBuilder builderSource =
        SecureTunnelBuilder(allocator, sourceToken.c_str(), AWS_SECURE_TUNNELING_SOURCE_MODE, endpoint.c_str());

    if (destinationClientToken.length() > 0)
    {
        builderDestination.WithClientToken(destinationClientToken.c_str());
    }
    if (sourceClientToken.length() > 0)
    {
        builderSource.WithClientToken(sourceClientToken.c_str());
    }

    builderDestination.WithOnMessageReceived(
        [&](SecureTunnel *secureTunnel, const MessageReceivedEventData &eventData) {
            {
                (void)secureTunnel;
                (void)eventData;
                fprintf(stdout, "Destination Client Received Message\n");
                promiseDestinationReceivedMessage.set_value();
            }
        });

    builderSource.WithOnMessageReceived([&](SecureTunnel *secureTunnel, const MessageReceivedEventData &eventData) {
        {
            (void)secureTunnel;
            (void)eventData;
            fprintf(stdout, "Source Client Received Message\n");
            promiseSourceReceivedMessage.set_value();
        }
    });

    builderDestination.WithOnSendMessageComplete(
        [&](SecureTunnel *secureTunnel, int errorCode, const SendMessageCompleteEventData &eventData) {
            (void)secureTunnel;
            (void)eventData;

            if (!errorCode)
            {
                fprintf(
                    stdout,
                    "Message of type '" PRInSTR "' sent successfully\n",
                    AWS_BYTE_CURSOR_PRI(eventData.sendMessageCompleteData->getMessageType()));
            }
            else
            {
                fprintf(stdout, "Send Message failed with error code %d(%s)\n", errorCode, ErrorDebugString(errorCode));
                aws_byte_buf_clean_up(&m_serviceIdStorage);
                exit(-1);
            }
        });

    builderSource.WithOnSendMessageComplete(
        [&](SecureTunnel *secureTunnel, int errorCode, const SendMessageCompleteEventData &eventData) {
            (void)secureTunnel;
            (void)eventData;

            if (!errorCode)
            {
                fprintf(
                    stdout,
                    "Message of type '" PRInSTR "' sent successfully\n",
                    AWS_BYTE_CURSOR_PRI(eventData.sendMessageCompleteData->getMessageType()));
            }
            else
            {
                fprintf(stdout, "Send Message failed with error code %d(%s)\n", errorCode, ErrorDebugString(errorCode));
                aws_byte_buf_clean_up(&m_serviceIdStorage);
                exit(-1);
            }
        });

    builderDestination.WithOnConnectionSuccess(
        [&](SecureTunnel *secureTunnel, const ConnectionSuccessEventData &eventData) {
            (void)secureTunnel;
            (void)eventData;
            fprintf(stdout, "Destination Client Connection Success\n");
            promiseDestinationConnected.set_value();
        });

    builderSource.WithOnConnectionSuccess([&](SecureTunnel *secureTunnel, const ConnectionSuccessEventData &eventData) {
        (void)secureTunnel;
        (void)eventData;

        fprintf(stdout, "Source Client Connection Success\n");

        /* Use a Multiplexing (Service Id) if available on this Secure Tunnel */
        if (eventData.connectionData->getServiceId1().has_value())
        {
            /* Store the service id for future use */
            aws_byte_buf_clean_up(&m_serviceIdStorage);
            AWS_ZERO_STRUCT(m_serviceIdStorage);
            aws_byte_buf_init_copy_from_cursor(
                &m_serviceIdStorage, allocator, eventData.connectionData->getServiceId1().value());
            m_serviceId = aws_byte_cursor_from_buf(&m_serviceIdStorage);
            secureTunnel->SendStreamStart(m_serviceId.value(), connectionId);
            fprintf(stdout, "Stream Start sent from Source Client.\n");
        }
        else
        {
            fprintf(stdout, "Secure Tunnel should have service ids set for proper testing\n");
            exit(-1);
        }

        promiseSourceConnected.set_value();
    });

    builderDestination.WithOnStreamStarted(
        [&](SecureTunnel *secureTunnel, int errorCode, const StreamStartedEventData &eventData) {
            (void)secureTunnel;
            (void)eventData;
            if (!errorCode)
            {
                fprintf(stdout, "Destination Client Stream Started with Source Client\n");
                promiseDestinationStreamStarted.set_value();
            }
            else
            {
                fprintf(stdout, "Stream Start failed with error code %d(%s)\n", errorCode, ErrorDebugString(errorCode));
                aws_byte_buf_clean_up(&m_serviceIdStorage);
                exit(-1);
            }
        });

    builderDestination.WithOnConnectionStarted([&](SecureTunnel *secureTunnel,
                                                   int errorCode,
                                                   const ConnectionStartedEventData &eventData) {
        (void)secureTunnel;
        (void)eventData;
        if (!errorCode)
        {
            fprintf(stdout, "Connection Started on Destination Client.\n");
            promiseDestinationConnectionStarted.set_value();
        }
        else
        {
            fprintf(stdout, "Connection Start failed with error code %d(%s)\n", errorCode, ErrorDebugString(errorCode));
            aws_byte_buf_clean_up(&m_serviceIdStorage);
            exit(-1);
        }
    });

    builderDestination.WithOnConnectionShutdown([&]() { fprintf(stdout, "Destination Connection Shutdown\n"); });

    builderSource.WithOnConnectionShutdown([&]() { fprintf(stdout, "Source Connection Shutdown\n"); });

    builderDestination.WithOnStopped([&](SecureTunnel *secureTunnel) {
        (void)secureTunnel;
        fprintf(stdout, "Destination entered Stopped State\n");
        promiseDestinationStopped.set_value();
    });

    builderSource.WithOnStopped([&](SecureTunnel *secureTunnel) {
        (void)secureTunnel;
        fprintf(stdout, "Source has entered Stopped State\n");
        promiseSourceStopped.set_value();
    });

    /* Create Secure Tunnel using the options set with the builder */
    std::shared_ptr<SecureTunnel> secureTunnelDestination = builderDestination.Build();

    if (!secureTunnelDestination)
    {
        fprintf(stderr, "Secure Tunnel Destination Creation failed: %s\n", ErrorDebugString(LastError()));
        exit(-1);
    }

    std::shared_ptr<SecureTunnel> secureTunnelSource = builderSource.Build();
    if (!secureTunnelSource)
    {
        fprintf(stderr, "Secure Tunnel Source Creation failed: %s\n", ErrorDebugString(LastError()));
        exit(-1);
    }

    fprintf(stdout, "Secure Tunnels Created\n");

    /* Set the Secure Tunnel Client to desire a connected state */

    if (secureTunnelDestination->Start())
    {
        fprintf(stderr, "Secure Tunnel Destination Connect call failed: %s\n", ErrorDebugString(LastError()));
        exit(-1);
    }

    promiseDestinationConnected.get_future().wait();

    if (secureTunnelSource->Start())
    {
        fprintf(stderr, "Secure Tunnel Source Connect call failed: %s\n", ErrorDebugString(LastError()));
        exit(-1);
    }

    promiseSourceConnected.get_future().wait();

    promiseDestinationStreamStarted.get_future().wait();

    secureTunnelSource->SendConnectionStart(m_serviceId.value(), connectionId2);

    promiseDestinationConnectionStarted.get_future().wait();

    std::shared_ptr<Message> message1 =
        std::make_shared<Message>(ByteCursorFromCString(aws_string_c_str(SECTUN_PAYLOAD_MESSAGE)));
    message1->WithServiceId(m_serviceId.value());
    message1->WithConnectionId(connectionId2);
    secureTunnelSource->SendMessage(message1);
    fprintf(stdout, "Source Client Sent Message\n");

    promiseDestinationReceivedMessage.get_future().wait();

    std::shared_ptr<Message> message2 =
        std::make_shared<Message>(ByteCursorFromCString(aws_string_c_str(SECTUN_PAYLOAD_MESSAGE)));
    message2->WithServiceId(m_serviceId.value());
    message2->WithConnectionId(connectionId);
    secureTunnelDestination->SendMessage(message2);
    fprintf(stdout, "Destination Client Sent Message\n");

    promiseSourceReceivedMessage.get_future().wait();

    if (secureTunnelDestination->Stop() == AWS_OP_ERR)
    {
        fprintf(stderr, "Secure Tunnel Destination Stop call failed: %s\n", ErrorDebugString(LastError()));
        aws_byte_buf_clean_up(&m_serviceIdStorage);
        exit(-1);
    }

    promiseDestinationStopped.get_future().wait();
    secureTunnelDestination = nullptr;

    if (secureTunnelSource->Stop() == AWS_OP_ERR)
    {
        fprintf(stderr, "Secure Tunnel Source Stop call failed: %s\n", ErrorDebugString(LastError()));
        aws_byte_buf_clean_up(&m_serviceIdStorage);
        exit(-1);
    }

    promiseSourceStopped.get_future().wait();
    secureTunnelSource = nullptr;

    /* Clean Up */
    aws_byte_buf_clean_up(&m_serviceIdStorage);

    fprintf(stdout, "Secure Tunnel Test Completed\n");

    return 0;
}

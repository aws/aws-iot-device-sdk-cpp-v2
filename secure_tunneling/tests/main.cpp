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

using namespace Aws::Crt;
using namespace Aws::Iotsecuretunneling;
using namespace Aws::Crt::Io;
using namespace std::chrono_literals;

AWS_STATIC_STRING_FROM_LITERAL(SECTUN_ENDPOINT, "SECTUN_ENDPOINT");
AWS_STATIC_STRING_FROM_LITERAL(SECTUN_SOURCE_TOKEN, "SECTUN_SOURCE_TOKEN");
AWS_STATIC_STRING_FROM_LITERAL(SECTUN_DESTINATION_TOKEN, "SECTUN_DESTINATION_TOKEN");

void setEnvVariable(struct aws_allocator *allocator, const struct aws_string *variable_name, String stringToSet)
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
    // Logging
    // apiHandle.InitializeLogging(Aws::Crt::LogLevel::Trace, stderr);

    aws_iotdevice_library_init(allocator);

    /* service id storage for use in test */
    Aws::Crt::ByteBuf m_serviceIdStorage;
    AWS_ZERO_STRUCT(m_serviceIdStorage);
    Aws::Crt::Optional<Aws::Crt::ByteCursor> m_serviceId;

    /*
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<bool> clientStoppedPromise;

    String endpoint;
    String accessToken;
    String destinationToken;
    String sourceToken;
    aws_secure_tunneling_local_proxy_mode localProxyMode = AWS_SECURE_TUNNELING_DESTINATION_MODE;
    /* Connection Id is used for Simultaneous HTTP Connections (Protocl V3) */
    uint32_t connectionId = 1;

    setEnvVariable(allocator, SECTUN_DESTINATION_TOKEN, destinationToken);
    setEnvVariable(allocator, SECTUN_SOURCE_TOKEN, sourceToken);
    setEnvVariable(allocator, SECTUN_ENDPOINT, endpoint);

    // aws_string *aws_string_endpoint = NULL;

    // aws_get_environment_value(allocator, SECTUN_ENDPOINT, &aws_string_endpoint);
    // endpoint = aws_string_endpoint == nullptr ? "" : aws_string_c_str(aws_string_endpoint);
    // aws_string_destroy(aws_string_endpoint);

    fprintf(stdout, "endpoint:%s\n", endpoint.c_str());
    fprintf(stdout, "source token:%s\n", sourceToken.c_str());
    fprintf(stdout, "destination token:%s\n", destinationToken.c_str());

    if (apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError() != AWS_ERROR_SUCCESS)
    {
        fprintf(
            stderr,
            "ClientBootstrap failed with error %s\n",
            ErrorDebugString(apiHandle.GetOrCreateStaticDefaultClientBootstrap()->LastError()));
        exit(-1);
    }

    /* Use a SecureTunnelBuilder to set up and build the secure tunnel client */
    SecureTunnelBuilder builder =
        SecureTunnelBuilder(allocator, destinationToken.c_str(), localProxyMode, endpoint.c_str());

    // builder.WithClientToken(clientToket.c_str());

    builder.WithOnMessageReceived([&](SecureTunnel *secureTunnel, const MessageReceivedEventData &eventData) {
        {
            (void)secureTunnel;
            (void)eventData;
        }
    });

    builder.WithOnSendMessageComplete(
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
            }
        });

    builder.WithOnConnectionSuccess([&](SecureTunnel *secureTunnel, const ConnectionSuccessEventData &eventData) {
        (void)secureTunnel;
        (void)eventData;

        /* Stream Start can only be called from Source Mode */
        if (localProxyMode == AWS_SECURE_TUNNELING_SOURCE_MODE)
        {
            /* Use a Multiplexing (Service Id) if available on this Secure Tunnel */
            if (eventData.connectionData->getServiceId1().has_value())
            {
                /* Store the service id for future use */
                aws_byte_buf_clean_up(&m_serviceIdStorage);
                AWS_ZERO_STRUCT(m_serviceIdStorage);
                aws_byte_buf_init_copy_from_cursor(
                    &m_serviceIdStorage, allocator, eventData.connectionData->getServiceId1().value());
                m_serviceId = aws_byte_cursor_from_buf(&m_serviceIdStorage);
                secureTunnel->SendStreamStart(eventData.connectionData->getServiceId1().value(), connectionId);
            }
            else
            {
                fprintf(stdout, "Sending Stream Start request\n");
                secureTunnel->SendStreamStart();
            }
        }
    });

    builder.WithOnStreamStarted(
        [&](SecureTunnel *secureTunnel, int errorCode, const StreamStartedEventData &eventData) {
            (void)secureTunnel;
            (void)eventData;
            if (!errorCode)
            {
                // logStreamStartData(eventData);
            }
            else
            {
                fprintf(stdout, "Stream Start failed with error code %d(%s)\n", errorCode, ErrorDebugString(errorCode));
            }
        });

    builder.WithOnConnectionStarted([&](SecureTunnel *secureTunnel,
                                        int errorCode,
                                        const ConnectionStartedEventData &eventData) {
        (void)secureTunnel;
        (void)eventData;
        if (!errorCode)
        {
            // logConnectionStartedData(eventData);
        }
        else
        {
            fprintf(stdout, "Connection Start failed with error code %d(%s)\n", errorCode, ErrorDebugString(errorCode));
        }
    });

    builder.WithOnStreamStopped([&](SecureTunnel *secureTunnel, const StreamStoppedEventData &eventData) {
        (void)secureTunnel;

        // logStreamStoppedData(eventData);
    });

    builder.WithOnConnectionShutdown([&]() { fprintf(stdout, "Connection Shutdown\n"); });

    builder.WithOnStopped([&](SecureTunnel *secureTunnel) {
        (void)secureTunnel;
        fprintf(stdout, "Secure Tunnel has entered Stopped State\n");
        clientStoppedPromise.set_value(true);
    });

    /* Create Secure Tunnel using the options set with the builder */
    std::shared_ptr<SecureTunnel> secureTunnel = builder.Build();

    if (!secureTunnel)
    {
        fprintf(stderr, "Secure Tunnel Creation failed: %s\n", ErrorDebugString(LastError()));
        exit(-1);
    }

    fprintf(stdout, "Secure Tunnel Created\n");

    /* Set the Secure Tunnel Client to desire a connected state */
    // if (secureTunnel->Start())
    // {
    //     fprintf(stderr, "Secure Tunnel Connect call failed: %s\n", ErrorDebugString(LastError()));
    //     exit(-1);
    // }

    // std::this_thread::sleep_for(3000ms);

    // fprintf(stdout, "Closing Connection\n");
    // /* Set the Secure Tunnel Client to desire a stopped state */
    // if (secureTunnel->Stop() == AWS_OP_ERR)
    // {
    //     fprintf(stderr, "Secure Tunnel Stop call failed: %s\n", ErrorDebugString(LastError()));
    //     exit(-1);
    // }

    // /* The Secure Tunnel Client at this point will report they are stopped and can be safely removed. */
    // if (clientStoppedPromise.get_future().get())
    // {
    //     secureTunnel = nullptr;
    // }

    secureTunnel = nullptr;

    /* Clean Up */
    aws_byte_buf_clean_up(&m_serviceIdStorage);

    fprintf(stdout, "Secure Tunnel Test Completed\n");

    return 0;
}

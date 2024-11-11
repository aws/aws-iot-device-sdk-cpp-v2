/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/environment.h>
#include <aws/crt/Api.h>
#include <aws/crt/JsonObject.h>
#include <aws/crt/Types.h>
#include <aws/crt/UUID.h>
#include <aws/crt/mqtt/Mqtt5Packets.h>
#include <aws/iot/MqttRequestResponseClient.h>
#include <aws/testing/aws_test_harness.h>

#include <aws/iotjobs/IotJobsClientV2.h>

#include <utility>

AWS_STATIC_STRING_FROM_LITERAL(s_rrEnvVariableHost, "AWS_TEST_MQTT5_IOT_CORE_HOST");
AWS_STATIC_STRING_FROM_LITERAL(s_rrEnvVariableCertificatePath, "AWS_TEST_MQTT5_IOT_CORE_RSA_CERT");
AWS_STATIC_STRING_FROM_LITERAL(s_rrEnvVariablePrivateKeyPath, "AWS_TEST_MQTT5_IOT_CORE_RSA_KEY");

static std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> s_createProtocolClient5(Aws::Crt::Allocator *allocator)
{
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> client = nullptr;

    struct aws_string *host = NULL;
    struct aws_string *certificatePath = NULL;
    struct aws_string *privateKeyPath = NULL;

    if (aws_get_environment_value(allocator, s_rrEnvVariableHost, &host) || host == nullptr)
    {
        goto done;
    }

    if (aws_get_environment_value(allocator, s_rrEnvVariableCertificatePath, &certificatePath) ||
        certificatePath == nullptr)
    {
        goto done;
    }

    if (aws_get_environment_value(allocator, s_rrEnvVariablePrivateKeyPath, &privateKeyPath) ||
        privateKeyPath == nullptr)
    {
        goto done;
    }

    {
        std::mutex lock;
        std::condition_variable signal;
        bool connected = false;

        Aws::Crt::Io::TlsContextOptions tlsCtxOptions = Aws::Crt::Io::TlsContextOptions::InitClientWithMtls(
            aws_string_c_str(certificatePath), aws_string_c_str(privateKeyPath), allocator);
        Aws::Crt::Io::TlsContext tlsContext(tlsCtxOptions, Aws::Crt::Io::TlsMode::CLIENT, allocator);

        Aws::Crt::Mqtt5::Mqtt5ClientOptions mqtt5Options(allocator);
        mqtt5Options.WithHostName(Aws::Crt::String(aws_string_c_str(host)));
        mqtt5Options.WithPort(8883);
        mqtt5Options.WithTlsConnectionOptions(tlsContext.NewConnectionOptions());
        mqtt5Options.WithClientConnectionSuccessCallback(
            [&lock, &signal, &connected](const Aws::Crt::Mqtt5::OnConnectionSuccessEventData &)
            {
                {
                    std::lock_guard<std::mutex> guard(lock);
                    connected = true;
                }
                signal.notify_all();
            });

        client = Aws::Crt::Mqtt5::Mqtt5Client::NewMqtt5Client(mqtt5Options, allocator);
        client->Start();

        std::unique_lock<std::mutex> waitLock(lock);
        signal.wait(waitLock, [&connected]() { return connected; });
    }

done:

    aws_string_destroy(host);
    aws_string_destroy(certificatePath);
    aws_string_destroy(privateKeyPath);

    return client;
}

static std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> s_createProtocolClient311(Aws::Crt::Allocator *allocator)
{
    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> connection = nullptr;

    struct aws_string *host = NULL;
    struct aws_string *certificatePath = NULL;
    struct aws_string *privateKeyPath = NULL;

    if (aws_get_environment_value(allocator, s_rrEnvVariableHost, &host) || host == nullptr)
    {
        goto done;
    }

    if (aws_get_environment_value(allocator, s_rrEnvVariableCertificatePath, &certificatePath) ||
        certificatePath == nullptr)
    {
        goto done;
    }

    if (aws_get_environment_value(allocator, s_rrEnvVariablePrivateKeyPath, &privateKeyPath) ||
        privateKeyPath == nullptr)
    {
        goto done;
    }

    {
        std::mutex lock;
        std::condition_variable signal;
        bool connected = false;

        Aws::Crt::Io::TlsContextOptions tlsCtxOptions = Aws::Crt::Io::TlsContextOptions::InitClientWithMtls(
            aws_string_c_str(certificatePath), aws_string_c_str(privateKeyPath), allocator);
        Aws::Crt::Io::TlsContext tlsContext(tlsCtxOptions, Aws::Crt::Io::TlsMode::CLIENT, allocator);

        Aws::Crt::Io::SocketOptions socketOptions;
        socketOptions.SetConnectTimeoutMs(3000);

        Aws::Crt::Mqtt::MqttClient client;
        connection = client.NewConnection(aws_string_c_str(host), 8883, socketOptions, tlsContext, false);

        connection->OnConnectionSuccess =
            [&connected, &lock, &signal](
                Aws::Crt::Mqtt::MqttConnection &, Aws::Crt::Mqtt::OnConnectionSuccessData *)
        {
            {
                std::lock_guard<std::mutex> guard(lock);
                connected = true;
            }
            signal.notify_all();
        };

        auto uuid = Aws::Crt::UUID().ToString();
        connection->Connect(uuid.c_str(), true, 30, 15000, 5000);

        std::unique_lock<std::mutex> waitLock(lock);
        signal.wait(waitLock, [&connected]() { return connected; });
    }

done:

    aws_string_destroy(host);
    aws_string_destroy(certificatePath);
    aws_string_destroy(privateKeyPath);

    return connection;
}

template <typename R> class ResultWaiter
{
  public:
    ResultWaiter() = default;

    void ApplyResult(R &&result)
    {
        {
            std::lock_guard<std::mutex> guard(m_lock);

            if (m_result.has_value())
            {
                return;
            }

            m_result = std::move(result);
        }

        m_signal.notify_all();
    }

    const R &GetResult()
    {
        {
            std::unique_lock<std::mutex> guard(m_lock);
            m_signal.wait(guard, [this]() { return this->m_result.has_value(); });
        }

        return m_result.value();
    }

  private:
    std::mutex m_lock;
    std::condition_variable m_signal;

    Aws::Crt::Optional<R> m_result;
};

static std::shared_ptr<Aws::Iotjobs::IClientV2> s_createIdentityClient5(
    std::shared_ptr<Aws::Crt::Mqtt5::Mqtt5Client> protocolClient,
    Aws::Crt::Allocator *allocator)
{
    if (!protocolClient)
    {
        return nullptr;
    }

    Aws::Iot::RequestResponse::RequestResponseClientOptions serviceClientOptions;

    serviceClientOptions.WithMaxRequestResponseSubscriptions(4);
    serviceClientOptions.WithMaxStreamingSubscriptions(2);
    serviceClientOptions.WithOperationTimeoutInSeconds(5);

    return Aws::Iotjobs::NewClientFrom5(*protocolClient, serviceClientOptions, allocator);
}

static std::shared_ptr<Aws::Iotjobs::IClientV2> s_createShadowClient311(
    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> protocolClient,
    Aws::Crt::Allocator *allocator)
{
    if (!protocolClient)
    {
        return nullptr;
    }

    Aws::Iot::RequestResponse::RequestResponseClientOptions serviceClientOptions;

    serviceClientOptions.WithMaxRequestResponseSubscriptions(4);
    serviceClientOptions.WithMaxStreamingSubscriptions(2);
    serviceClientOptions.WithOperationTimeoutInSeconds(5);

    return Aws::Iotjobs::NewClientFrom311(*protocolClient, serviceClientOptions, allocator);
}

static int s_JobsV2ClientCreateDestroy5(Aws::Crt::Allocator *allocator, void *)
{
    {
        Aws::Crt::ApiHandle handle;

        auto protocolClient = s_createProtocolClient5(allocator);
        auto shadowClient = s_createIdentityClient5(protocolClient, allocator);
        if (!shadowClient)
        {
            return AWS_OP_SKIP;
        }
    }

    return AWS_OP_SUCCESS;
}
AWS_TEST_CASE(JobsV2ClientCreateDestroy5, s_JobsV2ClientCreateDestroy5)

static int s_JobsV2ClientCreateDestroy311(Aws::Crt::Allocator *allocator, void *)
{
    {
        Aws::Crt::ApiHandle handle;

        auto protocolClient = s_createProtocolClient311(allocator);
        auto shadowClient = s_createShadowClient311(protocolClient, allocator);
        if (!shadowClient)
        {
            return AWS_OP_SKIP;
        }
    }

    return AWS_OP_SUCCESS;
}
AWS_TEST_CASE(JobsV2ClientCreateDestroy311, s_JobsV2ClientCreateDestroy311)

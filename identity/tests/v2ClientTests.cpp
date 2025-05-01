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
#include <aws/iotidentity/CreateCertificateFromCsrRequest.h>
#include <aws/iotidentity/CreateCertificateFromCsrResponse.h>
#include <aws/iotidentity/CreateKeysAndCertificateRequest.h>
#include <aws/iotidentity/CreateKeysAndCertificateResponse.h>
#include <aws/iotidentity/IotIdentityClientV2.h>
#include <aws/iotidentity/RegisterThingRequest.h>
#include <aws/iotidentity/RegisterThingResponse.h>
#include <aws/iotidentity/V2ErrorResponse.h>
#include <aws/testing/aws_test_harness.h>

#include <fstream>
#include <utility>

AWS_STATIC_STRING_FROM_LITERAL(s_rrEnvVariableHost, "AWS_TEST_IOT_CORE_PROVISIONING_HOST");
AWS_STATIC_STRING_FROM_LITERAL(s_rrEnvVariableCertificatePath, "AWS_TEST_IOT_CORE_PROVISIONING_CERTIFICATE_PATH");
AWS_STATIC_STRING_FROM_LITERAL(s_rrEnvVariablePrivateKeyPath, "AWS_TEST_IOT_CORE_PROVISIONING_KEY_PATH");
AWS_STATIC_STRING_FROM_LITERAL(s_rrEnvVariableProvisioningTemplateName, "AWS_TEST_IOT_CORE_PROVISIONING_TEMPLATE_NAME");
AWS_STATIC_STRING_FROM_LITERAL(s_rrEnvVariableProvisioningCsrPath, "AWS_TEST_IOT_CORE_PROVISIONING_CSR_PATH");

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

        Aws::Crt::String clientId = Aws::Crt::String("test-") + Aws::Crt::UUID().ToString();
        auto connectOptions = Aws::Crt::MakeShared<Aws::Crt::Mqtt5::ConnectPacket>(allocator);
        connectOptions->WithClientId(clientId);

        Aws::Crt::Mqtt5::Mqtt5ClientOptions mqtt5Options(allocator);
        mqtt5Options.WithHostName(Aws::Crt::String(aws_string_c_str(host)));
        mqtt5Options.WithPort(8883);
        mqtt5Options.WithTlsConnectionOptions(tlsContext.NewConnectionOptions());
        mqtt5Options.WithConnectOptions(connectOptions);

        mqtt5Options.WithClientConnectionSuccessCallback(
            [&lock, &signal, &connected](const Aws::Crt::Mqtt5::OnConnectionSuccessEventData &)
            {
                {
                    std::lock_guard<std::mutex> guard(lock);
                    connected = true;
                    signal.notify_all();
                }
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
            [&connected, &lock, &signal](Aws::Crt::Mqtt::MqttConnection &, Aws::Crt::Mqtt::OnConnectionSuccessData *)
        {
            {
                std::lock_guard<std::mutex> guard(lock);
                connected = true;
                signal.notify_all();
            }
        };

        auto clientId = "test-" + Aws::Crt::UUID().ToString();
        connection->Connect(clientId.c_str(), true, 30, 15000, 5000);

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
            m_signal.notify_all();
        }
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

static std::shared_ptr<Aws::Iotidentity::IClientV2> s_createIdentityClient5(
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

    return Aws::Iotidentity::NewClientFrom5(*protocolClient, serviceClientOptions, allocator);
}

static std::shared_ptr<Aws::Iotidentity::IClientV2> s_createIdentityClient311(
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

    return Aws::Iotidentity::NewClientFrom311(*protocolClient, serviceClientOptions, allocator);
}

static int s_IdentityV2ClientCreateDestroy5(Aws::Crt::Allocator *allocator, void *)
{
    {
        Aws::Crt::ApiHandle handle;

        auto protocolClient = s_createProtocolClient5(allocator);
        auto identityClient = s_createIdentityClient5(protocolClient, allocator);
        if (!identityClient)
        {
            return AWS_OP_SKIP;
        }
    }

    return AWS_OP_SUCCESS;
}
AWS_TEST_CASE(IdentityV2ClientCreateDestroy5, s_IdentityV2ClientCreateDestroy5)

static int s_IdentityV2ClientCreateDestroy311(Aws::Crt::Allocator *allocator, void *)
{
    {
        Aws::Crt::ApiHandle handle;

        auto protocolClient = s_createProtocolClient311(allocator);
        auto identityClient = s_createIdentityClient311(protocolClient, allocator);
        if (!identityClient)
        {
            return AWS_OP_SKIP;
        }
    }

    return AWS_OP_SUCCESS;
}
AWS_TEST_CASE(IdentityV2ClientCreateDestroy311, s_IdentityV2ClientCreateDestroy311)

static int s_doProvisionCertKeyTest(
    std::shared_ptr<Aws::Iotidentity::IClientV2> client,
    const Aws::Crt::String &templateName)
{

    ResultWaiter<Aws::Iotidentity::CreateKeysAndCertificateResult> createResult;
    Aws::Iotidentity::CreateKeysAndCertificateRequest createKeysRequest;

    client->CreateKeysAndCertificate(
        createKeysRequest,
        [&createResult](Aws::Iotidentity::CreateKeysAndCertificateResult &&result)
        { createResult.ApplyResult(std::move(result)); });

    const auto &result = createResult.GetResult();
    ASSERT_TRUE(result.IsSuccess());

    const auto &response = result.GetResponse();
    ASSERT_TRUE(response.CertificateId.has_value());
    ASSERT_TRUE(response.CertificatePem.has_value());
    ASSERT_TRUE(response.PrivateKey.has_value());
    ASSERT_TRUE(response.CertificateOwnershipToken.has_value());

    ResultWaiter<Aws::Iotidentity::RegisterThingResult> registerResult;
    Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String> params;
    params.emplace(Aws::Crt::String("SerialNumber"), Aws::Crt::UUID().ToString());
    params.emplace(Aws::Crt::String("DeviceLocation"), Aws::Crt::String("Seattle"));

    Aws::Iotidentity::RegisterThingRequest registerRequest;
    registerRequest.CertificateOwnershipToken = response.CertificateOwnershipToken.value();
    registerRequest.TemplateName = templateName;
    registerRequest.Parameters = params;

    client->RegisterThing(
        registerRequest,
        [&registerResult](Aws::Iotidentity::RegisterThingResult &&result)
        { registerResult.ApplyResult(std::move(result)); });

    ASSERT_TRUE(registerResult.GetResult().IsSuccess());

    return AWS_OP_SUCCESS;
}

static int s_IdentityV2ClientProvisionWithCertAndKey5(Aws::Crt::Allocator *allocator, void *)
{
    {
        Aws::Crt::ApiHandle handle;

        struct aws_string *template_name = NULL;
        if (aws_get_environment_value(allocator, s_rrEnvVariableProvisioningTemplateName, &template_name) ||
            template_name == nullptr)
        {
            return AWS_OP_SKIP;
        }

        Aws::Crt::String templateName(aws_string_c_str(template_name));
        aws_string_destroy(template_name);

        auto protocolClient = s_createProtocolClient5(allocator);
        auto identityClient = s_createIdentityClient5(protocolClient, allocator);
        if (!identityClient)
        {
            return AWS_OP_SKIP;
        }

        return s_doProvisionCertKeyTest(identityClient, templateName);
    }
}
AWS_TEST_CASE(IdentityV2ClientProvisionWithCertAndKey5, s_IdentityV2ClientProvisionWithCertAndKey5)

static int s_IdentityV2ClientProvisionWithCertAndKey311(Aws::Crt::Allocator *allocator, void *)
{
    {
        Aws::Crt::ApiHandle handle;

        struct aws_string *template_name = NULL;
        if (aws_get_environment_value(allocator, s_rrEnvVariableProvisioningTemplateName, &template_name) ||
            template_name == nullptr)
        {
            return AWS_OP_SKIP;
        }

        Aws::Crt::String templateName(aws_string_c_str(template_name));
        aws_string_destroy(template_name);

        auto protocolClient = s_createProtocolClient5(allocator);
        auto identityClient = s_createIdentityClient5(protocolClient, allocator);
        if (!identityClient)
        {
            return AWS_OP_SKIP;
        }

        return s_doProvisionCertKeyTest(identityClient, templateName);
    }
}
AWS_TEST_CASE(IdentityV2ClientProvisionWithCertAndKey311, s_IdentityV2ClientProvisionWithCertAndKey311)

static int s_doProvisionCsrTest(
    std::shared_ptr<Aws::Iotidentity::IClientV2> client,
    const Aws::Crt::String &templateName,
    const Aws::Crt::String &csr)
{

    ResultWaiter<Aws::Iotidentity::CreateCertificateFromCsrResult> createResult;
    Aws::Iotidentity::CreateCertificateFromCsrRequest createCsrRequest;
    createCsrRequest.CertificateSigningRequest = csr;

    client->CreateCertificateFromCsr(
        createCsrRequest,
        [&createResult](Aws::Iotidentity::CreateCertificateFromCsrResult &&result)
        { createResult.ApplyResult(std::move(result)); });

    const auto &result = createResult.GetResult();
    ASSERT_TRUE(result.IsSuccess());

    const auto &response = result.GetResponse();
    ASSERT_TRUE(response.CertificateId.has_value());
    ASSERT_TRUE(response.CertificatePem.has_value());
    ASSERT_TRUE(response.CertificateOwnershipToken.has_value());

    ResultWaiter<Aws::Iotidentity::RegisterThingResult> registerResult;
    Aws::Crt::Map<Aws::Crt::String, Aws::Crt::String> params;
    params.emplace(Aws::Crt::String("SerialNumber"), Aws::Crt::UUID().ToString());
    params.emplace(Aws::Crt::String("DeviceLocation"), Aws::Crt::String("Seattle"));

    Aws::Iotidentity::RegisterThingRequest registerRequest;
    registerRequest.CertificateOwnershipToken = response.CertificateOwnershipToken.value();
    registerRequest.TemplateName = templateName;
    registerRequest.Parameters = params;

    client->RegisterThing(
        registerRequest,
        [&registerResult](Aws::Iotidentity::RegisterThingResult &&result)
        { registerResult.ApplyResult(std::move(result)); });

    ASSERT_TRUE(registerResult.GetResult().IsSuccess());

    return AWS_OP_SUCCESS;
}

static int s_IdentityV2ClientProvisionWithCSR5(Aws::Crt::Allocator *allocator, void *)
{
    {
        Aws::Crt::ApiHandle handle;

        struct aws_string *template_name = NULL;
        if (aws_get_environment_value(allocator, s_rrEnvVariableProvisioningTemplateName, &template_name) ||
            template_name == nullptr)
        {
            return AWS_OP_SKIP;
        }

        Aws::Crt::String templateName(aws_string_c_str(template_name));
        aws_string_destroy(template_name);

        struct aws_string *csr_path = NULL;
        if (aws_get_environment_value(allocator, s_rrEnvVariableProvisioningCsrPath, &csr_path) || csr_path == nullptr)
        {
            return AWS_OP_SKIP;
        }

        Aws::Crt::String csrPath(aws_string_c_str(csr_path));
        aws_string_destroy(csr_path);

        std::ifstream ifs(csrPath.c_str());
        std::string csr;
        getline(ifs, csr, (char)ifs.eof());

        auto protocolClient = s_createProtocolClient5(allocator);
        auto identityClient = s_createIdentityClient5(protocolClient, allocator);
        if (!identityClient)
        {
            return AWS_OP_SKIP;
        }

        return s_doProvisionCsrTest(identityClient, templateName, csr.c_str());
    }
}
AWS_TEST_CASE(IdentityV2ClientProvisionWithCSR5, s_IdentityV2ClientProvisionWithCSR5)

static int s_IdentityV2ClientProvisionWithCSR311(Aws::Crt::Allocator *allocator, void *)
{
    {
        Aws::Crt::ApiHandle handle;

        struct aws_string *template_name = NULL;
        if (aws_get_environment_value(allocator, s_rrEnvVariableProvisioningTemplateName, &template_name) ||
            template_name == nullptr)
        {
            return AWS_OP_SKIP;
        }

        Aws::Crt::String templateName(aws_string_c_str(template_name));
        aws_string_destroy(template_name);

        struct aws_string *csr_path = NULL;
        if (aws_get_environment_value(allocator, s_rrEnvVariableProvisioningCsrPath, &csr_path) || csr_path == nullptr)
        {
            return AWS_OP_SKIP;
        }

        Aws::Crt::String csrPath(aws_string_c_str(csr_path));
        aws_string_destroy(csr_path);

        std::ifstream ifs(csrPath.c_str());
        std::string csr;
        getline(ifs, csr, (char)ifs.eof());

        auto protocolClient = s_createProtocolClient311(allocator);
        auto identityClient = s_createIdentityClient311(protocolClient, allocator);
        if (!identityClient)
        {
            return AWS_OP_SKIP;
        }

        return s_doProvisionCsrTest(identityClient, templateName, csr.c_str());
    }
}
AWS_TEST_CASE(IdentityV2ClientProvisionWithCSR311, s_IdentityV2ClientProvisionWithCSR311)

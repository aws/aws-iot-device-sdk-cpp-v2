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

#include <aws/iotshadow/DeleteNamedShadowRequest.h>
#include <aws/iotshadow/DeleteShadowResponse.h>
#include <aws/iotshadow/GetNamedShadowRequest.h>
#include <aws/iotshadow/GetShadowResponse.h>
#include <aws/iotshadow/IotShadowClientV2.h>
#include <aws/iotshadow/NamedShadowDeltaUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/NamedShadowUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/ShadowDeltaUpdatedEvent.h>
#include <aws/iotshadow/ShadowUpdatedEvent.h>
#include <aws/iotshadow/UpdateNamedShadowRequest.h>
#include <aws/iotshadow/UpdateShadowResponse.h>
#include <aws/iotshadow/V2ErrorResponse.h>

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
            [&connected, &lock, &signal](Aws::Crt::Mqtt::MqttConnection &, Aws::Crt::Mqtt::OnConnectionSuccessData *)
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

static std::shared_ptr<Aws::Iotshadow::IClientV2> s_createShadowClient5(
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

    return Aws::Iotshadow::NewClientFrom5(*protocolClient, serviceClientOptions, allocator);
}

static std::shared_ptr<Aws::Iotshadow::IClientV2> s_createShadowClient311(
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

    return Aws::Iotshadow::NewClientFrom311(*protocolClient, serviceClientOptions, allocator);
}

static int s_ShadowV2ClientCreateDestroy5(Aws::Crt::Allocator *allocator, void *)
{
    {
        Aws::Crt::ApiHandle handle;

        auto protocolClient = s_createProtocolClient5(allocator);
        auto shadowClient = s_createShadowClient5(protocolClient, allocator);
        if (!shadowClient)
        {
            return AWS_OP_SKIP;
        }
    }

    return AWS_OP_SUCCESS;
}
AWS_TEST_CASE(ShadowV2ClientCreateDestroy5, s_ShadowV2ClientCreateDestroy5)

static int s_ShadowV2ClientCreateDestroy311(Aws::Crt::Allocator *allocator, void *)
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
AWS_TEST_CASE(ShadowV2ClientCreateDestroy311, s_ShadowV2ClientCreateDestroy311)

static int s_doGetNonexistentShadowTest(std::shared_ptr<Aws::Iotshadow::IClientV2> shadowClient)
{
    if (!shadowClient)
    {
        return AWS_OP_SKIP;
    }

    ResultWaiter<Aws::Iotshadow::GetNamedShadowResult> getResult;

    Aws::Iotshadow::GetNamedShadowRequest request;
    request.ShadowName = Aws::Crt::UUID().ToString();
    request.ThingName = Aws::Crt::UUID().ToString();

    shadowClient->GetNamedShadow(
        request,
        [&getResult](Aws::Iotshadow::GetNamedShadowResult &&result) { getResult.ApplyResult(std::move(result)); });

    const auto &result = getResult.GetResult();
    ASSERT_FALSE(result.IsSuccess());

    const auto &error = result.GetError();
    ASSERT_INT_EQUALS(AWS_ERROR_MQTT_REQUEST_RESPONSE_MODELED_SERVICE_ERROR, error.GetErrorCode());

    const auto &modeledError = error.GetModeledError();

    ASSERT_INT_EQUALS(404, modeledError.Code.value());

    return AWS_OP_SUCCESS;
}

static int s_ShadowV2ClientGetNonExistentNamedShadow5(Aws::Crt::Allocator *allocator, void *)
{
    Aws::Crt::ApiHandle handle;
    auto protocolClient = s_createProtocolClient5(allocator);
    return s_doGetNonexistentShadowTest(s_createShadowClient5(protocolClient, allocator));
}
AWS_TEST_CASE(ShadowV2ClientGetNonExistentNamedShadow5, s_ShadowV2ClientGetNonExistentNamedShadow5)

static int s_ShadowV2ClientGetNonExistentNamedShadow311(Aws::Crt::Allocator *allocator, void *)
{
    Aws::Crt::ApiHandle handle;
    auto protocolClient = s_createProtocolClient311(allocator);
    return s_doGetNonexistentShadowTest(s_createShadowClient311(protocolClient, allocator));
}
AWS_TEST_CASE(ShadowV2ClientGetNonExistentNamedShadow311, s_ShadowV2ClientGetNonExistentNamedShadow311)

static void s_deleteNamedShadow(
    std::shared_ptr<Aws::Iotshadow::IClientV2> shadowClient,
    const Aws::Crt::String &thingName,
    const Aws::Crt::String &shadowName)
{
    ResultWaiter<Aws::Iotshadow::DeleteNamedShadowResult> deleteResultWaiter;

    Aws::Iotshadow::DeleteNamedShadowRequest deleteRequest;
    deleteRequest.ShadowName = shadowName;
    deleteRequest.ThingName = thingName;

    shadowClient->DeleteNamedShadow(
        deleteRequest,
        [&deleteResultWaiter](Aws::Iotshadow::DeleteNamedShadowResult &&result)
        { deleteResultWaiter.ApplyResult(std::move(result)); });

    deleteResultWaiter.GetResult();
}

static int s_doCreateDestroyNamedShadowTest(
    std::shared_ptr<Aws::Iotshadow::IClientV2> shadowClient,
    const Aws::Crt::String &thingName,
    const Aws::Crt::String &shadowName)
{
    if (!shadowClient)
    {
        return AWS_OP_SKIP;
    }

    // 1. Create the Shadow
    ResultWaiter<Aws::Iotshadow::UpdateNamedShadowResult> updateResultWaiter;

    Aws::Crt::JsonObject desired;
    desired.WithString("color", "green");

    Aws::Iotshadow::ShadowState state;
    state.Desired = desired;

    Aws::Iotshadow::UpdateNamedShadowRequest updateRequest;
    updateRequest.ShadowName = shadowName;
    updateRequest.ThingName = thingName;
    updateRequest.State = state;

    shadowClient->UpdateNamedShadow(
        updateRequest,
        [&updateResultWaiter](Aws::Iotshadow::UpdateNamedShadowResult &&result)
        { updateResultWaiter.ApplyResult(std::move(result)); });

    const auto &updateResult = updateResultWaiter.GetResult();
    ASSERT_TRUE(updateResult.IsSuccess());

    const auto &updateResponse = updateResult.GetResponse();
    const auto &updateState = updateResponse.State.value();

    auto desiredColor = updateState.Desired.value().View().GetString("color");
    ASSERT_TRUE(desiredColor == "green");

    // 2. Delete the shadow
    ResultWaiter<Aws::Iotshadow::DeleteNamedShadowResult> deleteResultWaiter;

    Aws::Iotshadow::DeleteNamedShadowRequest deleteRequest;
    deleteRequest.ShadowName = shadowName;
    deleteRequest.ThingName = thingName;

    shadowClient->DeleteNamedShadow(
        deleteRequest,
        [&deleteResultWaiter](Aws::Iotshadow::DeleteNamedShadowResult &&result)
        { deleteResultWaiter.ApplyResult(std::move(result)); });

    const auto &deleteResult = deleteResultWaiter.GetResult();
    ASSERT_TRUE(deleteResult.IsSuccess());

    return AWS_OP_SUCCESS;
}

static int s_ShadowV2ClientCreateDestroyNamedShadow5(Aws::Crt::Allocator *allocator, void *)
{
    Aws::Crt::ApiHandle handle;
    auto protocolClient = s_createProtocolClient5(allocator);
    auto client = s_createShadowClient5(protocolClient, allocator);
    auto thingName = Aws::Crt::UUID().ToString();
    auto shadowName = Aws::Crt::UUID().ToString();
    int result = s_doCreateDestroyNamedShadowTest(client, thingName, shadowName);
    if (result != AWS_OP_SKIP)
    {
        s_deleteNamedShadow(client, thingName, shadowName);
    }

    return result;
}
AWS_TEST_CASE(ShadowV2ClientCreateDestroyNamedShadow5, s_ShadowV2ClientCreateDestroyNamedShadow5)

static int s_ShadowV2ClientCreateDestroyNamedShadow311(Aws::Crt::Allocator *allocator, void *)
{
    Aws::Crt::ApiHandle handle;
    auto protocolClient = s_createProtocolClient311(allocator);
    auto client = s_createShadowClient311(protocolClient, allocator);
    auto thingName = Aws::Crt::UUID().ToString();
    auto shadowName = Aws::Crt::UUID().ToString();
    int result = s_doCreateDestroyNamedShadowTest(client, thingName, shadowName);
    if (result != AWS_OP_SKIP)
    {
        s_deleteNamedShadow(client, thingName, shadowName);
    }

    return result;
}
AWS_TEST_CASE(ShadowV2ClientCreateDestroyNamedShadow311, s_ShadowV2ClientCreateDestroyNamedShadow311)

static std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> s_createNamedShadowUpdatedStream(
    std::shared_ptr<Aws::Iotshadow::IClientV2> shadowClient,
    ResultWaiter<Aws::Iotshadow::ShadowUpdatedEvent> &eventWaiter,
    const Aws::Crt::String &thingName,
    const Aws::Crt::String &shadowName)
{
    Aws::Iotshadow::NamedShadowUpdatedSubscriptionRequest updateStreamRequest;
    updateStreamRequest.ThingName = thingName;
    updateStreamRequest.ShadowName = shadowName;

    ResultWaiter<Aws::Iot::RequestResponse::SubscriptionStatusEvent> subscribeWaiter;

    Aws::Iot::RequestResponse::StreamingOperationOptions<Aws::Iotshadow::ShadowUpdatedEvent> updateStreamOptions;
    updateStreamOptions.WithStreamHandler([&eventWaiter](Aws::Iotshadow::ShadowUpdatedEvent &&event)
                                          { eventWaiter.ApplyResult(std::move(event)); });
    updateStreamOptions.WithSubscriptionStatusEventHandler(
        [&subscribeWaiter](Aws::Iot::RequestResponse::SubscriptionStatusEvent &&event)
        {
            if (event.GetType() == Aws::Iot::RequestResponse::SubscriptionStatusEventType::SubscriptionEstablished)
            {
                subscribeWaiter.ApplyResult(std::move(event));
            }
        });

    auto updateStream = shadowClient->CreateNamedShadowUpdatedStream(updateStreamRequest, updateStreamOptions);
    updateStream->Open();

    subscribeWaiter.GetResult();

    return updateStream;
}

static std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> s_createNamedShadowDeltaUpdatedStream(
    std::shared_ptr<Aws::Iotshadow::IClientV2> shadowClient,
    ResultWaiter<Aws::Iotshadow::ShadowDeltaUpdatedEvent> &eventWaiter,
    const Aws::Crt::String &thingName,
    const Aws::Crt::String &shadowName)
{
    Aws::Iotshadow::NamedShadowDeltaUpdatedSubscriptionRequest deltaUpdateStreamRequest;
    deltaUpdateStreamRequest.ThingName = thingName;
    deltaUpdateStreamRequest.ShadowName = shadowName;

    ResultWaiter<Aws::Iot::RequestResponse::SubscriptionStatusEvent> subscribeWaiter;

    Aws::Iot::RequestResponse::StreamingOperationOptions<Aws::Iotshadow::ShadowDeltaUpdatedEvent>
        deltaUpdateStreamOptions;
    deltaUpdateStreamOptions.WithStreamHandler([&eventWaiter](Aws::Iotshadow::ShadowDeltaUpdatedEvent &&event)
                                               { eventWaiter.ApplyResult(std::move(event)); });
    deltaUpdateStreamOptions.WithSubscriptionStatusEventHandler(
        [&subscribeWaiter](Aws::Iot::RequestResponse::SubscriptionStatusEvent &&event)
        {
            if (event.GetType() == Aws::Iot::RequestResponse::SubscriptionStatusEventType::SubscriptionEstablished)
            {
                subscribeWaiter.ApplyResult(std::move(event));
            }
        });

    auto deltaUpdateStream =
        shadowClient->CreateNamedShadowDeltaUpdatedStream(deltaUpdateStreamRequest, deltaUpdateStreamOptions);
    deltaUpdateStream->Open();

    subscribeWaiter.GetResult();

    return deltaUpdateStream;
}

static int s_doDeltaUpdateNamedShadowTest(
    std::shared_ptr<Aws::Iotshadow::IClientV2> shadowClient,
    const Aws::Crt::String &thingName,
    const Aws::Crt::String &shadowName)
{
    if (!shadowClient)
    {
        return AWS_OP_SKIP;
    }

    // 1. create the shadow with a specific desired state
    ResultWaiter<Aws::Iotshadow::UpdateNamedShadowResult> createResultWaiter;

    Aws::Crt::JsonObject createDocument;
    createDocument.WithString("color", "green");

    Aws::Iotshadow::ShadowState createState;
    createState.Desired = createDocument;
    createState.Reported = createDocument;

    Aws::Iotshadow::UpdateNamedShadowRequest createRequest;
    createRequest.ShadowName = shadowName;
    createRequest.ThingName = thingName;
    createRequest.State = createState;

    shadowClient->UpdateNamedShadow(
        createRequest,
        [&createResultWaiter](Aws::Iotshadow::UpdateNamedShadowResult &&result)
        { createResultWaiter.ApplyResult(std::move(result)); });

    ASSERT_TRUE(createResultWaiter.GetResult().IsSuccess());

    // 2. Create both kinds of streaming operations

    // Result waiters can work as one-shot notifiers for streams too
    ResultWaiter<Aws::Iotshadow::ShadowUpdatedEvent> updateStreamFirstEventWaiter;
    auto updateStream =
        s_createNamedShadowUpdatedStream(shadowClient, updateStreamFirstEventWaiter, thingName, shadowName);

    ResultWaiter<Aws::Iotshadow::ShadowDeltaUpdatedEvent> deltaUpdateStreamFirstEventWaiter;
    auto updateDeltaStream =
        s_createNamedShadowDeltaUpdatedStream(shadowClient, deltaUpdateStreamFirstEventWaiter, thingName, shadowName);

    // 3. Update the desired state of the shadow to a different color
    ResultWaiter<Aws::Iotshadow::UpdateNamedShadowResult> updateResultWaiter;

    Aws::Crt::JsonObject updateDocument;
    updateDocument.WithString("color", "blue");

    Aws::Iotshadow::ShadowState updateState;
    updateState.Desired = updateDocument;

    Aws::Iotshadow::UpdateNamedShadowRequest updateRequest;
    updateRequest.ShadowName = shadowName;
    updateRequest.ThingName = thingName;
    updateRequest.State = updateState;

    shadowClient->UpdateNamedShadow(
        updateRequest,
        [&updateResultWaiter](Aws::Iotshadow::UpdateNamedShadowResult &&result)
        { updateResultWaiter.ApplyResult(std::move(result)); });

    ASSERT_TRUE(updateResultWaiter.GetResult().IsSuccess());

    // 4. Verify both streaming operations got an appropriate update
    const auto &oldDesiredState =
        updateStreamFirstEventWaiter.GetResult().Previous.value().State.value().Desired.value();
    auto oldColor = oldDesiredState.View().GetString("color");
    ASSERT_TRUE(oldColor == "green");

    const auto &currentDesiredState =
        updateStreamFirstEventWaiter.GetResult().Current.value().State.value().Desired.value();
    auto currentColor = currentDesiredState.View().GetString("color");
    ASSERT_TRUE(currentColor == "blue");

    const auto &deltaState = deltaUpdateStreamFirstEventWaiter.GetResult().State.value();
    auto deltaColor = deltaState.View().GetString("color");
    ASSERT_TRUE(deltaColor == "blue");

    // 5. Delete the shadow
    ResultWaiter<Aws::Iotshadow::DeleteNamedShadowResult> deleteResultWaiter;

    Aws::Iotshadow::DeleteNamedShadowRequest deleteRequest;
    deleteRequest.ShadowName = shadowName;
    deleteRequest.ThingName = thingName;

    shadowClient->DeleteNamedShadow(
        deleteRequest,
        [&deleteResultWaiter](Aws::Iotshadow::DeleteNamedShadowResult &&result)
        { deleteResultWaiter.ApplyResult(std::move(result)); });

    const auto &deleteResult = deleteResultWaiter.GetResult();
    ASSERT_TRUE(deleteResult.IsSuccess());

    return AWS_OP_SUCCESS;
}

static int s_ShadowV2ClientDeltaUpdateNamedShadow5(Aws::Crt::Allocator *allocator, void *)
{
    Aws::Crt::ApiHandle handle;
    auto protocolClient = s_createProtocolClient5(allocator);
    auto client = s_createShadowClient5(protocolClient, allocator);
    auto thingName = Aws::Crt::UUID().ToString();
    auto shadowName = Aws::Crt::UUID().ToString();
    int result = s_doDeltaUpdateNamedShadowTest(client, thingName, shadowName);
    if (result != AWS_OP_SKIP)
    {
        s_deleteNamedShadow(client, thingName, shadowName);
    }

    return result;
}
AWS_TEST_CASE(ShadowV2ClientDeltaUpdateNamedShadow5, s_ShadowV2ClientDeltaUpdateNamedShadow5)

static int s_ShadowV2ClientDeltaUpdateNamedShadow311(Aws::Crt::Allocator *allocator, void *)
{
    Aws::Crt::ApiHandle handle;
    auto protocolClient = s_createProtocolClient311(allocator);
    auto client = s_createShadowClient311(protocolClient, allocator);
    auto thingName = Aws::Crt::UUID().ToString();
    auto shadowName = Aws::Crt::UUID().ToString();
    int result = s_doDeltaUpdateNamedShadowTest(client, thingName, shadowName);
    if (result != AWS_OP_SKIP)
    {
        s_deleteNamedShadow(client, thingName, shadowName);
    }

    return result;
}
AWS_TEST_CASE(ShadowV2ClientDeltaUpdateNamedShadow311, s_ShadowV2ClientDeltaUpdateNamedShadow311)
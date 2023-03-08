/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/crt/UUID.h>

#include "MqttOperationQueueTests.h"

// ==================================================
// HELPER / UTILS
// ==================================================

// Switch to true to see queue logs. Can be helpful when debugging
static bool PRINT_QUEUE_LOGS = false;

int MqttOperationQueueTests::Tester::OnApplicationFailure(Aws::Crt::String message)
{
    fprintf(stderr, "ERROR: OperationQueue test failed with message: %s\n", message.c_str());
    return -1;
}

int MqttOperationQueueTests::Tester::TestOperationSuccess(
    MqttOperationQueue::QueueResult result,
    Aws::Crt::String testName)
{
    if (result != MqttOperationQueue::QueueResult::SUCCESS)
    {
        return OnApplicationFailure(testName + ": A Queue operation was not successful");
    }
    return 0;
}

void MqttOperationQueueTests::Tester::TestConnectionSetup()
{
    m_connection = cmdUtils->BuildMQTTConnection();
    m_connectionCompletedPromise = std::promise<bool>();
    m_connectionClosedPromise = std::promise<void>();

    auto onConnectionCompleted =
        [&](Aws::Crt::Mqtt::MqttConnection &, int errorCode, Aws::Crt::Mqtt::ReturnCode, bool) {
            if (errorCode)
            {
                m_connectionCompletedPromise.set_value(false);
            }
            else
            {
                m_connectionCompletedPromise.set_value(true);
            }
        };
    auto onDisconnect = [&](Aws::Crt::Mqtt::MqttConnection &) {
        {
            fprintf(stdout, "Disconnect completed\n");
            m_connectionClosedPromise.set_value();
        }
    };

    m_connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    m_connection->OnDisconnect = std::move(onDisconnect);
}

void MqttOperationQueueTests::Tester::TestConnectionTeardown()
{
    m_connection = nullptr;
}

// ==================================================
// TESTS
// ==================================================

/**
 * Tests that the queue can perform all operations and that they are performed in the right order (not checking
 * overflow).
 */
int MqttOperationQueueTests::Tester::TestConnectSubPubUnsub()
{
    TestConnectionSetup();
    Aws::Crt::String TEST_TOPIC = "test/topic/" + Aws::Crt::UUID().ToString();

    /* CREATE THE QUEUE CALLBACKS */
    std::promise<void> onQueueEmptyPromise;
    auto onQueueEmpty = [&]() { onQueueEmptyPromise.set_value(); };
    std::promise<MqttOperationQueue::QueueOperation> onQueueSentPromise;
    auto onQueueOperationSent = [&](MqttOperationQueue::QueueOperation operation) {
        onQueueSentPromise.set_value(operation);
    };
    std::promise<MqttOperationQueue::QueueOperation> onQueueDroppedPromise;
    auto onQueueOperationDropped = [&](MqttOperationQueue::QueueOperation operation) {
        onQueueDroppedPromise.set_value(operation);
    };
    /* CREATE THE QUEUE */
    MqttOperationQueue::MqttOperationQueueBuilder queueBuilder = MqttOperationQueue::MqttOperationQueueBuilder();
    queueBuilder.WithConnection(m_connection)
        .WithEnableLogging(PRINT_QUEUE_LOGS)
        .WithQueueLoopTimeMs(2000); // 2 seconds
    queueBuilder.WithOnQueueEmptyCallback(std::move(onQueueEmpty));
    queueBuilder.WithOnOperationSentCallback(std::move(onQueueOperationSent));
    queueBuilder.WithOnOperationDroppedCallback(std::move(onQueueOperationDropped));
    std::shared_ptr<MqttOperationQueue::MqttOperationQueue> operationQueue = queueBuilder.Build();

    Aws::Crt::String clientId = Aws::Crt::String("test-") + Aws::Crt::UUID().ToString();
    if (!m_connection->Connect(clientId.c_str(), true /*cleanSession*/, 1000 /*keepAliveTimeSecs*/))
    {
        return OnApplicationFailure("MQTT Connection failed");
    }

    if (m_connectionCompletedPromise.get_future().get())
    {
        /* Needed to call subscribe, even though we're not doing anything with it */
        auto onMessage = [&](Aws::Crt::Mqtt::MqttConnection &,
                             const Aws::Crt::String /*topic*/,
                             const Aws::Crt::ByteBuf /*byteBuf*/,
                             bool /*dup*/,
                             Aws::Crt::Mqtt::QOS /*qos*/,
                             bool /*retain*/) {};

        int opResult = operationQueue->Subscribe(TEST_TOPIC.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, onMessage, NULL, NULL);
        if (opResult != 0)
        {
            return opResult;
        }

        Aws::Crt::String sendPayload = "Hello World";
        Aws::Crt::ByteBuf payload =
            Aws::Crt::ByteBufFromArray((const uint8_t *)sendPayload.data(), sendPayload.length());
        opResult = operationQueue->Publish(TEST_TOPIC.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, false, payload, NULL);
        if (opResult != 0)
        {
            return opResult;
        }

        opResult = operationQueue->Unsubscribe(TEST_TOPIC.c_str(), NULL);
        if (opResult != 0)
        {
            return opResult;
        }

        if (operationQueue->GetQueueSize() != 3)
        {
            return OnApplicationFailure("TestConnectSubPubUnsub: Queue size is not 3!");
        }

        operationQueue->Start();

        // Make sure the order is right. Order should be: Sub, Pub, Unsub
        MqttOperationQueue::QueueOperation returnOperation;
        returnOperation = onQueueSentPromise.get_future().get();
        if (returnOperation.type != MqttOperationQueue::OperationType::SUBSCRIBE)
        {
            return OnApplicationFailure("TestConnectSubPubUnsub: First operation is not subscribe!");
        }
        onQueueSentPromise = std::promise<MqttOperationQueue::QueueOperation>();
        returnOperation = onQueueSentPromise.get_future().get();
        if (returnOperation.type != MqttOperationQueue::OperationType::PUBLISH)
        {
            return OnApplicationFailure("TestConnectSubPubUnsub: Second operation is not publish!");
        }
        onQueueSentPromise = std::promise<MqttOperationQueue::QueueOperation>();
        returnOperation = onQueueSentPromise.get_future().get();
        if (returnOperation.type != MqttOperationQueue::OperationType::UNSUBSCRIBE)
        {
            return OnApplicationFailure("TestConnectSubPubUnsub: Third operation is not unsubscribe!");
        }

        onQueueEmptyPromise.get_future().wait();
        operationQueue->Stop();

        if (m_connection->Disconnect())
        {
            m_connectionClosedPromise.get_future().wait();
        }
    }
    TestConnectionTeardown();
    fprintf(stderr, "TestConnectSubPubUnsub passed\n");
    return 0; // SUCCESS
}

/**
 * Tests that when the queue is full it keeps the proper size, that operations are properly dropped from the BACK
 * of the queue rather than from the front (BACK = newest but last to be performed), and the the operations
 * are performed in proper order as expected.
 */
int MqttOperationQueueTests::Tester::TestDropBack()
{
    TestConnectionSetup();
    Aws::Crt::String TEST_TOPIC = "test/topic/" + Aws::Crt::UUID().ToString();

    /* CREATE THE QUEUE CALLBACKS */
    std::promise<void> onQueueEmptyPromise;
    auto onQueueEmpty = [&]() { onQueueEmptyPromise.set_value(); };
    std::promise<MqttOperationQueue::QueueOperation> onQueueSentPromise;
    auto onQueueOperationSent = [&](MqttOperationQueue::QueueOperation operation) {
        onQueueSentPromise.set_value(operation);
    };
    std::promise<MqttOperationQueue::QueueOperation> onQueueDroppedPromise;
    auto onQueueOperationDropped = [&](MqttOperationQueue::QueueOperation operation) {
        onQueueDroppedPromise.set_value(operation);
    };
    /* CREATE THE QUEUE */
    MqttOperationQueue::MqttOperationQueueBuilder queueBuilder = MqttOperationQueue::MqttOperationQueueBuilder();
    queueBuilder.WithConnection(m_connection)
        .WithEnableLogging(PRINT_QUEUE_LOGS)
        .WithQueueLoopTimeMs(2000); // 2 seconds
    queueBuilder.WithOnQueueEmptyCallback(std::move(onQueueEmpty));
    queueBuilder.WithOnOperationSentCallback(std::move(onQueueOperationSent));
    queueBuilder.WithOnOperationDroppedCallback(std::move(onQueueOperationDropped));

    queueBuilder.WithQueueLimitSize(2); // Limit the queue to 2

    std::shared_ptr<MqttOperationQueue::MqttOperationQueue> operationQueue = queueBuilder.Build();

    Aws::Crt::String clientId = Aws::Crt::String("test-") + Aws::Crt::UUID().ToString();
    if (!m_connection->Connect(clientId.c_str(), true /*cleanSession*/, 1000 /*keepAliveTimeSecs*/))
    {
        return OnApplicationFailure("MQTT Connection failed");
    }

    if (m_connectionCompletedPromise.get_future().get())
    {
        /* Needed to call subscribe, even though we're not doing anything with it */
        auto onMessage = [&](Aws::Crt::Mqtt::MqttConnection &,
                             const Aws::Crt::String /*topic*/,
                             const Aws::Crt::ByteBuf /*byteBuf*/,
                             bool /*dup*/,
                             Aws::Crt::Mqtt::QOS /*qos*/,
                             bool /*retain*/) {};

        int opResult = operationQueue->Subscribe(TEST_TOPIC.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, onMessage, NULL, NULL);
        if (opResult != 0)
        {
            return opResult;
        }

        Aws::Crt::String sendPayload = "Hello World";
        Aws::Crt::ByteBuf payload =
            Aws::Crt::ByteBufFromArray((const uint8_t *)sendPayload.data(), sendPayload.length());
        // Perform 10 publishes
        for (int i = 0; i < 10; i++)
        {
            onQueueDroppedPromise = std::promise<MqttOperationQueue::QueueOperation>();
            opResult = operationQueue->Publish(TEST_TOPIC.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, false, payload, NULL);
            if (opResult != 0)
            {
                return opResult;
            }
        }

        if (operationQueue->GetQueueSize() != 2)
        {
            return OnApplicationFailure("TestDropBack: Queue size is not 2!");
        }

        onQueueDroppedPromise = std::promise<MqttOperationQueue::QueueOperation>();
        opResult = operationQueue->Unsubscribe(TEST_TOPIC.c_str(), NULL);
        if (opResult != 0)
        {
            return opResult;
        }
        MqttOperationQueue::QueueOperation droppedOperation = onQueueDroppedPromise.get_future().get();
        if (droppedOperation.type != MqttOperationQueue::OperationType::PUBLISH)
        {
            return OnApplicationFailure("TestDropBack: Dropped operation is not publish!");
        }

        operationQueue->Start();

        // Make sure the order is right. Order should be: Sub, Unsub
        MqttOperationQueue::QueueOperation returnOperation;
        returnOperation = onQueueSentPromise.get_future().get();
        if (returnOperation.type != MqttOperationQueue::OperationType::SUBSCRIBE)
        {
            return OnApplicationFailure("TestDropBack: First operation is not subscribe!");
        }
        onQueueSentPromise = std::promise<MqttOperationQueue::QueueOperation>();
        returnOperation = onQueueSentPromise.get_future().get();
        if (returnOperation.type != MqttOperationQueue::OperationType::UNSUBSCRIBE)
        {
            return OnApplicationFailure("TestDropBack: Second operation is not unsubscribe!");
        }

        onQueueEmptyPromise.get_future().wait();
        operationQueue->Stop();

        if (m_connection->Disconnect())
        {
            m_connectionClosedPromise.get_future().wait();
        }
    }
    TestConnectionTeardown();
    fprintf(stderr, "TestDropBack passed\n");
    return 0; // SUCCESS
}

/**
 * Tests that when the queue is full it that operations are properly dropped from the FRONT of the queue
 */
int MqttOperationQueueTests::Tester::TestDropFront()
{
    TestConnectionSetup();
    Aws::Crt::String TEST_TOPIC = "test/topic/" + Aws::Crt::UUID().ToString();

    /* CREATE THE QUEUE CALLBACKS */
    std::promise<void> onQueueEmptyPromise;
    auto onQueueEmpty = [&]() { onQueueEmptyPromise.set_value(); };
    std::promise<MqttOperationQueue::QueueOperation> onQueueSentPromise;
    auto onQueueOperationSent = [&](MqttOperationQueue::QueueOperation operation) {
        onQueueSentPromise.set_value(operation);
    };
    std::promise<MqttOperationQueue::QueueOperation> onQueueDroppedPromise;
    auto onQueueOperationDropped = [&](MqttOperationQueue::QueueOperation operation) {
        onQueueDroppedPromise.set_value(operation);
    };
    /* CREATE THE QUEUE */
    MqttOperationQueue::MqttOperationQueueBuilder queueBuilder = MqttOperationQueue::MqttOperationQueueBuilder();
    queueBuilder.WithConnection(m_connection)
        .WithEnableLogging(PRINT_QUEUE_LOGS)
        .WithQueueLoopTimeMs(2000); // 2 seconds
    queueBuilder.WithOnQueueEmptyCallback(std::move(onQueueEmpty));
    queueBuilder.WithOnOperationSentCallback(std::move(onQueueOperationSent));
    queueBuilder.WithOnOperationDroppedCallback(std::move(onQueueOperationDropped));

    queueBuilder.WithQueueLimitSize(2); // Limit the queue to 2
    queueBuilder.WithQueueLimitBehavior(MqttOperationQueue::LimitBehavior::DROP_FRONT);

    std::shared_ptr<MqttOperationQueue::MqttOperationQueue> operationQueue = queueBuilder.Build();

    Aws::Crt::String clientId = Aws::Crt::String("test-") + Aws::Crt::UUID().ToString();
    if (!m_connection->Connect(clientId.c_str(), true /*cleanSession*/, 1000 /*keepAliveTimeSecs*/))
    {
        return OnApplicationFailure("MQTT Connection failed");
    }

    if (m_connectionCompletedPromise.get_future().get())
    {
        /* Needed to call subscribe, even though we're not doing anything with it */
        auto onMessage = [&](Aws::Crt::Mqtt::MqttConnection &,
                             const Aws::Crt::String /*topic*/,
                             const Aws::Crt::ByteBuf /*byteBuf*/,
                             bool /*dup*/,
                             Aws::Crt::Mqtt::QOS /*qos*/,
                             bool /*retain*/) {};
        // Add subscribe and unsubscribe to the queue
        int opResult = operationQueue->Subscribe(TEST_TOPIC.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, onMessage, NULL, NULL);
        if (opResult != 0)
        {
            return opResult;
        }
        opResult = operationQueue->Unsubscribe(TEST_TOPIC.c_str(), NULL);
        if (opResult != 0)
        {
            return opResult;
        }

        // Add two publishes and make sure the drop order is correct
        Aws::Crt::String sendPayload = "Hello World";
        Aws::Crt::ByteBuf payload =
            Aws::Crt::ByteBufFromArray((const uint8_t *)sendPayload.data(), sendPayload.length());
        // First drop
        opResult = operationQueue->Publish(TEST_TOPIC.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, false, payload, NULL);
        if (opResult != 0)
        {
            return opResult;
        }
        MqttOperationQueue::QueueOperation droppedOperation = onQueueDroppedPromise.get_future().get();
        if (droppedOperation.type != MqttOperationQueue::OperationType::SUBSCRIBE)
        {
            return OnApplicationFailure("TestDropFront: First dropped operation is not publish!");
        }
        // Second drop
        onQueueDroppedPromise = std::promise<MqttOperationQueue::QueueOperation>();
        opResult = operationQueue->Publish(TEST_TOPIC.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, false, payload, NULL);
        if (opResult != 0)
        {
            return opResult;
        }
        droppedOperation = onQueueDroppedPromise.get_future().get();
        if (droppedOperation.type != MqttOperationQueue::OperationType::UNSUBSCRIBE)
        {
            return OnApplicationFailure("TestDropFront: Second dropped operation is not publish!");
        }

        if (m_connection->Disconnect())
        {
            m_connectionClosedPromise.get_future().wait();
        }
    }
    TestConnectionTeardown();
    fprintf(stderr, "TestDropFront passed\n");
    return 0; // SUCCESS
}

/**
 * Tests that adding/inserting operations to the front of the queue works as expected and the send order
 * is correct and expected.
 */
int MqttOperationQueueTests::Tester::TestAddFront()
{
    TestConnectionSetup();
    Aws::Crt::String TEST_TOPIC = "test/topic/" + Aws::Crt::UUID().ToString();

    /* CREATE THE QUEUE CALLBACKS */
    std::promise<void> onQueueEmptyPromise;
    auto onQueueEmpty = [&]() { onQueueEmptyPromise.set_value(); };
    std::promise<MqttOperationQueue::QueueOperation> onQueueSentPromise;
    auto onQueueOperationSent = [&](MqttOperationQueue::QueueOperation operation) {
        onQueueSentPromise.set_value(operation);
    };
    std::promise<MqttOperationQueue::QueueOperation> onQueueDroppedPromise;
    auto onQueueOperationDropped = [&](MqttOperationQueue::QueueOperation operation) {
        onQueueDroppedPromise.set_value(operation);
    };
    /* CREATE THE QUEUE */
    MqttOperationQueue::MqttOperationQueueBuilder queueBuilder = MqttOperationQueue::MqttOperationQueueBuilder();
    queueBuilder.WithConnection(m_connection)
        .WithEnableLogging(PRINT_QUEUE_LOGS)
        .WithQueueLoopTimeMs(2000); // 2 seconds
    queueBuilder.WithOnQueueEmptyCallback(std::move(onQueueEmpty));
    queueBuilder.WithOnOperationSentCallback(std::move(onQueueOperationSent));
    queueBuilder.WithOnOperationDroppedCallback(std::move(onQueueOperationDropped));

    queueBuilder.WithQueueLimitSize(2); // Limit the queue to 2
    queueBuilder.WithQueueLimitBehavior(MqttOperationQueue::LimitBehavior::DROP_BACK);
    queueBuilder.WithQueueInsertBehavior(MqttOperationQueue::InsertBehavior::INSERT_FRONT);

    std::shared_ptr<MqttOperationQueue::MqttOperationQueue> operationQueue = queueBuilder.Build();

    Aws::Crt::String clientId = Aws::Crt::String("test-") + Aws::Crt::UUID().ToString();
    if (!m_connection->Connect(clientId.c_str(), true /*cleanSession*/, 1000 /*keepAliveTimeSecs*/))
    {
        return OnApplicationFailure("MQTT Connection failed");
    }

    if (m_connectionCompletedPromise.get_future().get())
    {
        Aws::Crt::String sendPayload = "Hello World";
        Aws::Crt::ByteBuf payload =
            Aws::Crt::ByteBufFromArray((const uint8_t *)sendPayload.data(), sendPayload.length());
        // Fill with publishes
        int opResult = 0;
        for (int i = 0; i < 2; i++)
        {
            opResult = operationQueue->Publish(TEST_TOPIC.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, false, payload, NULL);
            if (opResult != 0)
            {
                return opResult;
            }
        }
        if (operationQueue->GetQueueSize() != 2)
        {
            return OnApplicationFailure("TestAddFront: Queue size is not 2!");
        }

        /* Needed to call subscribe, even though we're not doing anything with it */
        auto onMessage = [&](Aws::Crt::Mqtt::MqttConnection &,
                             const Aws::Crt::String /*topic*/,
                             const Aws::Crt::ByteBuf /*byteBuf*/,
                             bool /*dup*/,
                             Aws::Crt::Mqtt::QOS /*qos*/,
                             bool /*retain*/) {};

        // Add unsubscribe then subscribe, which should result in the order of subscribe, unsubscribe
        onQueueDroppedPromise = std::promise<MqttOperationQueue::QueueOperation>();
        opResult = operationQueue->Unsubscribe(TEST_TOPIC.c_str(), NULL);
        if (opResult != 0)
        {
            return opResult;
        }
        onQueueDroppedPromise = std::promise<MqttOperationQueue::QueueOperation>();
        opResult = operationQueue->Subscribe(TEST_TOPIC.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, onMessage, NULL, NULL);
        if (opResult != 0)
        {
            return opResult;
        }
        if (operationQueue->GetQueueSize() != 2)
        {
            return OnApplicationFailure("TestAddFront: Queue size is not 2!");
        }

        operationQueue->Start();

        // Make sure the order is right. Order should be: Sub, Unsub
        MqttOperationQueue::QueueOperation returnOperation;
        returnOperation = onQueueSentPromise.get_future().get();
        if (returnOperation.type != MqttOperationQueue::OperationType::SUBSCRIBE)
        {
            return OnApplicationFailure("TestAddFront: First operation is not subscribe!");
        }
        onQueueSentPromise = std::promise<MqttOperationQueue::QueueOperation>();
        returnOperation = onQueueSentPromise.get_future().get();
        if (returnOperation.type != MqttOperationQueue::OperationType::UNSUBSCRIBE)
        {
            return OnApplicationFailure("TestAddFront: Second operation is not unsubscribe!");
        }

        onQueueEmptyPromise.get_future().wait();
        operationQueue->Stop();

        if (m_connection->Disconnect())
        {
            m_connectionClosedPromise.get_future().wait();
        }
    }
    TestConnectionTeardown();
    fprintf(stderr, "TestAddFront passed\n");
    return 0; // SUCCESS
}

/**
 * Tests that when the queue is full and set to return an error, that it does so when you try to add more to it
 */
int MqttOperationQueueTests::Tester::TestAddError()
{
    TestConnectionSetup();
    Aws::Crt::String TEST_TOPIC = "test/topic/" + Aws::Crt::UUID().ToString();

    /* CREATE THE QUEUE */
    MqttOperationQueue::MqttOperationQueueBuilder queueBuilder = MqttOperationQueue::MqttOperationQueueBuilder();
    queueBuilder.WithConnection(m_connection)
        .WithEnableLogging(PRINT_QUEUE_LOGS)
        .WithQueueLoopTimeMs(2000);     // 2 seconds
    queueBuilder.WithQueueLimitSize(2); // Limit the queue to 2
    queueBuilder.WithQueueLimitBehavior(MqttOperationQueue::LimitBehavior::RETURN_ERROR);

    std::shared_ptr<MqttOperationQueue::MqttOperationQueue> operationQueue = queueBuilder.Build();

    Aws::Crt::String clientId = Aws::Crt::String("test-") + Aws::Crt::UUID().ToString();
    if (!m_connection->Connect(clientId.c_str(), true /*cleanSession*/, 1000 /*keepAliveTimeSecs*/))
    {
        return OnApplicationFailure("MQTT Connection failed");
    }

    if (m_connectionCompletedPromise.get_future().get())
    {
        // Fill with unsubscribe
        int opResult = TestOperationSuccess(operationQueue->Unsubscribe(TEST_TOPIC.c_str(), NULL), "TestAddError");
        if (opResult != 0)
        {
            return opResult;
        }
        opResult = TestOperationSuccess(operationQueue->Unsubscribe(TEST_TOPIC.c_str(), NULL), "TestAddError");
        if (opResult != 0)
        {
            return opResult;
        }

        // Try to add another unsubscribe, but it should return an error since the queue is full
        MqttOperationQueue::QueueResult addResult = operationQueue->Unsubscribe(TEST_TOPIC.c_str(), NULL);
        if (addResult != MqttOperationQueue::QueueResult::ERROR_QUEUE_FULL)
        {
            return OnApplicationFailure("TestAddError: Unsubscribe when full did not return queue full error!");
        }

        if (m_connection->Disconnect())
        {
            m_connectionClosedPromise.get_future().wait();
        }
    }
    TestConnectionTeardown();
    fprintf(stderr, "TestAddError passed\n");
    return 0; // SUCCESS
}

int MqttOperationQueueTests::Tester::RunTests()
{
    fprintf(stdout, "Starting test: TestConnectSubPubUnsub\n");
    int result = TestConnectSubPubUnsub();
    if (result != 0)
    {
        fprintf(stderr, "\nERROR: TestConnectSubPubUnsub test failed! \n");
        return result;
    }

    fprintf(stdout, "Starting test: TestDropBack\n");
    result = TestDropBack();
    if (result != 0)
    {
        fprintf(stderr, "\nERROR: TestDropBack test failed! \n");
        return result;
    }

    fprintf(stdout, "Starting test: TestDropFront\n");
    result = TestDropFront();
    if (result != 0)
    {
        fprintf(stderr, "\nERROR: TestDropFront test failed! \n");
        return result;
    }

    fprintf(stdout, "Starting test: TestAddFront\n");
    result = TestAddFront();
    if (result != 0)
    {
        fprintf(stderr, "\nERROR: TestAddFront test failed! \n");
        return result;
    }

    fprintf(stdout, "Starting test: TestAddError\n");
    result = TestAddError();
    if (result != 0)
    {
        fprintf(stderr, "\nERROR: TestAddError test failed! \n");
        return result;
    }

    // SUCCESS
    return 0;
}

int MqttOperationQueueTests::Tester::CreateAndRunTester(Utils::CommandLineUtils *cmdUtils)
{
    MqttOperationQueueTests::Tester tester = MqttOperationQueueTests::Tester();
    tester.cmdUtils = cmdUtils;
    return tester.RunTests();
}

/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/crt/Api.h>
#include <aws/crt/StlAllocator.h>

#include <chrono>
#include <thread>
#include <string>

#include "MqttOperationQueue.h"

namespace MqttOperationQueue {

    /*****************************************************
     *
     * MqttOperationQueue
     *
     *****************************************************/

    MqttOperationQueue::MqttOperationQueue(MqttOperationQueueBuilder &builder, Aws::Crt::Allocator *allocator) {
        if (builder.GetConnection() == nullptr) {
            throw std::invalid_argument("Connection is not defined!");
        }
        m_operationQueue = std::vector<QueueOperation>();
        m_allocator = allocator;

        m_connection = builder.GetConnection();
        m_queueLimitSize = builder.GetQueueLimitSize();
        m_queueLimitBehavior = builder.GetQueueLimitBehavior();
        m_queueInsertBehavior = builder.GetQueueInsertBehavior();
        m_incompleteLimit = builder.GetIncompleteLimit();
        m_inflightLimit = builder.GetInflightLimit();
        m_enableLogging = builder.GetEnableLogging();

        m_callbackSent = builder.GetOperationSentCallback();
        m_callbackSentFailure = builder.GetOperationSentFailureCallback();
        m_callbackDropped = builder.GetOperationDroppedCallback();
        m_callbackEmpty = builder.GetQueueEmptyCallback();
        m_callbackFull = builder.GetQueueFullCallback();

        m_isStarted = false;
        std::atomic_init(&m_isLooping, false);
        std::atomic_init(&m_queueLoopTimeMs, builder.GetQueueLoopTimeMs());
        m_isLooping.store(false);
    }

    std::shared_ptr<MqttOperationQueue> MqttOperationQueue::NewOperationQueue(
        MqttOperationQueueBuilder &builder, Aws::Crt::Allocator *allocator) noexcept
    {
        /* Copied from MqttClient.cpp:ln754 */
        // As the constructor is private, make share would not work here. We do make_share manually.
        MqttOperationQueue *toSeat = reinterpret_cast<MqttOperationQueue *>(aws_mem_acquire(allocator, sizeof(MqttOperationQueue)));

        if (!toSeat)
        {
            return nullptr;
        }
        toSeat = new (toSeat) MqttOperationQueue(builder, allocator);
        return std::shared_ptr<MqttOperationQueue>(
            toSeat, [allocator](MqttOperationQueue *queue) { Aws::Crt::Delete(queue, allocator); });
    }

    void MqttOperationQueue::PrintLogMessage(Aws::Crt::String message)
    {
        if (m_enableLogging) {
            fprintf(stdout, "[MqttOperationQueue] %s\n", message.c_str());
        }
    }

    std::string MqttOperationQueue::OperationTypeToString(OperationType type) {
        if (type == OperationType::NONE) {
            return std::string("None");
        } else if (type == OperationType::PUBLISH) {
            return std::string("Publish");
        } else if (type == OperationType::SUBSCRIBE) {
            return std::string("Subscribe");
        } else if (type == OperationType::UNSUBSCRIBE) {
            return std::string("Unsubscribe");
        } else {
            return std::string("Unknown");
        }
    }

    /*****************************************************
     *
     * MqttOperationQueue Task/Loop logic
     *
     *****************************************************/

    bool MqttOperationQueue::CheckOperationStatistics() {
        Aws::Crt::Mqtt::MqttConnectionOperationStatistics statistics = m_connection->GetOperationStatistics();
        if (statistics.incompleteOperationCount >= m_incompleteLimit) {
            if (m_incompleteLimit > 0) {
                PrintLogMessage("Skipping running operation due to incomplete operation count being equal or higher than maximum");
                return false;
            }
        }
        if (statistics.unackedOperationCount >= m_inflightLimit) {
            if (m_inflightLimit > 0) {
                PrintLogMessage("Skipping running operation due to inflight operation count being equal or higher than maximum");
                return false;
            }
        }
        return true;
    }

    void MqttOperationQueue::RunOperation() {
        // CRITICAL SECTION
        m_queueLock.lock();

        try {
            if (m_operationQueue.size() > 0) {
                QueueOperation operation = m_operationQueue[0];
                m_operationQueue.erase(m_operationQueue.begin());

                PrintLogMessage((std::string("Starting to perform operation of type ") + OperationTypeToString(operation.type)).c_str());
                PerformOperation(&operation);

                if (m_operationQueue.size() <= 0) {
                    if (m_callbackEmpty) {
                        m_callbackEmpty();
                    }
                }
            }
            else {
                PrintLogMessage("No operations to perform");
            }
        } catch (const std::exception &exception) {
            PrintLogMessage((std::string("EXCEPTION occurred trying to perform operation! What: ") + std::string(exception.what())).c_str());
        }

        m_queueLock.unlock();
        // END CRITICAL SECTION
    }

    void MqttOperationQueue::PerformOperation(QueueOperation *operation)
    {
        if (operation == nullptr) {
            PerformOperationUnknown(operation);
        } else if (operation->type == OperationType::PUBLISH) {
            PerformOperationPublish(operation);
        } else if (operation->type == OperationType::SUBSCRIBE) {
            PerformOperationSubscribe(operation);
        } else if (operation->type == OperationType::UNSUBSCRIBE) {
            PerformOperationUnsubscribe(operation);
        } else {
            PerformOperationUnknown(operation);
        }
    }

    void MqttOperationQueue::PerformOperationPublish(QueueOperation *operation)
    {
        m_connection->Publish(operation->topic.c_str(), operation->qos, operation->retain, operation->payload, std::move(operation->onOpComplete));
        if (m_callbackSent) {
            m_callbackSent(*operation);
        }
    }

    void MqttOperationQueue::PerformOperationSubscribe(QueueOperation *operation)
    {
        // TODO: Support the other subscribe callback(s)
        m_connection->Subscribe(operation->topic.c_str(), operation->qos, std::move(operation->onMessage), std::move(operation->onSubAck));

        if (m_callbackSent) {
            m_callbackSent(*operation);
        }
    }

    void MqttOperationQueue::PerformOperationUnsubscribe(QueueOperation *operation)
    {
        m_connection->Unsubscribe(operation->topic.c_str(), std::move(operation->onOpComplete));
        if (m_callbackSent) {
            m_callbackSent(*operation);
        }
    }

    void MqttOperationQueue::PerformOperationUnknown(QueueOperation *operation)
    {
        if (operation == nullptr) {
            PrintLogMessage("ERROR - got null pointer operation to perform!");
        } else {
            PrintLogMessage("ERROR - got unknown operation to perform!");
            if (m_callbackSentFailure) {
                QueueResult result = QueueResult::UNKNOWN_OPERATION;
                m_callbackSentFailure(*operation, result);
            }
        }
    }

    void MqttOperationQueue::ProcessQueue()
    {
        PrintLogMessage("Performing operation loop...");
        if (CheckOperationStatistics()) {
            RunOperation();
        }
    }

    void MqttOperationQueue::s_QueueLoop(MqttOperationQueue *queue)
    {
        // Get a smart pointer just to ensure it stays alive
        std::shared_ptr<MqttOperationQueue> smartQueue = queue->getptr();

        while (smartQueue->GetIsLooping() == true) {
            smartQueue->ProcessQueue();
            // Loop finished, so sleep for a bit
            std::this_thread::sleep_for(std::chrono::milliseconds(smartQueue->GetQueueLoopTime()));
        }
    }

    /*****************************************************
     *
     * MqttOperationQueue Queue management
     *
     *****************************************************/

    QueueResult MqttOperationQueue::AddOperationToQueue(QueueOperation operation)
    {
        QueueResult result = QueueResult::SUCCESS;
        QueueOperation droppedOperation = QueueOperation();

        // CRITICAL SECTION
        m_queueLock.lock();
        try {

            if (m_queueLimitSize <= 0) {
                result = AddOperationToQueueInsert(&operation);
            } else {
                if (m_operationQueue.size() + 1 <= m_queueLimitSize) {
                    AddOperationToQueueInsert(&operation);
                } else {
                    result = AddOperationToQueueOverflow(&operation, &droppedOperation);
                }
            }

        } catch (...) {
            PrintLogMessage("ERROR: Exception ocurred trying to add operation to queue!");
            if (result == QueueResult::SUCCESS) {
                result = QueueResult::UNKNOWN_ERROR;
            }
        }
        m_queueLock.unlock();
        // END CRITICAL SECTION

        if (result == QueueResult::SUCCESS) {
            PrintLogMessage((std::string("Added operation of type ") + OperationTypeToString(operation.type) + std::string(" successfully to queue")).c_str());
            if (m_operationQueue.size() == m_queueLimitSize && droppedOperation.type == OperationType::NONE) {
                if (m_callbackFull) {
                    m_callbackFull();
                }
            }
        }

        if (droppedOperation.type != OperationType::NONE) {
            if (m_callbackDropped) {
                m_callbackDropped(droppedOperation);
            }
        }

        return result;
    }

    QueueResult MqttOperationQueue::AddOperationToQueueInsert(QueueOperation *operation)
    {
        QueueResult result = QueueResult::SUCCESS;
        if (m_queueInsertBehavior == InsertBehavior::INSERT_BACK) {
            m_operationQueue.push_back(*operation);
        } else if (m_queueInsertBehavior == InsertBehavior::INSERT_FRONT) {
            m_operationQueue.insert(m_operationQueue.begin(), *operation);
        } else {
            result = QueueResult::UNKNOWN_QUEUE_INSERT_BEHAVIOR;
        }
        return result;
    }

    QueueResult MqttOperationQueue::AddOperationToQueueOverflow(QueueOperation *operation, QueueOperation *oldOperation)
    {
        QueueResult result = QueueResult::SUCCESS;
        if (m_queueLimitBehavior == LimitBehavior::RETURN_ERROR) {
            PrintLogMessage("Did not drop any operation, instead returning error...");
            result = QueueResult::ERROR_QUEUE_FULL;
        } else if (m_queueLimitBehavior == LimitBehavior::DROP_FRONT) {
            *oldOperation = m_operationQueue[0];
            m_operationQueue.erase(m_operationQueue.begin());
            PrintLogMessage((std::string("Dropped operation of type ") + OperationTypeToString(oldOperation->type) + std::string(" from the front...")).c_str());
            result = AddOperationToQueueInsert(operation);
        } else if (m_queueLimitBehavior == LimitBehavior::DROP_BACK) {
            *oldOperation = m_operationQueue[m_operationQueue.size() - 1];
            m_operationQueue.erase(m_operationQueue.begin() + m_operationQueue.size() - 1);
            PrintLogMessage((std::string("Dropped operation of type ") + OperationTypeToString(oldOperation->type) + std::string(" from the back...")).c_str());
            result = AddOperationToQueueInsert(operation);
        } else {
            result = QueueResult::UNKNOWN_QUEUE_LIMIT_BEHAVIOR;
        }
        return result;
    }

    /*****************************************************
     *
     * MqttOperationQueue Operations
     *
     *****************************************************/

    void MqttOperationQueue::Start() noexcept
    {
        if (m_isStarted == true) {
            PrintLogMessage("Cannot start because already started!");
            return;
        }
        m_isStarted = true;
        m_isLooping.store(true);
        m_thread = std::thread(&MqttOperationQueue::s_QueueLoop, this);

        PrintLogMessage("Started successfully");
    }

    void MqttOperationQueue::Stop() noexcept
    {
        if (m_isStarted == false) {
            PrintLogMessage("Cannot stop because already stopped!");
            return;
        }
        m_isStarted = false;
        m_isLooping.store(false);

        // We have to wait for the thread to stop
        PrintLogMessage("Waiting for thread to stop...");
        m_thread.join();
        PrintLogMessage("Stopped successfully");
    }

    QueueResult MqttOperationQueue::Publish(
        const char *topic,
        Aws::Crt::Mqtt::QOS qos,
        bool retain,
        Aws::Crt::ByteBuf &payload,
        const Aws::Crt::Mqtt::OnOperationCompleteHandler onOpComplete)
    {
        QueueOperation newOperation = QueueOperation();
        newOperation.type = OperationType::PUBLISH;
        newOperation.topic = Aws::Crt::String(topic); // makes a copy
        newOperation.qos = qos;
        newOperation.retain = retain;
        newOperation.payload = Aws::Crt::ByteBufNewCopy(m_allocator, payload.buffer, payload.len); // make a copy
        newOperation.onOpComplete = onOpComplete;
        return AddOperationToQueue(std::move(newOperation));
    }

    QueueResult MqttOperationQueue::Subscribe(
        const char *topicFilter,
        Aws::Crt::Mqtt::QOS qos,
        const Aws::Crt::Mqtt::OnMessageReceivedHandler onMessage,
        const Aws::Crt::Mqtt::OnSubAckHandler onSubAck)
    {
        QueueOperation newOperation = QueueOperation();
        newOperation.type = OperationType::SUBSCRIBE;
        newOperation.topic = Aws::Crt::String(topicFilter); // makes a copy
        newOperation.qos = qos;
        newOperation.onMessage = onMessage;
        newOperation.onSubAck = onSubAck;
        return AddOperationToQueue(std::move(newOperation));
    }

    QueueResult MqttOperationQueue::Unsubscribe(
        const char *topicFilter,
        const Aws::Crt::Mqtt::OnOperationCompleteHandler onOpComplete)
    {
        QueueOperation newOperation;
        newOperation.type = OperationType::UNSUBSCRIBE;
        newOperation.topic = Aws::Crt::String(topicFilter); // makes a copy
        newOperation.onOpComplete = onOpComplete;
        return AddOperationToQueue(std::move(newOperation));
    }

    QueueResult MqttOperationQueue::AddQueueOperation(QueueOperation operation)
    {
        /* Basic validation */
        if (operation.type == OperationType::NONE) {
            return QueueResult::ERROR_INVALID_ARGUMENT;
        } else if (operation.type == OperationType::PUBLISH) {
            if (operation.topic.length() > 0 || operation.payload.len > 0) {
                return QueueResult::ERROR_INVALID_ARGUMENT;
            }
        } else if (operation.type == OperationType::SUBSCRIBE) {
            if (operation.topic.length() > 0) {
                return QueueResult::ERROR_INVALID_ARGUMENT;
            }
        } else if (operation.type == OperationType::UNSUBSCRIBE) {
            if (operation.topic.length() > 0) {
                return QueueResult::ERROR_INVALID_ARGUMENT;
            }
        } else {
            return QueueResult::UNKNOWN_ERROR;
        }
        return AddOperationToQueue(std::move(operation));
    }

    uint32_t MqttOperationQueue::GetQueueSize() {
        // CRITICAL SECTION
        m_queueLock.lock();
        uint32_t returnSize = m_operationQueue.size();
        m_queueLock.unlock();
        // END CRITICAL SECTION
        return returnSize;
    }

    uint32_t MqttOperationQueue::GetQueueLimit() {
        return m_queueLimitSize;
    }

    bool MqttOperationQueue::GetIsLooping()
    {
        return m_isLooping.load();
    }

    uint32_t MqttOperationQueue::GetQueueLoopTime()
    {
        return m_queueLoopTimeMs.load();
    }

    /*****************************************************
     *
     * MqttOperationQueueBuilder
     *
     *****************************************************/

    MqttOperationQueueBuilder::MqttOperationQueueBuilder() noexcept
                : m_queueLimitSize(10), m_queueLimitBehavior(DROP_BACK),
                m_queueInsertBehavior(INSERT_BACK), m_incompleteLimit(1), m_inflightLimit(1),
                m_queueLoopTimeMs(1000), m_enableLogging(false)
                {}

    MqttOperationQueueBuilder &MqttOperationQueueBuilder::WithConnection(std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> connection) noexcept
    {
        m_connection = connection;
        return *this;
    }

    MqttOperationQueueBuilder &MqttOperationQueueBuilder::WithQueueLimitSize(uint32_t limit) noexcept
    {
        m_queueLimitSize = limit;
        return *this;
    }

    MqttOperationQueueBuilder &MqttOperationQueueBuilder::WithQueueLimitBehavior(LimitBehavior behavior) noexcept
    {
        m_queueLimitBehavior = behavior;
        return *this;
    }

    MqttOperationQueueBuilder &MqttOperationQueueBuilder::WithQueueInsertBehavior(InsertBehavior behavior) noexcept
    {
        m_queueInsertBehavior = behavior;
        return *this;
    }

    MqttOperationQueueBuilder &MqttOperationQueueBuilder::WithIncompleteLimit(uint32_t limit) noexcept
    {
        m_incompleteLimit = limit;
        return *this;
    }

    MqttOperationQueueBuilder &MqttOperationQueueBuilder::WithInflightLimit(uint32_t limit) noexcept
    {
        m_inflightLimit = limit;
        return *this;
    }

    MqttOperationQueueBuilder &MqttOperationQueueBuilder::WithQueueLoopTimeMs(uint32_t time) noexcept
    {
        m_queueLoopTimeMs = time;
        return *this;
    }

    MqttOperationQueueBuilder &MqttOperationQueueBuilder::WithEnableLogging(bool limit) noexcept
    {
        m_enableLogging = limit;
        return *this;
    }

    MqttOperationQueueBuilder &MqttOperationQueueBuilder::WithOperationSentCallback(OperationSent callback) noexcept
    {
        m_callbackSent = std::move(callback);
        return *this;
    }

    MqttOperationQueueBuilder &MqttOperationQueueBuilder::WithOperationSentFailureCallback(OperationSentFailure callback) noexcept
    {
        m_callbackSentFailure = std::move(callback);
        return *this;
    }

    MqttOperationQueueBuilder &MqttOperationQueueBuilder::WithOperationDroppedCallback(OperationDropped callback) noexcept
    {
        m_callbackDropped = std::move(callback);
        return *this;
    }

    MqttOperationQueueBuilder &MqttOperationQueueBuilder::WithQueueFullCallback(QueueFull callback) noexcept
    {
        m_callbackFull = std::move(callback);
        return *this;
    }

    MqttOperationQueueBuilder &MqttOperationQueueBuilder::WithQueueEmptyCallback(QueueEmpty callback) noexcept
    {
        m_callbackEmpty = std::move(callback);
        return *this;
    }

    std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> MqttOperationQueueBuilder::GetConnection() noexcept
    {
        return m_connection;
    }

    uint32_t MqttOperationQueueBuilder::GetQueueLimitSize() noexcept
    {
        return m_queueLimitSize;
    }

    LimitBehavior MqttOperationQueueBuilder::GetQueueLimitBehavior()
    {
        return m_queueLimitBehavior;
    }

    InsertBehavior MqttOperationQueueBuilder::GetQueueInsertBehavior()
    {
        return m_queueInsertBehavior;
    }

    uint32_t MqttOperationQueueBuilder::GetIncompleteLimit()
    {
        return m_incompleteLimit;
    }

    uint32_t MqttOperationQueueBuilder::GetInflightLimit()
    {
        return m_inflightLimit;
    }

    uint32_t MqttOperationQueueBuilder::GetQueueLoopTimeMs()
    {
        return m_queueLoopTimeMs;
    }

    bool MqttOperationQueueBuilder::GetEnableLogging()
    {
        return m_enableLogging;
    }

    OperationSent MqttOperationQueueBuilder::GetOperationSentCallback()
    {
        return m_callbackSent;
    }

    OperationSentFailure MqttOperationQueueBuilder::GetOperationSentFailureCallback()
    {
        return m_callbackSentFailure;
    }

    OperationDropped MqttOperationQueueBuilder::GetOperationDroppedCallback()
    {
        return m_callbackDropped;
    }

    QueueFull MqttOperationQueueBuilder::GetQueueFullCallback()
    {
        return m_callbackFull;
    }

    QueueEmpty MqttOperationQueueBuilder::GetQueueEmptyCallback()
    {
        return m_callbackEmpty;
    }

    std::shared_ptr<MqttOperationQueue> MqttOperationQueueBuilder::Build() noexcept
    {
        return MqttOperationQueue::NewOperationQueue(*this, Aws::Crt::ApiAllocator());
    }
}

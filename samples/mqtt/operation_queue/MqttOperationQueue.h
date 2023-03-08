/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>

#include <aws/crt/io/EventLoopGroup.h>
#include <aws/crt/mqtt/MqttClient.h>


namespace MqttOperationQueue {

    /**
     * Forward defines
     */
    class MqttOperationQueue;
    class MqttOperationQueueBuilder;

    /**
     * Data Structures / Enums
     */
    enum QueueResult {
        SUCCESS,
        ERROR_QUEUE_FULL,
        ERROR_INVALID_ARGUMENT,
        UNKNOWN_QUEUE_LIMIT_BEHAVIOR,
        UNKNOWN_QUEUE_INSERT_BEHAVIOR,
        UNKNOWN_OPERATION,
        UNKNOWN_ERROR
    };

    enum OperationType {
        NONE,
        PUBLISH,
        SUBSCRIBE,
        UNSUBSCRIBE
    };

    enum LimitBehavior {
        DROP_FRONT,
        DROP_BACK,
        RETURN_ERROR
    };

    enum InsertBehavior {
        INSERT_FRONT,
        INSERT_BACK
    };

    struct QueueOperation {
        OperationType type;
        Aws::Crt::String topic;
        Aws::Crt::Mqtt::QOS qos;
        bool retain;
        Aws::Crt::ByteBuf payload;
        Aws::Crt::Mqtt::OnPublishReceivedHandler onPublish;
        Aws::Crt::Mqtt::OnSubAckHandler onSubAck;
        Aws::Crt::Mqtt::OnMessageReceivedHandler onMessage;
        Aws::Crt::Mqtt::OnOperationCompleteHandler onOpComplete;

        QueueOperation() {
            type = OperationType::NONE;
            topic = "";
            qos = AWS_MQTT_QOS_AT_MOST_ONCE;
            retain = false;
            payload = Aws::Crt::ByteBufFromCString("");
            onPublish = NULL;
            onSubAck = NULL;
            onOpComplete = NULL;
        }

        // Copy constructor
        QueueOperation(const QueueOperation& other) {
            type = other.type;
            topic = other.topic;
            qos = other.qos;
            retain = other.retain;
            payload = other.payload;
            onPublish = other.onPublish;
            onSubAck = other.onSubAck;
            onMessage = other.onMessage;
            onOpComplete = other.onOpComplete;
        }
    };

    /**
     * Functions
     */

    using OperationSent = std::function<void(QueueOperation &)>;
    using OperationSentFailure = std::function<void(QueueOperation &, QueueResult &)>;
    using OperationDropped = std::function<void(QueueOperation &)>;
    using QueueFull = std::function<void()>;
    using QueueEmpty = std::function<void()>;

    class MqttOperationQueue  : public std::enable_shared_from_this<MqttOperationQueue> {
        public:

            static std::shared_ptr<MqttOperationQueue> NewOperationQueue(
                MqttOperationQueueBuilder &options, Aws::Crt::Allocator *allocator) noexcept;

            std::shared_ptr<MqttOperationQueue> getptr() { return shared_from_this(); }

            void Start() noexcept;
            void Stop() noexcept;

            QueueResult Publish(
                const char *topic,
                Aws::Crt::Mqtt::QOS qos,
                bool retain,
                Aws::Crt::ByteBuf &payload,
                const Aws::Crt::Mqtt::OnOperationCompleteHandler onOpComplete
            );

            QueueResult Subscribe(
                const char *topicFilter,
                Aws::Crt::Mqtt::QOS qos,
                const Aws::Crt::Mqtt::OnMessageReceivedHandler onMessage,
                const Aws::Crt::Mqtt::OnSubAckHandler onSubAck
            );

            QueueResult Unsubscribe(
                const char *topicFilter,
                const Aws::Crt::Mqtt::OnOperationCompleteHandler onOpComplete
            );

            QueueResult AddQueueOperation(
                QueueOperation operation
            );

            uint32_t GetQueueSize();

            uint32_t GetQueueLimit();

            bool GetIsLooping();

            uint32_t GetQueueLoopTime();

            void ProcessQueue();

        private:
            MqttOperationQueue(MqttOperationQueueBuilder &builder, Aws::Crt::Allocator *allocator);

            void ScheduleRunnableLoop();
            void PrintLogMessage(Aws::Crt::String message);
            std::string OperationTypeToString(OperationType type);

            bool CheckOperationStatistics();
            void RunOperation();
            void PerformOperation(QueueOperation operation);
            void PerformOperationPublish(QueueOperation operation);
            void PerformOperationSubscribe(QueueOperation operation);
            void PerformOperationUnsubscribe(QueueOperation operation);
            void PerformOperationUnknown(QueueOperation operation);

            QueueResult AddOperationToQueue(QueueOperation operation);
            QueueResult AddOperationToQueueInsert(QueueOperation operation);
            QueueResult AddOperationToQueueOverflow(QueueOperation operation, QueueOperation *oldOperation);

            static void s_QueueLoop(MqttOperationQueue *queue);

            std::atomic_bool m_isLooping;
            std::atomic_uint32_t m_queueLoopTimeMs;

            std::vector<QueueOperation> m_operationQueue;
            std::mutex m_queueLock;
            std::thread m_thread;
            Aws::Crt::Allocator *m_allocator;
            bool m_isStarted;
            struct aws_task m_loopTask;

            std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> m_connection;
            uint32_t m_queueLimitSize;
            LimitBehavior m_queueLimitBehavior;
            InsertBehavior m_queueInsertBehavior;
            uint32_t m_incompleteLimit;
            uint32_t m_inflightLimit;
            bool m_enableLogging;

            OperationSent m_callbackSent;
            OperationSentFailure m_callbackSentFailure;
            OperationDropped m_callbackDropped;
            QueueFull m_callbackFull;
            QueueEmpty m_callbackEmpty;
    };

    /**
     * The class used to make a new queue
     */
    class MqttOperationQueueBuilder {
        public:

            MqttOperationQueueBuilder &WithConnection(std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> connection) noexcept;
            MqttOperationQueueBuilder &WithQueueLimitSize(uint32_t size) noexcept;
            MqttOperationQueueBuilder &WithQueueLimitBehavior(LimitBehavior behavior) noexcept;
            MqttOperationQueueBuilder &WithQueueInsertBehavior(InsertBehavior behavior) noexcept;
            MqttOperationQueueBuilder &WithIncompleteLimit(uint32_t limit) noexcept;
            MqttOperationQueueBuilder &WithInflightLimit(uint32_t limit) noexcept;
            MqttOperationQueueBuilder &WithQueueLoopTimeMs(uint32_t time) noexcept;
            MqttOperationQueueBuilder &WithEnableLogging(bool logging) noexcept;

            MqttOperationQueueBuilder &WithOperationSentCallback(OperationSent callback) noexcept;
            MqttOperationQueueBuilder &WithOperationSentFailureCallback(OperationSentFailure callback) noexcept;
            MqttOperationQueueBuilder &WithOperationDroppedCallback(OperationDropped callback) noexcept;
            MqttOperationQueueBuilder &WithQueueFullCallback(QueueFull callback) noexcept;
            MqttOperationQueueBuilder &WithQueueEmptyCallback(QueueEmpty callback) noexcept;

            std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> GetConnection() noexcept;
            uint32_t GetQueueLimitSize() noexcept;
            LimitBehavior GetQueueLimitBehavior();
            InsertBehavior GetQueueInsertBehavior();
            uint32_t GetIncompleteLimit();
            uint32_t GetInflightLimit();
            uint32_t GetQueueLoopTimeMs();
            bool GetEnableLogging();
            OperationSent GetOperationSentCallback();
            OperationSentFailure GetOperationSentFailureCallback();
            OperationDropped GetOperationDroppedCallback();
            QueueFull GetQueueFullCallback();
            QueueEmpty GetQueueEmptyCallback();

            std::shared_ptr<MqttOperationQueue> Build() noexcept;

            MqttOperationQueueBuilder() noexcept;

        private:
            // MqttOperationQueueBuilder() noexcept;

            std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> m_connection;
            uint32_t m_queueLimitSize;
            LimitBehavior m_queueLimitBehavior;
            InsertBehavior m_queueInsertBehavior;
            uint32_t m_incompleteLimit;
            uint32_t m_inflightLimit;
            uint32_t m_queueLoopTimeMs;
            bool m_enableLogging;

            OperationSent m_callbackSent;
            OperationSentFailure m_callbackSentFailure;
            OperationDropped m_callbackDropped;
            QueueFull m_callbackFull;
            QueueEmpty m_callbackEmpty;
    };
}

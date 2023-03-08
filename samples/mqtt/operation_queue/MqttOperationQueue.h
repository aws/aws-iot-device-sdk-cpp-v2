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
     * The result of attempting to perform an operation on the MqttOperationQueue.
     * The value indicates either success or what type of issue was encountered.
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

    /**
     * An enum to indicate the type of data the QueueOperation contains. Used
     * to differentiate between different operations in a common blob object.
     */
    enum OperationType {
        NONE,
        PUBLISH,
        SUBSCRIBE,
        UNSUBSCRIBE
    };

    /**
     * An enum to indicate what happens when the MqttOperationQueue is completely full
     * but new operations are requested to be added to the queue.
     *
     */
    enum LimitBehavior {
        /** Drops/Removes the oldest (but first to be run) operation in the queue */
        DROP_FRONT,
        /** Drops/Removes the newest (but last to be run) operation in the queue */
        DROP_BACK,
        /** Does not add the new operation at all and instead returns ERROR_QUEUE_FULL */
        RETURN_ERROR
    };

    /**
     * An enum to indicate what happens when the MqttOperationQueue has a new operation
     * it needs to add to the queue, configuring where the operation is added.
     */
    enum InsertBehavior {
        /** Adds the operation to the front, so it will be executed first */
        INSERT_FRONT,
        /** Adds the operation to the back, so it will be executed last */
        INSERT_BACK
    };

    /**
     * A blob class/struct containing all of the data an operation can possibly possess,
     * as well as an enum to indicate what type of operation is stored within. Used to
     * provide a common base that operations can be derived from.
     */
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
     * Invoked when an operation was removed from the queue and was just sent to the MQTT connection.
     * Note: At this point the operation was JUST sent, it has not necessarily been seen by the MQTT server/broker
     * nor has it necessarily gone out onto the socket, but rather the operation (like publish()) was just invoked.
     */
    using OnOperationSent = std::function<void(QueueOperation &)>;
    /**
     * Invoked when an operation was removed from the queue and attempted to be sent to the MQTT connection, but it failed.
     */
    using OnOperationSentFailure = std::function<void(QueueOperation &, QueueResult &)>;
    /**
     * Invoked when an operation was dropped/removed from the queue because the queue is full and a new operation was
     * just added to the queue.
     */
    using OnOperationDropped = std::function<void(QueueOperation &)>;
    /**
     * Invoked when the operation queue has just become full.
     *
     */
    using OnQueueFull = std::function<void()>;
    /**
     * Invoked when the operation queue has just become empty.
     */
    using OnQueueEmpty = std::function<void()>;

    /**
     * The operation queue class itself. This handles all logic for operating the queue, sending operations, etc.
     */
    class MqttOperationQueue  : public std::enable_shared_from_this<MqttOperationQueue> {
        public:

            /**
             * Creates a new operation queue from the given operation queue options builder.
             * @param options The options for how the queue should behave
             * @param allocator The allocator for the queue to use
             * @return std::shared_ptr<MqttOperationQueue> The new operation queue
             */
            static std::shared_ptr<MqttOperationQueue> NewOperationQueue(
                MqttOperationQueueBuilder &options, Aws::Crt::Allocator *allocator) noexcept;

            std::shared_ptr<MqttOperationQueue> getptr() { return shared_from_this(); }

            /**
             * Starts the MqttOperationQueue running so it can process the queue.
             * Every queueLoopTimeMs milliseconds it will check the queue to see if there is at least a single
             * operation waiting. If there is, it will check the MQTT client statistics to determine if
             * the MQTT connection has the bandwidth for the next operation (based on incompleteLimit and inflightLimit)
             * and, if the MQTT connection has bandwidth, will start a next operation from the queue.
             */
            void Start() noexcept;

            /**
             * Stops the MqttOperationQueue from running and processing operations that may be left in the queue.
             * Once stopped, the MqttOperationQueue can be restarted by calling start() again.
             * Note: calling stop() will block the thread temporarily as it waits for the operation queue thread to finish
             */
            void Stop() noexcept;

            /**
             * Creates a new Publish operation and adds it to the queue to be run.
             *
             * Note that the inputs to this function are exactly the same as the publish() function in
             * the MqttClientConnection, but instead of executing the operation as soon as possible, it
             * will be added to the queue based on the queueInsertBehavior and processed accordingly.
             *
             * The OnOperationSent callback function will be invoked when the operation is
             * processed and sent by the client.
             *
             * @param topic The topic to send the publish to
             * @param qos The QoS of the publish
             * @param retain Whether the server is requested to retain the publish
             * @param payload The payload of the publish
             * @param onOpComplete The callback invoked when the publish is complete
             * @return QueueResult The result of adding the operation to the queue.
             */
            QueueResult Publish(
                const char *topic,
                Aws::Crt::Mqtt::QOS qos,
                bool retain,
                Aws::Crt::ByteBuf &payload,
                const Aws::Crt::Mqtt::OnOperationCompleteHandler onOpComplete
            );

            /**
             * Creates a new subscribe operation and adds it to the queue to be run.
             *
             * Note that the inputs to this function are exactly the same as the subscribe() function in
             * the MqttClientConnection, but instead of executing the operation as soon as possible, it
             * will be added to the queue based on the queueInsertBehavior and processed accordingly.
             *
             * The OnOperationSent callback function will be invoked when the operation is
             * processed and sent by the client.
             *
             * @param topicFilter The topic to subscribe to
             * @param qos The QoS of the subscribe
             * @param onMessage The callback invoked when the topic gets a publish
             * @param onSubAck The callback invoked when the subscribe gets an ACK
             * @return QueueResult The result of adding the operation to the queue.
             */
            QueueResult Subscribe(
                const char *topicFilter,
                Aws::Crt::Mqtt::QOS qos,
                const Aws::Crt::Mqtt::OnMessageReceivedHandler onMessage,
                const Aws::Crt::Mqtt::OnSubAckHandler onSubAck
            );

            /**
             * Creates a new unsubscribe operation and adds it to the queue to be run.
             *
             * Note that the inputs to this function are exactly the same as the unsubscribe() function in
             * the MqttClientConnection, but instead of executing the operation as soon as possible, it
             * will be added to the queue based on the queueInsertBehavior and processed accordingly.
             *
             * The OnOperationSent callback function will be invoked when the operation is
             * processed and sent by the client.
             *
             * @param topicFilter The topic to unsubscribe from
             * @param onOpComplete The callback invoked when the unsubscribe gets an ACK
             * @return QueueResult The result of adding the operation to the queue.
             */
            QueueResult Unsubscribe(
                const char *topicFilter,
                const Aws::Crt::Mqtt::OnOperationCompleteHandler onOpComplete
            );

            /**
             * Adds a new queue operation (publish, subscribe, unsubscribe) to the queue to be run.
             *
             * Note: This function provides only basic validation of the operation data. It is primarily
             * intended to be used with the OnOperationDropped callback for when you may want to
             * add a dropped message back to the queue.
             * (for example, say it's an important message you know you want to send)
             *
             * @param operation The operation you want to add to the queue
             * @return QueueResult The result of adding the operation to the queue.
             */
            QueueResult AddQueueOperation(
                QueueOperation operation
            );

            /**
             * Returns the size of the queue
             * @return uint32_t The size of the queue
             */
            uint32_t GetQueueSize();

            /**
             * Returns the maximum size of the queue
             * @return uint32_t The maximum size of the queue
             */
            uint32_t GetQueueLimit();

            /**
             * Returns whether the queue is looping/started.
             * This is only meant for the thread that calls the queue at periodic intervals.
             * @return true If the queue is looping/started.
             * @return false If the queue is not-looping/stopped.
             */
            bool GetIsLooping();

            /**
             * Returns the amount of time the queue is configured to wait between loops.
             * This is only meant for the thread that calls the queue at periodic intervals.
             * @return uint32_t The amount of time the queue is configured to wait between loops.
             */
            uint32_t GetQueueLoopTime();

            /**
             * Tells the queue to process the next operation on the queue, if there is one.
             * This is only meant for the thread that calls the queue at periodic intervals.
             */
            void ProcessQueue();

        private:
            MqttOperationQueue(MqttOperationQueueBuilder &builder, Aws::Crt::Allocator *allocator);

            /**
             * Helper function: Prints a message to stdout if the operation queue is configured to print messages.
             * Just makes the code a little cleaner and easier to process.
             * @param message The message to print
             */
            void PrintLogMessage(Aws::Crt::String message);

            /**
             * Helper function: Takes an operation type and returns a string equivalent.
             * Just makes the code a little cleaner and easier to process.
             * @param type The type to convert
             * @return std::string The converted string
             */
            std::string OperationTypeToString(OperationType type);

            /**
             * Helper function: Checks the MQTT connection operation statistics to see if their values are higher than the maximum
             * values set in MqttOperationQueue. If the value is higher than the maximum in MqttOperationQueue, then it returns false
             * so an operation on the queue will not be processed.
             * Called by the ProcessQueue() function.
             * @return true if an operation can be processed based on the MQTT connection operation statistics
             * @return false if an operation cannot be processed based on the MQTT connection operation statistics
             */
            bool CheckOperationStatistics();

            /**
             * Helper function: Takes the operation off the queue, checks what operation it is, and passes
             * it to the MQTT connection to be run.
             * Called by the ProcessQueue() function.
             */
            void RunOperation();

            /**
             * Helper function: Based on the operation type, calls the appropriate helper function.
             * @param operation The operation to process
             */
            void PerformOperation(QueueOperation *operation);

            /**
             * Helper function: Takes the publish operation and pass it to the MQTT connection
             * @param operation The operation to process
             */
            void PerformOperationPublish(QueueOperation *operation);

            /**
             * Helper function: Takes the subscribe operation and pass it to the MQTT connection
             * @param operation The operation to process
             */
            void PerformOperationSubscribe(QueueOperation *operation);

            /**
             * Helper function: Takes the unsubscribe operation and pass it to the MQTT connection
             * @param operation The operation to process
             */
            void PerformOperationUnsubscribe(QueueOperation *operation);

            /**
             * Helper function: Takes the unknown operation and sends it as a failure
             * @param operation The operation to process
             */
            void PerformOperationUnknown(QueueOperation *operation);

            /**
             * Helper function: Adds the given operation to the operation queue to be processed
             * @param operation The operation to add
             * @return QueueResult The result of adding the operation
             */
            QueueResult AddOperationToQueue(QueueOperation operation);

            /**
             * Helper function: Inserts the given QueueOperation into the queue/list directly.
             * Used to simplify inserting in front/back based on configuration options.
             * Called by both AddOperationToQueue and AddOperationToQueueOverflow.
             * @param operation The operation to add
             * @return QueueResult The result of adding the operation
             */
            QueueResult AddOperationToQueueInsert(QueueOperation *operation);

            /**
             * Helper function: Adds the given QueueOperation to the queue when the queue is full.
             * Used to make separate the logic for when the queue is full from when it is not yet full.
             * Called by AddOperationToQueue.
             * @param operation The operation to add
             * @param oldOperation Possibly the dropped operation
             * @return QueueResult The result of adding the operation
             */
            QueueResult AddOperationToQueueOverflow(QueueOperation *operation, QueueOperation *oldOperation);

            /**
             * The function called by the operation queue loop thread, used to tell the operation queue
             * to process at set intervals.
             * @param queue The operation queue to process in the thread.
             */
            static void s_QueueLoop(MqttOperationQueue *queue);

            std::atomic_bool m_isLooping;
            std::atomic_uint32_t m_queueLoopTimeMs;

            std::vector<QueueOperation> m_operationQueue;
            std::mutex m_queueLock;
            std::thread m_thread;
            Aws::Crt::Allocator *m_allocator;
            bool m_isStarted;

            std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> m_connection;
            uint32_t m_queueLimitSize;
            LimitBehavior m_queueLimitBehavior;
            InsertBehavior m_queueInsertBehavior;
            uint32_t m_incompleteLimit;
            uint32_t m_inflightLimit;
            bool m_enableLogging;

            OnOperationSent m_callbackSent;
            OnOperationSentFailure m_callbackSentFailure;
            OnOperationDropped m_callbackDropped;
            OnQueueFull m_callbackFull;
            OnQueueEmpty m_callbackEmpty;
    };

    /**
     * The class used to make a new queue
     */
    class MqttOperationQueueBuilder {
        public:

            /**
             * Sets the MqttClientConnection that will be used by the MqttOperationQueue.
             * This is a REQUIRED argument that has to be set in order for the MqttOperationQueue to function.
             *
             * @param connection The MqttClientConnection that will be used by the MqttOperationQueue.
             * @return MqttOperationQueueBuilder& The builder.
             */
            MqttOperationQueueBuilder &WithConnection(std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> connection) noexcept;

            /**
             * Sets the maximum size of the operation queue in the MqttOperationQueue.
             * Default operation queue size is 10.
             *
             * If the number of operations exceeds this number, then the queue will be adjusted
             * based on the queueLimitBehavior.
             *
             * @param size The maximum size of the operation queue.
             * @return MqttOperationQueueBuilder& The builder.
             */
            MqttOperationQueueBuilder &WithQueueLimitSize(uint32_t size) noexcept;

            /**
             * Sets how the MqttOperationQueue will behave when the operation queue is full but a
             * new operation is requested to be added to the queue.
             * The default is DROP_BACK, which will drop the newest (but last to be executed) operation at the back of the queue.
             *
             * @param behavior How the MqttOperationQueue will behave when the operation queue is full.
             * @return MqttOperationQueueBuilder& The builder.
             */
            MqttOperationQueueBuilder &WithQueueLimitBehavior(LimitBehavior behavior) noexcept;

            /**
             * Sets how the MqttOperationQueue will behave when inserting a new operation into the queue.
             * The default is INSERT_BACK, which will add the new operation to the back (last to be executed) of the queue.
             *
             * @param behavior How the MqttOperationQueue will behave when inserting a new operation into the queue.
             * @return MqttOperationQueueBuilder& The builder.
             */
            MqttOperationQueueBuilder &WithQueueInsertBehavior(InsertBehavior behavior) noexcept;

            /**
             * Sets the maximum number of incomplete operations that the MQTT connection can have before the
             * MqttOperationQueue will wait for them to be complete. Incomplete operations are those that have been
             * sent to the MqttClientConnection but have not been fully processed and responded to from the MQTT server/broker.
             *
             * Once the maximum number of incomplete operations is met, the MqttOperationQueue will wait until the number
             * of incomplete operations is below the set maximum.
             *
             * Default is set to 1. Set to 0 for no limit.
             *
             * @param limit The maximum number of incomplete operations before waiting.
             * @return MqttOperationQueueBuilder& The builder.
             */
            MqttOperationQueueBuilder &WithIncompleteLimit(uint32_t limit) noexcept;

            /**
             * Sets the maximum number of inflight operations that
             *
             * Sets the maximum number of inflight operations that the MQTT connection can have before the
             * MqttOperationQueue will wait for them to be complete. inflight operations are those that have been
             * sent to the MqttClientConnection and sent out to the MQTT server/broker, but an acknowledgement from
             * the MQTT server/broker has not yet been received.
             *
             * Once the maximum number of inflight operations is met, the MqttOperationQueue will wait until the number
             * of inflight operations is below the set maximum.
             *
             * Default is set to 1. Set to 0 for no limit.
             *
             * @param limit The maximum number of inflight operations before waiting.
             * @return MqttOperationQueueBuilder& The builder.
             */
            MqttOperationQueueBuilder &WithInflightLimit(uint32_t limit) noexcept;

            /**
             * Sets the interval, in milliseconds, that the MqttOperationQueue will wait before checking the queue and (possibly)
             * processing an operation based on the statistics and state of the MqttClientConnection assigned to the MqttOperationQueue.
             * The default is every second.
             * @param time The interval, in milliseconds, that the MqttOperationQueue will be checking the queue.
             * @return MqttOperationQueueBuilder& The builder.
             */
            MqttOperationQueueBuilder &WithQueueLoopTimeMs(uint32_t time) noexcept;

            /**
             * Sets whether the MqttOperationQueue will print logging statements to help debug and determine how the
             * MqttOperationQueue is functioning.
             * @param logging Whether to enable logging.
             * @return MqttOperationQueueBuilder& The builder.
             */
            MqttOperationQueueBuilder &WithEnableLogging(bool logging) noexcept;

            /**
             * Sets the callback that will be invoked when an operation is removed from the queue and sent successfully.
             * @param callback The callback to invoke.
             * @return MqttOperationQueueBuilder& The builder.
             */
            MqttOperationQueueBuilder &WithOnOperationSentCallback(OnOperationSent callback) noexcept;

            /**
             * Sets the callback that will be invoked when an operation is removed from the queue but fails to be sent.
             * @param callback The callback to invoke.
             * @return MqttOperationQueueBuilder& The builder.
             */
            MqttOperationQueueBuilder &WithOnOperationSentFailureCallback(OnOperationSentFailure callback) noexcept;

            /**
             * Sets the callback that will be invoked when the operation queue is full, a new operation was added
             * to the queue, and so an operation had to be removed/dropped from the queue.
             * @param callback The callback to invoke.
             * @return MqttOperationQueueBuilder& The builder.
             */
            MqttOperationQueueBuilder &WithOnOperationDroppedCallback(OnOperationDropped callback) noexcept;

            /**
             * Sets the callback that will be invoked when the operation queue is full.
             * @param callback The callback to invoke.
             * @return MqttOperationQueueBuilder& The builder.
             */
            MqttOperationQueueBuilder &WithOnQueueFullCallback(OnQueueFull callback) noexcept;

            /**
             * Sets the callback that will be invoked when the operation queue is completely empty.
             * @param callback The callback to invoke.
             * @return MqttOperationQueueBuilder& The builder.
             */
            MqttOperationQueueBuilder &WithOnQueueEmptyCallback(OnQueueEmpty callback) noexcept;

            /**
             * Returns the MqttClientConnection that will be used by the MqttOperationQueue.
             * @return std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> The MqttClientConnection that will be used by the MqttOperationQueue.
             */
            std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> GetConnection() noexcept;

            /**
             * Returns the maximum size of the operation queue in the MqttOperationQueue.
             * @return uint32_t The maximum size of the operation queue in the MqttOperationQueue.
             */
            uint32_t GetQueueLimitSize() noexcept;

            /**
             * Returns how the MqttOperationQueue will behave when the operation queue is full.
             * @return LimitBehavior How the MqttOperationQueue will behave when the operation queue is full.
             */
            LimitBehavior GetQueueLimitBehavior();

            /**
             * Returns how the MqttOperationQueue will behave when inserting a new operation into the queue.
             * @return InsertBehavior How the MqttOperationQueue will behave when inserting a new operation into the queue.
             */
            InsertBehavior GetQueueInsertBehavior();

            /**
             * Returns the maximum number of incomplete operations the MqttOperationQueue will check for.
             * @return uint32_t The maximum number of incomplete operations the MqttOperationQueue will check for.
             */
            uint32_t GetIncompleteLimit();

            /**
             * Returns the maximum number of inflight operations the MqttOperationQueue will check for.
             * @return uint32_t The maximum number of inflight operations the MqttOperationQueue will check for.
             */
            uint32_t GetInflightLimit();

            /**
             * Returns the interval, in milliseconds, that the MqttOperationQueue will wait before checking the queue.
             * @return uint32_t The interval, in milliseconds, that the MqttOperationQueue will wait before checking the queue.
             */
            uint32_t GetQueueLoopTimeMs();

            /**
             * Returns whether logging is enabled.
             * @return true Logging is enabled.
             * @return false Logging is not enabled.
             */
            bool GetEnableLogging();

            /**
             * Returns the queue OnOperationSent callback if it is defined.
             *
             * @return OnOperationSent The callback
             */
            OnOperationSent GetOnOperationSentCallback();

            /**
             * Returns the queue OnOperationSentFailure callback if it is defined.
             *
             * @return OnOperationSentFailure The callback
             */
            OnOperationSentFailure GetOnOperationSentFailureCallback();

            /**
             * Returns the queue OnOperationDropped callback if it is defined.
             *
             * @return OnOperationDropped The callback
             */
            OnOperationDropped GetOnOperationDroppedCallback();

            /**
             * Returns the queue OnQueueFull callback if it is defined.
             * @return OnQueueFull The callback
             */
            OnQueueFull GetOnQueueFullCallback();

            /**
             * Returns the queue OnQueueEmpty callback if it is defined.
             * @return OnQueueEmpty The callback
             */
            OnQueueEmpty GetOnQueueEmptyCallback();

            /**
             * Creates a new MqttOperationQueue with the settings in the MqttOperationQueueBuilder.
             * @return std::shared_ptr<MqttOperationQueue> The new MqttOperationQueue
             */
            std::shared_ptr<MqttOperationQueue> Build() noexcept;

            /**
             * Creates a new MqttOperationQueueBuilder
             */
            MqttOperationQueueBuilder() noexcept;

        private:
            std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> m_connection;
            uint32_t m_queueLimitSize;
            LimitBehavior m_queueLimitBehavior;
            InsertBehavior m_queueInsertBehavior;
            uint32_t m_incompleteLimit;
            uint32_t m_inflightLimit;
            uint32_t m_queueLoopTimeMs;
            bool m_enableLogging;

            OnOperationSent m_callbackSent;
            OnOperationSentFailure m_callbackSentFailure;
            OnOperationDropped m_callbackDropped;
            OnQueueFull m_callbackFull;
            OnQueueEmpty m_callbackEmpty;
    };
}

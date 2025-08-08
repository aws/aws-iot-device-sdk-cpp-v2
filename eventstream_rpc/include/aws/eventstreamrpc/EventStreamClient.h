#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/eventstreamrpc/Exports.h>

#include <aws/crt/DateTime.h>
#include <aws/crt/JsonObject.h>
#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>
#include <aws/crt/io/SocketOptions.h>
#include <aws/crt/io/TlsOptions.h>

#include <aws/event-stream/event_stream_rpc_client.h>

#include <functional>
#include <future>

namespace Aws
{
    namespace Crt
    {
        namespace Io
        {
            class ClientBootstrap;
        }
    } // namespace Crt
    namespace Eventstreamrpc
    {
        class EventStreamHeader;
        class MessageAmendment;
        class ClientOperation;
        class ClientConnection;
        class ClientContinuationHandler;

        using HeaderValueType = aws_event_stream_header_value_type;
        using MessageType = aws_event_stream_rpc_message_type;

        /**
         * A callback prototype that is called upon flushing a message over the wire.
         * @param errorCode A non-zero value if an error occurred while attempting to flush the message.
         */
        using OnMessageFlushCallback = std::function<void(int errorCode)>;

        /**
         * Allows the application to add headers and change the payload of the CONNECT
         * packet sent out by the client.
         * @return The `MessageAmendment` for the client to use during an attempt to connect.
         */
        using ConnectMessageAmender = std::function<const MessageAmendment &(void)>;

        /**
         * A wrapper around an `aws_event_stream_header_value_pair` object.
         */
        class AWS_EVENTSTREAMRPC_API EventStreamHeader final
        {
          public:
            EventStreamHeader(const EventStreamHeader &lhs) noexcept;
            EventStreamHeader(EventStreamHeader &&rhs) noexcept;
            explicit EventStreamHeader(
                const struct aws_event_stream_header_value_pair &header,
                Crt::Allocator *allocator = Crt::g_allocator);
            EventStreamHeader(
                const Crt::String &name,
                const Crt::String &value,
                Crt::Allocator *allocator = Crt::g_allocator) noexcept;

            ~EventStreamHeader() noexcept;

            EventStreamHeader &operator=(const EventStreamHeader &lhs) noexcept;

            Crt::String GetHeaderName() const noexcept;

            bool GetValueAsString(Crt::String &) const noexcept;

            const struct aws_event_stream_header_value_pair *GetUnderlyingHandle() const;

          private:
            Crt::Allocator *m_allocator;
            Crt::ByteBuf m_valueByteBuf;
            struct aws_event_stream_header_value_pair m_underlyingHandle;
        };

        /**
         * A means to append headers or modify the payload of a message to be sent by the client.
         * @note The exception specifiers for move, copy constructors & assignment operators are required since
         * this class is usually wrapped with `Crt::Optional`.
         */
        class AWS_EVENTSTREAMRPC_API MessageAmendment final
        {
          public:
            explicit MessageAmendment(Crt::Allocator *allocator = Crt::g_allocator) noexcept;
            MessageAmendment(const MessageAmendment &lhs);
            MessageAmendment(MessageAmendment &&rhs) noexcept;
            MessageAmendment(
                const Crt::List<EventStreamHeader> &headers,
                Crt::Optional<Crt::ByteBuf> &payload,
                Crt::Allocator *allocator) noexcept;
            explicit MessageAmendment(
                const Crt::List<EventStreamHeader> &headers,
                Crt::Allocator *allocator = Crt::g_allocator) noexcept;
            explicit MessageAmendment(
                Crt::List<EventStreamHeader> &&headers,
                Crt::Allocator *allocator = Crt::g_allocator) noexcept;
            explicit MessageAmendment(
                const Crt::ByteBuf &payload,
                Crt::Allocator *allocator = Crt::g_allocator) noexcept;

            ~MessageAmendment() noexcept;

            MessageAmendment &operator=(const MessageAmendment &lhs);
            MessageAmendment &operator=(MessageAmendment &&rhs) noexcept;

            /**
             * Add a given header to the end of the header list.
             */
            void AddHeader(EventStreamHeader &&header) noexcept;

            /**
             * Add given headers to the beginning of the header list.
             */
            void PrependHeaders(Crt::List<EventStreamHeader> &&headers);
            void SetPayload(const Crt::Optional<Crt::ByteBuf> &payload) noexcept;
            void SetPayload(Crt::Optional<Crt::ByteBuf> &&payload);
            const Crt::List<EventStreamHeader> &GetHeaders() const & noexcept;
            Crt::List<EventStreamHeader> &&GetHeaders() &&;
            const Crt::Optional<Crt::ByteBuf> &GetPayload() const & noexcept;
            Crt::Optional<Crt::ByteBuf> &&GetPayload() &&;

          private:
            Crt::List<EventStreamHeader> m_headers;
            Crt::Optional<Crt::ByteBuf> m_payload;
            Crt::Allocator *m_allocator;
        };

        enum EventStreamRpcStatusCode
        {
            EVENT_STREAM_RPC_SUCCESS = 0,
            EVENT_STREAM_RPC_NULL_PARAMETER,
            EVENT_STREAM_RPC_UNINITIALIZED,
            EVENT_STREAM_RPC_ALLOCATION_ERROR,
            EVENT_STREAM_RPC_CONNECTION_SETUP_FAILED,
            EVENT_STREAM_RPC_CONNECTION_ACCESS_DENIED,
            EVENT_STREAM_RPC_CONNECTION_ALREADY_ESTABLISHED,
            EVENT_STREAM_RPC_CONNECTION_CLOSED,
            EVENT_STREAM_RPC_CONTINUATION_CLOSED,
            EVENT_STREAM_RPC_UNKNOWN_PROTOCOL_MESSAGE,
            EVENT_STREAM_RPC_UNMAPPED_DATA,
            EVENT_STREAM_RPC_UNSUPPORTED_CONTENT_TYPE,
            EVENT_STREAM_RPC_CRT_ERROR,
            EVENT_STREAM_RPC_CONTINUATION_NOT_YET_OPENED,
        };

        /**
         * Configuration structure holding all configurations relating to eventstream RPC connection establishment
         */
        class AWS_EVENTSTREAMRPC_API ConnectionConfig
        {
          public:
            ConnectionConfig() noexcept : m_clientBootstrap(nullptr), m_connectRequestCallback(nullptr) {}
            Crt::Optional<Crt::String> GetHostName() const noexcept { return m_hostName; }
            Crt::Optional<uint32_t> GetPort() const noexcept { return m_port; }
            Crt::Optional<Crt::Io::SocketOptions> GetSocketOptions() const noexcept { return m_socketOptions; }
            Crt::Optional<MessageAmendment> GetConnectAmendment() const noexcept { return m_connectAmendment; }
            Crt::Optional<Crt::Io::TlsConnectionOptions> GetTlsConnectionOptions() const noexcept
            {
                return m_tlsConnectionOptions;
            }
            Crt::Io::ClientBootstrap *GetClientBootstrap() const noexcept { return m_clientBootstrap; }
            OnMessageFlushCallback GetConnectRequestCallback() const noexcept { return m_connectRequestCallback; }
            ConnectMessageAmender GetConnectMessageAmender() const noexcept
            {
                return [&]() -> const MessageAmendment & { return m_connectAmendment; };
            }

            void SetHostName(Crt::String hostName) noexcept { m_hostName = std::move(hostName); }
            void SetPort(uint32_t port) noexcept { m_port = port; }
            void SetSocketOptions(const Crt::Io::SocketOptions &socketOptions) noexcept
            {
                m_socketOptions = socketOptions;
            }
            void SetConnectAmendment(const MessageAmendment &connectAmendment) noexcept
            {
                m_connectAmendment = connectAmendment;
            }
            void SetTlsConnectionOptions(Crt::Io::TlsConnectionOptions tlsConnectionOptions) noexcept
            {
                m_tlsConnectionOptions = std::move(tlsConnectionOptions);
            }
            void SetClientBootstrap(Crt::Io::ClientBootstrap *clientBootstrap) noexcept
            {
                m_clientBootstrap = clientBootstrap;
            }
            void SetConnectRequestCallback(OnMessageFlushCallback connectRequestCallback) noexcept
            {
                m_connectRequestCallback = std::move(connectRequestCallback);
            }

            EventStreamRpcStatusCode Validate() const noexcept;

          protected:
            Crt::Optional<Crt::String> m_hostName;
            Crt::Optional<uint32_t> m_port;
            Crt::Optional<Crt::Io::SocketOptions> m_socketOptions;
            Crt::Optional<Crt::Io::TlsConnectionOptions> m_tlsConnectionOptions;
            Crt::Io::ClientBootstrap *m_clientBootstrap;
            MessageAmendment m_connectAmendment;
            OnMessageFlushCallback m_connectRequestCallback;
        };

        struct AWS_EVENTSTREAMRPC_API RpcError
        {
            explicit operator bool() const noexcept { return baseStatus == EVENT_STREAM_RPC_SUCCESS; }
            Crt::String StatusToString() const;

            EventStreamRpcStatusCode baseStatus;
            int crtError;
        };

        /**
         * Handler interface for connection lifecycle events.
         */
        class AWS_EVENTSTREAMRPC_API ConnectionLifecycleHandler
        {
          public:
            virtual ~ConnectionLifecycleHandler() noexcept = default;

            /**
             * This callback is only invoked upon receiving a CONNECT_ACK with the
             * CONNECTION_ACCEPTED flag set by the server. Therefore, once this callback
             * is invoked, the `ClientConnection` is ready to be used for sending messages.
             */
            virtual void OnConnectCallback();

            /**
             * Invoked upon connection shutdown.
             * @param status The status upon disconnection. It can be treated as a bool
             * with true implying a successful disconnection.
             */
            virtual void OnDisconnectCallback(RpcError status);

            /**
             * Invoked upon receiving an error. Use the return value to determine
             * whether or not to force the connection to close.
             * @param status Details about the error encountered.
             */
            virtual bool OnErrorCallback(RpcError status);

            /**
             * Invoked upon receiving a ping from the server. The `headers` and `payload`
             * refer to what is contained in the ping message.
             */
            virtual void OnPingCallback(
                const Crt::List<EventStreamHeader> &headers,
                const Crt::Optional<Crt::ByteBuf> &payload);
        };

        class ClientConnectionImpl;
        class ClientContinuationImpl;

        /**
         * Class representing a connection to an RPC server.
         */
        class AWS_EVENTSTREAMRPC_API ClientConnection final
        {
          public:
            ClientConnection(Crt::Allocator *allocator, aws_client_bootstrap *bootstrap) noexcept;
            ~ClientConnection() noexcept;

            /**
             * Initiates a new outgoing event-stream-rpc connection.
             * @param connectionOptions Connection options.
             * @param connectionLifecycleHandler Handler to process connection lifecycle events.
             * @return Future that will be resolved when connection either succeeds or fails.
             */
            std::future<RpcError> Connect(
                const ConnectionConfig &connectionOptions,
                ConnectionLifecycleHandler *connectionLifecycleHandler) noexcept;

            /**
             * Create a new stream (continuation).
             * @return A newly created continuation.
             */
            std::shared_ptr<ClientContinuationImpl> NewStream() noexcept;

            /**
             * Close the connection.
             */
            void Close() noexcept;

            /**
             * Check if the connection is open.
             * @return True if the connection is open, false otherwise.
             */
            bool IsOpen() const noexcept;

          private:
            std::shared_ptr<ClientConnectionImpl> m_impl;
        };

        /**
         * Vestigial, do-nothing class that remains for backwards compatibility with the
         * original, publicly-visible class hierarchy.
         */
        class AWS_EVENTSTREAMRPC_API ClientContinuationHandler
        {
          public:
            virtual ~ClientContinuationHandler() noexcept = default;
        };

        /**
         * Base class for types used by operations.
         */
        class AWS_EVENTSTREAMRPC_API AbstractShapeBase
        {
          public:
            AbstractShapeBase() noexcept;
            virtual ~AbstractShapeBase() noexcept = default;
            static void s_customDeleter(AbstractShapeBase *shape) noexcept;
            virtual void SerializeToJsonObject(Crt::JsonObject &payloadObject) const = 0;
            virtual Crt::String GetModelName() const noexcept = 0;

          protected:
            Crt::Allocator *m_allocator;
        };

        /**
         * Base class for errors used by operations.
         */
        class AWS_EVENTSTREAMRPC_API OperationError : public AbstractShapeBase
        {
          public:
            explicit OperationError() noexcept = default;
            static void s_customDeleter(OperationError *shape) noexcept;
            void SerializeToJsonObject(Crt::JsonObject &payloadObject) const override;
            virtual Crt::Optional<Crt::String> GetMessage() const noexcept = 0;
        };

        /**
         * Base class for all operation stream handlers.
         * For operations with a streaming response (0+ messages that may arrive
         * after the initial response).
         */
        class AWS_EVENTSTREAMRPC_API StreamResponseHandler
        {
          public:
            virtual ~StreamResponseHandler() noexcept = default;

            /**
             * Invoked when stream is closed, so no more messages will be received.
             */
            virtual void OnStreamClosed();

          protected:
            friend class ClientContinuationImpl;
            /**
             * Invoked when a message is received on this continuation.
             */
            virtual void OnStreamEvent(Crt::ScopedResource<AbstractShapeBase> response);
            /**
             * Invoked when a message is received on this continuation but results in an error.
             *
             * This callback can return true so that the stream is closed afterwards.
             */
            virtual bool OnStreamError(Crt::ScopedResource<OperationError> operationError, RpcError rpcError);
        };

        enum ResultType
        {
            NONE,
            OPERATION_RESPONSE,
            OPERATION_ERROR,
            RPC_ERROR
        };

        class AWS_EVENTSTREAMRPC_API EventstreamResultVariantType
        {
          public:
            EventstreamResultVariantType();
            explicit EventstreamResultVariantType(Crt::ScopedResource<AbstractShapeBase> &&modeledResponse) noexcept;
            explicit EventstreamResultVariantType(Crt::ScopedResource<OperationError> &&modeledError) noexcept;
            explicit EventstreamResultVariantType(RpcError rpcError) noexcept;
            EventstreamResultVariantType(EventstreamResultVariantType &&rhs) noexcept;
            ~EventstreamResultVariantType() = default;

            EventstreamResultVariantType &operator=(EventstreamResultVariantType &&rhs) noexcept;

            ResultType GetType() const { return m_type; }

            AbstractShapeBase *GetModeledResponse() const;

            OperationError *GetModeledError() const;

            RpcError GetRpcError() const;

          private:
            ResultType m_type;

            Crt::ScopedResource<AbstractShapeBase> m_modeledResponse;
            Crt::ScopedResource<OperationError> m_modeledError;
            RpcError m_rpcError;
        };

        using ExpectedResponseFactory = std::function<
            Crt::ScopedResource<AbstractShapeBase>(const Crt::StringView &payload, Crt::Allocator *allocator)>;
        using ErrorResponseFactory = std::function<
            Crt::ScopedResource<OperationError>(const Crt::StringView &payload, Crt::Allocator *allocator)>;

        using LoneResponseRetriever = std::function<ExpectedResponseFactory(const Crt::String &modelName)>;
        using StreamingResponseRetriever = std::function<ExpectedResponseFactory(const Crt::String &modelName)>;
        using ErrorResponseRetriever = std::function<ErrorResponseFactory(const Crt::String &modelName)>;

        class AWS_EVENTSTREAMRPC_API ResponseRetriever
        {
            /* An interface shared by all operations for retrieving the response object given the model name. */
          public:
            virtual ~ResponseRetriever() noexcept = default;
            virtual ExpectedResponseFactory GetInitialResponseFromModelName(
                const Crt::String &modelName) const noexcept = 0;
            virtual ExpectedResponseFactory GetStreamingResponseFromModelName(
                const Crt::String &modelName) const noexcept = 0;
            virtual ErrorResponseFactory GetOperationErrorFromModelName(
                const Crt::String &modelName) const noexcept = 0;
        };

        class AWS_EVENTSTREAMRPC_API ServiceModel
        {
          public:
            virtual ~ServiceModel() noexcept = default;
            virtual Crt::ScopedResource<OperationError> AllocateOperationErrorFromPayload(
                const Crt::String &errorModelName,
                Crt::StringView stringView,
                Crt::Allocator *allocator) const noexcept = 0;
        };

        /**
         * All generated model types implement this interface, including errors.
         */
        class AWS_EVENTSTREAMRPC_API OperationModelContext
        {
          public:
            explicit OperationModelContext(const ServiceModel &serviceModel) noexcept;

            virtual ~OperationModelContext() noexcept = default;

            /**
             * Parse the given string into an initial response object.
             * @param stringView String to parse the response from.
             * @param allocator Allocator to use.
             * @return The initial response object.
             */
            virtual Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
                Crt::StringView stringView,
                Crt::Allocator *allocator) const noexcept = 0;

            /**
             * Parse the given string into a streaming response object.
             * @param stringView String to parse the response from.
             * @param allocator Allocator to use.
             * @return The streaming response object.
             */
            virtual Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
                Crt::StringView stringView,
                Crt::Allocator *allocator) const noexcept = 0;

            /**
             * Get the initial response type name.
             * @return The initial response type name.
             */
            virtual Crt::String GetInitialResponseModelName() const noexcept = 0;

            /**
             * Get the request type name.
             * @return The request type name.
             */
            virtual Crt::String GetRequestModelName() const noexcept = 0;

            /**
             * Get the streaming response type name.
             * @return The streaming response type name.
             */
            virtual Crt::Optional<Crt::String> GetStreamingResponseModelName() const noexcept = 0;

            /**
             * Returns the canonical operation name associated with this context across any client language.
             * Namespace included.
             * Example: aws.greengrass#SubscribeToTopic
             * @return The canonical operation name associated with this context across any client language.
             */
            virtual Crt::String GetOperationName() const noexcept = 0;

            /**
             * Parse the given string into an operation error.
             * @param errorModelName The model name.
             * @param stringView String to parse the error from.
             * @param allocator Allocator to use.
             * @return The operation error.
             */
            Crt::ScopedResource<OperationError> AllocateOperationErrorFromPayload(
                const Crt::String &errorModelName,
                Crt::StringView stringView,
                Crt::Allocator *allocator) const noexcept
            {
                return m_serviceModel.AllocateOperationErrorFromPayload(errorModelName, stringView, allocator);
            }

          private:
            const ServiceModel &m_serviceModel;
        };

        /**
         * Interface for an RPC operation.
         */
        class AWS_EVENTSTREAMRPC_API ClientOperation : public ClientContinuationHandler
        {
          public:
            ClientOperation(
                ClientConnection &connection,
                std::shared_ptr<StreamResponseHandler> streamHandler,
                const std::shared_ptr<OperationModelContext> &operationModelContext,
                Crt::Allocator *allocator) noexcept;
            ~ClientOperation() noexcept override;

            ClientOperation(const ClientOperation &clientOperation) noexcept = delete;
            ClientOperation(ClientOperation &&clientOperation) noexcept = delete;
            bool operator=(const ClientOperation &clientOperation) noexcept = delete;
            bool operator=(ClientOperation &&clientOperation) noexcept = delete;

            /**
             * Close the stream on which operation is sent.
             * @note This function sends a message with the message flag set to terminate the stream.
             * @param onMessageFlushCallback Callback to invoke when the closing message is flushed to the underlying
             * transport.
             * @return Future which will be resolved once the message is sent.
             */
            std::future<RpcError> Close(OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;

            /**
             * @deprecated This no longer does anything useful.  Launch policy was added because of a
             * mistake in the implementation that was attempting to chain two promises together.
             *
             * Set the launch mode for executing operations. The mode is set to std::launch::deferred by default.
             * @param mode The launch mode to use.
             */
            void WithLaunchMode(std::launch mode) noexcept;

          protected:
            /**
             * Initiate a new client stream. Send the shape for the new stream.
             * @param shape A parameter for RPC operation.
             * @param onMessageFlushCallback Callback to invoke when the shape is flushed to the underlying transport.
             * @param onResultCallback Callback to invoke with the untransformed activation result
             * @return Future which will be resolved once the message is sent.
             */
            std::future<RpcError> Activate(
                const AbstractShapeBase *shape,
                OnMessageFlushCallback &&onMessageFlushCallback,
                std::function<void(EventstreamResultVariantType &&)> &&onResultCallback) noexcept;

            /**
             * Sends a message on the stream
             *
             * @param shape Modeled representation of the message to send
             * @param onMessageFlushCallback Optional callback to invoke when the message is written or fails to send
             *
             * @return Future which will be resolved once the message is sent.
             */
            std::future<RpcError> SendStreamMessage(
                const AbstractShapeBase *shape,
                OnMessageFlushCallback &&onMessageFlushCallback) noexcept;

            /**
             * Returns the canonical model name associated with this operation across any client language.
             * Namespace included.
             * @return The model name.
             */
            virtual Crt::String GetModelName() const noexcept;

          private:
            Aws::Crt::Allocator *m_allocator;

            std::shared_ptr<ClientContinuationImpl> m_impl;
        };

    } // namespace Eventstreamrpc
} // namespace Aws

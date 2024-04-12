#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

/* This file is generated. */

#include <aws/eventstreamrpc/EventStreamClient.h>

#include <aws/crt/DateTime.h>
#include <awstest/Exports.h>

#include <memory>

using namespace Aws::Eventstreamrpc;

namespace Awstest
{
    class EchoTestRpcClient;
    class EchoTestRpcServiceModel;
    /**
     * A simple product definition
     */
    class AWS_ECHOTESTRPC_API Product : public AbstractShapeBase
    {
      public:
        Product() noexcept {}
        Product(const Product &) = default;
        /**
         * The product's name
         */
        void SetName(const Aws::Crt::String &name) noexcept { m_name = name; }
        /**
         * The product's name
         */
        Aws::Crt::Optional<Aws::Crt::String> GetName() noexcept { return m_name; }
        /**
         * How much the product costs
         */
        void SetPrice(const float &price) noexcept { m_price = price; }
        /**
         * How much the product costs
         */
        Aws::Crt::Optional<float> GetPrice() noexcept { return m_price; }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(Product &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(Product *) noexcept;
        /* This needs to be defined so that `Product` can be used as a key in maps. */
        bool operator<(const Product &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        Aws::Crt::Optional<Aws::Crt::String> m_name;
        Aws::Crt::Optional<float> m_price;
    };

    /**
     * Shape representing a pair of values
     */
    class AWS_ECHOTESTRPC_API Pair : public AbstractShapeBase
    {
      public:
        Pair() noexcept {}
        Pair(const Pair &) = default;
        /**
         * Pair.key as a string
         */
        void SetKey(const Aws::Crt::String &key) noexcept { m_key = key; }
        /**
         * Pair.key as a string
         */
        Aws::Crt::Optional<Aws::Crt::String> GetKey() noexcept { return m_key; }
        /**
         * Pair.value also a string!
         */
        void SetValue(const Aws::Crt::String &value) noexcept { m_value = value; }
        /**
         * Pair.value also a string!
         */
        Aws::Crt::Optional<Aws::Crt::String> GetValue() noexcept { return m_value; }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(Pair &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(Pair *) noexcept;
        /* This needs to be defined so that `Pair` can be used as a key in maps. */
        bool operator<(const Pair &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        Aws::Crt::Optional<Aws::Crt::String> m_key;
        Aws::Crt::Optional<Aws::Crt::String> m_value;
    };

    enum FruitEnum
    {
        FRUIT_ENUM_APPLE,
        FRUIT_ENUM_ORANGE,
        FRUIT_ENUM_BANANA,
        FRUIT_ENUM_PINEAPPLE
    };

    /**
     * A simple customer definition
     */
    class AWS_ECHOTESTRPC_API Customer : public AbstractShapeBase
    {
      public:
        Customer() noexcept {}
        Customer(const Customer &) = default;
        /**
         * Opaque customer identifier
         */
        void SetId(const int64_t &id) noexcept { m_id = id; }
        /**
         * Opaque customer identifier
         */
        Aws::Crt::Optional<int64_t> GetId() noexcept { return m_id; }
        /**
         * First name of the customer
         */
        void SetFirstName(const Aws::Crt::String &firstName) noexcept { m_firstName = firstName; }
        /**
         * First name of the customer
         */
        Aws::Crt::Optional<Aws::Crt::String> GetFirstName() noexcept { return m_firstName; }
        /**
         * Last name of the customer
         */
        void SetLastName(const Aws::Crt::String &lastName) noexcept { m_lastName = lastName; }
        /**
         * Last name of the customer
         */
        Aws::Crt::Optional<Aws::Crt::String> GetLastName() noexcept { return m_lastName; }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(Customer &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(Customer *) noexcept;
        /* This needs to be defined so that `Customer` can be used as a key in maps. */
        bool operator<(const Customer &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        Aws::Crt::Optional<int64_t> m_id;
        Aws::Crt::Optional<Aws::Crt::String> m_firstName;
        Aws::Crt::Optional<Aws::Crt::String> m_lastName;
    };

    /**
     * Data associated with some notion of a message
     */
    class AWS_ECHOTESTRPC_API MessageData : public AbstractShapeBase
    {
      public:
        MessageData() noexcept {}
        MessageData(const MessageData &) = default;
        /**
         * Some string data
         */
        void SetStringMessage(const Aws::Crt::String &stringMessage) noexcept { m_stringMessage = stringMessage; }
        /**
         * Some string data
         */
        Aws::Crt::Optional<Aws::Crt::String> GetStringMessage() noexcept { return m_stringMessage; }
        /**
         * Some boolean data
         */
        void SetBooleanMessage(const bool &booleanMessage) noexcept { m_booleanMessage = booleanMessage; }
        /**
         * Some boolean data
         */
        Aws::Crt::Optional<bool> GetBooleanMessage() noexcept { return m_booleanMessage; }
        /**
         * Some timestamp data
         */
        void SetTimeMessage(const Aws::Crt::DateTime &timeMessage) noexcept { m_timeMessage = timeMessage; }
        /**
         * Some timestamp data
         */
        Aws::Crt::Optional<Aws::Crt::DateTime> GetTimeMessage() noexcept { return m_timeMessage; }
        /**
         * Some document data
         */
        void SetDocumentMessage(const Aws::Crt::JsonObject &documentMessage) noexcept
        {
            m_documentMessage = documentMessage;
        }
        /**
         * Some document data
         */
        Aws::Crt::Optional<Aws::Crt::JsonObject> GetDocumentMessage() noexcept { return m_documentMessage; }
        /**
         * Some FruitEnum data
         */
        void SetEnumMessage(FruitEnum enumMessage) noexcept;
        /**
         * Some FruitEnum data
         */
        Aws::Crt::Optional<FruitEnum> GetEnumMessage() noexcept;
        /**
         * Some blob data
         */
        void SetBlobMessage(const Aws::Crt::Vector<uint8_t> &blobMessage) noexcept { m_blobMessage = blobMessage; }
        /**
         * Some blob data
         */
        Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> GetBlobMessage() noexcept { return m_blobMessage; }
        /**
         * Some list of strings data
         */
        void SetStringListMessage(const Aws::Crt::Vector<Aws::Crt::String> &stringListMessage) noexcept
        {
            m_stringListMessage = stringListMessage;
        }
        /**
         * Some list of strings data
         */
        Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> GetStringListMessage() noexcept
        {
            return m_stringListMessage;
        }
        /**
         * A list of key-value pairs
         */
        void SetKeyValuePairList(const Aws::Crt::Vector<Pair> &keyValuePairList) noexcept
        {
            m_keyValuePairList = keyValuePairList;
        }
        /**
         * A list of key-value pairs
         */
        Aws::Crt::Optional<Aws::Crt::Vector<Pair>> GetKeyValuePairList() noexcept { return m_keyValuePairList; }
        /**
         * A map from strings to Product shapes
         */
        void SetStringToValue(const Aws::Crt::Map<Aws::Crt::String, Product> &stringToValue) noexcept
        {
            m_stringToValue = stringToValue;
        }
        /**
         * A map from strings to Product shapes
         */
        Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Product>> GetStringToValue() noexcept
        {
            return m_stringToValue;
        }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(MessageData &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(MessageData *) noexcept;
        /* This needs to be defined so that `MessageData` can be used as a key in maps. */
        bool operator<(const MessageData &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        Aws::Crt::Optional<Aws::Crt::String> m_stringMessage;
        Aws::Crt::Optional<bool> m_booleanMessage;
        Aws::Crt::Optional<Aws::Crt::DateTime> m_timeMessage;
        Aws::Crt::Optional<Aws::Crt::JsonObject> m_documentMessage;
        Aws::Crt::Optional<Aws::Crt::String> m_enumMessage;
        Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> m_blobMessage;
        Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> m_stringListMessage;
        Aws::Crt::Optional<Aws::Crt::Vector<Pair>> m_keyValuePairList;
        Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Product>> m_stringToValue;
    };

    /**
     * A union of values related to a streaming message.  Only one field may bet set.
     */
    class AWS_ECHOTESTRPC_API EchoStreamingMessage : public AbstractShapeBase
    {
      public:
        EchoStreamingMessage() noexcept {}
        EchoStreamingMessage &operator=(const EchoStreamingMessage &) noexcept;
        EchoStreamingMessage(const EchoStreamingMessage &objectToCopy) { *this = objectToCopy; }
        /**
         * A message data record
         */
        void SetStreamMessage(const MessageData &streamMessage) noexcept
        {
            m_streamMessage = streamMessage;
            m_chosenMember = TAG_STREAM_MESSAGE;
        }
        /**
         * A message data record
         */
        Aws::Crt::Optional<MessageData> GetStreamMessage() noexcept
        {
            if (m_chosenMember == TAG_STREAM_MESSAGE)
            {
                return m_streamMessage;
            }
            else
            {
                return Aws::Crt::Optional<MessageData>();
            }
        }
        /**
         * A key value pair
         */
        void SetKeyValuePair(const Pair &keyValuePair) noexcept
        {
            m_keyValuePair = keyValuePair;
            m_chosenMember = TAG_KEY_VALUE_PAIR;
        }
        /**
         * A key value pair
         */
        Aws::Crt::Optional<Pair> GetKeyValuePair() noexcept
        {
            if (m_chosenMember == TAG_KEY_VALUE_PAIR)
            {
                return m_keyValuePair;
            }
            else
            {
                return Aws::Crt::Optional<Pair>();
            }
        }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(EchoStreamingMessage &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(EchoStreamingMessage *) noexcept;
        /* This needs to be defined so that `EchoStreamingMessage` can be used as a key in maps. */
        bool operator<(const EchoStreamingMessage &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        enum ChosenMember
        {
            TAG_STREAM_MESSAGE,
            TAG_KEY_VALUE_PAIR
        } m_chosenMember;
        Aws::Crt::Optional<MessageData> m_streamMessage;
        Aws::Crt::Optional<Pair> m_keyValuePair;
    };

    /**
     * A sample error shape
     */
    class AWS_ECHOTESTRPC_API ServiceError : public OperationError
    {
      public:
        ServiceError() noexcept {}
        ServiceError(const ServiceError &) = default;
        /**
         * An error message
         */
        void SetMessage(const Aws::Crt::String &message) noexcept { m_message = message; }
        /**
         * An error message
         */
        Aws::Crt::Optional<Aws::Crt::String> GetMessage() noexcept override { return m_message; }
        /**
         * Some auxiliary value
         */
        void SetValue(const Aws::Crt::String &value) noexcept { m_value = value; }
        /**
         * Some auxiliary value
         */
        Aws::Crt::Optional<Aws::Crt::String> GetValue() noexcept { return m_value; }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(ServiceError &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<OperationError> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(ServiceError *) noexcept;
        /* This needs to be defined so that `ServiceError` can be used as a key in maps. */
        bool operator<(const ServiceError &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        Aws::Crt::Optional<Aws::Crt::String> m_message;
        Aws::Crt::Optional<Aws::Crt::String> m_value;
    };

    /**
     * All data associated with the result of a GetAllProducts operation
     */
    class AWS_ECHOTESTRPC_API GetAllProductsResponse : public AbstractShapeBase
    {
      public:
        GetAllProductsResponse() noexcept {}
        GetAllProductsResponse(const GetAllProductsResponse &) = default;
        /**
         * A map from strings to products
         */
        void SetProducts(const Aws::Crt::Map<Aws::Crt::String, Product> &products) noexcept { m_products = products; }
        /**
         * A map from strings to products
         */
        Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Product>> GetProducts() noexcept { return m_products; }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(GetAllProductsResponse &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(GetAllProductsResponse *) noexcept;
        /* This needs to be defined so that `GetAllProductsResponse` can be used as a key in maps. */
        bool operator<(const GetAllProductsResponse &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Product>> m_products;
    };

    /**
     * Data needed to perform a GetAllProducts operation
     */
    class AWS_ECHOTESTRPC_API GetAllProductsRequest : public AbstractShapeBase
    {
      public:
        GetAllProductsRequest() noexcept {}
        GetAllProductsRequest(const GetAllProductsRequest &) = default;
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(GetAllProductsRequest &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(GetAllProductsRequest *) noexcept;
        /* This needs to be defined so that `GetAllProductsRequest` can be used as a key in maps. */
        bool operator<(const GetAllProductsRequest &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
    };

    /**
     * All data associated with the result of a GetAllCustomers operation
     */
    class AWS_ECHOTESTRPC_API GetAllCustomersResponse : public AbstractShapeBase
    {
      public:
        GetAllCustomersResponse() noexcept {}
        GetAllCustomersResponse(const GetAllCustomersResponse &) = default;
        /**
         * A list of all known customers
         */
        void SetCustomers(const Aws::Crt::Vector<Customer> &customers) noexcept { m_customers = customers; }
        /**
         * A list of all known customers
         */
        Aws::Crt::Optional<Aws::Crt::Vector<Customer>> GetCustomers() noexcept { return m_customers; }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(GetAllCustomersResponse &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(GetAllCustomersResponse *) noexcept;
        /* This needs to be defined so that `GetAllCustomersResponse` can be used as a key in maps. */
        bool operator<(const GetAllCustomersResponse &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        Aws::Crt::Optional<Aws::Crt::Vector<Customer>> m_customers;
    };

    /**
     * Data needed to perform a GetAllCustomers operation
     */
    class AWS_ECHOTESTRPC_API GetAllCustomersRequest : public AbstractShapeBase
    {
      public:
        GetAllCustomersRequest() noexcept {}
        GetAllCustomersRequest(const GetAllCustomersRequest &) = default;
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(GetAllCustomersRequest &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(GetAllCustomersRequest *) noexcept;
        /* This needs to be defined so that `GetAllCustomersRequest` can be used as a key in maps. */
        bool operator<(const GetAllCustomersRequest &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
    };

    /**
     * Data associated with the response to starting an EchoStreaming streaming operation
     */
    class AWS_ECHOTESTRPC_API EchoStreamingResponse : public AbstractShapeBase
    {
      public:
        EchoStreamingResponse() noexcept {}
        EchoStreamingResponse(const EchoStreamingResponse &) = default;
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(EchoStreamingResponse &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(EchoStreamingResponse *) noexcept;
        /* This needs to be defined so that `EchoStreamingResponse` can be used as a key in maps. */
        bool operator<(const EchoStreamingResponse &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
    };

    /**
     * Data needed to start an EchoStreaming streaming operation
     */
    class AWS_ECHOTESTRPC_API EchoStreamingRequest : public AbstractShapeBase
    {
      public:
        EchoStreamingRequest() noexcept {}
        EchoStreamingRequest(const EchoStreamingRequest &) = default;
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(EchoStreamingRequest &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(EchoStreamingRequest *) noexcept;
        /* This needs to be defined so that `EchoStreamingRequest` can be used as a key in maps. */
        bool operator<(const EchoStreamingRequest &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
    };

    /**
     * All data associated with the result of an EchoMessage operation
     */
    class AWS_ECHOTESTRPC_API EchoMessageResponse : public AbstractShapeBase
    {
      public:
        EchoMessageResponse() noexcept {}
        EchoMessageResponse(const EchoMessageResponse &) = default;
        /**
         * Some message data
         */
        void SetMessage(const MessageData &message) noexcept { m_message = message; }
        /**
         * Some message data
         */
        Aws::Crt::Optional<MessageData> GetMessage() noexcept { return m_message; }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(EchoMessageResponse &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(EchoMessageResponse *) noexcept;
        /* This needs to be defined so that `EchoMessageResponse` can be used as a key in maps. */
        bool operator<(const EchoMessageResponse &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        Aws::Crt::Optional<MessageData> m_message;
    };

    /**
     * Data needed to perform an EchoMessage operation
     */
    class AWS_ECHOTESTRPC_API EchoMessageRequest : public AbstractShapeBase
    {
      public:
        EchoMessageRequest() noexcept {}
        EchoMessageRequest(const EchoMessageRequest &) = default;
        /**
         * Some message data
         */
        void SetMessage(const MessageData &message) noexcept { m_message = message; }
        /**
         * Some message data
         */
        Aws::Crt::Optional<MessageData> GetMessage() noexcept { return m_message; }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(EchoMessageRequest &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(EchoMessageRequest *) noexcept;
        /* This needs to be defined so that `EchoMessageRequest` can be used as a key in maps. */
        bool operator<(const EchoMessageRequest &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        Aws::Crt::Optional<MessageData> m_message;
    };

    /**
     * All data associated with the result of an EchoMessage operation
     */
    class AWS_ECHOTESTRPC_API CauseServiceErrorResponse : public AbstractShapeBase
    {
      public:
        CauseServiceErrorResponse() noexcept {}
        CauseServiceErrorResponse(const CauseServiceErrorResponse &) = default;
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(CauseServiceErrorResponse &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(CauseServiceErrorResponse *) noexcept;
        /* This needs to be defined so that `CauseServiceErrorResponse` can be used as a key in maps. */
        bool operator<(const CauseServiceErrorResponse &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
    };

    /**
     * Data needed to perform a CauseServiceError operation
     */
    class AWS_ECHOTESTRPC_API CauseServiceErrorRequest : public AbstractShapeBase
    {
      public:
        CauseServiceErrorRequest() noexcept {}
        CauseServiceErrorRequest(const CauseServiceErrorRequest &) = default;
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(CauseServiceErrorRequest &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(CauseServiceErrorRequest *) noexcept;
        /* This needs to be defined so that `CauseServiceErrorRequest` can be used as a key in maps. */
        bool operator<(const CauseServiceErrorRequest &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
    };

    class AWS_ECHOTESTRPC_API GetAllProductsOperationContext : public OperationModelContext
    {
      public:
        GetAllProductsOperationContext(const EchoTestRpcServiceModel &serviceModel) noexcept;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::String GetRequestModelName() const noexcept override;
        Aws::Crt::String GetInitialResponseModelName() const noexcept override;
        Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
        Aws::Crt::String GetOperationName() const noexcept override;
    };

    class AWS_ECHOTESTRPC_API GetAllProductsResult
    {
      public:
        GetAllProductsResult() noexcept {}
        GetAllProductsResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
        GetAllProductsResponse *GetOperationResponse() const noexcept
        {
            return static_cast<GetAllProductsResponse *>(m_taggedResult.GetOperationResponse());
        }
        /**
         * @return true if the response is associated with an expected response;
         * false if the response is associated with an error.
         */
        operator bool() const noexcept { return m_taggedResult == true; }
        OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
        RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
        ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

      private:
        TaggedResult m_taggedResult;
    };

    class AWS_ECHOTESTRPC_API GetAllProductsOperation : public ClientOperation
    {
      public:
        GetAllProductsOperation(
            ClientConnection &connection,
            const GetAllProductsOperationContext &operationContext,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
        /**
         * Used to activate a stream for the `GetAllProductsOperation`
         * @param request The request used for the `GetAllProductsOperation`
         * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
         * @return An `RpcError` that can be used to check whether the stream was activated.
         */
        std::future<RpcError> Activate(
            const GetAllProductsRequest &request,
            OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
        /**
         * Retrieve the result from activating the stream.
         */
        std::future<GetAllProductsResult> GetResult() noexcept;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;
    };

    class AWS_ECHOTESTRPC_API CauseServiceErrorOperationContext : public OperationModelContext
    {
      public:
        CauseServiceErrorOperationContext(const EchoTestRpcServiceModel &serviceModel) noexcept;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::String GetRequestModelName() const noexcept override;
        Aws::Crt::String GetInitialResponseModelName() const noexcept override;
        Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
        Aws::Crt::String GetOperationName() const noexcept override;
    };

    class AWS_ECHOTESTRPC_API CauseServiceErrorResult
    {
      public:
        CauseServiceErrorResult() noexcept {}
        CauseServiceErrorResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
        CauseServiceErrorResponse *GetOperationResponse() const noexcept
        {
            return static_cast<CauseServiceErrorResponse *>(m_taggedResult.GetOperationResponse());
        }
        /**
         * @return true if the response is associated with an expected response;
         * false if the response is associated with an error.
         */
        operator bool() const noexcept { return m_taggedResult == true; }
        OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
        RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
        ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

      private:
        TaggedResult m_taggedResult;
    };

    class AWS_ECHOTESTRPC_API CauseServiceErrorOperation : public ClientOperation
    {
      public:
        CauseServiceErrorOperation(
            ClientConnection &connection,
            const CauseServiceErrorOperationContext &operationContext,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
        /**
         * Used to activate a stream for the `CauseServiceErrorOperation`
         * @param request The request used for the `CauseServiceErrorOperation`
         * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
         * @return An `RpcError` that can be used to check whether the stream was activated.
         */
        std::future<RpcError> Activate(
            const CauseServiceErrorRequest &request,
            OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
        /**
         * Retrieve the result from activating the stream.
         */
        std::future<CauseServiceErrorResult> GetResult() noexcept;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;
    };

    class AWS_ECHOTESTRPC_API CauseStreamServiceToErrorStreamHandler : public StreamResponseHandler
    {
      public:
        virtual void OnStreamEvent(EchoStreamingMessage *response) { (void)response; }

        /**
         * A callback that is invoked when an error occurs while parsing a message from the stream.
         * @param rpcError The RPC error containing the status and possibly a CRT error.
         */
        virtual bool OnStreamError(RpcError rpcError)
        {
            (void)rpcError;
            return true;
        }

        /**
         * A callback that is invoked upon receiving an error of type `ServiceError`.
         * @param operationError The error message being received.
         */
        virtual bool OnStreamError(ServiceError *operationError)
        {
            (void)operationError;
            return true;
        }

        /**
         * A callback that is invoked upon receiving ANY error response from the server.
         * @param operationError The error message being received.
         */
        virtual bool OnStreamError(OperationError *operationError)
        {
            (void)operationError;
            return true;
        }

      private:
        /**
         * Invoked when a message is received on this continuation.
         */
        void OnStreamEvent(Aws::Crt::ScopedResource<AbstractShapeBase> response) override;
        /**
         * Invoked when a message is received on this continuation but results in an error.
         *
         * This callback can return true so that the stream is closed afterwards.
         */
        bool OnStreamError(Aws::Crt::ScopedResource<OperationError> error, RpcError rpcError) override;
    };
    class AWS_ECHOTESTRPC_API CauseStreamServiceToErrorOperationContext : public OperationModelContext
    {
      public:
        CauseStreamServiceToErrorOperationContext(const EchoTestRpcServiceModel &serviceModel) noexcept;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::String GetRequestModelName() const noexcept override;
        Aws::Crt::String GetInitialResponseModelName() const noexcept override;
        Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
        Aws::Crt::String GetOperationName() const noexcept override;
    };

    class AWS_ECHOTESTRPC_API CauseStreamServiceToErrorResult
    {
      public:
        CauseStreamServiceToErrorResult() noexcept {}
        CauseStreamServiceToErrorResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult))
        {
        }
        EchoStreamingResponse *GetOperationResponse() const noexcept
        {
            return static_cast<EchoStreamingResponse *>(m_taggedResult.GetOperationResponse());
        }
        /**
         * @return true if the response is associated with an expected response;
         * false if the response is associated with an error.
         */
        operator bool() const noexcept { return m_taggedResult == true; }
        OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
        RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
        ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

      private:
        TaggedResult m_taggedResult;
    };

    class AWS_ECHOTESTRPC_API CauseStreamServiceToErrorOperation : public ClientOperation
    {
      public:
        CauseStreamServiceToErrorOperation(
            ClientConnection &connection,
            std::shared_ptr<CauseStreamServiceToErrorStreamHandler> streamHandler,
            const CauseStreamServiceToErrorOperationContext &operationContext,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
        /**
         * Used to activate a stream for the `CauseStreamServiceToErrorOperation`
         * @param request The request used for the `CauseStreamServiceToErrorOperation`
         * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
         * @return An `RpcError` that can be used to check whether the stream was activated.
         */
        std::future<RpcError> Activate(
            const EchoStreamingRequest &request,
            OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
        /**
         * Retrieve the result from activating the stream.
         */
        std::future<CauseStreamServiceToErrorResult> GetResult() noexcept;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;
    };

    class AWS_ECHOTESTRPC_API EchoStreamMessagesStreamHandler : public StreamResponseHandler
    {
      public:
        virtual void OnStreamEvent(EchoStreamingMessage *response) { (void)response; }

        /**
         * A callback that is invoked when an error occurs while parsing a message from the stream.
         * @param rpcError The RPC error containing the status and possibly a CRT error.
         */
        virtual bool OnStreamError(RpcError rpcError)
        {
            (void)rpcError;
            return true;
        }

        /**
         * A callback that is invoked upon receiving ANY error response from the server.
         * @param operationError The error message being received.
         */
        virtual bool OnStreamError(OperationError *operationError)
        {
            (void)operationError;
            return true;
        }

      private:
        /**
         * Invoked when a message is received on this continuation.
         */
        void OnStreamEvent(Aws::Crt::ScopedResource<AbstractShapeBase> response) override;
        /**
         * Invoked when a message is received on this continuation but results in an error.
         *
         * This callback can return true so that the stream is closed afterwards.
         */
        bool OnStreamError(Aws::Crt::ScopedResource<OperationError> error, RpcError rpcError) override;
    };
    class AWS_ECHOTESTRPC_API EchoStreamMessagesOperationContext : public OperationModelContext
    {
      public:
        EchoStreamMessagesOperationContext(const EchoTestRpcServiceModel &serviceModel) noexcept;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::String GetRequestModelName() const noexcept override;
        Aws::Crt::String GetInitialResponseModelName() const noexcept override;
        Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
        Aws::Crt::String GetOperationName() const noexcept override;
    };

    class AWS_ECHOTESTRPC_API EchoStreamMessagesResult
    {
      public:
        EchoStreamMessagesResult() noexcept {}
        EchoStreamMessagesResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
        EchoStreamingResponse *GetOperationResponse() const noexcept
        {
            return static_cast<EchoStreamingResponse *>(m_taggedResult.GetOperationResponse());
        }
        /**
         * @return true if the response is associated with an expected response;
         * false if the response is associated with an error.
         */
        operator bool() const noexcept { return m_taggedResult == true; }
        OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
        RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
        ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

      private:
        TaggedResult m_taggedResult;
    };

    class AWS_ECHOTESTRPC_API EchoStreamMessagesOperation : public ClientOperation
    {
      public:
        EchoStreamMessagesOperation(
            ClientConnection &connection,
            std::shared_ptr<EchoStreamMessagesStreamHandler> streamHandler,
            const EchoStreamMessagesOperationContext &operationContext,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
        /**
         * Used to activate a stream for the `EchoStreamMessagesOperation`
         * @param request The request used for the `EchoStreamMessagesOperation`
         * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
         * @return An `RpcError` that can be used to check whether the stream was activated.
         */
        std::future<RpcError> Activate(
            const EchoStreamingRequest &request,
            OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
        /**
         * Retrieve the result from activating the stream.
         */
        std::future<EchoStreamMessagesResult> GetResult() noexcept;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;
    };

    class AWS_ECHOTESTRPC_API EchoMessageOperationContext : public OperationModelContext
    {
      public:
        EchoMessageOperationContext(const EchoTestRpcServiceModel &serviceModel) noexcept;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::String GetRequestModelName() const noexcept override;
        Aws::Crt::String GetInitialResponseModelName() const noexcept override;
        Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
        Aws::Crt::String GetOperationName() const noexcept override;
    };

    class AWS_ECHOTESTRPC_API EchoMessageResult
    {
      public:
        EchoMessageResult() noexcept {}
        EchoMessageResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
        EchoMessageResponse *GetOperationResponse() const noexcept
        {
            return static_cast<EchoMessageResponse *>(m_taggedResult.GetOperationResponse());
        }
        /**
         * @return true if the response is associated with an expected response;
         * false if the response is associated with an error.
         */
        operator bool() const noexcept { return m_taggedResult == true; }
        OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
        RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
        ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

      private:
        TaggedResult m_taggedResult;
    };

    class AWS_ECHOTESTRPC_API EchoMessageOperation : public ClientOperation
    {
      public:
        EchoMessageOperation(
            ClientConnection &connection,
            const EchoMessageOperationContext &operationContext,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
        /**
         * Used to activate a stream for the `EchoMessageOperation`
         * @param request The request used for the `EchoMessageOperation`
         * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
         * @return An `RpcError` that can be used to check whether the stream was activated.
         */
        std::future<RpcError> Activate(
            const EchoMessageRequest &request,
            OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
        /**
         * Retrieve the result from activating the stream.
         */
        std::future<EchoMessageResult> GetResult() noexcept;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;
    };

    class AWS_ECHOTESTRPC_API GetAllCustomersOperationContext : public OperationModelContext
    {
      public:
        GetAllCustomersOperationContext(const EchoTestRpcServiceModel &serviceModel) noexcept;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::String GetRequestModelName() const noexcept override;
        Aws::Crt::String GetInitialResponseModelName() const noexcept override;
        Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
        Aws::Crt::String GetOperationName() const noexcept override;
    };

    class AWS_ECHOTESTRPC_API GetAllCustomersResult
    {
      public:
        GetAllCustomersResult() noexcept {}
        GetAllCustomersResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
        GetAllCustomersResponse *GetOperationResponse() const noexcept
        {
            return static_cast<GetAllCustomersResponse *>(m_taggedResult.GetOperationResponse());
        }
        /**
         * @return true if the response is associated with an expected response;
         * false if the response is associated with an error.
         */
        operator bool() const noexcept { return m_taggedResult == true; }
        OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
        RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
        ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

      private:
        TaggedResult m_taggedResult;
    };

    class AWS_ECHOTESTRPC_API GetAllCustomersOperation : public ClientOperation
    {
      public:
        GetAllCustomersOperation(
            ClientConnection &connection,
            const GetAllCustomersOperationContext &operationContext,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
        /**
         * Used to activate a stream for the `GetAllCustomersOperation`
         * @param request The request used for the `GetAllCustomersOperation`
         * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
         * @return An `RpcError` that can be used to check whether the stream was activated.
         */
        std::future<RpcError> Activate(
            const GetAllCustomersRequest &request,
            OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
        /**
         * Retrieve the result from activating the stream.
         */
        std::future<GetAllCustomersResult> GetResult() noexcept;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;
    };

    class AWS_ECHOTESTRPC_API EchoTestRpcServiceModel : public ServiceModel
    {
      public:
        EchoTestRpcServiceModel() noexcept;
        Aws::Crt::ScopedResource<OperationError> AllocateOperationErrorFromPayload(
            const Aws::Crt::String &errorModelName,
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        void AssignModelNameToErrorResponse(Aws::Crt::String, ErrorResponseFactory) noexcept;

      private:
        friend class EchoTestRpcClient;
        GetAllProductsOperationContext m_getAllProductsOperationContext;
        CauseServiceErrorOperationContext m_causeServiceErrorOperationContext;
        CauseStreamServiceToErrorOperationContext m_causeStreamServiceToErrorOperationContext;
        EchoStreamMessagesOperationContext m_echoStreamMessagesOperationContext;
        EchoMessageOperationContext m_echoMessageOperationContext;
        GetAllCustomersOperationContext m_getAllCustomersOperationContext;
        Aws::Crt::Map<Aws::Crt::String, ErrorResponseFactory> m_modelNameToErrorResponse;
    };
} // namespace Awstest

#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotshadow/Exports.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>
#include <aws/iot/MqttRequestResponseClient.h>

namespace Aws
{
    namespace Crt {
        namespace Mqtt {
            class MqttConnection;
        }
        namespace Mqtt5 {
            class Mqtt5Client;
        }
    }

    namespace Iotshadow
    {
        template <typename E> class ServiceErrorV2
        {
          public:
            ServiceErrorV2() = delete;
            ServiceErrorV2(const ServiceErrorV2<E> &rhs) = default;
            ServiceErrorV2(ServiceErrorV2<E> &&rhs) = default;

            explicit ServiceErrorV2(int errorCode) :
                  m_errorCode(errorCode),
                  m_modeledError()
            {}

            ServiceErrorV2(int errorCode, E &&modeledError) :
                  m_errorCode(errorCode),
                  m_modeledError(std::move(modeledError))
            {}

            ~ServiceErrorV2() = default;

            ServiceErrorV2<E> &operator =(const ServiceErrorV2<E> &rhs) = default;
            ServiceErrorV2<E> &operator =(ServiceErrorV2<E> &&rhs) = default;

            int GetErrorCode() const { return m_errorCode; }

            bool HasModeledError() const { return m_modeledError.has_value(); }
            const E &GetModeledError() const { return m_modeledError.value(); }

          private:

            int m_errorCode;

            Aws::Crt::Optional<E> m_modeledError;
        };

        class ErrorResponse;
        class DeleteNamedShadowRequest;
        class DeleteShadowResponse;
        class GetNamedShadowRequest;
        class GetShadowResponse;
        class NamedShadowDeltaUpdatedSubscriptionRequest;
        class NamedShadowUpdatedSubscriptionRequest;
        class ShadowDeltaUpdatedEvent;
        class ShadowUpdatedEvent;
        class UpdateNamedShadowRequest;
        class UpdateShadowResponse;

        using DeleteShadowResult = Aws::Iot::RequestResponse::Result<DeleteShadowResponse, ServiceErrorV2<ErrorResponse>>;
        using GetShadowResult = Aws::Iot::RequestResponse::Result<GetShadowResponse, ServiceErrorV2<ErrorResponse>>;
        using UpdateShadowResult = Aws::Iot::RequestResponse::Result<UpdateShadowResponse, ServiceErrorV2<ErrorResponse>>;

        class AWS_IOTSHADOW_API IClientV2
        {
          public:

            virtual ~IClientV2() = default;

            virtual bool DeleteNamedShadow(const Aws::Iotshadow::DeleteNamedShadowRequest &request, const std::function<void(DeleteShadowResult &&)> &handler) = 0;

            virtual bool GetNamedShadow(const Aws::Iotshadow::GetNamedShadowRequest &request, const std::function<void(GetShadowResult &&)> &handler) = 0;

            virtual bool UpdateNamedShadow(const Aws::Iotshadow::UpdateNamedShadowRequest &request, const std::function<void(UpdateShadowResult &&)> &handler) = 0;

            virtual std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> CreateNamedShadowDeltaUpdatedStream(const Aws::Iotshadow::NamedShadowDeltaUpdatedSubscriptionRequest &request, const Aws::Iot::RequestResponse::StreamingOperationOptions<ShadowDeltaUpdatedEvent> &options) = 0;

            virtual std::shared_ptr<Aws::Iot::RequestResponse::IStreamingOperation> CreateNamedShadowUpdatedStream(const Aws::Iotshadow::NamedShadowUpdatedSubscriptionRequest &request, const Aws::Iot::RequestResponse::StreamingOperationOptions<ShadowUpdatedEvent> &options) = 0;
        };

        AWS_IOTSHADOW_API std::shared_ptr<IClientV2> NewClientFrom5(const Aws::Crt::Mqtt5::Mqtt5Client &protocolClient,
                                                   const Aws::Iot::RequestResponse::RequestResponseClientOptions &options,
                                                   Aws::Crt::Allocator *allocator = Aws::Crt::ApiAllocator());

        AWS_IOTSHADOW_API std::shared_ptr<IClientV2> NewClientFrom311(const Aws::Crt::Mqtt::MqttConnection &protocolClient,
                                                     const Aws::Iot::RequestResponse::RequestResponseClientOptions &options,
                                                     Aws::Crt::Allocator *allocator = Aws::Crt::ApiAllocator());
    }
}
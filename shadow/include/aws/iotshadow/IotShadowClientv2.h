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

    template <typename E> class Error {
      public:

      private:
        Aws::Crt::Variant<E, int> rawError;
    };

    namespace Iotshadow
    {
        class ErrorResponse;
        class GetNamedShadowRequest;
        class GetNamedShadowResponse;

        using GetNamedShadowResult = Aws::Iot::RequestResponse::Result<GetNamedShadowResponse, Error<ErrorResponse>>;

        class AWS_IOTSHADOW_API IClientv2
        {
          public:

            virtual ~IClientv2() = 0;

            static std::shared_ptr<IClientv2> newFrom5(const Aws::Crt::Mqtt5::Mqtt5Client &protocolClient,
                                       Aws::Iot::RequestResponse::RequestResponseClientOptions &&options,
                                       Aws::Crt::Allocator *allocator = Aws::Crt::ApiAllocator());

            static std::shared_ptr<IClientv2> newFrom311(const Aws::Crt::Mqtt::MqttConnection &protocolClient,
                                         Aws::Iot::RequestResponse::RequestResponseClientOptions &&options,
                                         Aws::Crt::Allocator *allocator = Aws::Crt::ApiAllocator());

            virtual bool GetNamedShadow(const Aws::Iotshadow::GetNamedShadowRequest &request, const std::function<void(GetNamedShadowResult &&)> &handler) = 0;

        };
    }
}
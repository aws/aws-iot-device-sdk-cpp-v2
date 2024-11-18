#pragma once

/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#include <aws/iotidentity/Exports.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>
#include <aws/iot/MqttRequestResponseClient.h>

#include <functional>

namespace Aws
{
    namespace Crt
    {
        namespace Mqtt
        {
            class MqttConnection;
        }
        namespace Mqtt5
        {
            class Mqtt5Client;
        }
    } // namespace Crt
} // namespace Aws

namespace Aws
{
    namespace Iotidentity
    {

        template <typename E> class ServiceErrorV2
        {
          public:
            ServiceErrorV2() = default;
            ServiceErrorV2(const ServiceErrorV2<E> &rhs) = default;
            ServiceErrorV2(ServiceErrorV2<E> &&rhs) = default;

            explicit ServiceErrorV2(int errorCode) : m_errorCode(errorCode), m_modeledError() {}

            ServiceErrorV2(int errorCode, E &&modeledError)
                : m_errorCode(errorCode), m_modeledError(std::move(modeledError))
            {
            }

            ~ServiceErrorV2() = default;

            ServiceErrorV2<E> &operator=(const ServiceErrorV2<E> &rhs) = default;
            ServiceErrorV2<E> &operator=(ServiceErrorV2<E> &&rhs) = default;

            int GetErrorCode() const { return m_errorCode; }

            bool HasModeledError() const { return m_modeledError.has_value(); }
            const E &GetModeledError() const { return m_modeledError.value(); }

          private:
            int m_errorCode;

            Aws::Crt::Optional<E> m_modeledError;
        };

        class CreateCertificateFromCsrRequest;
        class CreateCertificateFromCsrResponse;
        class CreateKeysAndCertificateRequest;
        class CreateKeysAndCertificateResponse;
        class RegisterThingRequest;
        class RegisterThingResponse;
        class V2ErrorResponse;

        using CreateCertificateFromCsrResult =
            Aws::Iot::RequestResponse::Result<CreateCertificateFromCsrResponse, ServiceErrorV2<V2ErrorResponse>>;
        using CreateCertificateFromCsrResultHandler = std::function<void(CreateCertificateFromCsrResult &&)>;

        using CreateKeysAndCertificateResult =
            Aws::Iot::RequestResponse::Result<CreateKeysAndCertificateResponse, ServiceErrorV2<V2ErrorResponse>>;
        using CreateKeysAndCertificateResultHandler = std::function<void(CreateKeysAndCertificateResult &&)>;

        using RegisterThingResult =
            Aws::Iot::RequestResponse::Result<RegisterThingResponse, ServiceErrorV2<V2ErrorResponse>>;
        using RegisterThingResultHandler = std::function<void(RegisterThingResult &&)>;

        /**
         * An AWS IoT service that assists with provisioning a device and installing unique client certificates on it
         *
         * https://docs.aws.amazon.com/iot/latest/developerguide/provision-wo-cert.html
         *
         */
        class AWS_IOTIDENTITY_API IClientV2
        {
          public:
            virtual ~IClientV2() = default;

            /**
             * Creates a certificate from a certificate signing request (CSR). AWS IoT provides client certificates that
             * are signed by the Amazon Root certificate authority (CA). The new certificate has a PENDING_ACTIVATION
             * status. When you call RegisterThing to provision a thing with this certificate, the certificate status
             * changes to ACTIVE or INACTIVE as described in the template.
             *
             *
             * AWS documentation:
             * https://docs.aws.amazon.com/iot/latest/developerguide/provision-wo-cert.html#fleet-provision-api
             *
             * @param request operation to perform
             * @param handler function object to invoke upon operation completion
             *
             * @returns success/failure
             */
            virtual bool CreateCertificateFromCsr(
                const CreateCertificateFromCsrRequest &request,
                const CreateCertificateFromCsrResultHandler &handler) = 0;

            /**
             * Creates new keys and a certificate. AWS IoT provides client certificates that are signed by the Amazon
             * Root certificate authority (CA). The new certificate has a PENDING_ACTIVATION status. When you call
             * RegisterThing to provision a thing with this certificate, the certificate status changes to ACTIVE or
             * INACTIVE as described in the template.
             *
             *
             * AWS documentation:
             * https://docs.aws.amazon.com/iot/latest/developerguide/provision-wo-cert.html#fleet-provision-api
             *
             * @param request operation to perform
             * @param handler function object to invoke upon operation completion
             *
             * @returns success/failure
             */
            virtual bool CreateKeysAndCertificate(
                const CreateKeysAndCertificateRequest &request,
                const CreateKeysAndCertificateResultHandler &handler) = 0;

            /**
             * Provisions an AWS IoT thing using a pre-defined template.
             *
             *
             * AWS documentation:
             * https://docs.aws.amazon.com/iot/latest/developerguide/provision-wo-cert.html#fleet-provision-api
             *
             * @param request operation to perform
             * @param handler function object to invoke upon operation completion
             *
             * @returns success/failure
             */
            virtual bool RegisterThing(
                const RegisterThingRequest &request,
                const RegisterThingResultHandler &handler) = 0;
        };

        /**
         * Creates a new service client that uses an SDK MQTT5 client for transport.
         *
         * @param protocolClient MQTT client to use as transport
         * @param options request-response MQTT client configuration options
         * @param allocator memory allocator to use for all client functionality
         *
         * @return a new service client
         */
        AWS_IOTIDENTITY_API std::shared_ptr<IClientV2> NewClientFrom5(
            const Aws::Crt::Mqtt5::Mqtt5Client &protocolClient,
            const Aws::Iot::RequestResponse::RequestResponseClientOptions &options,
            Aws::Crt::Allocator *allocator = Aws::Crt::ApiAllocator());

        /**
         * Creates a new service client that uses an SDK MQTT311 client for transport.
         *
         * @param protocolClient MQTT client to use as transport
         * @param options request-response MQTT client configuration options
         * @param allocator memory allocator to use for all client functionality
         *
         * @return a new service client
         */
        AWS_IOTIDENTITY_API std::shared_ptr<IClientV2> NewClientFrom311(
            const Aws::Crt::Mqtt::MqttConnection &protocolClient,
            const Aws::Iot::RequestResponse::RequestResponseClientOptions &options,
            Aws::Crt::Allocator *allocator = Aws::Crt::ApiAllocator());

    } // namespace Iotidentity
} // namespace Aws

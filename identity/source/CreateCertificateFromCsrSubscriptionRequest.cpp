/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotidentity/CreateCertificateFromCsrSubscriptionRequest.h>

namespace Aws
{
    namespace Iotidentity
    {

        void CreateCertificateFromCsrSubscriptionRequest::LoadFromObject(
            CreateCertificateFromCsrSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void CreateCertificateFromCsrSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;
        }

        CreateCertificateFromCsrSubscriptionRequest::CreateCertificateFromCsrSubscriptionRequest(
            const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        CreateCertificateFromCsrSubscriptionRequest &CreateCertificateFromCsrSubscriptionRequest::operator=(
            const Crt::JsonView &doc)
        {
            *this = CreateCertificateFromCsrSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotidentity
} // namespace Aws

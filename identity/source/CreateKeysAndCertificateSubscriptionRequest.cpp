/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotidentity/CreateKeysAndCertificateSubscriptionRequest.h>

namespace Aws
{
    namespace Iotidentity
    {

        void CreateKeysAndCertificateSubscriptionRequest::LoadFromObject(
            CreateKeysAndCertificateSubscriptionRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void CreateKeysAndCertificateSubscriptionRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;
        }

        CreateKeysAndCertificateSubscriptionRequest::CreateKeysAndCertificateSubscriptionRequest(
            const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        CreateKeysAndCertificateSubscriptionRequest &CreateKeysAndCertificateSubscriptionRequest::operator=(
            const Crt::JsonView &doc)
        {
            *this = CreateKeysAndCertificateSubscriptionRequest(doc);
            return *this;
        }

    } // namespace Iotidentity
} // namespace Aws

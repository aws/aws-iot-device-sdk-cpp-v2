/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotidentity/CreateKeysAndCertificateRequest.h>

namespace Aws
{
    namespace Iotidentity
    {

        void CreateKeysAndCertificateRequest::LoadFromObject(
            CreateKeysAndCertificateRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;
        }

        void CreateKeysAndCertificateRequest::SerializeToObject(Aws::Crt::JsonObject &object) const { (void)object; }

        CreateKeysAndCertificateRequest::CreateKeysAndCertificateRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        CreateKeysAndCertificateRequest &CreateKeysAndCertificateRequest::operator=(const Crt::JsonView &doc)
        {
            *this = CreateKeysAndCertificateRequest(doc);
            return *this;
        }

    } // namespace Iotidentity
} // namespace Aws

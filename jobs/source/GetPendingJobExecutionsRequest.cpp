/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */
#include <aws/iotjobs/GetPendingJobExecutionsRequest.h>

namespace Aws
{
    namespace Iotjobs
    {

        void GetPendingJobExecutionsRequest::LoadFromObject(
            GetPendingJobExecutionsRequest &val,
            const Aws::Crt::JsonView &doc)
        {
            (void)val;
            (void)doc;

            if (doc.ValueExists("clientToken"))
            {
                val.ClientToken = doc.GetString("clientToken");
            }
        }

        void GetPendingJobExecutionsRequest::SerializeToObject(Aws::Crt::JsonObject &object) const
        {
            (void)object;

            if (ClientToken)
            {
                object.WithString("clientToken", *ClientToken);
            }
        }

        GetPendingJobExecutionsRequest::GetPendingJobExecutionsRequest(const Crt::JsonView &doc)
        {
            LoadFromObject(*this, doc);
        }

        GetPendingJobExecutionsRequest &GetPendingJobExecutionsRequest::operator=(const Crt::JsonView &doc)
        {
            *this = GetPendingJobExecutionsRequest(doc);
            return *this;
        }

    } // namespace Iotjobs
} // namespace Aws

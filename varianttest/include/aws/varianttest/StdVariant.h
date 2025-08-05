#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/varianttest/Exports.h>

#include <aws/crt/StlAllocator.h>
#include <aws/crt/Types.h>

#include <variant>

namespace Aws
{
    namespace Varianttest
    {
 
        struct AWS_VARIANTTEST_API TestError
        {
            explicit operator bool() const noexcept { return baseStatus; }

            bool baseStatus;
            int crtError;
        };

        struct AWS_VARIANTTEST_API TestError2
        {
            explicit operator bool() const noexcept { return baseStatus; }

            bool baseStatus;
            int crtError;
        };


        /* Failure case - variant with move-only type and copyable type */
        using FailResultVariantType = std::variant<Crt::ScopedResource<Crt::String>, TestError>;

        class AWS_VARIANTTEST_API FailVariantTestResult
        {
        public:
            FailVariantTestResult() noexcept {};
            FailVariantTestResult(FailResultVariantType &&result) noexcept : m_result(std::move(result)) {}

            Crt::String *GetFirst() const noexcept
            {
                if (std::holds_alternative<Crt::ScopedResource<Crt::String>>(m_result))
                {
                    return std::get<Crt::ScopedResource<Crt::String>>(m_result).get();
                }
                
                return nullptr;
            }


            const TestError *GetSecond() const noexcept {
                if (std::holds_alternative<TestError>(m_result))
                {
                    return &std::get<TestError>(m_result);
                }

                return nullptr;
            }

        private:
            FailResultVariantType m_result;
        };


        /* Both copyable case */

        using CopyResultVariantType = std::variant<TestError, TestError2>;

        class AWS_VARIANTTEST_API CopyVariantTestResult
        {
          public:
            CopyVariantTestResult() noexcept {};
            CopyVariantTestResult(CopyResultVariantType &&result) noexcept : m_result(std::move(result)) {}

            const TestError *GetFirst() const noexcept
            {
                if (std::holds_alternative<TestError>(m_result))
                {
                    return &std::get<TestError>(m_result);
                }

                return nullptr;
            }

            const TestError2 *GetSecond() const noexcept
            {
                if (std::holds_alternative<TestError2>(m_result))
                {
                    return &std::get<TestError2>(m_result);
                }

                return nullptr;
            }

          private:
            CopyResultVariantType m_result;
        };
      
        /* Both move-only case */
        using MoveResultVariantType = std::variant<Crt::ScopedResource<Crt::String>, Crt::ScopedResource<int>>;

        class AWS_VARIANTTEST_API MoveVariantTestResult
        {
          public:
            MoveVariantTestResult() noexcept {};
            MoveVariantTestResult(MoveResultVariantType &&result) noexcept : m_result(std::move(result)) {}

            Crt::String *GetFirst() const noexcept
            {
                if (std::holds_alternative<Crt::ScopedResource<Crt::String>>(m_result))
                {
                    return std::get<Crt::ScopedResource<Crt::String>>(m_result).get();
                }

                return nullptr;
            }

            int *GetSecond() const noexcept
            {
                if (std::holds_alternative<Crt::ScopedResource<int>>(m_result))
                {
                    return std::get<Crt::ScopedResource<int>>(m_result).get();
                }

                return nullptr;
            }

          private:
            MoveResultVariantType m_result;
        };


    } // namespace Varianttest
} // namespace Aws

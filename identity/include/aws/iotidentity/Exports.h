#pragma once
/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#if defined(AWS_IOTIDENTITY_USE_WINDOWS_DLL_SEMANTICS) || defined(WIN32)
#    ifdef AWS_IOTIDENTITY_USE_IMPORT_EXPORT
#        ifdef AWS_IOTIDENTITY_EXPORTS
#            define AWS_IOTIDENTITY_API __declspec(dllexport)
#        else
#            define AWS_IOTIDENTITY_API __declspec(dllimport)
#        endif /* AWS_IOTIDENTITY_EXPORTS */
#    else
#        define AWS_IOTIDENTITY_API
#    endif /* AWS_IOTIDENTITY_USE_IMPORT_EXPORT */

#else /* defined (AWS_IOTIDENTITY_USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */
#    define AWS_IOTIDENTITY_API
#endif /* defined (AWS_IOTIDENTITY__USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */

/*
 * Deprecation warnings are emitted unless callers compile with -DAWS_CRT_DISABLE_DEPRECATION_WARNINGS=ON.
 */
#ifndef AWS_CRT_SOFT_DEPRECATED
#    if !defined(AWS_CRT_DISABLE_DEPRECATION_WARNINGS)
/* On compilers that support __has_atribute check whether the `deprecated` attribute exists */
#        if defined(__has_attribute)
#            if __has_attribute(deprecated)
#                define AWS_CRT_SOFT_DEPRECATED(msg) __attribute__((deprecated(msg)))
#            endif
#        endif
/* Otherwise we fallback to standard C++14 or MSVC syntax */
#        if !defined(AWS_CRT_SOFT_DEPRECATED)
#            if __cplusplus >= 201402L /* C++14 supports [[deprecated]] */
#                define AWS_CRT_SOFT_DEPRECATED(msg) [[deprecated(msg)]]
#            elif defined(_MSC_VER) /* Older MSVC */
#                define AWS_CRT_SOFT_DEPRECATED(msg) __declspec(deprecated(msg))
#            else /* Unknown compiler. Do nothing. */
#                define AWS_CRT_SOFT_DEPRECATED(msg)
#            endif
#        endif
#    else
/* If AWS_CRT_DISABLE_DEPRECATION_WARNINGS do nothing */
#        define AWS_CRT_SOFT_DEPRECATED(msg)
#    endif
#endif

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

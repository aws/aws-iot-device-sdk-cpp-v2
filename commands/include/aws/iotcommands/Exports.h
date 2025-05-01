#pragma once
/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#if defined(AWS_IOTCOMMANDS_USE_WINDOWS_DLL_SEMANTICS) || defined(WIN32)
#    ifdef AWS_IOTCOMMANDS_USE_IMPORT_EXPORT
#        ifdef AWS_IOTCOMMANDS_EXPORTS
#            define AWS_IOTCOMMANDS_API __declspec(dllexport)
#        else
#            define AWS_IOTCOMMANDS_API __declspec(dllimport)
#        endif /* AWS_IOTCOMMANDS_EXPORTS */
#    else
#        define AWS_IOTCOMMANDS_API
#    endif /* AWS_IOTCOMMANDS_USE_IMPORT_EXPORT */

#else /* defined (AWS_IOTCOMMANDS_USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */
#    define AWS_IOTCOMMANDS_API
#endif /* defined (AWS_IOTCOMMANDS__USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */

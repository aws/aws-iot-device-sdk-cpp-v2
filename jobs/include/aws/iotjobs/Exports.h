#pragma once
/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#if defined(AWS_IOTJOBS_USE_WINDOWS_DLL_SEMANTICS) || defined(WIN32)
#    ifdef AWS_IOTJOBS_USE_IMPORT_EXPORT
#        ifdef AWS_IOTJOBS_EXPORTS
#            define AWS_IOTJOBS_API __declspec(dllexport)
#        else
#            define AWS_IOTJOBS_API __declspec(dllimport)
#        endif /* AWS_IOTJOBS_EXPORTS */
#    else
#        define AWS_IOTJOBS_API
#    endif /* AWS_IOTJOBS_USE_IMPORT_EXPORT */

#else /* defined (AWS_IOTJOBS_USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */
#    define AWS_IOTJOBS_API
#endif /* defined (AWS_IOTJOBS__USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */

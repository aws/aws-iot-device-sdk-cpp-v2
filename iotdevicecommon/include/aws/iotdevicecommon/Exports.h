#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#if defined(AWS_IOTDEVICECOMMON_USE_WINDOWS_DLL_SEMANTICS) || defined(WIN32)
#    ifdef AWS_IOTDEVICECOMMON_USE_IMPORT_EXPORT
#        ifdef AWS_IOTDEVICECOMMON_EXPORTS
#            define AWS_IOTDEVICECOMMON_API __declspec(dllexport)
#        else
#            define AWS_IOTDEVICECOMMON_API __declspec(dllimport)
#        endif /* AWS_IOTDEVICECOMMON_EXPORTS */
#    else
#        define AWS_IOTDEVICECOMMON_API
#    endif /* AWS_IOTDEVICECOMMON_USE_IMPORT_EXPORT */

#else /* defined (AWS_IOTDEVICECOMMON_USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */
#    define AWS_IOTDEVICECOMMON_API
#endif /* defined (AWS_IOTDEVICECOMMON__USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */

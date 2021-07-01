#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#if defined(AWS_IOTDEVICEDEFENDER_USE_WINDOWS_DLL_SEMANTICS) || defined(WIN32)
#    ifdef AWS_IOTDEVICEDEFENDER_USE_IMPORT_EXPORT
#        ifdef AWS_IOTDEVICEDEFENDER_EXPORTS
#            define AWS_IOTDEVICEDEFENDER_API __declspec(dllexport)
#        else
#            define AWS_IOTDEVICEDEFENDER_API __declspec(dllimport)
#        endif /* AWS_IOTDEVICEDEFENDER_EXPORTS */
#    else
#        define AWS_IOTDEVICEDEFENDER_API
#    endif /* AWS_IOTDEVICEDEFENDER_USE_IMPORT_EXPORT */

#else /* defined (AWS_IOTDEVICEDEFENDER_USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */
#    define AWS_IOTDEVICEDEFENDER_API
#endif /* defined (AWS_IOTDEVICEDEFENDER__USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */

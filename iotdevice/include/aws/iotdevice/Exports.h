#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#if defined(AWS_IOTDEVICE_USE_WINDOWS_DLL_SEMANTICS) || defined(WIN32)
#    ifdef AWS_IOTDEVICE_USE_IMPORT_EXPORT
#        ifdef AWS_IOTDEVICE_EXPORTS
#            define AWS_IOTDEVICE_API __declspec(dllexport)
#        else
#            define AWS_IOTDEVICE_API __declspec(dllimport)
#        endif /* AWS_IOTDEVICE_EXPORTS */
#    else
#        define AWS_IOTDEVICE_API
#    endif /* AWS_IOTDEVICE_USE_IMPORT_EXPORT */

#else /* defined (AWS_IOTDEVICE_USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */
#    define AWS_IOTDEVICE_API
#endif /* defined (AWS_IOTDEVICE__USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */

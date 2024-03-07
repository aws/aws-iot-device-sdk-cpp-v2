#pragma once
/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#if defined(AWS_IOTSHADOW_USE_WINDOWS_DLL_SEMANTICS) || defined(WIN32)
#    ifdef AWS_IOTSHADOW_USE_IMPORT_EXPORT
#        ifdef AWS_IOTSHADOW_EXPORTS
#            define AWS_IOTSHADOW_API __declspec(dllexport)
#        else
#            define AWS_IOTSHADOW_API __declspec(dllimport)
#        endif /* AWS_IOTSHADOW_EXPORTS */
#    else
#        define AWS_IOTSHADOW_API
#    endif /* AWS_IOTSHADOW_USE_IMPORT_EXPORT */

#else /* defined (AWS_IOTSHADOW_USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */
#    define AWS_IOTSHADOW_API
#endif /* defined (AWS_IOTSHADOW__USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */

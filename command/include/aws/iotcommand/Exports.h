#pragma once
/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 *
 * This file is generated
 */

#if defined(AWS_IOTCOMMAND_USE_WINDOWS_DLL_SEMANTICS) || defined(WIN32)
#    ifdef AWS_IOTCOMMAND_USE_IMPORT_EXPORT
#        ifdef AWS_IOTCOMMAND_EXPORTS
#            define AWS_IOTCOMMAND_API __declspec(dllexport)
#        else
#            define AWS_IOTCOMMAND_API __declspec(dllimport)
#        endif /* AWS_IOTCOMMAND_EXPORTS */
#    else
#        define AWS_IOTCOMMAND_API
#    endif /* AWS_IOTCOMMAND_USE_IMPORT_EXPORT */

#else /* defined (AWS_IOTCOMMAND_USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */
#    define AWS_IOTCOMMAND_API
#endif /* defined (AWS_IOTCOMMAND__USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */

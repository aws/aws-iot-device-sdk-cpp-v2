#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#if defined(AWS_DISCOVERY_USE_WINDOWS_DLL_SEMANTICS) || defined(WIN32)
#    ifdef AWS_DISCOVERY_USE_IMPORT_EXPORT
#        ifdef AWS_DISCOVERY_EXPORTS
#            define AWS_DISCOVERY_API __declspec(dllexport)
#        else
#            define AWS_DISCOVERY_API __declspec(dllimport)
#        endif /* AWS_DISCOVERY_EXPORTS */
#    else
#        define AWS_DISCOVERY_API
#    endif /* AWS_DISCOVERY_USE_IMPORT_EXPORT */

#else /* defined (AWS_DISCOVERY_USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */
#    define AWS_DISCOVERY_API
#endif /* defined (AWS_DISCOVERY_USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */

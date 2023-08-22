#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#if defined(AWS_GREENGRASS_USE_WINDOWS_DLL_SEMANTICS) || defined(WIN32)
#    ifdef AWS_GREENGRASS_USE_IMPORT_EXPORT
#        ifdef AWS_GREENGRASS_EXPORTS
#            define AWS_GREENGRASS_API __declspec(dllexport)
#        else
#            define AWS_GREENGRASS_API __declspec(dllimport)
#        endif /* AWS_GREENGRASS_EXPORTS */
#    else
#        define AWS_GREENGRASS_API
#    endif /* AWS_GREENGRASS_USE_IMPORT_EXPORT */

#else /* defined (AWS_GREENGRASS_USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */
#    define AWS_GREENGRASS_API
#endif /* defined (AWS_GREENGRASS__USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */

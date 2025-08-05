#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#if defined(AWS_VARIANTTEST_USE_WINDOWS_DLL_SEMANTICS) || defined(WIN32)
#    ifdef AWS_VARIANTTEST_USE_IMPORT_EXPORT
#        ifdef AWS_VARIANTTEST_EXPORTS
#            define AWS_VARIANTTEST_API __declspec(dllexport)
#        else
#            define AWS_VARIANTTEST_API __declspec(dllimport)
#        endif /* AWS_VARIANTTEST_EXPORTS */
#    else
#        define AWS_VARIANTTEST_API
#    endif /* AWS_VARIANTTEST_USE_IMPORT_EXPORT */

#else /* defined (AWS_VARIANTTEST_USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */
#    define AWS_VARIANTTEST_API
#endif /* defined (AWS_VARIANTTEST_USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */

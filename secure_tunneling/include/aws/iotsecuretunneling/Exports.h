#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#if defined(AWS_IOTSECURETUNNELING_USE_WINDOWS_DLL_SEMANTICS) || defined(WIN32)
#    ifdef AWS_IOTSECURETUNNELING_USE_IMPORT_EXPORT
#        ifdef AWS_IOTSECURETUNNELING_EXPORTS
#            define AWS_IOTSECURETUNNELING_API __declspec(dllexport)
#        else
#            define AWS_IOTSECURETUNNELING_API __declspec(dllimport)
#        endif /* AWS_IOTSECURETUNNELING_EXPORTS */
#    else
#        define AWS_IOTSECURETUNNELING_API
#    endif /* AWS_IOTSECURETUNNELING_USE_IMPORT_EXPORT */

#else /* defined (AWS_IOTSECURETUNNELING_USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */
#    define AWS_IOTSECURETUNNELING_API
#endif /* defined (AWS_IOTSECURETUNNELING_USE_WINDOWS_DLL_SEMANTICS) || defined (WIN32) */

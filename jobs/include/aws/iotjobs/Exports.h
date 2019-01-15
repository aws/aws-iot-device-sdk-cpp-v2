#pragma once
/* Copyright 2010-2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License").
* You may not use this file except in compliance with the License.
* A copy of the License is located at
*
*  http://aws.amazon.com/apache2.0
*
* or in the "license" file accompanying this file. This file is distributed
* on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
* express or implied. See the License for the specific language governing
* permissions and limitations under the License.

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
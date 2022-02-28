#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/crt/Types.h>
#include <iostream>
#include <stdlib.h>

using namespace Aws::Crt;

namespace DATest_Utils
{
    void s_getenv(const char *env_name, String &var)
    {
        size_t requiredSize = 0;
        getenv_s(&requiredSize, NULL, 0, env_name);
        if (requiredSize == 0)
        {
            fprintf(stdout, "Failed to find environment variable: %s.\n", env_name);
            exit(-1);
        }

        char *libvar = (char *)malloc((requiredSize + 1) * sizeof(char));
        memset(libvar, 0, requiredSize + 1);
        if (!libvar)
        {
            printf("Failed to allocate memory!\n");
            exit(1);
        }

        // Get the value of the LIB environment variable.
        getenv_s(&requiredSize, libvar, requiredSize, env_name);

        var = libvar;
    }

    void s_printEnvError(const char *var) { fprintf(stdout, "Failed to found environment variable : %s\n", var); }
} // namespace DATest_Utils
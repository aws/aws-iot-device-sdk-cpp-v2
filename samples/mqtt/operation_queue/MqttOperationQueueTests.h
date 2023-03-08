#pragma once

/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "../../utils/CommandLineUtils.h"
#include "MqttOperationQueue.h"

namespace MqttOperationQueueTests
{
    /**
     * Class that implements as set of tests to make sure that the MqttOperationQueue works as expected.
     * This class is designed purely for testing purposes only.
     */
    class Tester
    {
      public:
        static int CreateAndRunTester(Utils::CommandLineUtils *cmdUtils);
        int RunTests();

      private:
        Utils::CommandLineUtils *cmdUtils;
        std::shared_ptr<Aws::Crt::Mqtt::MqttConnection> m_connection;
        std::promise<bool> m_connectionCompletedPromise;
        std::promise<void> m_connectionClosedPromise;

        int OnApplicationFailure(Aws::Crt::String message);
        void TestConnectionSetup();
        void TestConnectionTeardown();
        int TestOperationSuccess(MqttOperationQueue::QueueResult result, Aws::Crt::String testName);

        /* TESTS */
        int TestConnectSubPubUnsub();
        int TestDropBack();
        int TestDropFront();
        int TestAddFront();
        int TestAddError();
    };
} // namespace MqttOperationQueueTests

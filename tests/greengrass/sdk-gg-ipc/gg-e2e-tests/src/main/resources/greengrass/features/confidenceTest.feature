Feature: Confidence Test Suite

  Background:
    Given my device is registered as a Thing
    And my device is running Greengrass

  @ConfidenceTest
  Scenario: As a Developer, I can deploy GDK_COMPONENT_NAME to my device and see it is working as expected
    When I create a Greengrass deployment with components
      | GDK_COMPONENT_NAME | GDK_COMPONENT_RECIPE_FILE |
      | aws.greengrass.Cli | LATEST                    |
    And I deploy the Greengrass deployment configuration
    Then the Greengrass deployment is COMPLETED on the device after 180 seconds
    # Update component state accordingly. Possible states: {RUNNING, FINISHED, BROKEN, STOPPING}
    And I verify the GDK_COMPONENT_NAME component is RUNNING using the greengrass-cli

  @ConfidenceTest
  Scenario: As a Developer, I can deploy GDK_COMPONENT_NAME, stop it and start it again
    When I create a Greengrass deployment with components
      | GDK_COMPONENT_NAME | GDK_COMPONENT_RECIPE_FILE |
      | aws.greengrass.Cli | LATEST                    |
    And I deploy the Greengrass deployment configuration
    Then the Greengrass deployment is COMPLETED on the device after 180 seconds
    # Update component state accordingly. Possible states: {RUNNING, FINISHED, BROKEN, STOPPING}
    Then I verify the GDK_COMPONENT_NAME component is RUNNING using the greengrass-cli
    When I use greengrass-cli to stop the component GDK_COMPONENT_NAME
    # Update component state accordingly. Possible states: {RUNNING, FINISHED, BROKEN, STOPPING}
    Then I verify the GDK_COMPONENT_NAME component is FINISHED using the greengrass-cli
    When I use greengrass-cli to restart the component GDK_COMPONENT_NAME
    # Update component state accordingly. Possible states: {RUNNING, FINISHED, BROKEN, STOPPING}
    Then I verify the GDK_COMPONENT_NAME component is RUNNING using the greengrass-cli

  @ConfidenceTest
  Scenario: As a Developer, I can deploy GDK_COMPONENT_NAME and restart Greengrass to check if it is still working as expected
    When I create a Greengrass deployment with components
      | GDK_COMPONENT_NAME | GDK_COMPONENT_RECIPE_FILE |
      | aws.greengrass.Cli | LATEST                    |
    And I deploy the Greengrass deployment configuration
    Then the Greengrass deployment is COMPLETED on the device after 180 seconds
    # Update component state accordingly. Possible states: {RUNNING, FINISHED, BROKEN, STOPPING}
    Then I verify the GDK_COMPONENT_NAME component is RUNNING using the greengrass-cli
    When I restart Greengrass
    Then I wait 5 seconds
    # Update component state accordingly. Possible states: {RUNNING, FINISHED, BROKEN, STOPPING}
    And I verify the GDK_COMPONENT_NAME component is RUNNING using the greengrass-cli

  @ConfidenceTest
  Scenario: As a Developer, I can deploy GDK_COMPONENT_NAME and disable internet to check if component is working. Restore internet to check if component is still working
    When I create a Greengrass deployment with components
      | GDK_COMPONENT_NAME | GDK_COMPONENT_RECIPE_FILE |
      | aws.greengrass.Cli | LATEST                    |
    And I deploy the Greengrass deployment configuration
    Then the Greengrass deployment is COMPLETED on the device after 180 seconds
    # Update component state accordingly. Possible states: {RUNNING, FINISHED, BROKEN, STOPPING}
    Then I verify the GDK_COMPONENT_NAME component is RUNNING using the greengrass-cli
    When I set device network connectivity to OFFLINE
    And I restart Greengrass
    Then I wait 5 seconds
    # Update component state accordingly. Possible states: {RUNNING, FINISHED, BROKEN, STOPPING}
    Then I verify the GDK_COMPONENT_NAME component is RUNNING using the greengrass-cli
    When I set device network connectivity to ONLINE
    # Update component state accordingly. Possible states: {RUNNING, FINISHED, BROKEN, STOPPING}
    Then I verify the GDK_COMPONENT_NAME component is RUNNING using the greengrass-cli

  @ConfidenceTest
  Scenario: As a Developer, I can deploy GDK_COMPONENT_NAME to my device and see it is working as expected after an additional local deployment to change component configuration
    When I create a Greengrass deployment with components
      | GDK_COMPONENT_NAME | GDK_COMPONENT_RECIPE_FILE |
      | aws.greengrass.Cli | LATEST                    |
    And I deploy the Greengrass deployment configuration
    Then the Greengrass deployment is COMPLETED on the device after 180 seconds
    # Update component state accordingly. Possible states: {RUNNING, FINISHED, BROKEN, STOPPING}.
    Then I verify the GDK_COMPONENT_NAME component is RUNNING using the greengrass-cli
    # Update the MERGE deployment configuration with the intended key-value pairs.
    And I update my local deployment configuration, setting the component GDK_COMPONENT_NAME configuration to:
      """
        {
          "MERGE": {
            "configurationKey": "keyValue"
          }
        }
      """
    Then the local Greengrass deployment is SUCCEEDED on the device after 120 seconds
    # Update component state accordingly. Possible states: {RUNNING, FINISHED, BROKEN, STOPPING}. Additional verification steps may be added here too.
    And I verify the GDK_COMPONENT_NAME component is RUNNING using the greengrass-cli

  @ConfidenceTest
  Scenario: As a Developer, I can deploy GDK_COMPONENT_NAME to my device with configuration updates and see it is working as expected
    When I create a Greengrass deployment with components
      | GDK_COMPONENT_NAME | GDK_COMPONENT_RECIPE_FILE |
      | aws.greengrass.Cli | LATEST                    |
    # Update the MERGE deployment configuration with the intended key-value pairs.
    And I update my Greengrass deployment configuration, setting the component GDK_COMPONENT_NAME configuration to:
      """
        {
          "MERGE": {
            "configurationKey": "keyValue"
          }
        }
      """
    And I deploy the Greengrass deployment configuration
    Then the Greengrass deployment is COMPLETED on the device after 180 seconds
    # Update component state accordingly. Possible states: {RUNNING, FINISHED, BROKEN, STOPPING}. Additional verification steps may be added here too.
    Then I verify the GDK_COMPONENT_NAME component is RUNNING using the greengrass-cli

  @ConfidenceTest
  Scenario: As a Developer, I can deploy GDK_COMPONENT_NAME to my device and verify device RAM has not increased by more than a specified threshold
    # To extend the test case, record RAM usage statistic under a unique identifier, such as RAM1 here.
    When I record the device's RAM usage statistic as RAM1
    And I create a Greengrass deployment with components
      | GDK_COMPONENT_NAME | GDK_COMPONENT_RECIPE_FILE |
      | aws.greengrass.Cli | LATEST                    |
    And I deploy the Greengrass deployment configuration
    Then the Greengrass deployment is COMPLETED on the device after 180 seconds
    # Update component state accordingly. Possible states: {RUNNING, FINISHED, BROKEN, STOPPING}
    Then I verify the GDK_COMPONENT_NAME component is RUNNING using the greengrass-cli
    When I record the device's RAM usage statistic as RAM2
    # Update the intended threshold as needed, in MB. If extending the test case, use two unique identifiers recorded during previous steps.
    Then the increase in the RAM usage from RAM1 to RAM2 is less than 1024 MB

  @ConfidenceTest
  Scenario: As a Developer, I can deploy GDK_COMPONENT_NAME to my device and verify device CPU usage has not increased by more than a specified threshold
    # To extend the test case, record CPU usage statistic under a unique identifier, such as CPU1 here.
    When I record the device's CPU usage statistic as CPU1
    And I create a Greengrass deployment with components
      | GDK_COMPONENT_NAME | GDK_COMPONENT_RECIPE_FILE |
      | aws.greengrass.Cli | LATEST                    |
    And I deploy the Greengrass deployment configuration
    Then the Greengrass deployment is COMPLETED on the device after 180 seconds
    # Update component state accordingly. Possible states: {RUNNING, FINISHED, BROKEN, STOPPING}
    Then I verify the GDK_COMPONENT_NAME component is RUNNING using the greengrass-cli
    When I record the device's CPU usage statistic as CPU2
    # Update the intended threshold as needed, as percent utilization. If extending the test case, use two unique identifiers recorded during previous steps.
    Then the increase in the CPU usage from CPU1 to CPU2 is less than 25 percent
Feature: Testing features of Greengrassv2 IPC sample

    @testgg
    Scenario: As a developer, I can create a component and deploy it on my device
        Given my device is registered as a Thing
        And my device is running Greengrass
        When I create a Greengrass deployment with components
            | aws.greengrass.clientdevices.Auth          | LATEST           |
            | aws.greengrass.clientdevices.mqtt.Moquette | LATEST           |
            | aws.greengrass.clientdevices.mqtt.Bridge   | LATEST           |
            | aws.greengrass.clientdevices.IPDetector    | LATEST           |
            | software.amazon.awssdk.sdk-gg-ipc          | file:recipe.yaml |
        When I update my Greengrass deployment configuration, setting the component aws.greengrass.clientdevices.Auth configuration to:
        """
        {
            "MERGE": {
                "deviceGroups": {
                    "formatVersion": "2021-03-05",
                    "definitions": {
                        "MyPermissiveDeviceGroup": {
                            "selectionRule": "thingName: *",
                            "policyName": "MyPermissivePolicy"
                        }
                    },
                    "policies": {
                        "MyPermissivePolicy": {
                            "AllowAll": {
                                "statementDescription": "Allow client devices to perform all actions.",
                                "operations": [
                                    "*"
                                ],
                                "resources": [
                                    "*"
                                ]
                            }
                        }
                    }
                }
            }
        }
        """
        When I update my Greengrass deployment configuration, setting the component aws.greengrass.clientdevices.mqtt.Bridge configuration to:
        """
        {
            "MERGE": {
                "mqttTopicMapping": {
                    "ClientDeviceHelloWorld": {
                        "topic": "clients/+/hello/world",
                        "source": "LocalMqtt",
                        "target": "IotCore"
                    },
                    "ClientDeviceEvents": {
                        "topic": "clients/+/detections",
                        "targetTopicPrefix": "events/input/",
                        "source": "LocalMqtt",
                        "target": "Pubsub"
                    },
                    "ClientDeviceCloudStatusUpdate": {
                        "topic": "clients/+/status",
                        "targetTopicPrefix": "$aws/rules/StatusUpdateRule/",
                        "source": "LocalMqtt",
                        "target": "IotCore"
                    }
                }
            }
        }
        """
        And I deploy the Greengrass deployment configuration
        Then the Greengrass deployment is COMPLETED on the device after 120 seconds
        And the software.amazon.awssdk.sdk-gg-ipc log on the device contains the line "Received payload: hello" within 20 seconds

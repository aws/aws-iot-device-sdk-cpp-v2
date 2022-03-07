import boto3

client = boto3.client('iot')
response = client.create_thing(
    thingName='ci_test'
)

print("TEST: successed calling iot python sdk")
import boto3

client = boto3.client('iot')
print("boto3: create thing failed")
try:
    response = client.create_thing(
        thingName='ci_test'
    )
    print("Sucess: create thing failed")
except:
    print("Failed: create thing failed")
import boto3

def create_password_table(dynamodb=None):
    if not dynamodb:
        dynamodb = boto3.resource('dynamodb',region_name='us-east-1')

    table = dynamodb.create_table(
        TableName='securitycheck',
        KeySchema=[
            {
                'AttributeName': 'username',
                'KeyType': 'HASH'  # Partition key
            },
            # {
            #     'AttributeName': 'username',
            #     'KeyType': 'RANGE'  # Sort key
            # }
        ],
        AttributeDefinitions=[
            {
                'AttributeName': 'username',
                'AttributeType': 'S'
            },
            #  {
            #     'AttributeName': 'username',
            #     'AttributeType': 'S'
            # },
        ],
        ProvisionedThroughput={
            'ReadCapacityUnits': 10,
            'WriteCapacityUnits': 10
        }
    )
    return table


if __name__ == '__main__':
    game_table = create_password_table()
    print("Table status:", game_table.table_status)
7
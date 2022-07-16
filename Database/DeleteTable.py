import boto3

def delete_game_table(dynamodb=None):
    if not dynamodb:
        dynamodb = boto3.resource('dynamodb', region_name="us-east-1")

    table = dynamodb.Table('securitycheck')
    table.delete()


if __name__ == '__main__':
    delete_game_table()
    print("game table deleted.")

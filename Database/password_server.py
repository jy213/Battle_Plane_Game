import boto3
import pickle
import socket
from boto3.dynamodb.conditions import Key

def username_exists(username, dynamodb=None):
    if not dynamodb:
        dynamodb = boto3.resource('dynamodb', region_name='us-east-1')

    table = dynamodb.Table('securitycheck')

    response = table.get_item(Key={'username': username})
    if 'Item' not in response:
        return False
    else:
        return True

def password_check(username, password, dynamodb=None):
    if not dynamodb:
        dynamodb = boto3.resource('dynamodb', region_name='us-east-1')

    table = dynamodb.Table('securitycheck')

    response = table.get_item(Key={'username': str(username)})
    print(response)
    if 'Item' not in response:
        return False
    else:
        print(response["Item"]["password"])
        if password==response["Item"]["password"]:
            return True
        else: 
            return False

def put_game(username, password, dynamodb=None):
    if not dynamodb:
        dynamodb = boto3.resource('dynamodb', region_name='us-east-1')

    table = dynamodb.Table('securitycheck')
    response = table.put_item(
       Item={
            'username': username,
            'password': password,
        }
    )
    return response


if __name__ == '__main__':

    print("tcp server")
    server_port = 16000
    welcome_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    welcome_socket.bind(('0.0.0.0', server_port))
    welcome_socket.listen(1)
    print("server running on port", server_port)
    connection_socket, caddr = welcome_socket.accept()

    while True:
        cmsg = connection_socket.recv(1024)
        gamedata = pickle.loads(cmsg)   # cmsg [1, username, password]   0 sign up  1 sign in
        if gamedata["mode"]=='0':
            if username_exists(gamedata["username"])==True:
                message="0"
            else:
                put_game(gamedata["username"], gamedata["password"])
                message='1'
        elif gamedata["mode"]=='1':
            if password_check(gamedata["username"], gamedata["password"])==True:
                message='1'
            else:
                message='0'
        else:
            message="fail"

        cmsg = pickle.dumps(message)
        connection_socket.send(cmsg)        


   

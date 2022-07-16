import socket
import pickle
print("We're in tcp client...")

#the server name and port client wishes to access
score_server_name = '3.88.10.147'
score_server_port = 10000
score_client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
score_client_socket.connect((score_server_name, score_server_port))

login_server_name = '34.227.172.78'
login_server_port = 16000
login_client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
login_client_socket.connect((login_server_name, login_server_port))

while True:



    # Login

    username = input("Enter your name: ")
    password = input("Enter your password: ")  # need to be changed with a variable

    login_gamedata = {"mode": "1", "username": username, "password": password}

    # some work
    login_msg = pickle.dumps(login_gamedata)

    # send the message  to the udp server
    login_client_socket.send(login_msg)

    # return values from the server
    login_msg = login_client_socket.recv(1024)
    print(pickle.loads(login_msg))
    if pickle.loads(login_msg)[0] == "0":
        print("invalid")
    else:
        print("proceed")

    # Score

    username = input("Enter your name: ")
    score = '100'
    score_gamedata={"username":username, "score":score}

    #some work
    score_msg = pickle.dumps(score_gamedata)

    #send the message  to the udp server
    score_client_socket.send(score_msg)

    #return values from the server
    score_msg = score_client_socket.recv(1024)
    winner_data = pickle.loads(score_msg)[0]
    user_data = pickle.loads(score_msg)[1]
    print("Leaderboard: ",winner_data)
    print("Best scores: ",user_data)

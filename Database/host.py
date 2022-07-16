import subprocess
import socket
import os
import signal

#the server name and port client wishes to access
server_name = 'localhost'
server_port = 12000
#create a TCP client socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

#Set up a TCP connection
#connection_socket will be assigned to this client on the server side
client_socket.connect((server_name, server_port))


def send_on_jtag():
    usrname = client_socket.recv(1024).decode()  #receive username from game
    print("username: ",usrname)
    cmd = "nios2-terminal.exe <<< {}".format(usrname);
    output = subprocess.Popen(cmd, shell=True,executable='/bin/bash', stdout=subprocess.PIPE, preexec_fn=os.setsid)  #this bash will continuous to run until it detected ^D
    while True:
        vals = output.stdout.readline()   # extract the output from the subprocess call
        msg = vals.strip()
        msg = msg.decode("utf-8")  #decode

        client_socket.send(msg.encode())  #send accelerometer reading and button status to game
        
        msg = msg.split('<-->')
        try:
            msg = msg[1].split()
        except Exception:
            pass
        try:
            if msg[2] == "0": #restart interrupt
                break
        except Exception:
            pass
        

def terminate():
    usrname = client_socket.recv(1024).decode() #receive the score
    usrname = usrname.strip()
    cmd = "nios2-terminal.exe <<< {}".format(usrname);  
    output = subprocess.Popen(cmd, executable = '/bin/bash', shell=True, stdout=subprocess.PIPE)  #put it into fpga to display on seven-segment display

def main():
    while True:
        send_on_jtag()
        terminate()
    
if __name__ == '__main__':
    main()
    

client_socket.close()
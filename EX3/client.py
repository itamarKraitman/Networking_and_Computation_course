import sys
from socket import *

# receiving the arguments
serverName = sys.argv[1]
serverPort = int(sys.argv[2])
filename = "." + sys.argv[3]
# serverName = "141.226.12.148"
# serverPort = 6999

# creating client socket

try:
    clientSocket = socket(AF_INET, SOCK_STREAM)
    clientSocket.connect((serverName, serverPort))
    print("connected to server ", serverName, "and port ", serverPort)
    # print(filename)
    clientSocket.send(("/ " + filename).encode())
    response = (clientSocket.recv(2048)).decode()
    print(response)
except IOError:
    print("An error has occurred")
clientSocket.close()
sys.exit()

import time
from socket import *
import sys
from threading import Thread

serverSocket = socket(AF_INET, SOCK_STREAM)
# Prepare a sever socket
serverPort = 63343
serverSocket.bind(("0.0.0.0", serverPort))
serverSocket.listen(1)


def runThread(connectionSocket, addr):
    try:
        message = connectionSocket.recv(2048).decode()
        filename = message.split()[1]
        f = open(filename[1:])
        outputdata = f.read()  # outputdata == HelloWorld.html
        connectionSocket.send('HTTP/1.1 200 OK\n'.encode())  # Send one HTTP header line into socket
        for i in range(0, len(outputdata)):  # Send the content of the requested file to the client
            connectionSocket.send(outputdata[i].encode())
        connectionSocket.send("\r\n".encode())
        connectionSocket.close()
    except IOError:
        connectionSocket.send(b"HTTP/1.1 404 Not Found\r\n")
        connectionSocket.send(b"<h1>404 Not Found </h1> \nContent-Type: text/html\r\n\r\n <h1>404 Not Found </h1>")
        connectionSocket.close()  # Close client socket


print("ready to service...")
while True:
    connectionSocket, addr = serverSocket.accept()
    thread = Thread(target=runThread, args=(connectionSocket, addr))
    thread.start()
    thread.join()
    time.sleep(5)  # to show there is a difference between threads

serverSocket.close()
sys.exit()

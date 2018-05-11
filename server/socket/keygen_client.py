#!/usr/bin/python
# -*- coding: utf-8 -*-

import socket
import sys

class client:
#private functions:
    def calcu_checksum(self, buf): 
        checksum = 0  
        for item in buf:  
            checksum += item
            checksum &= 0xFFFF # cut down 
        return checksum
    
    def __init__(self, host, port):
        self.host = host
        self.port = port

#public functions:
    def connect(self):
        '''Create a socket (SOCK_STREAM means a TCP socket),Connect to server'''
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.sock.connect((HOST, PORT))
            print("connect to HOST %s successfully!" %HOST)
        except socket.gaierror as e:
            print("Address-related error connecting to server: %s" % e)
            sys.exit(1)
        except socket.error as e:
            print("Connection error: %s" % e)
            sys.exit(1)
            
    def recv(self):
        '''Receive data from the server(blocking)'''
        try:
            while True:
                received = self.sock.recv(1024)
                if received:
                    print("Received: {}".format(received))
                    break
        except socket.error as e:
            print("Connection error: %s" % e)

    def send(self):
        '''Send request to the server
        #Frame
        #bytes: |  1 |   1   |    2   |  
        #data:  |HEAD|COMMAND|checksum|
        #total: 4 bytes
        '''
        self.data = bytes(chr(HEAD),'utf-8')  #add HEAD to frame
        self.data += bytes(chr(COMMAND),'utf-8')  #add COMMAND to frame

        #add checksum
        checksum = bytes(chr(self.calcu_checksum(self.data)),'utf-8')
        if(len(checksum) == 1):
            self.data += bytes(chr(0x00),'utf-8')
            self.data += checksum
        else:
            self.data += checksum
    
        #send request
        try:
            self.sock.sendall(self.data)
            print("Sent: {}".format(self.data))
        except socket.error as e:
            print("Error sending data: %s" % e)
            sys.exit(1)

    def disconnect(self):
        '''Exit normally''' 
        print("Connection will close in a moment!")
        self.sock.close()
        sys.exit(1)



if __name__ == "__main__": 
    HOST, PORT = "45.77.171.135", 37465
    HEAD = 0x01
    COMMAND = 0x05  #0x07:send data cmd; 0x05:get invitation cmd

    myclient = client(HOST, PORT)
    myclient.connect()
    myclient.send()
    myclient.recv()
    myclient.disconnect()

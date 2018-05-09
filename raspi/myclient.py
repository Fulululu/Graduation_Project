#!/usr/bin/python
# -*- coding: utf-8 -*-

import socket
import sys
import time
import serial
import threading

class client:
#private functions:
    def calcu_checksum(self, buf): 
        checksum = 0  
        for item in buf:  
            checksum += item
            checksum &= 0xFFFF # cut down 
        return checksum

    def open_serial(self):
        print("opening a port")
        try:
            self.ser = serial.Serial("/dev/ttyUSB0", 115200, timeout=0.5)
        except serial.SerialException as e:
            print(e)
            sys.exit(1)
        print("port %s is opened!" % self.ser.name)

    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.open_serial()
        
    def serial_get(self):
        '''
        #USB Frame from CC2530
        #bytes:  |   1   |  2  |   4   |   1   |    4   |    1   |   total:13 bytes
        #datas:  |NODE_ID|light|airtemp|airhumi|soiltemp|soilhumi|
        '''
        try:
            self.buf = self.ser.readline()
            if self.buf:
                print("{}: {}".format(self.ser.name,self.buf))
                self.end = len(self.buf)
                return self.buf[0:self.end]
        except serial.SerialException as e:
            print(e)
            sys.exit(1)

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
        except socket.error as e:
            print("Connection error: %s" % e)

    def send(self):
        '''Send data to the server
        #Frame
        #bytes: |  1 |   1   | 1 |   1   |  2  |   4   |   1   |    4   |    1   |    2   |  
        total:18 bytes
        #datas: |HEAD|COMMAND|UID|NODE_ID|light|airtemp|airhumi|soiltemp|soilhumi|checksum|
        '''
        self.ser_data = self.serial_get()
        if(self.ser_data):
            self.data = bytes(chr(HEAD),'utf-8')  #add HEAD to frame
            self.data += bytes(chr(COMMAND),'utf-8')  #add COMMAND to frame
            self.data += bytes(chr(UID),'utf-8')  #add UID to frame
            self.data += self.ser_data
            if len(self.data) == 16:
                #add checksum
                checksum = bytes(chr(self.calcu_checksum(self.data)),'utf-8')
                if(len(checksum) == 1):
                    self.data += bytes(chr(0x00),'utf-8')
                    self.data += checksum
                else:
                    self.data += checksum
    
                #send
                try:
                    self.sock.sendall(self.data)
                    print("Sent: {}".format(self.data))
                except socket.error as e:
                    print("Error sending data: %s" % e)
                    sys.exit(1)
            else:
                pass
        else:
            pass

    def disconnect(self):
        '''Exit normally''' 
        print("Connection will close in a moment!")
        self.sock.close()
        sys.exit(1)



if __name__ == "__main__": 
    HOST, PORT = "45.77.171.135", 37465
    HEAD = 0x01
    COMMAND = 0x07  #0x07:send data cmd; 0x05:get invitation cmd
    UID = 0x01
    myclient = client(HOST, PORT)
    myclient.connect()
    recv_thread = threading.Thread(target = myclient.recv)
    recv_thread.daemon = True
    recv_thread.start()

    # Main Thread
    while True:
        myclient.send()
        time.sleep(5)
        
    myclient.disconnect()

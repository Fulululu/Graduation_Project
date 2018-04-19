#!/usr/bin/python
# -*- coding: utf-8 -*-

import socket
import sys
import time
import serial
import threading
#import struct

class client:
    def open_serial(self):
        print("opening a port")
        try:
            self.ser = serial.Serial("/dev/ttyUSB0", 115200, timeout=0.5)   #UART to USB
            #self.ser = serial.Serial("/dev/ttyAMA0", 115200, timeout=0.5)  #UART to UART
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
        #UART Frame from CC2530
        #bytes:  |   1   |  2  |   4   |   1   |    4   |    1   |   total:13 bytes
        #datas:  |NODE_ID|light|airtemp|airhumi|soiltemp|soilhumi|
        '''
        try:
            self.buf = self.ser.readline()
            if self.buf:
                print("{}: {}".format(self.ser.name,self.buf))
                self.end = len(self.buf)
                return self.buf[0:self.end]  #remove the end flag '\n',for example a[0:17] not contain a[17]
        except serial.SerialException as e:
            print(e)
            sys.exit(1)

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
        #bytes: | 1 |   1   |  2  |   4   |   1   |    4   |    1   |   total:14 bytes
        #datas: |UID|NODE_ID|light|airtemp|airhumi|soiltemp|soilhumi|
        '''
        self.data = bytes(chr(UID),'utf-8')  #add UID to frame
        self.ser_data = self.serial_get()
        if(self.ser_data):
            self.data += self.ser_data
            if len(self.data) == 14:
                #self.data_unpack = struct.unpack('!2bhfbfb', self.data)
                #print(self.data_unpack)
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

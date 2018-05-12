#!/usr/bin/python3
# -*- coding: utf-8 -*-

import socket
import sys
import time
import serial
import threading
import struct
import RPi.GPIO as GPIO
from mymodule.hidepwd import getpass

class client:
#private functions:
    def calcu_checksum(self, buf):
        checksum = 0
        for item in buf:
            checksum += item
            checksum &= 0xFFFF # cut down
        return checksum

    def recalculate(self, buf):
        buflen = len(buf)
        checksum = bytes(chr(self.calcu_checksum(buf[:buflen-2])),'utf-8')
        if(len(checksum) == 1):
            result = bytes(chr(0x00),'utf-8')
            result += checksum
            return result
        else:
            result = checksum
            return result

    def open_serial(self):
        print("opening a port")
        try:
            self.ser = serial.Serial("/dev/ttyUSB0", 115200, timeout=0.5)
            print("port %s is opened!" % self.ser.name)
        except serial.SerialException as e:
            print(e)
            GPIO.cleanup()
            sys.exit(1)

    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.is_login = False
        self.UID = UID
        self.open_serial()
        GPIO.setwarnings(False)
        GPIO.cleanup()
        GPIO.setmode(GPIO.BOARD)
        GPIO.setup(13, GPIO.OUT, initial=GPIO.HIGH)
        GPIO.setup(15, GPIO.OUT, initial=GPIO.HIGH)

    def serial_get(self):
        '''
        #USB Frame from CC2530
        #bytes:  |   1   |  2  |   4   |   1   |    4   |    1   |   total:13 bytes
        #datas:  |NODE_ID|light|airtemp|airhumi|soiltemp|soilhumi|
        '''
        try:
            #self.buf = self.ser.readline()
            self.buf = self.ser.read(13)
            if self.buf:
                print("{}: {}".format(self.ser.name,self.buf))
                self.end = len(self.buf)
                return self.buf[0:self.end]
        except serial.SerialException as e:
            print(e)
            GPIO.cleanup()
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
            GPIO.cleanup()
            sys.exit(1)
        except socket.error as e:
            print("Connection error: %s" % e)
            GPIO.cleanup()
            sys.exit(1)

    def login(self):
        '''
        #Login Frame to server
        #bytes: |  1 |   1   |  >0   |         1        |   >0   |    2   |
        #datas: |HEAD|COMMAND|account|US(unit separator)|password|checksum|
        #total: >6 bytes
        '''
        account = input('Please enter your account: \n')
        print('Please enter your password: ')
        password = getpass('*')
        account_utf8 = account.encode('utf-8')
        password_utf8 = password.encode('utf-8')
        len_acc = len(account_utf8)
        len_pwd = len(password_utf8)

        loginframe = struct.pack('!3b{}sb{}s'.format(len_acc,len_pwd),HEAD,COMMAND_syn,len_acc,account_utf8,len_pwd,password_utf8)

        #add checksum
        checksum = bytes(chr(self.calcu_checksum(loginframe)),'utf-8')
        if(len(checksum) == 1):
            loginframe += bytes(chr(0x00),'utf-8')
            loginframe += checksum
        else:
            loginframe += checksum
        self.sock.sendall(loginframe)
        print('\nlogin......')
        while(not self.is_login):
            time.sleep(1)
        print('{}(UID:{}) login successfully'.format(account, self.UID))

    def recv(self):
        '''Receive data from the server(blocking)'''
        try:
            self.sock.setblocking(True)
            while True:
                recvdata = self.sock.recv(1024)
                if recvdata:
                    recvlen = len(recvdata)
                    print("Recv_Data: {}".format(recvdata))
                    if(self.recalculate(recvdata) == recvdata[recvlen-2:]):
                        if(recvdata[1] == 0x11):
                            if(recvdata[2] == 0x01):
                                GPIO.output(13, GPIO.LOW)
                            else:
                                GPIO.output(13, GPIO.HIGH)
                            if(recvdata[3] == 0x01):
                                GPIO.output(15, GPIO.LOW)
                            else:
                                GPIO.output(15, GPIO.HIGH)
                        if(recvdata[1] == 0x16):
                            if(recvdata[2] != 0):
                                self.is_login = True
                                self.UID = recvdata[2]
        except socket.error as e:
            print("Connection error: %s" % e)
            GPIO.cleanup()
            sys.exit(1)

    def senddata(self):
        '''Send data to the server
        #Frame
        #bytes: |  1 |   1   | 1 |   1   |  2  |   4   |   1   |    4   |    1   |  1 |  1 |    2   |  
        #datas: |HEAD|COMMAND|UID|NODE_ID|light|airtemp|airhumi|soiltemp|soilhumi|PUMP|LAMP|checksum|
        #total:20 bytes
        '''
        self.ser_data = self.serial_get()
        if(self.ser_data):
            self.data = bytes(chr(HEAD),'utf-8')  #add HEAD to frame
            self.data += bytes(chr(COMMAND_data),'utf-8')  #add COMMAND_data to frame
            self.data += bytes(chr(self.UID),'utf-8')  #add UID to frame
            self.data += self.ser_data  # add sensor data to frame
            self.data += bytes(chr(PUMP),'utf-8')  #add PUMP to frame
            self.data += bytes(chr(LAMP),'utf-8')  #add LAMP to frame
            if len(self.data) == 18:
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
                    GPIO.cleanup()
                    sys.exit(1)
            else:
                pass
        else:
            pass
        
    def disconnect(self):
        '''Exit normally''' 
        print("Connection will close in a moment!")
        self.sock.close()
        GPIO.cleanup()
        sys.exit(1)



if __name__ == "__main__":
    HOST, PORT = "45.77.171.135", 37465
    HEAD = 0x01  #frame head
    # 0x07:send data cmd; 0x05:get invitation cmd; 0x16:synchronous account
    COMMAND_data = 0x07
    COMMAND_syn = 0x16
    UID = 0x00   #user ID
    PUMP = 0x00  #pump status
    LAMP = 0x00  #lamp status

    myclient = client(HOST, PORT)
    myclient.connect()
    recv_thread = threading.Thread(target = myclient.recv)
    recv_thread.daemon = True
    recv_thread.start()

    # Main Thread
    myclient.login()
    while True:
        myclient.senddata()
        time.sleep(10)

    myclient.disconnect()

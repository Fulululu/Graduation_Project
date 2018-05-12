#!/usr/bin/python3
# -*- coding: utf-8 -*-

import socket
import threading
import socketserver
import time
import sqlite3
import struct
import random

class ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    pass

class ThreadedTCPRequestHandler(socketserver.BaseRequestHandler):
#private functions:
    def keygen(self):
        chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
        code = ""
        length = len(chars)-1
        codelen = random.randint(6,9)
        for i in range(0,codelen):
            idx = random.randint(0, length)
            code += chars[idx]
            now = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
        try:
            conn = sqlite3.connect('../mysite/db.sqlite3')
            cursor = conn.cursor()
            cursor.execute("INSERT INTO index_invitation (CODE, CREATETIME) VALUES ('{}','{}')".format(code, now))
            print("Create invitation code: "+code)
        except:
            code = ""
            print("Fail to create invitation code")
        finally:
            cursor.close()
            conn.commit()
            conn.close()
            if(code):
                response = bytes("Create invitation code: "+code, 'utf-8')
                self.request.sendall(response)
            else:
                response = bytes("Fail to create invitation code", 'utf-8')
                self.request.sendall(response)

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

    def dataDispose(self, data):
        datalen = len(data)
        if datalen < 40:
            if(self.recalculate(data) == data[datalen-2:]):
                try:
                    head_unpack = struct.unpack('!2b', data[:2])
                    self.HEAD = head_unpack[0]
                    self.COMMAND = head_unpack[1]
                    if(self.COMMAND == 0x07):
                        data_unpack = struct.unpack('!2bhfbf3b', data[2:datalen-2])
                        self.UID = data_unpack[0]
                        self.NODE = data_unpack[1]
                        self.light = data_unpack[2]
                        self.airtemp = round(data_unpack[3], 2)
                        self.airhumi = data_unpack[4]
                        self.soiltemp = round(data_unpack[5], 2)
                        self.soilhumi = data_unpack[6]
                        self.pump = data_unpack[7]
                        self.lamp = data_unpack[8]
                        self.time = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
                        return 0
                    elif(self.COMMAND == 0x05):
                        return 0
                    elif(self.COMMAND == 0x16):
                        data_unpack = struct.unpack('!{}s'.format(data[2]), data[3:3+data[2]])
                        self.account = str(data_unpack[0], encoding='utf-8')
                        data_unpack = struct.unpack('!{}s'.format(data[3+data[2]]),data[3+data[2]+1:datalen-2])
                        self.password = str(data_unpack[0], encoding='utf-8')
                        return 0
                except struct.error as e:
                    print(e)
            else:
                print('data:')
                print(data)
                print('checkdata:')
                print(bytes(chr(self.calcu_checksum(data[:datalen-2])),'utf-8'))
                print('recv_checksum:')
                print(data[datalen-2:])
                return 2 #checksum error
        else:
            return 1 #datalength error

    def get_devstate(self):
        '''dev_state[0]--LAMP, dev_state[1]--PUMP,
           dev_state[2]--AUTO_LAMP, dev_state[3]--AUTO_PUMP'''
        try:
            conn = sqlite3.connect('../mysite/db.sqlite3')
            cursor = conn.cursor()
            obj = cursor.execute("SELECT LAMP,PUMP,AUTO_LAMP,AUTO_PUMP FROM index_device WHERE UID_id={}".format(self.UID))
            dev_state = obj.fetchone()
            cursor.close()
            conn.commit()
            conn.close()
            return dev_state
        except:
            print('an exception occured in get_devstate()')
            return 1

    def doStore(self):
        try:
            conn = sqlite3.connect('../mysite/db.sqlite3')
            cursor = conn.cursor()
            cursor.execute("INSERT INTO data_summary (UID_id, NODE, LIGHT, AIRTEMP, AIRHUMI, SOILTEMP, SOILHUMI, CREATETIME) VALUES ({}, {}, {}, {}, {}, {}, {}, '{}')".format(self.UID, self.NODE, self.light, self.airtemp, self.airhumi, self.soiltemp, self.soilhumi, self.time))
            # Judging dev status is manualctl or autoctl(Priority: manualctl > autoctl)
            obj = cursor.execute("select AUTO_LAMP,AUTO_PUMP from index_device where UID_id={}".format(self.UID))
            auto_flag = obj.fetchone()
            if(auto_flag[0] == True): #lamp
                cursor.execute("UPDATE index_device SET LAMP = {} WHERE UID_id = {}".format(self.lamp, self.UID))
            if(auto_flag[1] == True): #pump
                cursor.execute("UPDATE index_device SET PUMP = {} WHERE UID_id = {}".format(self.pump, self.UID))

            cursor.close()
            conn.commit()
            conn.close()
        except:
            print('an exception occured in doStore()')

    def devicectl(self):
        '''Send data to the server
        #Frame
        #bytes: |  1 |   1   |  1 |  1 |    2   |
        #datas: |HEAD|COMMAND|PUMP|LAMP|checksum|
        #total: 6 bytes
        '''
        fdbk_data = bytes(chr(HEAD),'utf-8')  #add HEAD to frame
        fdbk_data += bytes(chr(COMMAND_ctl),'utf-8')  #add COMMAND_ctl to frame
        dev_state = self.get_devstate()
        if(dev_state[3] == True): #pump_autoctl
            if(self.soilhumi < 30):
                fdbk_data += bytes(chr(0x01),'utf-8')  #set PUMP flag
            else:
                fdbk_data += bytes(chr(0x00),'utf-8')  #clear PUMP flag
        else: #manualctl
            fdbk_data += bytes(chr(dev_state[1]),'utf-8')  #set PUMP flag
        if(dev_state[2] == True): #lamp_autoctl
            if(self.light < 100):
                fdbk_data += bytes(chr(0x01),'utf-8')  #set LAMP flag
            else:
                fdbk_data += bytes(chr(0x00),'utf-8')  #clear LAMP flag
        else: #manualctl
            fdbk_data += bytes(chr(dev_state[0]),'utf-8')  #set LAMP flag

        checksum = bytes(chr(self.calcu_checksum(fdbk_data)),'utf-8')
        if(len(checksum) == 1):
            fdbk_data += bytes(chr(0x00),'utf-8')
            fdbk_data += checksum
        else:
            fdbk_data += checksum
        self.request.sendall(fdbk_data)

    def verify(self):
        try:
            conn = sqlite3.connect('../mysite/db.sqlite3')
            cursor = conn.cursor()
            obj = cursor.execute("SELECT UID FROM index_user WHERE ACCOUNT={} AND PASSWORD='{}'".format(self.account, self.password))
            account_list = obj.fetchall()
            self.UID = account_list[0][0]
            cursor.close()
            conn.commit()
            conn.close()

            response = struct.pack('!3b',HEAD,COMMAND_syn,self.UID)
            checksum = bytes(chr(self.calcu_checksum(response)),'utf-8')
            if(len(checksum) == 1):
                response += bytes(chr(0x00),'utf-8')
                response += checksum
            else:
                response += checksum
            self.request.sendall(response)
            return 0
        except sqlite3.OperationalError as e:
            print(e)
            return 1

#public functions:
    def setup(self):
        self.ip = self.client_address[0].strip()
        self.port = self.client_address[1]
        self.request.setblocking(True)  # Set Blocking(Socket default Non-blocking)
        self.timeOut = 300  # 5min
        self.request.settimeout(self.timeOut)
        self.cur_thread = threading.current_thread()
        print(self.cur_thread.name+": ["+self.ip+":"+str(self.port)+"] is connect!")
        client_addr.append(self.client_address)
        client_socket.append(self.request)

    def handle(self):
        while True:
            try:
                data = self.request.recv(1024)  # Blocking when no data
                if data:
                    ret = self.dataDispose(data)
                    if(not ret):
                        if(self.COMMAND == 0x07):
                            self.doStore()
                            print("{}: Client({}:{}): {}".format(self.cur_thread.name,self.ip,self.port,data))
                            self.devicectl()  # feedback control
                        elif(self.COMMAND == 0x05):
                            self.keygen()
                            break
                        elif(self.COMMAND == 0x16):
                            self.verify()
                    elif(ret == 2):
                        response = bytes("checksum error!", 'utf-8')
                        self.request.sendall(response)
                    else:
                        response = bytes("datalength error!", 'utf-8')
                        self.request.sendall(response)
                else:  #socket.recv() return None when disconnect(or socket broken)
                    break
                time.sleep(1)
            except socket.timeout as e:  #socket.timeout is throwed only when socket is block mode
                print(self.cur_thread.name+": ["+self.ip+":"+str(self.port)+"] {}".format(e))
                break
            except socket.error as e:
                print("{}: Connection error: {}".format(self.cur_thread.name, e))
                break

    def finish(self):
        print(self.cur_thread.name+": ["+self.ip+":"+str(self.port)+"] is disconnect! End of thread!")
        client_addr.remove(self.client_address)
        client_socket.remove(self.request)



if __name__ == "__main__": 
    client_addr = []
    client_socket = []
    # Port 0 means to select an arbitrary unused port
    HOST, PORT = "0.0.0.0", 37465
    HEAD = 0x01
    COMMAND_ctl = 0x11  # 0x11:devices control
    COMMAND_syn = 0x16  # 0x16:synchronous account

    with ThreadedTCPServer((HOST, PORT), ThreadedTCPRequestHandler) as server:
        ip, port = server.server_address
        print("Serving at {}:{}".format(ip,port))

        # Start a thread with the server -- that thread will then start one
        # more thread for each request
        server_thread = threading.Thread(target=server.serve_forever)

        # Set server thread as daemon thread meaning that Exit the server thread when the main thread terminates
        server_thread.daemon = True
        server_thread.start()
        print("Server loop running in thread:", server_thread.name)

        # Activate the server; this will keep running until you
        # interrupt the program with Ctrl-C
        server.serve_forever()

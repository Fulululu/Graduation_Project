#!/usr/bin/python
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
            return code
        
    def calcu_checksum(self, buf): 
        checksum = 0  
        for item in buf:  
            checksum += item
            checksum &= 0xFFFF # cut down 
        return checksum
    
    def dataDispose(self, data):
        datalen = len(data)
        if datalen < 19:
            if(bytes(chr(self.calcu_checksum(data[:datalen-2])),'utf-8') == data[datalen-2:]):
                try:
                    head_unpack = struct.unpack('!2b', data[:2])
                    self.HEAD = head_unpack[0]
                    self.COMMAND = head_unpack[1]
                    if(self.COMMAND == 0x07):
                        data_unpack = struct.unpack('!2bhfbfb', data[2:datalen-2])
                        self.UID = data_unpack[0]
                        self.NODE = data_unpack[1]
                        self.light = data_unpack[2]
                        self.airtemp = round(data_unpack[3], 2)
                        self.airhumi = data_unpack[4]
                        self.soiltemp = round(data_unpack[5], 2)
                        self.soilhumi = data_unpack[6]
                        self.time = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
                        return 0
                    elif(self.COMMAND == 0x05):
                        return 0
                except struct.error as e:
                    print(e)
            else:
                return 2 #checksum error
        else:
            return 1 #datalength error
        
    def doStore(self):
        conn = sqlite3.connect('../mysite/db.sqlite3')
        cursor = conn.cursor()
        cursor.execute("INSERT INTO data_summary (UID_id, NODE, LIGHT, AIRTEMP, AIRHUMI, SOILTEMP, SOILHUMI, CREATETIME) VALUES ({}, {}, {}, {}, {}, {}, {}, '{}')".format(self.UID, self.NODE, self.light, self.airtemp, self.airhumi, self.soiltemp, self.soilhumi, self.time))
        cursor.close()
        conn.commit()
        conn.close()

#public functions:
    def setup(self):
        self.ip = self.client_address[0].strip()     
        self.port = self.client_address[1]
        self.request.setblocking(True)  # Socket default Non-blocking
        self.timeOut = 120  # 2min
        self.request.settimeout(self.timeOut)
        self.cur_thread = threading.current_thread()
        print(self.cur_thread.name+": ["+self.ip+":"+str(self.port)+"] is connect!")  
        client_addr.append(self.client_address)
        client_socket.append(self.request)      
        
    def handle(self):
        while True:
            try:
                data = self.request.recv(1024)
                if data:
                    ret = self.dataDispose(data)
                    if(not ret):
                        if(self.COMMAND == 0x07):
                            self.doStore()
                            print("{}: Client({}:{}): {}".format(self.cur_thread.name,self.ip,self.port,data))
                            response = bytes("server got", 'utf-8')
                            self.request.sendall(response)
                        elif(self.COMMAND == 0x05):
                            code = keygen()
                            if(code):
                                response = bytes("Create invitation code: "+code, 'utf-8')
                                self.request.sendall(response)
                            else:
                                response = bytes("Fail to create invitation code", 'utf-8')
                                self.request.sendall(response)
                                
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

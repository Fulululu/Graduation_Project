#!/usr/bin/python
# -*- coding: utf-8 -*-

import socket
import threading
import socketserver
import time
import sqlite3
import struct


class ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    pass

class ThreadedTCPRequestHandler(socketserver.BaseRequestHandler):

    def dataDispose(self, data):
        try:
            self.data_unpack = struct.unpack('!2bhfbfbf', data)
        except struct.error as e:
            print(e)
        self.UID = self.data_unpack[0]
        self.NODE = self.data_unpack[1]
        self.light = self.data_unpack[2]
        self.airtemp = self.data_unpack[3]
        self.airhumi = self.data_unpack[4]
        self.soiltemp = self.data_unpack[5]
        self.soilhumi = self.data_unpack[6]
        self.co2 = self.data_unpack[7]
        print(self.data_unpack)
        
    def doStore(self, data):
        self.dataDispose(data)
        conn = sqlite3.connect('/home/ygf/design/test.db')
        cursor = conn.cursor()
        cursor.execute('INSERT INTO light (UID, NODE, DATA) VALUES ({}, {}, {})'.format(self.UID, self.NODE, self.light))
        cursor.execute('INSERT INTO airtemp (UID, NODE, DATA) VALUES ({}, {}, {})'.format(self.UID, self.NODE, self.airtemp))
        cursor.execute('INSERT INTO airhumi (UID, NODE, DATA) VALUES ({}, {}, {})'.format(self.UID, self.NODE, self.airhumi))
        cursor.execute('INSERT INTO soiltemp (UID, NODE, DATA) VALUES ({}, {}, {})'.format(self.UID, self.NODE, self.soiltemp))
        cursor.execute('INSERT INTO soilhumi (UID, NODE, DATA) VALUES ({}, {}, {})'.format(self.UID, self.NODE, self.soilhumi))
        cursor.execute('INSERT INTO co2 (UID, NODE, DATA) VALUES ({}, {}, {})'.format(self.UID, self.NODE, self.co2))
        cursor.close()
        conn.commit()
        conn.close()

    def setup(self):
        self.ip = self.client_address[0].strip()     
        self.port = self.client_address[1]
        self.request.setblocking(True)  #Socket default Non-blocking
        self.timeOut = 20
        self.request.settimeout(self.timeOut)
        self.cur_thread = threading.current_thread()
        print(self.cur_thread.name+": ["+self.ip+":"+str(self.port)+"] is connect!")  
        client_addr.append(self.client_address)
        client_socket.append(self.request)      
        
    def handle(self):
        while True:
            try:
                self.data = self.request.recv(1024)
                if self.data:
                    self.doStore(self.data)
                    self.dataDispose(self.data)
                    print("{}: Client({}:{}): {}".format(self.cur_thread.name,self.ip,self.port,self.data))
                    response = bytes("server got", 'utf-8')
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

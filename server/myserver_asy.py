import socket
import threading
import socketserver
import time


class ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    pass

class ThreadedTCPRequestHandler(socketserver.BaseRequestHandler):

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
                self.data = str(self.request.recv(1024), 'utf-8')
            except socket.timeout as e:  #socket.timeout is throwed only when socket is block mode
                print(self.cur_thread.name+": ["+self.ip+":"+str(self.port)+"] {}".format(e))                
                break
            
            if self.data:
                print("{}: Client({}:{}): {}".format(self.cur_thread.name,self.ip,self.port,self.data))
                response = bytes("server got", 'utf-8')
                self.request.sendall(response)
            else:  #socket.recv() return None when disconnect
                break
            time.sleep(1)
            
    def finish(self):  
        print(self.cur_thread.name+": ["+self.ip+":"+str(self.port)+"] is disconnect!")
        client_addr.remove(self.client_address)  
        client_socket.remove(self.request) 
        
if __name__ == "__main__": 
    client_addr = []
    client_socket = []
    # Port 0 means to select an arbitrary unused port
    HOST, PORT = "localhost", 9999
    
    with ThreadedTCPServer((HOST, PORT), ThreadedTCPRequestHandler) as server:
        ip, port = server.server_address
        print("Serving at {}:{}".format(ip,port))
        
        # Start a thread with the server -- that thread will then start one
        # more thread for each request
        server_thread = threading.Thread(target=server.serve_forever)
        
        # Exit the server thread when the main thread terminates
        server_thread.daemon = True
        server_thread.start()
        print("Server loop running in thread:", server_thread.name)

        # Activate the server; this will keep running until you  
        # interrupt the program with Ctrl-C  
        server.serve_forever() 

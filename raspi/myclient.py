import socket
import sys
import time

HOST, PORT = "localhost", 9999
data = " ".join(sys.argv[1:])
 
# Create a socket (SOCK_STREAM means a TCP socket)
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    # Connect to server and send data
    sock.connect((HOST, PORT))
except socket.gaierror as e:
    print("Address-related error connecting to server: %s" % e)
    sys.exit(1)
except socket.error as e:
    print("Connection error: %s" % e)
else:    
    while True:
        try:
            sock.sendall(bytes(data + "\n", "utf-8"))
            # Receive data from the server
            received = str(sock.recv(1024), "utf-8")
            print("Sent:     {}".format(data))
            print("Received: {}".format(received))
            time.sleep(100)
        except socket.error as e:
            print("Error sending data: %s" % e)
            sys.exit(1)
finally:
    print("Connection will close in a moment!")
    sock.close()
    sys.exit(1)

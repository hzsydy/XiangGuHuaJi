import socket
from XghjObject import *

serverIP='127.0.0.1'
serverPort=9999

def listenning(sock, addr):
    while True:
        try:
            data = sock.recv(1024)
            if len(data)==0:
                sock.close()
                print("[Error] Failed to connect the server. You've been offline.")
                return 1
            obj = XghjObject()
            obj.load(data)

            #if obj.action==XghjAction.NextRound or obj.action==XghjAction.BidPosition or obj.action==XghjAction.BidPrice:
            print obj.content

        except:
            sock.close()
            print("[Error] Failed to connect the server. You've been offline.")
            return 1

if "__main__"==__name__:

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    addr = (serverIP, serverPort)

    try:
        sock.connect(addr)
    except:
        print("[Error] Failed to connect the server. Please check the server: %s:%s" % addr)
        sys.exit(1)
    
    obj = XghjObject()
    obj.set(XghjSender.Viewer, XghjAction.OK, "Logger")
    try:
        sock.send(obj.toString())
    except:
        sock.close()
        print("[Error] Failed to send.")
        sys.exit(1)

    listenning(sock, addr)



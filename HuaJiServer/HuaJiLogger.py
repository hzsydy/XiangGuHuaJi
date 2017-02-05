import socket
from XghjObject import *

serverIP='127.0.0.1'
serverPort=9999

if "__main__"==__name__:

    # create a tcp socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    addr = (serverIP, serverPort)

    # connect
    try:
        sock.connect(addr)
    except:
        print("[Error] Failed to connect the server. Please check the server: %s:%s" % addr)
        sys.exit(1)
    
    # send my viewer info
    obj = XghjObject()
    obj.set(XghjSender.Viewer, XghjAction.OK, "Logger")
    try:
        sock.send(obj.toString())
    except:
        sock.close()
        print("[Error] Failed to send.")
        sys.exit(1)
    
    log_file = open("log.txt","w")

    # listen
    while True:
        try:
            data = sock.recv(1024)
            if len(data)==0:
                print("[Error] Failed to connect the server. You've been offline.")
                raise IOError
            obj = XghjObject()
            obj.load(data)
            
            print obj.content

            if obj.action==XghjAction.NextRound \
                        or obj.action==XghjAction.BidPosition \
                        or obj.action==XghjAction.BidPrice \
                        or obj.action==XghjAction.NewGame:
                log_file.write(obj.content+"\r\n")
        except:
            print("[Error] Failed to connect the server. You've been offline.")
            break

    sock.close()
    log_file.close()


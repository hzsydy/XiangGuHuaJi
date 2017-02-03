#-*- coding: UTF-8 -*-

# Created by sugar10w, 2017.2
# dependency: pip install enum34

import socket
import threading
from enum import Enum
from XghjObject import *

class ServerStatus(Enum):
    Ready = 0
    Playing = 1
    GameOver = 2
    Error = 3

status = ServerStatus.Ready

PLAYER_COUNT_MAX = 4
player_count=0

curr_round_count = 0;
playerRoundCount=[]
playerList=[]
viewerList=[]

def deleteUser(s,a,n):
    if (s,a,n) in playerList:
        playerList.remove((s,a,n))
        print "[Error] Player's connection %s:%s closed." % a
        s.close()
    elif (s,a,n) in viewerList:
        viewerList.remove((s,a,n))
        print "[Warning] Viewr's connection %s:%s closed." % a
        s.close()

def sendToAll(data):
    #print("\t\t"+data)
    for (s,a,n) in playerList:
        try:
            s.send(data)
        except:
            deleteUser(s,a,n)
    for (s,a,n) in viewerList:
        try:
            s.send(data)
        except:
            deleteUser(s,a,n) 

def sendNextRound():
    print "[Info] Everyone go ahead!"
    obj = XghjObject()
    obj.set(XghjSender.Server, XghjAction.NextRound, "Everyone go ahead!")
    sendToAll(obj.toString())

def sendGameOver():
    print "[Info] Good game, thank you!"
    obj = XghjObject()
    obj.set(XghjSender.Server, XghjAction.GameOver, "Good game, thank you!")
    sendToAll(obj.toString())
    status = ServerStatus.GameOver

def checkDeath(player_id):
    if player_id>=0:
        playerRoundCount[player_id] = 10000;
        obj = XghjObject()
        obj.set(XghjSender.Server, XghjAction.Kill, "P"+str(player_id)+" lost connection.")
        print "[Error] "+obj.content;
        sendToAll(obj.toString())
    
def tcplink(sock, addr):
    
    global status
    global player_count
    global curr_round_count
    global PLAYER_COUNT_MAX
    
    userName = "";
    player_id = -1;
    valid = False
    data = "";
    
    try:
        data = sock.recv(1024)
    except:
        sock.close()
        return 1;
    
    print "[Info] New connection from %s:%s" % (addr[0], addr[1]);
    
    first_obj = XghjObject()
    if not first_obj.load(data):
        print first_obj.content;
        first_obj.sender = XghjSender.Server;
        first_obj.action = XghjAction.Error;
        sock.send(first_obj.toString());        
        sock.close()
        return 1
    
    obj = XghjObject()
    obj.set(XghjSender.Server, XghjAction.Error, "Null")
    
    if first_obj.sender == XghjSender.Player:
        if first_obj.action == XghjAction.NewGame:
            if status == ServerStatus.Ready:
            
                # obj to send back
                player_id = player_count
                obj.set(XghjSender.Server, XghjAction.OK, \
                        "P" +str(player_id) + " accepted.")
                
                # keep in the list
                valid = True
                userName = "[P"+str(player_id)+"]"+first_obj.content
                playerList.append((sock,addr,userName))
                playerRoundCount.append(0)
                print '[Info] A new player entered: '+userName
                
                # player count & start playing
                player_count = player_count + 1
                
            else:
                # player wanted a new game, but the game is not ready
                obj.content = "Sorry the game is not ready yet."
                
        else:
            obj.content = "As a new player, you should join a new game before all."
            
    elif first_obj.sender == XghjSender.Viewer:
        if status == ServerStatus.Ready:
            # a new viewer
            obj.set(XghjSender.Server, XghjAction.OK, \
                        "viewer accepted.")
            
            # keep him
            valid = True
            userName = first_obj.content
            viewerList.append((sock, addrs, userName))
        else:
            # viewer but game's not ready
            obj.content = "Sorry the game is not ready yet."
            
    else:
        # ?? sender
        obj.content = "[Error] unidentified sender."
    
    sock.send(obj.toString())
    
    
    # status machine here 
    if status == ServerStatus.Ready:
        if PLAYER_COUNT_MAX == player_count:
            status = ServerStatus.Playing
            sendNextRound()
    
    if valid:
        while True:
            try:
                data = sock.recv(1024)
                if len(data)==0:
                    deleteUser(sock,addr,userName)
                    checkDeath(player_id)
                    return 1
            except:
                deleteUser(sock,addr,userName)
                checkDeath(player_id)
                return 1
            
            sendToAll(data)

if "__main__"==__name__: 

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # TCP socket
    s.bind(("", 9999)) 
    s.listen(100)  # no more than 100 clients
    print("[Info] Waiting for connection...")
    while True:
        sock, addr = s.accept()
        t = threading.Thread(target=tcplink, args=(sock, addr))
        t.start()

#-*- coding: UTF-8 -*-

# Created by sugar10w, 2017.2
# python 2.7
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
    Bidding = 4

class PlayerStatus(Enum):
    OK = 0
    Error = 1

player_status_list = [PlayerStatus.OK, PlayerStatus.OK, \
                      PlayerStatus.OK, PlayerStatus.OK]

status = ServerStatus.Ready

PLAYER_COUNT_MAX = 4
player_count=0

player_list=[]
viewer_list=[]

curr_round_count = 0

# kill a player
def killPlayer(player_id):
    if player_id<0 or player_id>=PLAYER_COUNT_MAX:
        return

    player_status_list[player_id] = PlayerStatus.Error

    obj = XghjObject()
    obj.set(XghjSender.Server, XghjAction.Kill, "P"+str(player_id)+" lost connection.")
    sendToAll(obj.toString())
    print "[Error] "+obj.content
    
    try:
        (s,a,n) = player_list[player_id]
        s.close()
    except:
        print "[Warning] failed to close a socket"

# delete a connection
def deleteConnection(s,a,n):
    if (s,a,n) in player_list:
        id = player_list.index((s,a,n))
        killPlayer(id)
    elif (s,a,n) in viewer_list:
        viewer_list.remove((s,a,n))
        print "[Warning] Viewr's connection %s:%s closed." % a
        s.close()

# send data to all users
def sendToAll(data, push_flag=False):

    if not push_flag and (status == ServerStatus.Ready or status == ServerStatus.Bidding):
        print "[Warning] Server ought to keep silent before `ServerStatus.Playing`"
        return

    for i in range(PLAYER_COUNT_MAX):
        if player_status_list[i]==PlayerStatus.OK:
            (s,a,n) = player_list[i]
            try:
                s.send(data)
            except:
                deleteConnection(s,a,n)
    for (s,a,n) in viewer_list:
        try:
            s.send(data)
        except:
            deleteConnection(s,a,n) 

# send XghjAction.sendNewGame to all 
def sendNewGame():
    print "[Info] New Game"
    global curr_round_count
    curr_round_count = 0
    content = str(PLAYER_COUNT_MAX)
    obj = XghjObject()
    obj.set(XghjSender.Server, XghjAction.NewGame, content)
    obj.round = curr_round_count

    sendToAll(obj.toString(), True)

# send XghjAction.GameOver
def sendGameOver():
    print "[Info] Good game, thank you!"
    obj = XghjObject()
    obj.set(XghjSender.Server, XghjAction.GameOver, "Good game, thank you!")
    obj.round = curr_round_count

    sendToAll(obj.toString(), True)
    status = ServerStatus.GameOver
# [Game] new connection
def checkNewConnection(sock, addr):
    
    global status
    global player_count
    global curr_round_count
    global PLAYER_COUNT_MAX
    
    userName = ""
    player_id = -1
    valid = False
    data = ""
    
    try:
        data = sock.recv(1024)
    except:
        sock.close()
        return 1
    
    print "[Info] New connection from %s:%s" % (addr[0], addr[1])
    
    first_obj = XghjObject()
    if not first_obj.load(data):
        print first_obj.content
        first_obj.sender = XghjSender.Server
        first_obj.action = XghjAction.Error
        sock.send(first_obj.toString())        
        sock.close()
        return 1
    
    # respond to first_obj
    obj = XghjObject()
    obj.set(XghjSender.Server, XghjAction.Error, "Null")
    if status == ServerStatus.Ready:
        if first_obj.sender == XghjSender.Player:
            if first_obj.action == XghjAction.NewGame:
                # obj to send back
                player_id = player_count
                obj.set(XghjSender.Server, XghjAction.OK, \
                        "P" +str(player_id) + " accepted.")
                
                # keep in the list
                valid = True
                userName = "[P"+str(player_id)+"] "+first_obj.content
                player_list.append((sock,addr,userName))
                print '[Info] A new player entered: '+userName
                
                # player count & start playing
                player_count = player_count + 1
            else:
                obj.content = "As a new player, you should join a new game before all."
        elif first_obj.sender == XghjSender.Viewer:
            # a new viewer
            obj.set(XghjSender.Server, XghjAction.OK, \
                        "viewer accepted.")
            # keep him
            valid = True
            userName = first_obj.content
            viewer_list.append((sock, addr, userName))
        else:
            # ?? sender
            obj.content = "[Error] unidentified sender."
    else:
        # viewer but game's not ready
        obj.content = "Sorry the game is not ready yet."

    try:
        sock.send(obj.toString())
    except:
        if valid:
            deleteConnection(sock, addr, userName)
        else:
            sock.close()
        return 

    # status machine
    if valid and player_id>=0:
        if PLAYER_COUNT_MAX <= player_count:
            if status == ServerStatus.Ready:
                status = ServerStatus.Bidding

# [Game] biddingPhase
def biddingPhase():
    bid_price_list = [0]*PLAYER_COUNT_MAX
    bid_position_list = [255,255]*PLAYER_COUNT_MAX

    # recv price in order
    for player_id in range(PLAYER_COUNT_MAX):
        if player_status_list[player_id]==PlayerStatus.OK:
            (s,a,n) = player_list[player_id]
            try:
                data = s.recv(1024)
                if len(data)==0:
                    raise IOError
                obj = XghjObject()
                if obj.load(data):
                    # TODO: check BidPrice
                    bid_price_list[player_id] = int(obj.content)
            except:
                killPlayer(player_id)
    
    # send bid_price_list to all 
    content = ""
    for player_id in range(PLAYER_COUNT_MAX): 
        content = content + " " + str(bid_price_list[player_id])
    obj = XghjObject()
    obj.set(XghjSender.Server, XghjAction.BidPrice, content)
    sendToAll(obj.toString(), True)

    # call the BidPosition (send posUnaccessible and recv pos) one by one
    content = ""
    for i in range(PLAYER_COUNT_MAX):
        player_id = bid_price_list.index(max(bid_price_list))
        bid_price_list[player_id] = -1
        if player_status_list[player_id]==PlayerStatus.OK:            
            (s,a,n) = player_list[player_id]
            try:
                obj = XghjObject()
                obj.set(XghjSender.Server, XghjAction.BidPosition, str(i)+" "+content)
                s.send(obj.toString())

                data = s.recv(1024)
                if len(data)==0:
                    raise IOError
                if obj.load(data):
                    pos = obj.content.split(None, 3)
                    bid_position_list[player_id*2] = int(pos[0])
                    bid_position_list[player_id*2+1] = int(pos[1])
            except:
                killPlayer(player_id)
            content = content  \
                + " " + str(bid_position_list[player_id*2]) \
                + " " + str(bid_position_list[player_id*2+1]) 
        else:
            content = content + " 0 0"

    # send full bid_position_list to all
    content = ""
    for i in range(len(bid_position_list)):
        content = content + " " + str(bid_position_list[i])
    obj = XghjObject()
    obj.set(XghjSender.Server, XghjAction.BidPosition, content)
    sendToAll(obj.toString(), True)
    
    print "[Game] biddingPhase finished."

# [Game] main round phase
def mainPhase():

    global curr_round_count

    active_players_count = 0

    print "[Info] It's round "+str(curr_round_count)+" now"
    null_command = " 0"*PLAYER_COUNT_MAX+" -1 255 255"
    player_command_list = [null_command]*PLAYER_COUNT_MAX
    for player_id in range(PLAYER_COUNT_MAX):
        if player_status_list[player_id]==PlayerStatus.OK:
            (s,a,n) = player_list[player_id]
            try:
                print "[Info] wait for P"+str(player_id)+" ..."
                data = s.recv(1024)
                if len(data)==0:
                    raise IOError
                obj = XghjObject()
                if obj.load(data):
                    # check validity of data
                    player_command_list[player_id] = obj.content
                    active_players_count = active_players_count + 1
            except:
                killPlayer(player_id)

    if active_players_count<2:
        sendGameOver()
        return False
    
    content = ""
    for i in range(PLAYER_COUNT_MAX):
        content = content + " " + player_command_list[i]
    obj = XghjObject()
    obj.set(XghjSender.Server, XghjAction.NextRound, content)

    obj.round = curr_round_count
    sendToAll(obj.toString())

    print "[Game] Round "+str(curr_round_count)+" finished."
    curr_round_count = curr_round_count+1
    return True

if "__main__"==__name__: 
    
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # TCP socket
    s.bind(("", 9999)) 
    s.listen(100)  # no more than 100 clients
    
    # ready for connection
    print "[Game] The Game is Ready. Waiting for connection..."
    while status==ServerStatus.Ready:
        sock, addr = s.accept()
        checkNewConnection(sock, addr)
        #t = threading.Thread(target=checkNewConnection, args=(sock, addr))
        #t.start()
    
    # bidding
    print "[Game] Bidding. Stop accepting connections."
    sendNewGame()
    biddingPhase()

    # game main phase
    status = ServerStatus.Playing
    curr_round_count = 1
    while status==ServerStatus.Playing:
        if not mainPhase():
            break
    
    status = ServerStatus.GameOver


#-*- coding: UTF-8 -*-

# Created by sugar10w, 2017.2
# dependency: pip install enum34

from enum import Enum

SIGNATURE = "XGHJxghj"
VERSION = 2

class XghjSender(Enum):
    Server = 0
    Player = 1
    Viewer = 2
    Unknown = 3

class XghjAction(Enum):
    OK = 0
    GameOver = 1
    NewGame = 2
    NextRound = 3
    Error = 4
    Invalid = 5
    Kill = 6
    
class XghjObject(object):
    def __init__(self):
        self.round = 0
        self.sender_id = -1
        self.map_checksum = 0
        self.version = VERSION
        self.sender = XghjSender.Unknown
        self.action = XghjAction.Invalid
        self.content="Null"
    
    def set(self, sender_, action_, content):
        self.sender = sender_
        self.action = action_
        self.content=content 
    
    def load(self, data):
    
        global SIGNATURE
        global VERSION
    
        list = data.split(None, 8)
        
        if list[0]!=SIGNATURE:
            self.content="[Error][XghjObject.load] Wrong Signature."
            return False
        if int(list[5])!=VERSION:
            self.content="[Error][XghjObject.load] Wrong XghjObject Version."
            return False
        self.round = int(list[2])
        self.sender_id = int(list[3])
        self.map_checksum = int(list[4])
        self.version = VERSION
        self.sender = XghjSender(int(list[6]))
        self.action = XghjAction(int(list[7]))
        if len(list)==9:
            self.content = list[8]
        else:
            self.content = ""
        return True
        
    def toString(self):
        buffer = "%d %d %d %d %d %d %s " % (self.round, self.sender_id, \
                 self.map_checksum, VERSION, \
                 self.sender.value, self.action.value, self.content)
        length = len(SIGNATURE) + 1 + 4 + 1 + len(buffer)
        buffer_2 = "%s %4d %s" % (SIGNATURE, length, buffer)
        return buffer_2
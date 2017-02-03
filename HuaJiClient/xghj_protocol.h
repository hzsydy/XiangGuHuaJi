// Created by sugar10w, 2017.2
//  tcp socket client by libboost

#ifndef __XGHJ_PROTOCOL__
#define __XGHJ_PROTOCOL__

#include <cstdio>
#include <cstring>

#include <iostream>
#include <string>
#include <sstream>
#include <queue>

#include <boost/asio.hpp>

namespace XGHJ_Client
{

int getNumber(const char* p);

struct XghjObject {


    enum XghjAction{
        OK, 
        GameOver,
        NewGame,
        NextRound, 
        Error,
        Invalid,
        Kill
    };
    
    enum XghjSender {
        Server,
        Player,
        Viewer,
        Unknown
    };

    // constructor
    XghjObject() 
        : round(0)
        , sender_id(-1)
        , map_checksum(0)
        , version(VERSION)
        , sender(Unknown)
        , action(Invalid)
        , content("Null") {}
    XghjObject(XghjSender sender_, XghjAction action_, std::string content_) 
        : round(0)
        , sender_id(-1)
        , map_checksum(0)
        , version(VERSION)
        , sender(sender_)
        , action(action_) 
        , content(content_)
    { }

    XghjObject(const std::string& s) {

        action = Invalid;

        std::string sign;
        int len;

        std::stringstream ss(s);

        ss >> sign >> len >> round >> sender_id >> map_checksum >> version >> 
            sender >> action;

        std::getline(ss, content);

        if (version != VERSION){
            std::cout << "[Warning] [XghjObject] get an object with a wrong version num: " << version << std::endl;
        }
    }

    std::string toString() const {
        
        char buffer[1024];
        char buffer_2[1024];

        sprintf(buffer, "%d %d %d %d %d %d %s ", 
            round, sender_id, map_checksum, VERSION, sender, action, content.c_str());
        
        int len = SIGNATURE.length() + 1 + 4 + 1 + strlen(buffer);

        sprintf(buffer_2, "%s %4d %s", SIGNATURE.c_str(), len, buffer);
        
        return buffer_2;
            
    }
    
    int round;
    int sender_id;
    int map_checksum;
    int version;
    int sender;
    int action;
    std::string content;


    static const std::string SIGNATURE;
    static const unsigned int HEADER_LENGTH;
    static const int VERSION;
};

class XghjProtocolSocket {

public:
    XghjProtocolSocket(boost::asio::io_service& io_service, const std::string& addr, int port);
    ~XghjProtocolSocket();

    bool send(std::string s);
    bool send(const XghjObject& obj);
    bool recv();
    XghjObject getObj();

    inline bool isValid() const { return _valid; }
    inline bool empty() const { return _obj_queue.empty(); }

private:

    std::queue<XghjObject> _obj_queue;

    std::string _buffer;
    bool _valid;
    boost::asio::ip::tcp::socket _socket;
};

}

#endif //__XGHJ_PROTOCOL__
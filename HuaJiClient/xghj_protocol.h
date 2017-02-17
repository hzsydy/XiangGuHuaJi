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

#include "definition.h"

namespace XGHJ_Client
{

int getNumber(const char* p);
int getNumber(char** p);

struct XghjObject {


    enum XghjSender {
        Server,
        Player,
        Viewer,
        Unknown
    };

    enum XghjAction{
        OK, 
        GameOver,
        NewGame,
        NextRound, 
        Error,
        Invalid,
        Kill,
        BidPrice,
        BidPosition
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

    bool getBidPrice(std::vector<TMoney>& bidPrice, int playerSize) {
        char buffer[1024];
        char *p = buffer;

        if (this->sender!=Server || this->action!=BidPrice) return false;

        sprintf(buffer, "%s", this->content.c_str());
        if (bidPrice.size()<playerSize) bidPrice.resize(playerSize);
        for (TId id=0; id<playerSize; ++id) bidPrice[id] = getNumber(&p);
        return true;
    }
    bool getBidPosUnaccessible(std::vector<TPosition>& posUnaccessible) {
        char buffer[1024];
        char* p = buffer;
        int n=0;

        if (this->sender!=Server || this->action!=BidPosition) return false;

        sprintf(buffer, "%s", this->content.c_str());
        n = getNumber(&p);
        for (int i=0; i<n; ++i) {
            TPosition pos;
            pos.x = getNumber(&p);
            pos.y = getNumber(&p);
            posUnaccessible.push_back(pos);
        }
        return true;
    }
    bool getBidPosition(std::vector<TPosition>& bidPosition, int playerSize) {
        char buffer[1024];       
        char *p = buffer;

        if (this->sender!=Server || this->action!=BidPosition) return false;

        sprintf(buffer, "%s", this->content.c_str());
        if (bidPosition.size()<playerSize) bidPosition.resize(playerSize);
        for (int id=0; id<playerSize; ++id) {
            TPosition pos;
            pos.x = getNumber(&p);
            pos.y = getNumber(&p);
            bidPosition[id] = pos;
        }
        return true;
    }
    bool getMilitaryCommand(std::vector<vector<TMilitaryCommand> >& MilitaryCommandMap,
		    std::vector<std::vector<TDiplomaticCommand> >& DiplomaticCommandMap,
		    vector<TPosition>& NewCapitalList,
            int playerSize) {
        
        if (this->sender!=Server || this->action!=NextRound) return false;

        std::stringstream ss;
        ss << this->content;

        MilitaryCommandMap.resize(playerSize);
        DiplomaticCommandMap.resize(playerSize);
        NewCapitalList.resize(playerSize);

        for (int player_id=0; player_id<playerSize; ++player_id) {
            std::vector<TDiplomaticCommand> player_dipt_command(playerSize);
            std::vector<TMilitaryCommand> player_military_command;
            TPosition player_new_capital;
            int t;

            for (int i=0; i<playerSize; ++i){
                ss >> t; player_dipt_command[i] = (TDiplomaticCommand)t;
            }

            while (true) {
                TMilitaryCommand tmc;
                ss >> t;
                if (t==-1) break; tmc.place.x = t;
                ss >> t; tmc.place.y = t;
                ss >> t; tmc.bomb_size = t;
                player_military_command.push_back(tmc);
            }

            ss >> t; player_new_capital.x = t;
            ss >> t; player_new_capital.y = t;

            DiplomaticCommandMap[player_id] = player_dipt_command;
            MilitaryCommandMap[player_id] = player_military_command;
            NewCapitalList[player_id] = player_new_capital;
        }
        return true;
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
    XghjObject getObj();

    inline bool isValid() const { return _valid; }
    inline bool empty() const { return _obj_queue.empty(); }

private:

    bool recv();

    std::queue<XghjObject> _obj_queue;

    std::string _buffer;
    bool _valid;
    boost::asio::ip::tcp::socket _socket;
};

}

#endif //__XGHJ_PROTOCOL__
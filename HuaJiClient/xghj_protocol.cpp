
#include "xghj_protocol.h"



namespace XGHJ_Client
{

using namespace boost::asio::ip;

const std::string XghjObject::SIGNATURE = "XGHJxghj";
const unsigned int XghjObject::HEADER_LENGTH = 24;
const int XghjObject::VERSION = 2;

int getNumber(const char* p){
    int t = 0;
    int sign = 1;
    
    while (!isdigit(*p) && *p!='-' && *p!='\0') ++p;
    if (*p=='-') {sign = -1; ++p; }

    while (isdigit(*p) &&  *p!='\0') {
        t = t*10 + *p - '0';
        ++p;
    }
    return t*sign;
}

int getNumber(char** p){
    int t = 0;
    int sign = 1;
    
    while (!isdigit(**p) && **p!='-' && **p!='\0') ++*p;
    if (**p=='-') {sign = -1; ++*p; }

    while (isdigit(**p) &&  **p!='\0') {
        t = t*10 + **p - '0';
        ++*p;
    }
    return t*sign;
}

XghjProtocolSocket::XghjProtocolSocket(boost::asio::io_service& io_service, const std::string& addr, int port)
    : _socket(io_service)
{
    try {
        tcp::endpoint ep(boost::asio::ip::address_v4::from_string(addr), port);
        _socket.connect(ep);
    }
    catch (std::exception& e) {
        _valid = false;
        std::cerr << e.what() << std::endl;
    }

    _valid = true;
}

XghjProtocolSocket::~XghjProtocolSocket() {
    _socket.close();
}

bool XghjProtocolSocket::send(std::string s) {
    if (!_valid) return false;

    std::cout << "[Info] sending: " << s << std::endl;

    try {
        _socket.write_some(boost::asio::buffer(s));
    }
    catch (std::exception& e) {
        _valid = false;
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

bool XghjProtocolSocket::send(const XghjObject& obj) {
    return send(obj.toString());
}

bool XghjProtocolSocket::recv() {

    if (!_obj_queue.empty()) return true;

    char buf[1024];
    size_t len = 0;
    size_t count = 0;

    if (!_valid) return false;

    while (_valid && count<1000) {
        ++count;

        try { 
            len = _socket.read_some(boost::asio::buffer(buf, 1024));
            buf[len] = '\0';
        }
        catch (std::exception& e) {
            _valid = false;
            std::cerr << e.what() << std::endl;
            return false;
        }

        if (len>0 || !empty()) break;
        _sleep(50);
        
    }

    _buffer = _buffer + buf;

    while (_buffer.length() > XghjObject::HEADER_LENGTH ) {

        unsigned int t = _buffer.find_first_of(XghjObject::SIGNATURE);
        if (t == std::string::npos) break;

        if (_buffer.length()> t + XghjObject::HEADER_LENGTH) {
            
            int length = getNumber(_buffer.c_str()+t+XghjObject::SIGNATURE.length());
            
            if (_buffer.length() < t+length) break;
            
            _obj_queue.push(XghjObject(
                _buffer.substr(t, length)));

            _buffer = _buffer.substr(t+length);

        }
        else break;
    }
    
    return !_obj_queue.empty();
}

XghjObject XghjProtocolSocket::getObj(){
    XghjObject obj;
    
    recv();
    if (_obj_queue.empty()) return obj;

    obj = _obj_queue.front();
    _obj_queue.pop();

    return obj;
}



}
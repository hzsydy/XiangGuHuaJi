/* XiangGuHuaJi 2016, net.hpp
 *
 */

#ifndef _XIANGGUHUAJI_HET_H__
#define _XIANGGUHUAJI_HET_H__

#include "../XiangGuHuaJi/definition.h"

#include <ctime>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
namespace XGHJ_Client
{
    std::string make_daytime_string()
    {
        using namespace std; // For time_t, time and ctime;
        time_t now = time(0);
        return ctime(&now);
    }

    //enable_shared_from_this 
    //这破玩意儿是和shared_from_this结合 
    //用来在类里面自己指代自己滑稽
    class tcp_connection
        : public boost::enable_shared_from_this<tcp_connection>
    {
    public:
        typedef boost::shared_ptr<tcp_connection> pointer;

        static pointer create(boost::asio::io_service& io_service)
        {
            return pointer(new tcp_connection(io_service));
        }

        tcp::socket& socket()
        {
            return socket_;
        }

        void start()
        {
            message_ = make_daytime_string();

            boost::asio::async_write(socket_, boost::asio::buffer(message_),
                boost::bind(&tcp_connection::handle_write, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        }

    private:
        tcp_connection(boost::asio::io_service& io_service)
            : socket_(io_service)
        {
        }

        void handle_write(const boost::system::error_code& /*error*/,
            size_t /*bytes_transferred*/)
        {
        }

        tcp::socket socket_;
        std::string message_;
    };

    class tcp_server
    {
    public:
        tcp_server(boost::asio::io_service& io_service)
            : acceptor_(io_service, tcp::endpoint(tcp::v4(), 23333))
        {
            start_accept();
        }

    private:
        void start_accept()
        {
            tcp_connection::pointer new_connection =
                tcp_connection::create(acceptor_.get_io_service());

            acceptor_.async_accept(new_connection->socket(),
                boost::bind(&tcp_server::handle_accept, this, new_connection,
                boost::asio::placeholders::error));
        }

        void handle_accept(tcp_connection::pointer new_connection,
            const boost::system::error_code& error)
        {
            if (!error)
            {
                new_connection->start();
            }

            start_accept();
        }

        tcp::acceptor acceptor_;
    };

}

#endif
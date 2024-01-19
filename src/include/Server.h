#pragma once

#include <cstdlib>
#include <iostream>
#include <memory>
#include <map>
#include <utility>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "Session.h"

using boost::asio::ip::tcp;

class Server
{
public:
  Server(boost::asio::io_context& io_context, short port)
    : ioContext(io_context),
      acceptor(io_context, tcp::endpoint(tcp::v4(), port)),
      socket(io_context)
  {
    doAccept();
  }

private:
  void doAccept()
  {
    acceptor.async_accept(socket,
        [this](boost::system::error_code ec)
        {
          if (!ec)
          {
            std::shared_ptr<Context> context = std::make_shared<Context>(ioContext);
            context->ui = std::make_shared<UserInterface>(std::move(socket));
            std::make_shared<Session>(context)->start();
          }

          doAccept();
        });
  }
  
  boost::asio::io_context& ioContext;
  tcp::acceptor acceptor;
  tcp::socket socket;

};
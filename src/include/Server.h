#pragma once

#include <cstdlib>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include "Session.h"

using boost::asio::ip::tcp;

class Server {
 public:
  Server(boost::asio::io_context& ctx, unsigned short port)
    : ioContext(ctx),
      acceptor(ctx, tcp::endpoint(tcp::v4(), port)),
      socket(ctx) {
    doAccept();
  }

 private:
  void doAccept() {
    acceptor.async_accept(socket,
      [this](boost::system::error_code ec) {
        if (!ec) {
          auto context = std::make_shared<Context>(ioContext);
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

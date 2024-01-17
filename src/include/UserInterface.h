#pragma once

#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

using boost::asio::ip::tcp;

class UserInterface: public std::enable_shared_from_this<UserInterface> {

public:
  
  UserInterface(tcp::socket socket)
    : socket(std::move(socket))
  {
  }
  
  template<typename HandlerType>
  void doRead(char* data, int maxLength, HandlerType onRead)
  {
    auto self(shared_from_this());
    socket.async_read_some(boost::asio::buffer(data, maxLength), onRead);
  }

  template<typename HandlerType>
  void doWrite(const std::string& msg, HandlerType onWrite)
  {
    auto self(shared_from_this());
    boost::asio::async_write(socket, boost::asio::buffer(msg.data(), msg.size()), onWrite);
  }

private:

  tcp::socket socket;

};

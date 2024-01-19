#pragma once

#include <functional>
#include <string>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "Utils.h"

using boost::asio::ip::tcp;

class UserInterface: public std::enable_shared_from_this<UserInterface> {

public:
  
  UserInterface(tcp::socket socket)
    : socket(std::move(socket))
  {
  }
  
  void askForNumber(std::string msg, std::function<void(int)> handler)
  {
    doWrite(msg, 
        [this, handler](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) readNumber(handler);
        }
    );
  }

  void askForString(std::string msg, std::function<void(std::string)> handler)
  {
    doWrite(msg, 
        [this, handler](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) readString(handler);
        }
    );
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

  void readNumber(std::function<void(int)> handler)
  {
    std::memset(buff, '\0', buffLength);
    doRead(buff, buffLength, 
      [this, handler](boost::system::error_code ec, std::size_t length)
      {
        if (!ec) onReadNumber(handler);
      }
    );

  }

  void onReadNumber(std::function<void(int)> handler) 
  {
    try {
      std::string buffStr(buff);
      auto number = std::stoi(buffStr);
      if (number > 0) {
        handler(number);
        return;
      }
    } catch(...) {}
    readNumber(handler);
  }

  void readString(std::function<void(std::string&)> handler)
  {
    std::memset(buff, '\0', buffLength);
    doRead(buff, buffLength, 
      [this, handler](boost::system::error_code ec, std::size_t length)
      {
        if (!ec) onReadString(handler);
      }
    );
  }

  void onReadString(std::function<void(std::string&)> handler) 
  {
    try {
      std::string buffStr(buff);
      trim(buffStr);
      if(!buffStr.empty()) {
        handler(buffStr);
        return ;
      }
    } catch(...) {}
    readString(handler);
  }

  tcp::socket socket;
  enum { buffLength = 1024 };
  char buff[buffLength];
};

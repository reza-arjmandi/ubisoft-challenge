#pragma once

#include <functional>
#include <utility>
#include <string>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "Utils.h"

using boost::asio::ip::tcp;

class UserInterface: public std::enable_shared_from_this<UserInterface> {
 public:
  explicit UserInterface(tcp::socket socket)
    : socket(std::move(socket)) {
    }

  void askForNumber(std::string msg, std::function<void(int, bool&)> handler) {
    doWrite(msg,
      [this, handler](boost::system::error_code ec, std::size_t length) {
        if (!ec) readNumber(handler);
      });
  }

  void askForString(std::string msg, std::function<void(std::string)> handler) {
    doWrite(msg,
      [this, handler](boost::system::error_code ec, std::size_t length) {
        if (!ec) readString(handler);
      });
  }

  template<typename HandlerType>
  void doWrite(const std::string& msg, HandlerType onWrite) {
    boost::asio::async_write(socket, boost::asio::buffer(msg.data(), msg.size()), onWrite);
  }

 private:

   void doRead(std::function<void(std::string)> onRead) {
    boost::asio::async_read_until(socket, buff, '\n', [onRead, this](boost::system::error_code ec, std::size_t length) {
      if (ec) return;
      std::istream is(&buff);
      std::string s;
      is >> s;
      onRead(s);
    });
  }

  void readNumber(std::function<void(int, bool&)> handler) {
    doRead(
      [this, handler](std::string result) {
        onReadNumber(result, handler);
      });
  }

  void onReadNumber(std::string& buffStr, std::function<void(int, bool&)> handler) {
    try {
      auto number = std::stoi(buffStr);
      if (number > 0) {
        bool reTake = false;
        handler(number, reTake);
        if (!reTake) return;
      }
    } catch(...) {}
    readNumber(handler);
  }

  void readString(std::function<void(std::string&)> handler) {
    doRead(
      [this, handler](std::string result) {
        onReadString(result, handler);
      });
  }

  void onReadString(std::string& buffStr, std::function<void(std::string&)> handler) {
    try {
      trim(buffStr);
      if (!buffStr.empty()) {
        handler(buffStr);
        return;
      }
    } catch(...) {}
    readString(handler);
  }

  tcp::socket socket;
  boost::asio::streambuf buff;
};

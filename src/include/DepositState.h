#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "State.h"

using boost::asio::ip::tcp;

class DepositState: public State {

public:

  void Enter(std::shared_ptr<StateManager> manager) override
  {
  }

  void Process(std::shared_ptr<StateManager> mgr, std::shared_ptr<Context> ctx) override
  {
    manager = mgr;
    context = ctx;
    context->ui->doWrite(GetDepositStateContent(context->user->name),
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) onWrite();
        }
    );
  }
  
  void Exit(std::shared_ptr<StateManager> manager) override
  {

  }

  std::string GetDepositStateContent(const std::string& username) {
    std::string content = "1- Deposit funds\r\n";
    content += "2- Deposit items\r\n";
    content += "3- Back\r\n";
    content += "Plaese enter one of the above number: ";
    return content;
  }

  void onWrite()
  {
    context->ui->doRead(menuItem, maxMenuItemLength, 
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) onRead();
        }
    );
  }

  void onRead()
  {
    std::string selected(menuItem);
    try {
      auto num = std::stoi(selected);
      if(num > 0 && num < 7) {
        std::cout << num << std::endl;
        return;
      }
      onWrite();

    } catch(...) {
        onWrite();
    }
  }


private:
  std::shared_ptr<StateManager> manager;
  std::shared_ptr<Context> context;
  enum { maxMenuItemLength = 1024 };
  char menuItem[maxMenuItemLength];
};
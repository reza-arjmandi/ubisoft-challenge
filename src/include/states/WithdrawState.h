#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "State.h"

using boost::asio::ip::tcp;

class WithdrawState: public State {

public:

  void Enter(std::shared_ptr<StateManager> manager) override
  {
  }

  void Process(std::shared_ptr<StateManager> mgr, std::shared_ptr<Context> ctx) override
  {
    manager = mgr;
    context = ctx;
    context->ui->doWrite(GetWithdrawStateContent(context->user->name),
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) onWrite();
        }
    );
  }
  
  void Exit(std::shared_ptr<StateManager> manager) override
  {

  }

  std::string GetWithdrawStateContent(const std::string& username) {
    std::string content = "=======================================================\r\n";
    content += "URI: /app/dashboard/withdraw\r\n";
    content += "-------------------------------------------------------\r\n";
    content += "1. Withdraw funds\r\n";
    content += "2. Withdraw items\r\n";
    content += "3. Back\r\n";
    content += "Choose a menu item by typing its number: ";
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
      if(num > 0 && num < 4) {
        navigateTo(num);
        return;
      }
      onWrite();

    } catch(...) {
        onWrite();
    }
  }

  void navigateTo(int menuIndex)
  {
    std::shared_ptr<State> nextState = nullptr;
    switch(menuIndex)
    {
      case 1:
        nextState = context->states[StateNames::WithdrawFundState];
        break;
      case 2: 
        nextState = context->states[StateNames::WithdrawItemState];
        break;
      case 3:
        nextState = context->states[StateNames::DashboardState];
        break;
      default:
        return;
    }
    manager->SetState(nextState, context);
  }


private:
  std::shared_ptr<StateManager> manager;
  std::shared_ptr<Context> context;
  enum { maxMenuItemLength = 1024 };
  char menuItem[maxMenuItemLength];
};
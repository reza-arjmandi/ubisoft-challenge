#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "State.h"
#include "Utils.h"

using boost::asio::ip::tcp;

class DepositItemState: public State {

public:

  void Enter(std::shared_ptr<StateManager> manager) override
  {
  }

  void Process(std::shared_ptr<StateManager> mgr, std::shared_ptr<Context> ctx) override
  {
    manager = mgr;
    context = ctx;
    context->ui->doWrite(GetDepositItemStateContent(),
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) onWrite();
        }
    );
  }
  
  void Exit(std::shared_ptr<StateManager> manager) override
  {

  }

  std::string GetDepositItemStateContent() {
    std::string content = "=======================================================\r\n";
    content += "URI: /app/dashboard/deposit/item\r\n";
    content += "-------------------------------------------------------\r\n";
    content += "Enter the item name: ";
    return content;
  }

  void onWrite()
  {
    context->ui->doRead(itemName, itemNameLength, 
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) onRead();
        }
    );
  }

  void onRead()
  {
    try {
      std::string itemStr(itemName);
      trim(itemStr);
      if(!itemStr.empty()) {
        context->user->items.push_back(itemStr);
        std::string message = "Adding item was successful.\r\n";
        context->ui->doWrite(message,
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) manager->SetState(context->states[StateNames::DepositState], context);
        }
    );
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
  enum { itemNameLength = 1024 };
  char itemName[itemNameLength];
};
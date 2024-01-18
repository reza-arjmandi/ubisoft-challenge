#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "State.h"
#include "Utils.h"

using boost::asio::ip::tcp;

class WithdrawItemState: public State {

public:

  void Enter(std::shared_ptr<StateManager> manager) override
  {
  }

  void Process(std::shared_ptr<StateManager> mgr, std::shared_ptr<Context> ctx) override
  {
    manager = mgr;
    context = ctx;
    context->ui->doWrite(GetWithdrawItemStateContent(),
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) onWrite();
        }
    );
  }
  
  void Exit(std::shared_ptr<StateManager> manager) override
  {

  }

  std::string GetWithdrawItemStateContent() {
    std::string content = "=======================================================\r\n";
    content += "URI: /app/dashboard/withdraw/item\r\n";
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
      if (!itemStr.empty()) {
        auto found = std::find(context->user->items.begin(), context->user->items.end(), itemStr);
        bool removed = false;
        if (found != std::end(context->user->items)) {
            context->user->items.erase(found);
            removed = true ;
        }
        std::string message = removed ? "Removing item was successful.\r\n" : "The item not found.\r\n";
        context->ui->doWrite(message,
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) manager->SetState(context->states[StateNames::WithdrawState], context);
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
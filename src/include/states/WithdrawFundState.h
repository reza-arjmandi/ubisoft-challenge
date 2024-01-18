#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "State.h"

using boost::asio::ip::tcp;

class WithdrawFundState: public State {

public:

  void Enter(std::shared_ptr<StateManager> manager) override
  {
  }

  void Process(std::shared_ptr<StateManager> mgr, std::shared_ptr<Context> ctx) override
  {
    manager = mgr;
    context = ctx;
    context->ui->doWrite(GetWithdrawFundStateContent(),
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) onWrite();
        }
    );
  }
  
  void Exit(std::shared_ptr<StateManager> manager) override
  {

  }

  std::string GetWithdrawFundStateContent() {
    std::string content = "=======================================================\r\n";
    content += "URI: /app/dashboard/withdraw/fund\r\n";
    content += "-------------------------------------------------------\r\n";
    content += "Enter the withdraw amount: ";
    return content;
  }

  void onWrite()
  {
    std::memset(withdrawStr, '\0', withdrawStrLength);
    context->ui->doRead(withdrawStr, withdrawStrLength, 
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) onRead();
        }
    );
  }

  void onRead()
  {
    try {
      auto withdrawAmount = std::stoi(std::string(withdrawStr));
      if(withdrawAmount > 0) {
        auto balance = context->user->balance;
        balance -= withdrawAmount;
        User::Collection.save();
        bool done = false;
        if (balance >= 0 ) {
          context->user->balance = balance;
          done = true;
        }
        std::string message = done ? "Withdraw was successful.\r\n" : "You don't have enough balance.\r\n";
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
  enum { withdrawStrLength = 1024 };
  char withdrawStr[withdrawStrLength];
};
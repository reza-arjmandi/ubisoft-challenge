#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "State.h"

using boost::asio::ip::tcp;

class DepositFundState: public State {

public:

  void Enter(std::shared_ptr<StateManager> manager) override
  {
  }

  void Process(std::shared_ptr<StateManager> mgr, std::shared_ptr<Context> ctx) override
  {
    manager = mgr;
    context = ctx;
    context->ui->doWrite(GetDepositFundStateContent(),
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) onWrite();
        }
    );
  }
  
  void Exit(std::shared_ptr<StateManager> manager) override
  {

  }

  std::string GetDepositFundStateContent() {
    std::string content = "=======================================================\r\n";
    content += "URI: /app/dashboard/deposit/fund\r\n";
    content += "-------------------------------------------------------\r\n";
    content += "Enter the deposit amount: ";
    return content;
  }

  void onWrite()
  {
    context->ui->doRead(fundStr, fundStrLength, 
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) onRead();
        }
    );
  }

  void onRead()
  {
    try {
      auto depositAmount = std::stoi(std::string(fundStr));
      if(depositAmount > 0) {
        context->user->balance += depositAmount;
        std::string message = "Deposit was successful.\r\n";
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
  enum { fundStrLength = 1024 };
  char fundStr[fundStrLength];
};
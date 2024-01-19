#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "Page.h"
#include "Utils.h"

using boost::asio::ip::tcp;

class DepositFund: public Page {

public:

  void render(std::shared_ptr<PageManager> mgr, std::shared_ptr<Context> ctx) override
  {
    manager = mgr;
    context = ctx;
    auto content = createUiContent(getUri(), "Enter the deposit amount: ");
    context->ui->askForNumber(content,
        [this](int depositAmount)
        {
          onReadDeposit(depositAmount);
        }
    );
  }

  std::string getUri() const override 
  {
    return PageURIs::DepositFund;
  }
  
private:

  void onReadDeposit(int depositAmount)
  {
    std::string result;
    if(depositAmount > 0) {
      context->user->balance += depositAmount;
      User::Collection.save();
      result = "Deposit was successful.\r\n";
      
    } else {
      result = "Deposit was failed, negative number.\r\n";
    }
    context->ui->doWrite(result,
      [this](boost::system::error_code ec, std::size_t length)
      {
        if (!ec) manager->navigate(PageURIs::Deposit, context);
      }
    );
  }

  std::shared_ptr<PageManager> manager;
  std::shared_ptr<Context> context;
};
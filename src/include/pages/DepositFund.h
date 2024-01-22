#pragma once

#include <vector>
#include <string>
#include <boost/asio.hpp>
#include "Page.h"
#include "Utils.h"

class DepositFund: public Page 
{

public:

  void render(std::shared_ptr<PageManager> mgr, std::shared_ptr<Context> ctx) override
  {
    manager = mgr;
    context = ctx;
    auto content = createUiContent(getUri(), "Enter the deposit amount: ");
    context->ui->askForNumber(content,
      [this](int depositAmount, bool& reTake)
      {
        if (depositAmount <= 0) 
        {
          reTake = true;
          return;
        }
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
    context->user->balance += depositAmount;
    User::Collection.save();
    std::string result = "Deposit was successful.\r\n";
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

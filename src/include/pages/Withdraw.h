#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "Page.h"
#include "Utils.h"

using boost::asio::ip::tcp;

class Withdraw: public Page {

public:

  void render(std::shared_ptr<PageManager> mgr, std::shared_ptr<Context> ctx) override
  {
    manager = mgr;
    context = ctx;
    context->ui->askForNumber(GetWithdrawPageContent(),
        [this](int number, bool& reTake)
        {
          switch(number)
          {
            case 1:
              manager->navigate(PageURIs::WithdrawFund, context);
              break;
            case 2:
              manager->navigate(PageURIs::WithdrawItem, context);
              break;
            case 3:
              manager->navigate(PageURIs::Dashboard, context);
              break;
            default:
              reTake = true;
              break;
          }
        }
    );
  }
  
  std::string getUri() const override 
  {
    return PageURIs::Withdraw;
  }

private:

  std::string GetWithdrawPageContent() {
    std::string content = "1. Withdraw funds\r\n";
    content += "2. Withdraw items\r\n";
    content += "3. Back\r\n";
    content += "Choose a menu item by typing its number: ";
    return createUiContent(getUri(), content);
  }

  std::shared_ptr<PageManager> manager;
  std::shared_ptr<Context> context;
};
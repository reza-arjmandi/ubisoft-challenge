#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "Page.h"
#include "Utils.h"

using boost::asio::ip::tcp;

class Deposit: public Page {

public:

  void render(std::shared_ptr<PageManager> mgr, std::shared_ptr<Context> ctx) override
  {
    manager = mgr;
    context = ctx;
    context->ui->askForNumber(GetDepositPageContent(),
        [this](int number)
        {
          switch(number)
          {
            case 1:
              manager->navigate(PageURIs::DepositFund, context);
              break;
            case 2:
              manager->navigate(PageURIs::DepositItem, context);
              break;
            case 3:
              manager->navigate(PageURIs::Dashboard, context);
              break;
            default:
              manager->navigate(PageURIs::Deposit, context);
              break;
          }
        }
    );
  }
  
  std::string getUri() const override 
  {
    return PageURIs::Deposit;
  }
  
private:

  std::string GetDepositPageContent() 
  {
    std::string content = "1. Deposit funds\r\n";
    content += "2. Deposit items\r\n";
    content += "3. Back\r\n";
    content += "Choose a menu item by typing its number: ";
    return createUiContent(getUri(), content);
  }

  std::shared_ptr<PageManager> manager;
  std::shared_ptr<Context> context;
};
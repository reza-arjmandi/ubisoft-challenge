#pragma once

#include <vector>
#include <string>
#include <boost/asio.hpp>
#include "Page.h"
#include "Utils.h"
#include "Context.h"
#include "PageManager.h"

class Dashboard: public Page 
{

public:

  void render(std::shared_ptr<PageManager> mgr, std::shared_ptr<Context> ctx) override
  {
    manager = mgr;
    context = ctx;
    context->ui->askForNumber(GetDashboardContent(),
      [this](int number, bool& reTake)
      {
        switch(number)
        {
          case 1:
            manager->navigate(PageURIs::Deposit, context);
            break;
          case 2:
            manager->navigate(PageURIs::Withdraw, context);
            break;
          case 3:
            manager->navigate(PageURIs::Sell, context);
            break;
          case 4:
            manager->navigate(PageURIs::Buy, context);
            break;
          case 5:
            manager->navigate(PageURIs::Display, context);
            break;
          case 6:
            manager->navigate(PageURIs::Auth, context);
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
    return PageURIs::Dashboard;
  }

private:

  std::string GetDashboardContent() 
  {
    std::string content = "Welcome *** " + context->user->name + " ***\r\n";
    content += "1. Deposit\r\n";
    content += "2. Withdraw\r\n";
    content += "3. Sell\r\n";
    content += "4. Buy\r\n";
    content += "5. Display\r\n";
    content += "6. Log out\r\n";
    content += "Choose a menu item by typing its number: ";
    return createUiContent(getUri(), content);
  }

  std::shared_ptr<PageManager> manager;
  std::shared_ptr<Context> context;

};

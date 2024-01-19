#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "Page.h"
#include "Utils.h"

using boost::asio::ip::tcp;

class WithdrawItem: public Page {

public:

  void render(std::shared_ptr<PageManager> mgr, std::shared_ptr<Context> ctx) override
  {
    manager = mgr;
    context = ctx;
    context->ui->askForString(createUiContent(getUri(), "Enter the item name: "),
        [this](std::string itemName)
        {
          onReadItem(itemName);
        }
    );
  }
  
  std::string getUri() const override 
  {
    return PageURIs::WithdrawItem;
  }

private:

  void onReadItem(const std::string& name)
  {
    std::string result;
    try {
      auto found = std::find(context->user->items.begin(), context->user->items.end(), name);
      bool removed = false;
      if (found != std::end(context->user->items)) {
        context->user->items.erase(found);
        removed = true ;
      }
      User::Collection.save();
      result = removed ? "Withdrawing item was successful.\r\n" : "The item not found.\r\n";
    } catch(...) {
      result = "Withdrawing item was failed.\r\n";
    }
    context->ui->doWrite(result,
      [this](boost::system::error_code ec, std::size_t length)
      {
        if (!ec) manager->navigate(PageURIs::Withdraw, context);
      }
    );
  }

  std::shared_ptr<PageManager> manager;
  std::shared_ptr<Context> context;
};
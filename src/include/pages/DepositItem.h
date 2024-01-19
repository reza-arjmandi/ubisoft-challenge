#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "Page.h"
#include "Utils.h"

using boost::asio::ip::tcp;

class DepositItem: public Page {

public:

  void render(std::shared_ptr<PageManager> mgr, std::shared_ptr<Context> ctx) override
  {
    manager = mgr;
    context = ctx;
    context->ui->askForString(createUiContent(getUri(), "Enter the item name: "),
        [this](std::string& itemName)
        {
          onReadItem(itemName);
        }
    );
  }
  
  std::string getUri() const override 
  {
    return PageURIs::DepositItem;
  }

private:

  void onReadItem(const std::string& itemName)
  {
    std::string result;
    try {
      context->user->items.push_back(itemName);
      User::Collection.save();
      result = "Adding item was successful.\r\n";
    } catch(...) {
      result = "Adding item was failed.\r\n";
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
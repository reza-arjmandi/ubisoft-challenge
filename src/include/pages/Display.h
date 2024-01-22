#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "Page.h"
#include "Utils.h"
#include "Context.h"
#include "PageManager.h"

using boost::asio::ip::tcp;

class Display: public Page {

public:

  void render(std::shared_ptr<PageManager> mgr, std::shared_ptr<Context> ctx) override
  {
    manager = mgr;
    context = ctx;
    context->ui->doWrite(GetContent(),
        [this](boost::system::error_code ec, std::size_t length)
        {
          if(ec) return;
          manager->navigate(PageURIs::Dashboard, context);
        }
    );
  }
  
  std::string getUri() const override 
  {
    return PageURIs::Display;
  }

private:

  std::string GetContent() 
  {
    std::string content = "The " + context->user->name + " data:\r\n";
    content += "*. Balance: " + std::to_string(context->user->balance) + "\r\n";
    content += "*. Items: \r\n";
    int counter = 1;
    for(const auto& item : context->user->items) {
      content += std::to_string(counter++);
      content += ". " + item + "\r\n";
    }
    return createUiContent(getUri(), content);
  }

  std::shared_ptr<PageManager> manager;
  std::shared_ptr<Context> context;
};
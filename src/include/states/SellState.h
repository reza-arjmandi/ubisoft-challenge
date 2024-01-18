#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <chrono>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <boost/asio.hpp>
#include "State.h"
#include "../Models.h"

using boost::asio::ip::tcp;

class SellState: public State {

public:

  void Enter(std::shared_ptr<StateManager> manager) override
  {
  }

  void Process(std::shared_ptr<StateManager> mgr, std::shared_ptr<Context> ctx) override
  {
    manager = mgr;
    context = ctx;
    std::string content = "=======================================================\r\n";
    content += "URI: /app/dashboard/sell\r\n";
    content += "-------------------------------------------------------\r\n";
    bool canProceed = false;
    if (context->user->items.size() == 0) {
      content += "No item found to sell, please deposit an item.\r\n";
    } else if (context->user->balance < fee) {
      content += "Balance is not enough, please deposit fund.\r\n";
    } else {
      GetItems(content);
      canProceed = true;
    }
    
    context->ui->doWrite(content,
        [this, canProceed](boost::system::error_code ec, std::size_t length)
        {
          if (ec) return;
          if(canProceed) onContentWrite();
          else manager->SetState(context->states[StateNames::DashboardState], context);
        }
    );
  }
  
  void Exit(std::shared_ptr<StateManager> manager) override
  {

  }

  void GetItems(std::string& content) {
    int counter = 1;
    content += "List of your items:\r\n";
    for(const auto& item : context->user->items) {
      content += std::to_string(counter);
      content += ". " + item + "\r\n";
    }
    content += "Choose an item to sell: ";
  }

  void onContentWrite()
  {
    std::memset(buffItem, '\0', buffLength);
    context->ui->doRead(buffItem, buffLength, 
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) onItemRead();
        }
    );
  }

  void onItemRead()
  {
    try {
      auto itemNumber = std::stoi(std::string(buffItem));
      if(itemNumber > 0 && itemNumber <= context->user->items.size()) {
        itemIndex = itemNumber -1;
        context->ui->doWrite("Enter the price amount: ",
            [this](boost::system::error_code ec, std::size_t length)
            {
            if (!ec) onWritePrice();
            }
        );
        return;
      }
      onContentWrite();
    } catch(...) {
      onContentWrite();
    }
  }

  void onWritePrice() {
    std::memset(buffPrice, '\0', buffLength);
    context->ui->doRead(buffPrice, buffLength, 
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) onPriceRead();
        }
    );
  }

  void onPriceRead() {
    try {
      auto price = std::stoi(std::string(buffPrice));
      if(price > 0) {
        //TODO TRANSACTION
        auto saleItem = SaleItem::Collection.create([&]() {
            auto result = std::make_shared<SaleItem>();
            result->item = context->user->items[itemIndex];
            result->seller = context->user->name;
            result->state = SaleState::avaiableForSale;
            auto currentTime = std::chrono::system_clock::now();
            result->createdAt = std::chrono::system_clock::to_time_t(currentTime);
            return result;
        });

        auto newItems = context->user->items;
        newItems.erase(newItems.begin() + itemIndex);
        auto newBalance = context->user->balance - fee;
        context->user->items = newItems;
        context->user->balance = newBalance;
        User::Collection.save();
        
        boost::asio::deadline_timer timer(context->ioContext);
        timer.expires_from_now(fiveMinutes);
        timer.async_wait([saleItem](const boost::system::error_code& ec){
            if (ec) return;

            if (saleItem->state == SaleState::avaiableForSale) {
                saleItem->state = SaleState::expired;
                SaleItem::Collection.save();
            }
        });

        context->ui->doWrite("The item has put up for sale successfully.\r\n",
            [this](boost::system::error_code ec, std::size_t length)
            {
              if (!ec) manager->SetState(context->states[StateNames::DashboardState], context);
            }
        );
        return;
      }
      onWritePrice();
    } catch(...) {
      onWritePrice();
    }
  }

private:
  std::shared_ptr<StateManager> manager;
  std::shared_ptr<Context> context;
  int fee = 2;
  enum { buffLength = 1024 };
  char buffItem[buffLength];
  char buffPrice[buffLength];
  int itemIndex = 0;
  boost::posix_time::minutes fiveMinutes{5};
};
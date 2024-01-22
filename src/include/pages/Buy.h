#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <chrono>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <boost/asio.hpp>
#include "Page.h"
#include "Models.h"
#include "Utils.h"

using boost::asio::ip::tcp;

class Buy: public Page 
{

public:

  void render(std::shared_ptr<PageManager> mgr, std::shared_ptr<Context> ctx) override
  {
    manager = mgr;
    context = ctx;
    auto saleItems = GetSaleItems();
    context->ui->askForNumber(CreateContent(saleItems),
      [this, saleItems](int itemNumber, bool& reTake)
      {
        if(itemNumber > 0 && itemNumber <= saleItems.size()) 
        {
            onItemRead(itemNumber, saleItems);
        }
        else 
        {
            reTake = true;
        }
      }
    );
  }
  
  std::string getUri() const override 
  {
    return PageURIs::Buy;
  }

private:

  std::string CreateContent(std::vector<std::shared_ptr<SaleItem>> saleItems) 
  {
    std::string content = "";
    int counter = 1;
    content += "List of items on sale:\r\n";
    for(const auto& elem : saleItems) 
    {
      content += std::to_string(counter);
      content += ". Item: " + elem->item + ",";
      content += ". Pricerice: " + elem->price + ",";
      content += ". Seller: " + elem->seller + "\r\n";
    }
    content += "Choose an item to buy: ";
    return createUiContent(getUri(), content);
  }

  std::vector<std::shared_ptr<SaleItem>> GetSaleItems() 
  {
    std::vector<std::shared_ptr<SaleItem>> result;
    for(const auto& elem : SaleItem::Collection.all()) 
    {
      if (elem->seller == context->user->name || elem->state == SaleState::expired) continue;
      result.push_back(elem);
    }
    return result;
  }

  void onItemRead(int itemNumber, std::vector<std::shared_ptr<SaleItem>> saleItems)
  {
    try {
      int itemIndex = itemNumber - 1;
      if (context->user->balance < saleItems[itemIndex]->price) {

      }
        context->ui->askForNumber("Enter the price amount: ",
            [this](int price)
            {
              onPriceRead(price);
            }
        );
    } catch(...) {}
    manager->navigate(PageURIs::Buy, context);
  }

  void onPriceRead(int price) {
    try {
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
              if (!ec) manager->navigate(PageURIs::Dashboard, context);
            }
        );
        return;
      }
    } catch(...) {}
    manager->navigate(PageURIs::Sell, context);
  }

  std::shared_ptr<PageManager> manager;
  std::shared_ptr<Context> context;
};
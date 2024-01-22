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
    std::string content = "List of items on sale:\r\n";
    int counter = 1;
    for(const auto& elem : saleItems) 
    {
      content += std::to_string(counter);
      content += ". Item: " + elem->item + ", ";
      content += "Price: " + std::to_string(elem->price) + ", ";
      content += "Seller: " + elem->seller->name + "\r\n";
    }
    content += "Choose an item to buy: ";
    return createUiContent(getUri(), content);
  }

  std::vector<std::shared_ptr<SaleItem>> GetSaleItems() 
  {
    std::vector<std::shared_ptr<SaleItem>> result;
    for(const auto& elem : SaleItem::Collection.all()) 
    {
      if (elem->seller->name == context->user->name || elem->state == SaleState::expired) continue;
      result.push_back(elem);
    }
    return result;
  }

  void onItemRead(int itemNumber, std::vector<std::shared_ptr<SaleItem>> saleItems)
  {
    int itemIndex = itemNumber - 1;
    std::string result = "";
    if (context->user->balance < saleItems[itemIndex]->price) {
        result = "Can't buy, fund is not enough.";
    } else {
        auto sellItem = saleItems[itemIndex];
        sellItem->state = SaleState::soldOut;
        sellItem->buyer = context->user;
        sellItem->buyer->balance -= sellItem->price;
        sellItem->seller->balance += sellItem->price;
        auto sellItemName = sellItem->item;
        sellItem->buyer->items.push_back(sellItemName);
        auto sellerItems = saleItems[itemIndex]->seller->items;
        auto found = std::find(sellerItems.begin(), sellerItems.end(), sellItemName);
        if (found != sellerItems.end()) {
          sellerItems.erase(found);
        }
        User::Collection.save();
        SaleItem::Collection.save();
        result = "Buying is successfull.\r\n";
    }
    context->ui->doWrite(result,
        [this](boost::system::error_code ec, std::size_t length)
        {
          if(ec) return;
          manager->navigate(PageURIs::Dashboard, context);
        }
    );
  }

  std::shared_ptr<PageManager> manager;
  std::shared_ptr<Context> context;
};
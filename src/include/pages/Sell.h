#pragma once

#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <boost/asio.hpp>
#include "Page.h"
#include "Models.h"
#include "Utils.h"

class Sell: public Page {
 public:
  void render(std::shared_ptr<PageManager> mgr, std::shared_ptr<Context> ctx) override {
    manager = mgr;
    context = ctx;
    auto failHandler = [this](boost::system::error_code ec, std::size_t length) {
        if (ec) return;
        manager->navigate(PageURIs::Dashboard, context);
      };
    if (context->user->items.size() == 0) {
      context->ui->doWrite(createUiContent(getUri(),
        "No item found to sell, please deposit an item.\r\n"), failHandler);
    } else if (context->user->balance < fee) {
      context->ui->doWrite(createUiContent(getUri(), "Balance is not enough, please deposit fund.\r\n"), failHandler);
    } else {
      context->ui->askForNumber(GetItems(),
        [this](int itemNumber, bool& reTake) {
          auto itemsSize = context->user->items.size();
          if (itemNumber > 0 && itemNumber <= itemsSize) {
            onItemRead(itemNumber);
          } else if (itemNumber == itemsSize + 1) {
            manager->navigate(PageURIs::Dashboard, context);
          } else {
            reTake = true;
          }
        });
    }
  }

  std::string getUri() const override {
    return PageURIs::Sell;
  }

 private:
  std::string GetItems() {
    std::string content = "";
    int counter = 1;
    content += "List of your items:\r\n";
    for (const auto& item : context->user->items) {
      content += std::to_string(counter++);
      content += ". " + item + "\r\n";
    }
    content += std::to_string(counter++) + ". Back\r\n";
    content += "Choose an item to sell: ";
    return createUiContent(getUri(), content);
  }

  void onItemRead(int itemNumber) {
    itemIndex = itemNumber -1;
    context->ui->askForNumber("Enter the price amount: ",
      [this](int price, bool& reTake) {
        if (price <= 0) {
          reTake = true;
          return;
        }
        onPriceRead(price);
      });
  }

  void onPriceRead(int price) {
    auto result = sell(price);
    std::string message = result ? "The item has put up for sale successfully.\r\n" : "Sell is failed.\r\n";
    context->ui->doWrite(message,
      [this](boost::system::error_code ec, std::size_t length) {
        if (!ec) manager->navigate(PageURIs::Dashboard, context);
      });
  }

  bool sell(int price) {
    bool result = true;
    try {
      auto expireTimer = std::make_shared<boost::asio::deadline_timer>(context->ioContext);
      expireTimer->expires_from_now(fiveMinutes);

      auto saleItem = SaleItem::Collection.create([&]() {
        auto result = std::make_shared<SaleItem>();
        result->item = context->user->items[itemIndex];
        result->seller = context->user;
        result->price = price;
        result->state = SaleState::avaiableForSale;
        auto currentTime = std::chrono::system_clock::now();
        result->createdAt = std::chrono::system_clock::to_time_t(currentTime);
        return result;
      }, false);

      expireTimer->async_wait([saleItem, expireTimer, this](const boost::system::error_code& ec){
        if (ec) return;
        if (saleItem->state == SaleState::avaiableForSale) {
          saleItem->state = SaleState::expired;
          SaleItem::Collection.save();
          context->user->items.push_back(saleItem->item);
          User::Collection.save();
        }
      });

      auto newItems = context->user->items;
      newItems.erase(newItems.begin() + itemIndex);
      auto newBalance = context->user->balance - fee;

      context->user->items = newItems;
      context->user->balance = newBalance;

      // TODO(TRANSACTION)
      User::Collection.save();
      SaleItem::Collection.save();
    }
    catch(...) {
      result = false;
    }
    return result;
  }

  std::shared_ptr<PageManager> manager;
  std::shared_ptr<Context> context;
  int fee = 2;
  int itemIndex = 0;
  boost::posix_time::minutes fiveMinutes{5};
};

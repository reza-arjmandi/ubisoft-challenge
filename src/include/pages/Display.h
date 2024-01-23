#pragma once

#include <vector>
#include <string>
#include <memory>
#include <boost/asio.hpp>
#include "Page.h"
#include "Utils.h"
#include "Context.h"
#include "PageManager.h"

class Display: public Page {
 public:
  void render(std::shared_ptr<PageManager> mgr, std::shared_ptr<Context> ctx) override {
    manager = mgr;
    context = ctx;
    context->ui->doWrite(GetContent(),
      [this](boost::system::error_code ec, std::size_t length) {
        if (ec) return;
        manager->navigate(PageURIs::Dashboard, context);
      });
  }

  std::string getUri() const override {
    return PageURIs::Display;
  }

 private:
  std::string GetContent() {
    std::string content = "The " + context->user->name + " data:\r\n";
    content += "*. Balance: " + std::to_string(context->user->balance) + "\r\n";
    content += "*. Items: \r\n";
    int counter = 1;
    for (const auto& item : context->user->items) {
      content += std::to_string(counter++);
      content += ". " + item + "\r\n";
    }


    auto saleItems = SaleItem::Collection.all();
    std::vector<std::shared_ptr<SaleItem>> bought;
    std::vector<std::shared_ptr<SaleItem>> onSale;
    std::vector<std::shared_ptr<SaleItem>> sold;
    std::vector<std::shared_ptr<SaleItem>> expiredSold;
    for (auto& saleItem : saleItems) {
      if (saleItem->buyer && saleItem->buyer->name == context->user->name) bought.push_back(saleItem);
      if (saleItem->seller->name == context->user->name) {
        switch (saleItem->state) {
        case SaleState::soldOut:
          sold.push_back(saleItem);
          break;
        case SaleState::expired:
          expiredSold.push_back(saleItem);
          break;
        case SaleState::avaiableForSale:
          onSale.push_back(saleItem);
          break;
        default:
          break;
        }
      }
    }

    if (bought.size() > 0) {
      content += "*. Bought:\r\n";
      counter = 1;
      for (auto& elem : bought) {
        content += std::to_string(counter++);
        content += ". Item: " + elem->item;
        content += ", Price: " + std::to_string(elem->price);
        content += ", Seller: " + elem->seller->name + "\r\n";
      }
    }

    if (sold.size() > 0) {
      content += "*. Sold:\r\n";
      counter = 1;
      for (auto& elem : sold) {
        content += std::to_string(counter++);
        content += ". Item: " + elem->item;
        content += ", Price: " + std::to_string(elem->price);
        content += ", Buyer: " + elem->buyer->name + "\r\n";
      }
    }

    if (expiredSold.size() > 0) {
      content += "*. Expired sold:\r\n";
      counter = 1;
      for (auto& elem : expiredSold) {
        content += std::to_string(counter++);
        content += ". Item: " + elem->item;
        content += ", Price: " + std::to_string(elem->price) + "\r\n";
      }
    }

    if (onSale.size() > 0) {
      content += "*. On sale:\r\n";
      counter = 1;
      for (auto& elem : onSale) {
        content += std::to_string(counter++);
        content += ". Item: " + elem->item;
        content += ", Price: " + std::to_string(elem->price) + "\r\n";
      }
    }
    return createUiContent(getUri(), content);
  }

  std::shared_ptr<PageManager> manager;
  std::shared_ptr<Context> context;
};

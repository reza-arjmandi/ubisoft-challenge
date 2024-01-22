#pragma once

#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <boost/json.hpp>
#include "ModelBase.h"

struct User: public ModelBase<User> 
{
  std::string name = "";
  int balance = 0;
  std::vector<std::string> items;

  std::string serialize(boost::json::value& jv) override 
  {
    boost::json::array itemsArray;
    for (const auto& item : items) 
    {
      itemsArray.emplace_back(item);
    }
    jv = {
        {"name", name},
        {"balance", balance},
        {"items", itemsArray}
    };
    return "users";
  }
};

ModelBase<User>::DBCollection ModelBase<User>::Collection{};

enum class SaleState 
{
  avaiableForSale,
  soldOut,
  expired,
};

struct SaleItem: public ModelBase<SaleItem> 
{
  std::time_t createdAt = 0;
  std::shared_ptr<User> seller = nullptr;
  std::shared_ptr<User> buyer = nullptr;
  std::string item = "";
  int price = 0;
  SaleState state = SaleState::avaiableForSale;

  std::string serialize(boost::json::value& jv) override 
  {
    jv = {
        {"createdAt", (int)createdAt},
        {"seller", seller->name},
        {"buyer", buyer != nullptr ? buyer->name : ""},
        {"item", item},
        {"price", price},
        {"state", (int)state}
    };
    return "saleItems";
  }
};

ModelBase<SaleItem>::DBCollection ModelBase<SaleItem>::Collection{};

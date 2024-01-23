#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
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

  void deSerialize(boost::json::value& jv) override 
  {
    name = jv.at("name").as_string();
    balance = jv.at("balance").as_int64();
    for (auto item : jv.at("items").as_array())
    {
      items.push_back(item.as_string().c_str());
    }
  }
};

ModelBase<User>::DBCollection ModelBase<User>::Collection{"users"};

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
        {"createdAt", createdAt},
        {"seller", seller->name},
        {"buyer", buyer != nullptr ? buyer->name : ""},
        {"item", item},
        {"price", price},
        {"state", (int)state}
    };
    return "saleItems";
  }

  void deSerialize(boost::json::value& jv) override 
  {
    item = jv.at("item").as_string();
    price = jv.at("price").as_int64();
    createdAt = jv.at("createdAt").as_int64();
    seller = User::Collection.findOrCreate(
      [&jv](const auto& user) {
        return user->name == jv.at("seller").as_string();
      },
      [&jv]() {
        auto newUser = std::make_shared<User>();
        newUser->name = jv.at("seller").as_string();
        return newUser;
      }
    );
    buyer = User::Collection.findOrCreate(
      [&jv](const auto& user) {
        return user->name == jv.at("buyer").as_string();
      },
      [&jv]() {
        auto newUser = std::make_shared<User>();
        newUser->name = jv.at("buyer").as_string();
        return newUser;
      }
    );
    auto currentTime = std::chrono::system_clock::now();
    auto currentTimeEpoch = std::chrono::duration_cast<std::chrono::seconds>(currentTime.time_since_epoch()).count();

    switch(jv.at("state").as_int64())
    {
      case 0:
        state = SaleState::avaiableForSale;
        if (currentTimeEpoch - createdAt >= 300)
        {
          state = SaleState::expired;
          seller->items.push_back(item);
        }
        break;
      case 1: 
        state = SaleState::soldOut;
        break;    
      case 2: 
        state = SaleState::expired;
        break;
      default:
        break;
    }
  }
};

ModelBase<SaleItem>::DBCollection ModelBase<SaleItem>::Collection{"saleItems"};

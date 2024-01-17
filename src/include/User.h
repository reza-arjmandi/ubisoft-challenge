#pragma once

#include <string>
#include <vector>

struct Item {
  std::string name;
  std::string price;
};

struct User {
  std::string name = "";
  int balance = 0;
  std::vector<Item> items;
};

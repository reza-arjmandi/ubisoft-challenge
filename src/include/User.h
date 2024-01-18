#pragma once

#include <string>
#include <vector>
#include "DBCollection.h"

struct User {
  std::string name = "";
  int balance = 0;
  std::vector<std::string> items;
  static DBCollection<User> Collection;
};

DBCollection<User> User::Collection{};

#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include "User.h"

class Database {

public:

  std::shared_ptr<User> findOrCreate(const std::string& username) 
  {
    auto found = std::find_if(users.cbegin(), users.cend(), [&](auto& user){
      return user->name == username;
    });
    if (found != std::cend(users)) {
      return *found;
    }

    auto newUser = std::make_shared<User>();
    newUser->name = username;
    users.push_back(newUser);
    sync();
    return newUser;
  }

private:

  void sync()
  {

  }

  std::vector<std::shared_ptr<User>> users;
};

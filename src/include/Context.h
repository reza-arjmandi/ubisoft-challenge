#pragma once

#include <memory>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "UserInterface.h"
#include "Database.h"
#include "User.h"

struct Context {
  std::shared_ptr<UserInterface> ui;
  std::shared_ptr<Database> db;
  std::shared_ptr<User> user;
};

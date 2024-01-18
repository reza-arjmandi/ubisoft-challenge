#pragma once

#include <memory>
#include <map>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "UserInterface.h"
#include "User.h"
#include "State.h"

struct Context {
  std::shared_ptr<UserInterface> ui;
  std::shared_ptr<User> user;
  std::map<StateNames, std::shared_ptr<State>> states;
};

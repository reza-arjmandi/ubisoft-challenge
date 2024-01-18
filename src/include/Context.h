#pragma once

#include <memory>
#include <map>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "UserInterface.h"
#include "Models.h"
#include "State.h"

struct Context {
  Context(boost::asio::io_context& ctx)
  : ioContext(ctx) {}

  std::shared_ptr<UserInterface> ui;
  std::shared_ptr<User> user;
  std::map<StateNames, std::shared_ptr<State>> states;
  boost::asio::io_context& ioContext;
};

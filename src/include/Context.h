#pragma once

#include <memory>
#include <boost/asio.hpp>
#include "UserInterface.h"
#include "Models.h"

struct Context 
{
  Context(boost::asio::io_context& ctx)
  : ioContext(ctx) {}

  std::shared_ptr<UserInterface> ui;
  std::shared_ptr<User> user;
  boost::asio::io_context& ioContext;
};

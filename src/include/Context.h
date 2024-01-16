#pragma once

#include <memory>
#include <string>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "EndUser.h"

struct Context {

  std::shared_ptr<EndUser> endUser;
  std::string userName;

};

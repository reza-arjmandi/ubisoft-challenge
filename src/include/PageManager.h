#pragma once

#include <memory>
#include <string>
#include "Context.h"

class PageManager {
 public:
  virtual ~PageManager() = default;
  virtual void navigate(const std::string& uri, std::shared_ptr<Context> ctx) = 0;
};

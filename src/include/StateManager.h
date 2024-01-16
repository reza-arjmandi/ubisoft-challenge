#pragma once

#include <memory>
#include "Context.h"

class StateManager {
public:
  virtual ~StateManager() = default;
  virtual void SetState(std::shared_ptr<State> newState, std::shared_ptr<Context> ctx) = 0;
};
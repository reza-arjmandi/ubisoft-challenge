#pragma once

#include <string>
#include <memory>

#include "Context.h"

class StateManager;

class State {
public:
  virtual ~State() = default;
  virtual void Enter(std::shared_ptr<StateManager> manager) = 0;
  virtual void Process(std::shared_ptr<StateManager> manager, std::shared_ptr<Context> ctx) = 0;
  virtual void Exit(std::shared_ptr<StateManager> manager) = 0;
};
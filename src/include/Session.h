#pragma once

#include <memory>
#include "State.h"
#include "StateManager.h"
#include "Context.h"
#include "UserInterface.h"

using boost::asio::ip::tcp;

class Session
  : public std::enable_shared_from_this<Session>, public StateManager
{
public:
  Session(std::shared_ptr<Context> ctx)
  : context(ctx)
  {
  }
  
  void SetState(std::shared_ptr<State> newState, std::shared_ptr<Context> ctx) override
  {
    auto self(shared_from_this());
    currentState->Exit(self);
    currentState = newState;
    currentState->Enter(self);
    currentState->Process(self, context);
  }

  void start()
  {
    currentState = context->states[StateNames::AuthState];
    auto self(shared_from_this());
    currentState->Process(self, context);
  }

private:

  std::shared_ptr<State> currentState;
  std::shared_ptr<Context> context;
};
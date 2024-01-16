#pragma once

#include <memory>
#include "State.h"
#include "StateManager.h"
#include "AuthState.h"
#include "Context.h"
#include "EndUser.h"

using boost::asio::ip::tcp;

class Session
  : public std::enable_shared_from_this<Session>, public StateManager
{
public:
  Session(tcp::socket socket)
  {
    context->endUser = std::make_shared<EndUser>(std::move(socket));
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
    auto self(shared_from_this());
    currentState->Process(self, context);
  }

private:

  std::shared_ptr<State> currentState = std::make_shared<AuthState>();
  std::shared_ptr<Context> context = std::make_shared<Context>();
};
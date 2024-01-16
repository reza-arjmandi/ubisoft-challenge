#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "State.h"
#include <DashboardState.h>

using boost::asio::ip::tcp;

class AuthState: public State {

public:

  void Enter(std::shared_ptr<StateManager> mgr) override
  {
  }

  void Process(std::shared_ptr<StateManager> mgr, std::shared_ptr<Context> ctx) override
  {
    manager = mgr;
    context = ctx;
    context->endUser->doWrite("Login with user: ",
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) onWrite();
        }
    );
  }
  
  void Exit(std::shared_ptr<StateManager> mgr) override
  {
  }


  void onWrite()
  {
    context->endUser->doRead(user, maxUserLength, 
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) onRead();
        }
    );
  }

  void onRead()
  {
    context->userName = std::string(user);
    auto dashboard = std::make_shared<DashboardState>();
    manager->SetState(dashboard, context);
  }


private:
  std::shared_ptr<StateManager> manager;
  std::shared_ptr<Context> context;
  enum { maxUserLength = 1024 };
  char user[maxUserLength];
};
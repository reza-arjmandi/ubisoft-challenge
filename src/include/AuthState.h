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
    context->ui->doWrite("Login with user: ",
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
    context->ui->doRead(username, maxUsernameLength, 
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) onRead();
        }
    );
  }

  void onRead()
  {
    context->user = context->db->findOrCreate(username);
    auto dashboard = std::make_shared<DashboardState>();
    manager->SetState(dashboard, context);
  }


private:

  std::shared_ptr<StateManager> manager;
  std::shared_ptr<Context> context;
  enum { maxUsernameLength = 1024 };
  char username[maxUsernameLength];
};
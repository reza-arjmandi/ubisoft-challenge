#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "State.h"
#include "Models.h"
#include "DashboardState.h"

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
    context->ui->doWrite(GetAuthContent(),
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) onWrite();
        }
    );
  }
  
  void Exit(std::shared_ptr<StateManager> mgr) override
  {
  }

    
  std::string GetAuthContent() const 
  {
    std::string content = "=======================================================\r\n";
    content += "URI: /app/login\r\n";
    content += "-------------------------------------------------------\r\n";
    content += "Enter username for login or create a new user: ";
    return content;
  }

  void onWrite()
  {
    std::memset(username, '\0', maxUsernameLength);
    context->ui->doRead(username, maxUsernameLength, 
        [this](boost::system::error_code ec, std::size_t length)
        {
          if (!ec) onRead();
        }
    );
  }

  void onRead()
  {
    std::string name(username);
    context->user = User::Collection.findOrCreate(
        [&name](const auto& user) {
          return user->name == name;
        },
        [&name]() {
          auto newUser = std::make_shared<User>();
          newUser->name = name;
          return newUser;
        }
        );
    auto dashboard = std::make_shared<DashboardState>();
    manager->SetState(dashboard, context);
  }


private:

  std::shared_ptr<StateManager> manager;
  std::shared_ptr<Context> context;
  enum { maxUsernameLength = 1024 };
  char username[maxUsernameLength];
};
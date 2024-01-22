#pragma once

#include <string>
#include "Page.h"
#include "Models.h"
#include "Utils.h"

class Auth: public Page 
{

public:

  void render(std::shared_ptr<PageManager> mgr, std::shared_ptr<Context> ctx) override
  {
    manager = mgr;
    context = ctx;
    auto authContent = createUiContent(getUri(), "Enter username for login or create a new user: ");
    context->ui->askForString(authContent,
        [this](std::string username)
        {
          onReadUsername(username);
        }
    );
  }
  
  std::string getUri() const override 
  {
    return PageURIs::Auth;
  }

private:

  void onReadUsername(const std::string& username)
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
    manager->navigate(PageURIs::Dashboard, context);
  }

  std::shared_ptr<PageManager> manager;
  std::shared_ptr<Context> context;

};

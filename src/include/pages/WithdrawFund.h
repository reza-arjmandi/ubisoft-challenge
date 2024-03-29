#pragma once

#include <vector>
#include <string>
#include <memory>
#include <boost/asio.hpp>
#include "Page.h"
#include "Utils.h"

class WithdrawFund: public Page {
 public:
  void render(std::shared_ptr<PageManager> mgr, std::shared_ptr<Context> ctx) override {
    manager = mgr;
    context = ctx;
    context->ui->askForNumber(createUiContent(getUri(), "Enter the withdraw amount: "),
      [this](int amount, bool& reTake) {
        if (amount <= 0) {
          reTake = true;
          return;
        }
        onReadWithdraw(amount);
      });
  }

  std::string getUri() const override {
    return PageURIs::WithdrawFund;
  }

 private:
  void onReadWithdraw(int amount) {
    auto balance = context->user->balance;
    balance -= amount;
    User::Collection.save();
    bool done = false;
    if (balance >= 0) {
      context->user->balance = balance;
      done = true;
    }
    std::string result = done ? "Withdraw was successful.\r\n" : "You don't have enough balance.\r\n";
    context->ui->doWrite(result,
      [this](boost::system::error_code ec, std::size_t length) {
        if (!ec) manager->navigate(PageURIs::Withdraw, context);
      });
  }

  std::shared_ptr<PageManager> manager;
  std::shared_ptr<Context> context;
};

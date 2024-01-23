#pragma once

#include <memory>
#include <string>
#include <map>
#include "Page.h"
#include "pages/Dashboard.h"
#include "pages/Auth.h"
#include "pages/Deposit.h"
#include "pages/Withdraw.h"
#include "pages/WithdrawFund.h"
#include "pages/WithdrawItem.h"
#include "pages/DepositFund.h"
#include "pages/DepositItem.h"
#include "pages/Sell.h"
#include "pages/Buy.h"
#include "pages/Display.h"
#include "PageManager.h"
#include "Context.h"
#include "UserInterface.h"

class Session : public std::enable_shared_from_this<Session>, public PageManager {
 public:
  explicit Session(std::shared_ptr<Context> ctx)
  : context(ctx) {
    initPages();
  }

  void navigate(const std::string& uri, std::shared_ptr<Context> ctx) override {
    auto self(shared_from_this());
    currentPage = pages[uri];
    currentPage->render(self, context);
  }

  void start() {
    currentPage = pages[PageURIs::Auth];
    auto self(shared_from_this());
    currentPage->render(self, context);
  }

 private:
  void initPages() {
    pages[PageURIs::Auth] = std::make_shared<Auth>();
    pages[PageURIs::Dashboard] = std::make_shared<Dashboard>();
    pages[PageURIs::Deposit] = std::make_shared<Deposit>();
    pages[PageURIs::DepositFund] = std::make_shared<DepositFund>();
    pages[PageURIs::DepositItem] = std::make_shared<DepositItem>();
    pages[PageURIs::Withdraw] = std::make_shared<Withdraw>();
    pages[PageURIs::WithdrawFund] = std::make_shared<WithdrawFund>();
    pages[PageURIs::WithdrawItem] = std::make_shared<WithdrawItem>();
    pages[PageURIs::Sell] = std::make_shared<Sell>();
    pages[PageURIs::Buy] = std::make_shared<Buy>();
    pages[PageURIs::Display] = std::make_shared<Display>();
  }

  std::map<std::string, std::shared_ptr<Page>> pages;
  std::shared_ptr<Page> currentPage;
  std::shared_ptr<Context> context;
};

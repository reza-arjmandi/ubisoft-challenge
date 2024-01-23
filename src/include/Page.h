#pragma once

#include <string>
#include <memory>

class PageManager;
struct Context;

namespace PageURIs {
  const std::string Auth = "/app/login";
  const std::string Dashboard = "/app/dashboard";
  const std::string Deposit = "/app/dashboard/deposit";
  const std::string DepositFund = "/app/dashboard/deposit/fund";
  const std::string DepositItem = "/app/dashboard/deposit/item";
  const std::string Withdraw = "/app/dashboard/withdraw";
  const std::string WithdrawFund = "/app/dashboard/withdraw/fund";
  const std::string WithdrawItem = "/app/dashboard/withdraw/item";
  const std::string Sell = "/app/dashboard/sell";
  const std::string Buy = "/app/dashboard/buy";
  const std::string Display = "/app/dashboard/display";
};  // namespace PageURIs

class Page {
 public:
  virtual ~Page() = default;
  virtual void render(std::shared_ptr<PageManager> manager, std::shared_ptr<Context> ctx) = 0;
  virtual std::string getUri() const = 0;
};

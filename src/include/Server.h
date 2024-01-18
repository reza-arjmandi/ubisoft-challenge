#pragma once

#include <cstdlib>
#include <iostream>
#include <memory>
#include <map>
#include <utility>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "DBCollection.h"
#include "Session.h"
#include "State.h"
#include "states/DashboardState.h"
#include "states/AuthState.h"
#include "states/DepositState.h"
#include "states/WithdrawState.h"
#include "states/WithdrawFundState.h"
#include "states/WithdrawItemState.h"
#include "states/DepositFundState.h"
#include "states/DepositItemState.h"

using boost::asio::ip::tcp;

class Server
{
public:
  Server(boost::asio::io_context& io_context, short port)
    : acceptor(io_context, tcp::endpoint(tcp::v4(), port)),
      socket(io_context)
  {
    doAccept();
  }

private:
  void doAccept()
  {
    acceptor.async_accept(socket,
        [this](boost::system::error_code ec)
        {
          if (!ec)
          {
            std::shared_ptr<Context> context = std::make_shared<Context>();
            context->ui = std::make_shared<UserInterface>(std::move(socket));
            context->states = createStates();
            std::make_shared<Session>(context)->start();
          }

          doAccept();
        });
  }
  
  std::map<StateNames, std::shared_ptr<State>> createStates() {
    std::map<StateNames, std::shared_ptr<State>> states;
    states[StateNames::AuthState] = std::make_shared<AuthState>();
    states[StateNames::DashboardState] = std::make_shared<DashboardState>();
    states[StateNames::DepositState] = std::make_shared<DepositState>();
    states[StateNames::DepositFundState] = std::make_shared<DepositFundState>();
    states[StateNames::DepositItemState] = std::make_shared<DepositItemState>();
    states[StateNames::WithdrawState] = std::make_shared<WithdrawState>();
    states[StateNames::WithdrawFundState] = std::make_shared<WithdrawFundState>();
    states[StateNames::WithdrawItemState] = std::make_shared<WithdrawItemState>();
    return states;
  }
  
  tcp::acceptor acceptor;
  tcp::socket socket;

};
#pragma once

#include <string>
#include <memory>
#include <vector>

template<typename T>
class Model {

public:

  class DBCollection {

  public:
    std::shared_ptr<T> findOrCreate(std::function<bool(const std::shared_ptr<T>&)> matcher, std::function<std::shared_ptr<T>()> factory) 
    {
      auto found = std::find_if(buffer.cbegin(), buffer.cend(), matcher);
      if (found != std::cend(buffer)) {
        return *found;
      }

      return create(factory);
    }

    std::shared_ptr<T> create(std::function<std::shared_ptr<T>()> factory) 
    {
      auto newElem = factory();
      buffer.push_back(newElem);
      save();
      return newElem;
    }

    void save() {

    }

  private:
    std::vector<std::shared_ptr<T>> buffer;

  };

  static DBCollection Collection;

private:

};

struct User: public Model<User> {
  std::string name = "";
  int balance = 0;
  std::vector<std::string> items;
};

Model<User>::DBCollection Model<User>::Collection{};

enum class SaleState {
  avaiableForSale,
  soldOut,
  expired,
};

struct SaleItem: public Model<SaleItem> {
  std::time_t createdAt = 0;
  std::string seller = "";
  std::string buyer = "";
  std::string item = "";
  int price = 0;
  SaleState state = SaleState::avaiableForSale;
};

Model<SaleItem>::DBCollection Model<SaleItem>::Collection{};

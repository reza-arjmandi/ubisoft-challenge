#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include <functional>

template<typename T>
class DBCollection {

public:

  std::shared_ptr<T> findOrCreate(std::function<bool(const std::shared_ptr<T>&)> matcher, std::function<std::shared_ptr<T>()> factory) 
  {
    auto found = std::find_if(buffer.cbegin(), buffer.cend(), matcher);
    if (found != std::cend(buffer)) {
      return *found;
    }

    auto newElem = factory();
    buffer.push_back(newElem);
    sync();
    return newElem;
  }

private:

  void sync()
  {

  }

  std::vector<std::shared_ptr<T>> buffer;
};

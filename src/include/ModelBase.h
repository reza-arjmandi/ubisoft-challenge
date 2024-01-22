#pragma once

#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/json.hpp>
#include "Active.h"

namespace json = boost::json;

void pretty_print( std::ostream& os, json::value const& jv, std::string* indent = nullptr )
{
    std::string indent_;
    if(! indent)
        indent = &indent_;
    switch(jv.kind())
    {
    case json::kind::object:
    {
        os << "{\n";
        indent->append(4, ' ');
        auto const& obj = jv.get_object();
        if(! obj.empty())
        {
            auto it = obj.begin();
            for(;;)
            {
                os << *indent << json::serialize(it->key()) << " : ";
                pretty_print(os, it->value(), indent);
                if(++it == obj.end())
                    break;
                os << ",\n";
            }
        }
        os << "\n";
        indent->resize(indent->size() - 4);
        os << *indent << "}";
        break;
    }

    case json::kind::array:
    {
        os << "[\n";
        indent->append(4, ' ');
        auto const& arr = jv.get_array();
        if(! arr.empty())
        {
            auto it = arr.begin();
            for(;;)
            {
                os << *indent;
                pretty_print( os, *it, indent);
                if(++it == arr.end())
                    break;
                os << ",\n";
            }
        }
        os << "\n";
        indent->resize(indent->size() - 4);
        os << *indent << "]";
        break;
    }

    case json::kind::string:
    {
        os << json::serialize(jv.get_string());
        break;
    }

    case json::kind::uint64:
    case json::kind::int64:
    case json::kind::double_:
        os << jv;
        break;

    case json::kind::bool_:
        if(jv.get_bool())
            os << "true";
        else
            os << "false";
        break;

    case json::kind::null:
        os << "null";
        break;
    }

    if(indent->empty())
        os << "\n";
}

template<typename T>
class ModelBase {

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

    std::vector<std::shared_ptr<T>>> all() const
    {
      return buffer;
    }

    void save() {
       active.send([&](){
        dumpToJson();
       });
    }

  private:
    void dumpToJson() {
        boost::json::value root;
        std::string key;
        boost::json::array itemsArray;

        for (auto& elem : buffer) {
            boost::json::value value;
            key = elem->serialize(value);
            itemsArray.emplace_back(value);
        }
        boost::json::object jsonObject;
        jsonObject.emplace(key, itemsArray);
        root.emplace_object() = jsonObject;

        std::string dbPath = key + ".json";
        std::ofstream outFile(dbPath);
        if (outFile.is_open()) {
            pretty_print(outFile, root);
        } else {
            std::cerr << "Error opening the file for writing." << std::endl;
        }
    }

    std::vector<std::shared_ptr<T>> buffer;
    Active active;

  };

  virtual std::string serialize(boost::json::value& jv) = 0;
  static DBCollection Collection;

};

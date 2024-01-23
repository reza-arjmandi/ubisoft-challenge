#pragma once

#include <algorithm>
#include <cctype>
#include <locale>
#include <string>

inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
    return !std::isspace(ch);
  }));
}

inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
    return !std::isspace(ch);
  }).base(), s.end());
}

inline void trim(std::string &s) {
  rtrim(s);
  ltrim(s);
}

inline std::string createUiContent(std::string uri, std::string message) {
  std::string content  = "=======================================================\r\n";
  content += "URI: " + uri + "\r\n";
  content += "-------------------------------------------------------\r\n";
  content += message;
  return content;
}

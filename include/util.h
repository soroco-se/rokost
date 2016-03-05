#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <string>

namespace Util
{

inline std::string trim(std::string s, char c)
{
  size_t p1 = s.find_first_not_of(c);
  size_t p2 = s.find_last_not_of(c);
  if (p1 ==  std::string::npos)
    p1=0;
  if (p2 == std::string::npos)
    p2=s.size();
  return s.substr(p1,p2+1-p1);
}

}
#endif // UTIL_H_INCLUDED

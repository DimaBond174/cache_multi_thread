/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef SPECTOOLS_H
#define SPECTOOLS_H

#include <string>
#include <sstream>
#include "specstatic.h"

namespace spec {

template <typename T>
std::string to_string(T  value)
{
  std::ostringstream os;
  os << value;
  return os.str();
}

template<typename N>
int64_t to_int64t(N  value) {
  const std::string  &str_value  =  to_string(value);
  return  stoll(str_value.c_str(), str_value.length());
}

};



#endif // SPECTOOLS_H

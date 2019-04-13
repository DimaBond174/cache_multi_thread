#ifndef TIMETOOLS_H
#define TIMETOOLS_H
#include <iomanip>
#include <ctime>
#include <chrono>
#include <sstream>

template<typename  T>
auto  chronos_to_string(T  from_time)  {
  auto in_time_t = std::chrono::system_clock::to_time_t(from_time);
  return  std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X") ;
}

std::string return_current_time_and_date()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

#endif // TIMETOOLS_H

/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef IPROPERTIES_H
#define IPROPERTIES_H
#include <string>
#include "i/isystem.h"
#include <memory>
#include <stdint.h>

/*
 * Common interface for
 * Property container pattern
 */
class IConfig  {
 public:
  IConfig()  =  default;
  virtual ~IConfig()  {  }
  virtual  void  loadConfig(int  argc,  char  **argv, std::shared_ptr<ISystem>  &cur_system)  =  0;
  virtual int64_t  getLongValue(const std::string  &key)  =  0;
  virtual std::string  getStringValue(const std::string  &key)  =  0;
  virtual void  setValue(const std::string  &key,  const std::string  &value)  =  0;
private:
 IConfig(const  IConfig&)  =  delete;
 IConfig& operator=(const  IConfig&)  =  delete;
};

#endif // IPROPERTIES_H

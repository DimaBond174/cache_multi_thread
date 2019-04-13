/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef CONFIGHARDCODE_H
#define CONFIGHARDCODE_H

#include "i/iconfig.h"

class ConfigHardCode  :  IConfig  {
 public:
  ConfigHardCode();
  void  loadConfig(int  argc,  char  **argv, std::shared_ptr<ISystem>  &cur_system)  override;
  int64_t  getLongValue(const std::string & key)  override;
  std::string  getStringValue(const std::string & key)  override;
  void  setValue(const std::string  &key,  const std::string  &value) override {}
};

#endif // CONFIGHARDCODE_H

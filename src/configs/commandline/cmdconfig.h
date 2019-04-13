/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef CMDCONFIG_H
#define CMDCONFIG_H

#include "i/iconfig.h"
#include "configs/json/configjson.h"

class  CmdConfig  :  public IConfig  {
 public:
  CmdConfig();
  void  loadConfig(int  argc,  char  **argv, std::shared_ptr<ISystem>  &cur_system)  override;
  int64_t  getLongValue(const std::string  &key)  override;
  std::string  getStringValue(const std::string  &key)  override;
  void  setValue(const std::string  &key,  const std::string  &value)  override;
 private:
  ConfigJson  jsonConfig;
  std::map<std::string, std::string>  mapConfig;
};

#endif // CMDCONFIG_H

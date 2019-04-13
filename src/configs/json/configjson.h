/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef CONFIGJSON_H
#define CONFIGJSON_H

#include <map>
#include "i/iconfig.h"
#include "configs/hardcode/confighardcode.h"
#include "specjson.h"

class  ConfigJson  :  public IConfig  {
 public:
  ConfigJson();
  void  loadConfig(int  argc,  char  **argv, std::shared_ptr<ISystem>  &cur_system)  override;
  int64_t  getLongValue(const std::string  &key)  override;
  std::string  getStringValue(const std::string  &key)  override;
  void  setValue(const std::string  &key,  const std::string  &value) override;
 private:
  ConfigHardCode  defConfig;
  std::map<std::string, std::string>  mapConfig;

  void  traverse(TNode  *node);
};

#endif // CONFIGJSON_H

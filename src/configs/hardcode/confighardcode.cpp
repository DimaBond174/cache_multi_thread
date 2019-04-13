/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#include "confighardcode.h"

ConfigHardCode::ConfigHardCode()  {

}

void  ConfigHardCode::loadConfig(int  argc,  char  **argv,  std::shared_ptr<ISystem>  &cur_system)  {
  return;
}

int64_t  ConfigHardCode::getLongValue(const std::string  &key)  {
  int64_t  re  =  0;
  if  (0==key.compare("TEST_MAXSIZE"))  {
    re  =  1000000;
  } else if (0==key.compare("START_LOGPOINT")) {
    re  =  1000;
  } else if (0==key.compare("LOAD_DATA_FROM_TEST_CASE_FILE")) {
    re  =  0;
  } else if (0==key.compare("MAKE_HASH_NOT_UNIQUE")) {
    re  =  0;
  } else if (0==key.compare("The number of threads to insert")) {
    re  =  1;
  }  else if (0==key.compare("The number of threads to find")) {
    re  =  1;
  }
    //LogLevel default to 0
    return re;
}

std::string  ConfigHardCode::getStringValue(const std::string  &key)  {
  std::string  re;
  if  (0==key.compare("FileWithTestData"))  {
    re  =  std::string("./testCase.txt");
  }  else if (0==key.compare("TestResultsDir"))  {
    re  =  std::string("./testResults");
  }
  return re;
}

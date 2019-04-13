#include "cmdconfig.h"
#include "spec/specstatic.h"

CmdConfig::CmdConfig()
{

}


void  CmdConfig::loadConfig(int  argc,  char  **argv,  std::shared_ptr<ISystem>  &cur_system)  {
  mapConfig.clear();
  for (int i = 1;  i < argc;  i += 2)  {
    mapConfig.emplace(std::make_pair(argv[i], argv[i + 1]));
  }
  jsonConfig.loadConfig(argc,  argv,  cur_system);
}  //  loadConfig

std::string  CmdConfig::getStringValue(const std::string  &key)  {
  //std::map<std::string, std::string>::const_iterator
  auto&&  it_exists  =  mapConfig.find(key);
  if  (it_exists  !=  mapConfig.end())  {
    return  it_exists->second;
  }  else  {
    return  jsonConfig.getStringValue(key);
  }
}

int64_t  CmdConfig::getLongValue(const std::string  &key)  {
  const std::string  &strVal  =  getStringValue(key);
  int64_t  re  =  0;
  std::size_t  len  =  strVal.length();
  if  (len  >  0)  {
    re  =  stoll(strVal.c_str(),  len);
  }
  return re;
}

void  CmdConfig::setValue(const std::string  &key,  const std::string  &value)  {
  std::pair<std::map<std::string, std::string>::iterator,bool> re =
      mapConfig.emplace(std::make_pair(key,  value));
  if (!re.second)  {
    re.first->second  =  value;
  }
//  std::map<std::string, std::string>::iterator  it  =  mapConfig.find(key);
//  if  (mapConfig.end()  ==  it)  {
//    mapConfig.emplace(std::make_pair(key,  value));
//  }  else  {

//  }
}

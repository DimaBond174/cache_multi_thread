/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#include <iostream>
#include "configjson.h"
#include "spec/specstatic.h"


ConfigJson::ConfigJson()  {

}

void  ConfigJson::setValue(const std::string  &key,  const std::string  &value)  {
  mapConfig.emplace(std::make_pair(key,  value));
}

void  ConfigJson::loadConfig(int  argc,  char  **argv,  std::shared_ptr<ISystem>  &cur_system)  {
  const std::string  &json  =  cur_system.get()->load_text_file("./assets/settings.json");
  int  length  =  json.length();
  if  (length  >  0)  {
    SpecJson  specJson(json.c_str(),  length,  false);
    if  (specJson.parse())  {
			traverse(specJson.getFirstNodeOfObject(nullptr));
		}      
  }  //  if (length > 0)

  for  (std::map<std::string,  std::string>::const_iterator  it  =  mapConfig.begin();
            it  !=  mapConfig.end();  ++it)  {
        std::cout  <<  it->first  <<  " : "  <<  it->second  <<  std::endl;
  };  
}  //  loadConfig

std::string  ConfigJson::getStringValue(const std::string  &key)  {
  //std::map<std::string, std::string>::const_iterator
  auto&&  it_exists  =  mapConfig.find(key);
  if  (it_exists  !=  mapConfig.end())  {
    return  it_exists->second;
  }  else  {
    return  defConfig.getStringValue(key);
  }
}

int64_t  ConfigJson::getLongValue(const std::string  &key)  {
  const std::string  &strVal  =  getStringValue(key);
  int64_t  re  =  0;
  int  len  =  strVal.length();
  if  (len  >  0)  {
    re  =  stoll(strVal.c_str(),  len);
  }
  return re;
}

void  ConfigJson::traverse(TNode  *node)  {
  TNode  *cur  =  node;
  while  (cur)  {
    switch  (cur->type)  {
		case 's':
		case 'd':
      if  (cur->lenKey>0  &&  cur->lenData>0)  {
				mapConfig.insert(std::make_pair(
          std::string(cur->pKey,  cur->lenKey),
          std::string(static_cast<const char *>(cur->pData),  cur->lenData)));
			}
      break;
		case 'o':
			traverse(cur);
			break;
		default:
			std::cerr << "Error: ConfigJson::TNode->type=" << cur->type << std::endl;
			break;
		}
    cur  =  cur->nextNode;
	}
}



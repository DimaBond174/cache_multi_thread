/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#include "testoncachesmru2.h"
#include "spec/specstatic.h"
#include <memory>


TestOnCacheSMRU2::TestOnCacheSMRU2()
{

}


TestOnCacheSMRU2::~TestOnCacheSMRU2() {
  onStop();
}

void  TestOnCacheSMRU2::onStart(std::shared_ptr<IConfig>  &cfg)  {
  int64_t  capacity  = cfg.get()->getLongValue("capacity");
  assert(capacity > 0);
  onStop() ;
  cache  =  new OnCacheSMRU<ElemNSizeKey *, ElemNSizeKey *>(capacity,  10);
  return;
}

void  TestOnCacheSMRU2::onStop()  {
  if (cache) {
    delete  cache;
    cache  =  nullptr;
  }
  return;
}

void  TestOnCacheSMRU2::insert(void  *elem)  {
  ElemNSizeKey  *e  =  static_cast<ElemNSizeKey *>(elem);
  cache->insertNode(e,
                    std::make_shared<ElemNSizeKey *>(e));
  return;
}

bool  TestOnCacheSMRU2::exist(void  *elem)  {
  bool  re  =  false;
  ElemNSizeKey  *e  =  static_cast<ElemNSizeKey *>(elem);
  std::shared_ptr<ElemNSizeKey *> p_elem  =  cache->getData(e);
  if (p_elem  &&  *p_elem.get() == e) {
     re  =  true;
  }
  return  re;
}

const char *  TestOnCacheSMRU2::get_algorithm_name()  {
  static ConstString my_name("OnCacheSMRU");
  return  my_name.c_str;
}

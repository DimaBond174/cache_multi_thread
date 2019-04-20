/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#include "testoncachemmru2.h"
#include "spec/specstatic.h"
#include <memory>

TestOnCacheMMRU2::TestOnCacheMMRU2()
{

}


TestOnCacheMMRU2::~TestOnCacheMMRU2() {
  onStop();
}

void  TestOnCacheMMRU2::onStart(std::shared_ptr<IConfig>  &cfg)  {
  int64_t  capacity  = cfg.get()->getLongValue("capacity");
  assert(capacity > 0);
  onStop() ;
  cache  =  new OnCacheMMRU<ElemNSizeKey *, ElemNSizeKey *>(capacity,  10);
  return;
}

void  TestOnCacheMMRU2::onStop()  {
  if (cache) {
    delete  cache;
    cache  =  nullptr;
  }
  return;
}

void  TestOnCacheMMRU2::insert(void  *elem)  {
  ElemNSizeKey  *e  =  static_cast<ElemNSizeKey *>(elem);
  cache->insertNode(e,  std::make_shared<ElemNSizeKey *>(e));
  return;
}

bool  TestOnCacheMMRU2::exist(void  *elem)  {
  bool  re  =  false;
  ElemNSizeKey  *e  =  static_cast<ElemNSizeKey *>(elem);
  std::shared_ptr<ElemNSizeKey *> p_elem  =  cache->getData(e);
  if (p_elem  &&  *p_elem.get() == e) {
     re  =  true;
  }
  return  re;
}

const char *  TestOnCacheMMRU2::get_algorithm_name()  {
  static ConstString my_name("OnCacheMMRU");
  return  my_name.c_str;
}

/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#include "testoncachemlru2.h"
#include "spec/specstatic.h"
#include <memory>

TestOnCacheMLRU2::TestOnCacheMLRU2()
{

}



TestOnCacheMLRU2::~TestOnCacheMLRU2() {
  onStop();
}

void  TestOnCacheMLRU2::onStart(std::shared_ptr<IConfig>  &cfg)  {
  int64_t  capacity  = cfg.get()->getLongValue("capacity");
  assert(capacity > 0);
  onStop() ;
  cache  =  new OnCacheMLRU<ElemNSizeKey *, ElemNSizeKey *>(capacity);
  return;
}

void  TestOnCacheMLRU2::onStop()  {
  if (cache) {
    delete  cache;
    cache  =  nullptr;
  }
  return;
}

void  TestOnCacheMLRU2::insert(void  *elem)  {
  ElemNSizeKey  *e  =  static_cast<ElemNSizeKey *>(elem);
  cache->insertNode(e,  std::make_shared<ElemNSizeKey *>(e));
  return;
}

bool  TestOnCacheMLRU2::exist(void  *elem)  {
  bool  re  =  false;
  ElemNSizeKey  *e  =  static_cast<ElemNSizeKey *>(elem);
  std::shared_ptr<ElemNSizeKey *> p_elem  =  cache->getData(e);
  if (p_elem  &&  *p_elem.get() == e) {
     re  =  true;
  }
  return  re;
}

const char *  TestOnCacheMLRU2::get_algorithm_name()  {
  static ConstString my_name("OnCacheMLRU");
  return  my_name.c_str;
}


/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#include "testoncachesmru.h"
#include "spec/specstatic.h"
#include <memory>


TestOnCacheSMRU::TestOnCacheSMRU()
{

}


TestOnCacheSMRU::~TestOnCacheSMRU() {
  onStop();
}

void  TestOnCacheSMRU::onStart(std::shared_ptr<IConfig>  &cfg)  {
  int64_t  capacity  = cfg.get()->getLongValue("capacity");
  assert(capacity > 0);
  onStop() ;
  cache  =  new OnCacheSMRU<TKey *, Elem *>(capacity,  10);
  return;
}

void  TestOnCacheSMRU::onStop()  {
  if (cache) {
    delete  cache;
    cache  =  nullptr;
  }
  return;
}

void  TestOnCacheSMRU::insert(void  *elem)  {
  Elem  *e  =  static_cast<Elem *>(elem);
  cache->insertNode(&(e->key),
                    std::make_shared<Elem *>(e));
  return;
}

bool  TestOnCacheSMRU::exist(void  *elem)  {
  bool  re  =  false;
  Elem  *e  =  static_cast<Elem *>(elem);
  std::shared_ptr<Elem *> p_elem  =  cache->getData(&e->key);
  if (p_elem  &&  *p_elem.get() == e) {
     re  =  true;
  }
  return  re;
}

const char *  TestOnCacheSMRU::get_algorithm_name()  {
  static ConstString my_name("OnCacheSMRU");
  return  my_name.c_str;
}

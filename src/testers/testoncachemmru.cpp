/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#include "testoncachemmru.h"
#include "spec/specstatic.h"
#include <memory>

TestOnCacheMMRU::TestOnCacheMMRU()
{

}


TestOnCacheMMRU::~TestOnCacheMMRU() {
  onStop();
}

void  TestOnCacheMMRU::onStart(std::shared_ptr<IConfig>  &cfg)  {
  int64_t  capacity  = cfg.get()->getLongValue("capacity");
  assert(capacity > 0);
  onStop() ;
  cache  =  new OnCacheMMRU<TKey *, Elem *>(capacity,  10);
  return;
}

void  TestOnCacheMMRU::onStop()  {
  if (cache) {
    delete  cache;
    cache  =  nullptr;
  }
  return;
}

void  TestOnCacheMMRU::insert(Elem  *elem)  {
  cache->insertNode(&(elem->key),
                    std::make_shared<Elem *>(elem));
  return;
}

bool  TestOnCacheMMRU::exist(Elem  *elem)  {
  bool  re  =  false;
  std::shared_ptr<Elem *> p_elem  =  cache->getData(&elem->key);
  if (p_elem  &&  *p_elem.get() == elem) {
     re  =  true;
  }
//  if  (cache->getData(&(elem->key))  ==  elem->data.c_str())  {
//    re  =  true;
//  }
  return  re;
}

const char *  TestOnCacheMMRU::get_algorithm_name()  {
  static ConstString my_name("OnCacheMMRU");
  return  my_name.c_str;
}


#include "testoncachem.h"
#include "spec/specstatic.h"
#include <memory>

TestOnCacheM::TestOnCacheM()   {

}

TestOnCacheM::~TestOnCacheM() {
  onStop();
}

void  TestOnCacheM::onStart(std::shared_ptr<IConfig>  &cfg)  {
  int64_t  capacity  = cfg.get()->getLongValue("capacity");
  assert(capacity > 0);
  onStop() ;
  cache  =  new OnCacheM<TKey *, Elem *>(capacity);
  return;
}

void  TestOnCacheM::onStop()  {
  if (cache) {
    delete  cache;
    cache  =  nullptr;
  }
  return;
}

void  TestOnCacheM::insert(Elem  *elem)  {
  cache->insertNode(&(elem->key),
                    std::make_shared<Elem *>(elem));
  return;
}

bool  TestOnCacheM::exist(Elem  *elem)  {
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

const char *  TestOnCacheM::get_algorithm_name()  {
  static ConstString my_name("OnCacheM");
  return  my_name.c_str;
}

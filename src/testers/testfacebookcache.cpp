#include "testfacebookcache.h"
#include "spec/specstatic.h"

TestFacebookCache::TestFacebookCache()
{

}


TestFacebookCache::~TestFacebookCache() {
  onStop();
}

void  TestFacebookCache::onStart(std::shared_ptr<IConfig>  &cfg)  {
  int64_t  capacity  = cfg.get()->getLongValue("capacity");
  assert(capacity > 0);
  onStop() ;
  //cache  =  new OnCacheM<TKey *, Elem *>(capacity);
  return;
}

void  TestFacebookCache::onStop()  {
//  if (cache) {
//    delete  cache;
//    cache  =  nullptr;
//  }
  return;
}

void  TestFacebookCache::insert(Elem  *elem)  {
//  cache->insertNode(&(elem->key),
//                    std::make_shared<Elem *>(elem));
  return;
}

bool  TestFacebookCache::exist(Elem  *elem)  {
  bool  re  =  false;
//  std::shared_ptr<Elem *> p_elem  =  cache->getData(&elem->key);
//  if (p_elem  &&  *p_elem.get() == elem) {
//     re  =  true;
//  }

  return  re;
}

const char *  TestFacebookCache::get_algorithm_name()  {
  static ConstString my_name("FacebookCache");
  return  my_name.c_str;
}

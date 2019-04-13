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
  cache  =  new HPHP::ConcurrentLRUCache<TKey *, Elem *, TKey_hash_compare>(capacity);
  return;
}

void  TestFacebookCache::onStop()  {
  if (cache) {
    delete  cache;
    cache  =  nullptr;
  }
  return;
}

void  TestFacebookCache::insert(Elem  *elem)  {
//  cache->insertNode(&(elem->key),
//                    std::make_shared<Elem *>(elem));
  cache->insert(&elem->key, elem);
  return;
}

bool  TestFacebookCache::exist(Elem  *elem)  {
  bool  re  =  false;
//  std::shared_ptr<Elem *> p_elem  =  cache->getData(&elem->key);
//  if (p_elem  &&  *p_elem.get() == elem) {
//     re  =  true;
//  }
  HPHP::ConcurrentLRUCache<TKey *, Elem *, TKey_hash_compare>::ConstAccessor  pipec;
  if  (cache->find(pipec, &elem->key)) {
    Elem  *f  =  *pipec.get();
      if (f == elem) {
         re  =  true;
      }
  }
  return  re;
}

const char *  TestFacebookCache::get_algorithm_name()  {
  static ConstString my_name("FacebookCache");
  return  my_name.c_str;
}

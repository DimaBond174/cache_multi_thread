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

void  TestFacebookCache::insert(void  *elem)  {
  Elem  *e  =  static_cast<Elem *>(elem);
  cache->insert(&e->key, e);
  return;
}

bool  TestFacebookCache::exist(void  *elem)  {
  bool  re  =  false;
  Elem  *e  =  static_cast<Elem *>(elem);
  HPHP::ConcurrentLRUCache<TKey *, Elem *, TKey_hash_compare>::ConstAccessor  pipec;
  if  (cache->find(pipec, &e->key)) {
    Elem  *f  =  *pipec.get();
      if (f == e) {
         re  =  true;
      }
  }
  return  re;
}

const char *  TestFacebookCache::get_algorithm_name()  {
  static ConstString my_name("FacebookCache");
  return  my_name.c_str;
}

#include "testintelcache.h"
#include "spec/specstatic.h"


TestIntelCache::TestIntelCache()
{

}



TestIntelCache::~TestIntelCache() {
  onStop();
}

void  TestIntelCache::onStart(std::shared_ptr<IConfig>  &cfg)  {
  int64_t  capacity  = cfg.get()->getLongValue("capacity");
  assert(capacity > 0);
  onStop() ;
  t_f_key_to_value f = &key_to_value;
  cache  =  new tbb::interface6::concurrent_lru_cache<
            Elem,  Elem,  t_f_key_to_value>(f,  capacity);
  return;
}

void  TestIntelCache::onStop()  {
  if (cache) {
    delete  cache;
    cache  =  nullptr;
  }
  return;
}

void  TestIntelCache::insert(void  *elem)  {
  Elem  *e  =  static_cast<Elem *>(elem);
  (*cache)[e];
  return;
}

bool  TestIntelCache::exist(void  *elem)  {
  // Try №1:
  //bool  re  =  (*cache)[elem];  // This always TRUE
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  // Try №2:
//  Elem fake(elem->key.keyArray[0],
//      elem->key.keyArray[1],
//      elem->key.keyArray[2],
//      "fake");
//  return  0 != (*cache)[fake].value().data.compare("fake"); // This always FALSE
  //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  // I now know why Intel demanded to put #define TBB_PREVIEW_CONCURRENT_LRU_CACHE true
  //But i need only speed from Intel's awesome concurrent_lru_cache:
  Elem  *e  =  static_cast<Elem *>(elem);
  return  (*cache)[e]; // Okey... get me your operator bool()..
  // Thanks concurrent_lru_cache..
}

const char *  TestIntelCache::get_algorithm_name()  {
  static ConstString my_name("IntelCache");
  return  my_name.c_str;
}

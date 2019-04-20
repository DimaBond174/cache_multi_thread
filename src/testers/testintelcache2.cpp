#include "testintelcache2.h"
#include "spec/specstatic.h"


TestIntelCache2::TestIntelCache2()
{

}


TestIntelCache2::~TestIntelCache2() {
  onStop();
}

void  TestIntelCache2::onStart(std::shared_ptr<IConfig>  &cfg)  {
  int64_t  capacity  = cfg.get()->getLongValue("capacity");
  assert(capacity > 0);
  onStop() ;
  t_f_string f = &key_to_value;
  cache  =  new tbb::interface6::concurrent_lru_cache<
            std::string,  std::string,  t_f_string>(f,  capacity);
  return;
}

void  TestIntelCache2::onStop()  {
  if (cache) {
    try {
      //Intel fails here:
      delete  cache;
    } catch (...) {
    }

    cache  =  nullptr;
  }
  return;
}

void  TestIntelCache2::insert(void  *elem)  {
  ElemNSizeKey  *e  =  static_cast<ElemNSizeKey *>(elem);
  (*cache)[e->data];
  return;
}

bool  TestIntelCache2::exist(void  *elem)  {
  ElemNSizeKey  *e  =  static_cast<ElemNSizeKey *>(elem);
  return  (*cache)[e->data]; // Okey... get me your operator bool()..
}

const char *  TestIntelCache2::get_algorithm_name()  {
  static ConstString my_name("IntelCache");
  return  my_name.c_str;
}

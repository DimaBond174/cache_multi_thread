#include "testfacebookcache2.h"
#include "spec/specstatic.h"

TestFacebookCache2::TestFacebookCache2()
{

}

TestFacebookCache2::~TestFacebookCache2() {
  onStop();
}

void  TestFacebookCache2::onStart(std::shared_ptr<IConfig>  &cfg)  {
  int64_t  capacity  = cfg.get()->getLongValue("capacity");
  assert(capacity > 0);
  onStop() ;

  cache  =  new HPHP::ConcurrentLRUCache<std::string, std::string>(capacity);
  return;
}

void  TestFacebookCache2::onStop()  {
  if (cache) {
    delete  cache;
    cache  =  nullptr;
  }
  return;
}

void  TestFacebookCache2::insert(void  *elem)  {
  ElemNSizeKey  *e  =  static_cast<ElemNSizeKey *>(elem);
  cache->insert(e->data, e->data);
  return;
}

bool  TestFacebookCache2::exist(void  *elem)  {
  bool  re  =  false;
  ElemNSizeKey  *e  =  static_cast<ElemNSizeKey *>(elem);
  HPHP::ConcurrentLRUCache<std::string, std::string>::ConstAccessor  pipec;
  if  (cache->find(pipec, e->data)) {    
      if (pipec.get()) {
         re  =  true;
      }
  }
  return  re;
}

const char *  TestFacebookCache2::get_algorithm_name()  {
  static ConstString my_name("FacebookCache");
  return  my_name.c_str;
}

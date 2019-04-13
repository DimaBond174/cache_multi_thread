#ifndef TESTFACEBOOKCACHE_H
#define TESTFACEBOOKCACHE_H
#include  "i/itester.h"
//#include  "caches/facebook/concurrent-lru-cache.h"

class TestFacebookCache : public IAlgorithmTester {
 public:
  TestFacebookCache();
  ~TestFacebookCache()  override;
  void  onStart(std::shared_ptr<IConfig>  &cfg)  override;
  void  onStop()  override;
  void  insert(Elem  *elem)  override;
  bool  exist(Elem  *elem)  override;
  const char *  get_algorithm_name()  override;
 private:

};

#endif // TESTFACEBOOKCACHE_H

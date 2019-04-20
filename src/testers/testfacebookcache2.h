#ifndef TESTFACEBOOKCACHE2_H
#define TESTFACEBOOKCACHE2_H

#include  "i/itester.h"
#include  "caches/facebook/concurrent-lru-cache.h"
#include "i/ikey2.h"
#include <string>
#include <functional>

/*
*  From: https://github.com/facebook/hhvm/blob/master/hphp/util/concurrent-lru-cache.h
*
*/

class TestFacebookCache2 : public IAlgorithmTester {
public:
 TestFacebookCache2();
 ~TestFacebookCache2()  override;
 void  onStart(std::shared_ptr<IConfig>  &cfg)  override;
 void  onStop()  override;
 void  insert(void  *elem)  override;
 bool  exist(void  *elem)  override;
 const char *  get_algorithm_name()  override;
private:
 HPHP::ConcurrentLRUCache<std::string, std::string>  *cache  =  nullptr;
};

#endif // TESTFACEBOOKCACHE2_H

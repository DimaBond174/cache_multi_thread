#ifndef TESTINTELCACHE2_H
#define TESTINTELCACHE2_H

/*
*  From: https://github.com/01org/tbb/blob/2ace525889b0c3de9c90da943fac9259220ef35f/src/test/test_concurrent_lru_cache.cpp
*
*/

#include  "i/itester.h"
#include "i/ikey2.h"

#define TBB_PREVIEW_CONCURRENT_LRU_CACHE true
#include "tbb/concurrent_lru_cache.h"

static  std::string& key_to_value(std::string  &lhv) {
   return lhv;
}

typedef std::string& (*t_f_string) (std::string &);

class TestIntelCache2 : public IAlgorithmTester {
public:
 TestIntelCache2();
 ~TestIntelCache2()  override;
 void  onStart(std::shared_ptr<IConfig>  &cfg)  override;
 void  onStop()  override;
 void  insert(void  *elem)  override;
 bool  exist(void  *elem)  override;
 const char *  get_algorithm_name()  override;
private:
 //template <typename key_type, typename value_type, typename value_functor_type = value_type (*)(key_type) >
 tbb::interface6::concurrent_lru_cache<std::string,  std::string,  t_f_string>  *cache;
 //tbb::concurrent_lru_cache<TKey *, Elem *, TKey_hash_compare>  *cache  =  nullptr;
};

#endif // TESTINTELCACHE2_H

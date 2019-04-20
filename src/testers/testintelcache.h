#ifndef TESTINTELCACHE_H
#define TESTINTELCACHE_H

/*
*  From: https://github.com/01org/tbb/blob/2ace525889b0c3de9c90da943fac9259220ef35f/src/test/test_concurrent_lru_cache.cpp
*
*/

#include  "i/itester.h"
#include "i/ikey.h"

#define TBB_PREVIEW_CONCURRENT_LRU_CACHE true
#include "tbb/concurrent_lru_cache.h"

inline bool  operator<(const Elem  &lhv,  const  Elem  &rhv)  {
  return lhv.key.cmp(&rhv.key) < 0;
}

inline bool  operator<(Elem  &lhv,  Elem  &rhv)  {
  return lhv.key.cmp(&rhv.key) < 0;
}

static  Elem& key_to_value(Elem  &lhv) {
   return lhv;
}

typedef Elem& (*t_f_key_to_value) (Elem &);

class TestIntelCache : public IAlgorithmTester {
 public:
  TestIntelCache();
  ~TestIntelCache()  override;
  void  onStart(std::shared_ptr<IConfig>  &cfg)  override;
  void  onStop()  override;
  void  insert(void  *elem)  override;
  bool  exist(void  *elem)  override;
  const char *  get_algorithm_name()  override;
 private:
  //template <typename key_type, typename value_type, typename value_functor_type = value_type (*)(key_type) >
  tbb::interface6::concurrent_lru_cache<Elem,  Elem,  t_f_key_to_value>  *cache;
  //tbb::concurrent_lru_cache<TKey *, Elem *, TKey_hash_compare>  *cache  =  nullptr;
};

#endif // TESTINTELCACHE_H

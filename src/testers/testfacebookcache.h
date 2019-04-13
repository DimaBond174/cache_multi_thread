#ifndef TESTFACEBOOKCACHE_H
#define TESTFACEBOOKCACHE_H
#include  "i/itester.h"
#include  "caches/facebook/concurrent-lru-cache.h"

/*
*  From: https://github.com/facebook/hhvm/blob/master/hphp/util/concurrent-lru-cache.h
*
*/

inline std::size_t  tbb_hasher(TKey  *key) {
  return  key->hash();
}

inline bool equal( const TKey*  &a, const TKey*  &b )  {
  return  a->cmp(b) == 0;
}

struct TKey_hash_compare {
    static size_t hash( const TKey*  &a ) {
      return  a->hash();
    }

    static bool equal( const TKey*  &a,  const TKey*  &b) {
      return  a->cmp(b) == 0;
    }

    static size_t hash(TKey const * const  &a ) {
      return  a->hash();
    }

    static bool equal( TKey const * const   &a,  TKey const * const   &b) {
      return  a->cmp(b) == 0;
    }
};


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
  HPHP::ConcurrentLRUCache<TKey *, Elem *, TKey_hash_compare>  *cache  =  nullptr;
};

#endif // TESTFACEBOOKCACHE_H

#ifndef TESTONCACHEM_H
#define TESTONCACHEM_H
#include  "i/itester.h"
#include  "caches/oncachem.h"

class TestOnCacheM : public IAlgorithmTester {
 public:
  TestOnCacheM();
  ~TestOnCacheM() override;
  void  onStart(std::shared_ptr<IConfig>  &cfg)  override;
  void  onStop()  override;
  void  insert(Elem  *elem)  override;
  bool  exist(Elem  *elem)  override;
  const char *  get_algorithm_name()  override;
 private:
  OnCacheM<TKey *, Elem *>  *cache  =  nullptr;
};

#endif // TESTONCACHEM_H

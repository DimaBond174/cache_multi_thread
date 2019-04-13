/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

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

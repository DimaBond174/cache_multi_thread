/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef TESTONCACHEMMRU_H
#define TESTONCACHEMMRU_H

#include  "i/itester.h"
#include  "caches/oncachemmru.h"


class TestOnCacheMMRU : public IAlgorithmTester {
 public:
  TestOnCacheMMRU();
  ~TestOnCacheMMRU() override;
  void  onStart(std::shared_ptr<IConfig>  &cfg)  override;
  void  onStop()  override;
  void  insert(Elem  *elem)  override;
  bool  exist(Elem  *elem)  override;
  const char *  get_algorithm_name()  override;
 private:
  OnCacheMMRU<TKey *, Elem *>  *cache  =  nullptr;
};

#endif // TESTONCACHEMMRU_H

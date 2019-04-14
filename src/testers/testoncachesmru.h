/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef TESTONCACHESMRU_H
#define TESTONCACHESMRU_H
#include  "i/itester.h"
#include  "caches/oncachesmru.h"


class TestOnCacheSMRU : public IAlgorithmTester {
 public:
  TestOnCacheSMRU();
  ~TestOnCacheSMRU() override;
  void  onStart(std::shared_ptr<IConfig>  &cfg)  override;
  void  onStop()  override;
  void  insert(Elem  *elem)  override;
  bool  exist(Elem  *elem)  override;
  const char *  get_algorithm_name()  override;
 private:
  OnCacheSMRU<TKey *, Elem *>  *cache  =  nullptr;
};

#endif // TESTONCACHESMRU_H

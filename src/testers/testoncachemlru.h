/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef TESTONCACHEMLRU_H
#define TESTONCACHEMLRU_H

#include  "i/itester.h"
#include  "caches/oncachemlru.h"

class TestOnCacheMLRU : public IAlgorithmTester {
 public:
  TestOnCacheMLRU();
  ~TestOnCacheMLRU() override;
  void  onStart(std::shared_ptr<IConfig>  &cfg)  override;
  void  onStop()  override;
  void  insert(Elem  *elem)  override;
  bool  exist(Elem  *elem)  override;
  const char *  get_algorithm_name()  override;
 private:
  OnCacheMLRU<TKey *, Elem *>  *cache  =  nullptr;
};

#endif // TESTONCACHEMLRU_H

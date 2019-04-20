/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef TESTONCACHESMRU2_H
#define TESTONCACHESMRU2_H
#include  "i/itester.h"
#include  "caches/oncachesmru.h"
#include  "i/ikey2.h"


class TestOnCacheSMRU2 : public IAlgorithmTester {
public:
 TestOnCacheSMRU2();
 ~TestOnCacheSMRU2() override;
 void  onStart(std::shared_ptr<IConfig>  &cfg)  override;
 void  onStop()  override;
 void  insert(void  *elem)  override;
 bool  exist(void  *elem)  override;
 const char *  get_algorithm_name()  override;
private:
 //using TKey  =  NSizeKey<ElemNSizeKey::key_ElemNSizeKey_size>;
 //using Elem  =  ElemNSizeKey;
 OnCacheSMRU<ElemNSizeKey *, ElemNSizeKey *>  *cache  =  nullptr;
};

#endif // TESTONCACHESMRU2_H

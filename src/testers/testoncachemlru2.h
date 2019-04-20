/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef TESTONCACHEMLRU2_H
#define TESTONCACHEMLRU2_H

#include  "i/itester.h"
#include  "caches/oncachemlru.h"
#include "i/ikey2.h"

class TestOnCacheMLRU2 : public IAlgorithmTester {
public:
 TestOnCacheMLRU2();
 ~TestOnCacheMLRU2() override;
 void  onStart(std::shared_ptr<IConfig>  &cfg)  override;
 void  onStop()  override;
 void  insert(void  *elem)  override;
 bool  exist(void  *elem)  override;
 const char *  get_algorithm_name()  override;
private:
 OnCacheMLRU<ElemNSizeKey *, ElemNSizeKey *>  *cache  =  nullptr;
};

#endif // TESTONCACHEMLRU2_H

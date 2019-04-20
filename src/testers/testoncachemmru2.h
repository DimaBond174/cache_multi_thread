/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef TESTONCACHEMMRU2_H
#define TESTONCACHEMMRU2_H

#include  "i/itester.h"
#include  "caches/oncachemmru.h"
#include  "i/ikey2.h"

class TestOnCacheMMRU2 : public IAlgorithmTester {
public:
 TestOnCacheMMRU2();
 ~TestOnCacheMMRU2() override;
 void  onStart(std::shared_ptr<IConfig>  &cfg)  override;
 void  onStop()  override;
 void  insert(void  *elem)  override;
 bool  exist(void  *elem)  override;
 const char *  get_algorithm_name()  override;
private:
 OnCacheMMRU<ElemNSizeKey *, ElemNSizeKey *>  *cache  =  nullptr;
};

#endif // TESTONCACHEMMRU2_H

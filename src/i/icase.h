/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef ITESTCASE_H
#define ITESTCASE_H
#include <string>
#include <memory>
#include "i/isystem.h"
#include "i/iconfig.h"
#include "i/itester.h"

class ITestCase  {
 public:
  ITestCase()  =  default;
  virtual ~ITestCase()  {  }
  virtual  void  prepareTestCase(std::shared_ptr<IConfig>  &cfg, std::shared_ptr<ISystem>  &cur_system)  =  0;
  virtual  void  stop()  =  0;
  virtual  void  addAlgorithmTester_1thread(std::shared_ptr<IAlgorithmTester>  &tester)  =  0;
  virtual  void  addAlgorithmTester_Nthread(std::shared_ptr<IAlgorithmTester>  &tester)  =  0;
  virtual  void  start_1thread_tests()  =  0;
  virtual  void  start_Nthread_tests()  =  0;
 private:
  ITestCase(const  ITestCase&)  =  delete;
  ITestCase& operator=(const  ITestCase&)  =  delete;
};

#endif // ITESTCASE_H

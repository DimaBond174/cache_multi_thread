/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef ITESTER_H
#define ITESTER_H
#include "i/ikey.h"
#include <memory>
#include "i/iconfig.h"

/*
 * Common interface for
 * Property container pattern
 */
class IAlgorithmTester  {
 public:
  IAlgorithmTester()  =  default;
  virtual ~IAlgorithmTester()  {  }
  virtual void  onStart(std::shared_ptr<IConfig>  &cfg)  =  0;
  virtual void  onStop()  =  0;
  virtual void  insert(Elem  *elem)  =  0;
  virtual bool  exist(Elem  *elem)  =  0;
  virtual const char *  get_algorithm_name()  =  0;
 private:
  IAlgorithmTester(const  IAlgorithmTester&)  =  delete;
  IAlgorithmTester& operator=(const  IAlgorithmTester&)  =  delete;
};

#endif // ITESTER_H

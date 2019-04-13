/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef TESTCASE1_H
#define TESTCASE1_H

#include "i/icase.h"
#include <deque>
#include <map>
#include <chrono>
#include <future>

using TimePoint  =  std::chrono::time_point<std::chrono::system_clock>;

class FindResults {
 public:
  int64_t  thread_id;
  int64_t  found;
  int64_t  not_found;
  TimePoint  start;
  TimePoint  stop;
};

class InsertResults {
 public:
  int64_t  thread_id;
  TimePoint  start;
  TimePoint  stop;
};

class TestCase1  :  public ITestCase {
 public:
  TestCase1();
  void  prepareTestCase(std::shared_ptr<IConfig>  &cfg, std::shared_ptr<ISystem>  &cur_system)  override;
  void  stop()  override;
  void  addAlgorithmTester_1thread(std::shared_ptr<IAlgorithmTester>  &tester)  override;
  void  addAlgorithmTester_Nthread(std::shared_ptr<IAlgorithmTester>  &tester)  override;
  void  start_1thread_tests()  override;
  void  start_Nthread_tests()  override;
 private:
  std::shared_ptr<IConfig>  l_cfg;
  std::shared_ptr<ISystem>  l_cur_system;
  std::map<TKey, Elem *>  test_data;
  std::deque<std::shared_ptr<IAlgorithmTester>>  testers_1thread;
  std::deque<std::shared_ptr<IAlgorithmTester>>  testers_Nthread;

  void  loadCaseFromFile();
  void  generateCase();
  void  make_header(int64_t  startSize,  int64_t  testSize,  std::string  *str);
 private:
    FindResults  find_in_thread(int  count,  IAlgorithmTester  *p_tester);
    void  insert_in_thread(int  count,  std::promise<InsertResults>  prom,
      IAlgorithmTester  *p_tester);
};


#endif // TESTCASE1_H

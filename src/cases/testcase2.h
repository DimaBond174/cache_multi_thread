/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */


#ifndef TESTCASE2_H
#define TESTCASE2_H

#include "i/icase.h"
#include "i/ikey2.h"
#include <deque>
#include <chrono>
#include <future>

/*
*  Test case to insert/find intems with key(string)
*/


using TimePoint  =  std::chrono::time_point<std::chrono::system_clock>;

class TestCase2 :  public ITestCase {
public:
 TestCase2();
 void  prepareTestCase(std::shared_ptr<IConfig>  &cfg, std::shared_ptr<ISystem>  &cur_system)  override;
 void  stop()  override;
 void  addAlgorithmTester_1thread(std::shared_ptr<IAlgorithmTester>  &tester)  override;
 void  addAlgorithmTester_Nthread(std::shared_ptr<IAlgorithmTester>  &tester)  override;
 void  start_1thread_tests()  override;
 void  start_Nthread_tests()  override;
private:
 std::shared_ptr<IConfig>  l_cfg;
 std::shared_ptr<ISystem>  l_cur_system;
 std::deque<ElemNSizeKey *>  test_data;
 std::deque<std::shared_ptr<IAlgorithmTester>>  testers_1thread;
 std::deque<std::shared_ptr<IAlgorithmTester>>  testers_Nthread;

 void  make_header(int64_t  startSize,  int64_t  testSize,  std::string  *str);
private:
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

   FindResults  find_in_thread(int  count,  IAlgorithmTester  *p_tester);
   void  insert_in_thread(int  count,  std::promise<InsertResults>  prom,
     IAlgorithmTester  *p_tester);
};

#endif // TESTCASE2_H

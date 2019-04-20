#include <iostream>
#if defined(Linux)
    #include "system/linux/linuxsystem.h"
#elif defined(Windows)
    #include "system/windows/windowssystem.h"
#endif
#include "i/iconfig.h"
#include "configs/commandline/cmdconfig.h"
#include <deque>
#include "i/icase.h"
#include "i/itester.h"
#include "cases/testcase1.h"
#include "cases/testcase2.h"
#include "testers/testfacebookcache.h"
#include "testers/testoncachemlru.h"
#include "testers/testoncachemmru.h"
#include "testers/testoncachesmru.h"
#include "testers/testintelcache.h"
#include "testers/testoncachesmru2.h"
#include "testers/testoncachemmru2.h"
#include "testers/testfacebookcache2.h"
#include "testers/testintelcache2.h"
#include "testers/testoncachemlru2.h"

void  do_tests_with_numbers(std::shared_ptr<IConfig>  &cfg,
     std::shared_ptr<ISystem>  &cur_system);
void  do_tests_with_strings(std::shared_ptr<IConfig>  &cfg,
     std::shared_ptr<ISystem>  &cur_system);

int main(int  argc,  char  **argv)  {
  std::shared_ptr<ISystem>  cur_system =
#if defined(Linux)
      std::make_shared<LinuxSystem>();
#elif defined(Windows)
      std::make_shared<WindowsSystem>();
#endif
  //TODO need i align TONode  to cache line size?
  std::cout << "CACHE_LINE_SIZE = "
            << cur_system.get()->get_CPU_cache_line_size() << '\n';
  std::cout << "Size of shared_mutex =" << sizeof(std::shared_mutex) << '\n';

  //  Config uses Delegation pattern:
  // <- Command line args <- ./assets/settings.json <- Hardcoded settings
  std::shared_ptr<IConfig>  cur_config =
    std::make_shared<CmdConfig>();
  cur_config.get()->loadConfig(argc,  argv,  cur_system);

  //  Test cases with numbers (ikey.h):
  do_tests_with_numbers(cur_config,  cur_system);

  //  Test cases with strings (ikey2.h):
  do_tests_with_strings(cur_config,  cur_system);

  std::cout << "\nEnd of tests!\n";
}

void  do_tests_with_numbers(std::shared_ptr<IConfig>  &cfg,
    std::shared_ptr<ISystem>  &cur_system)  {
  //  Test cases:
  std::deque<std::shared_ptr<ITestCase>>  cases_list;
  cases_list.emplace_back(std::make_shared<TestCase1>());

  //  Testers:
  // SINGLE THREADED:
  std::deque<std::shared_ptr<IAlgorithmTester>>  algorithms_list_singl;
  algorithms_list_singl.emplace_back(std::make_shared<TestOnCacheSMRU>());

  // MULTI THREADED:
  std::deque<std::shared_ptr<IAlgorithmTester>>  algorithms_list_both;
  algorithms_list_both.emplace_back(std::make_shared<TestFacebookCache>());
  algorithms_list_both.emplace_back(std::make_shared<TestOnCacheMMRU>());
  algorithms_list_both.emplace_back(std::make_shared<TestOnCacheMLRU>());
  algorithms_list_both.emplace_back(std::make_shared<TestIntelCache>());

  //  Run tests:
  for (auto&& it_case :  cases_list) {
    ITestCase  *p_case  =  it_case.get();
    p_case->prepareTestCase(cfg,  cur_system);
    //Single threaded:
    for (auto&& it_algorithm :  algorithms_list_singl) {
      p_case->addAlgorithmTester_1thread(it_algorithm);
    }
    //Both, thread safe:
    for (auto&& it_algorithm :  algorithms_list_both) {
      p_case->addAlgorithmTester_1thread(it_algorithm);
      p_case->addAlgorithmTester_Nthread(it_algorithm);
    }
    //Begin tests:
    p_case->start_1thread_tests();
    p_case->start_Nthread_tests();
    p_case->stop();
  }
}  //  do_tests_with_numbers

void  do_tests_with_strings(std::shared_ptr<IConfig>  &cfg,
    std::shared_ptr<ISystem>  &cur_system)  {
  //  Test cases:
  std::deque<std::shared_ptr<ITestCase>>  cases_list;
  cases_list.emplace_back(std::make_shared<TestCase2>());

  //  Testers:
  // SINGLE THREADED:
  std::deque<std::shared_ptr<IAlgorithmTester>>  algorithms_list_singl;
  algorithms_list_singl.emplace_back(std::make_shared<TestOnCacheSMRU2>());

  // MULTI THREADED:
  std::deque<std::shared_ptr<IAlgorithmTester>>  algorithms_list_both;
  algorithms_list_both.emplace_back(std::make_shared<TestFacebookCache2>());
  algorithms_list_both.emplace_back(std::make_shared<TestOnCacheMMRU2>());
  algorithms_list_both.emplace_back(std::make_shared<TestOnCacheMLRU2>());
 //Intel fails on Deallocate: algorithms_list_both.emplace_back(std::make_shared<TestIntelCache2>());

  //  Run tests:
  for (auto&& it_case :  cases_list) {
    ITestCase  *p_case  =  it_case.get();
    p_case->prepareTestCase(cfg,  cur_system);
    //Single threaded:
    for (auto&& it_algorithm :  algorithms_list_singl) {
      p_case->addAlgorithmTester_1thread(it_algorithm);
    }
    //Both, thread safe:
    for (auto&& it_algorithm :  algorithms_list_both) {
      p_case->addAlgorithmTester_1thread(it_algorithm);
      p_case->addAlgorithmTester_Nthread(it_algorithm);
    }
    //Begin tests:
    p_case->start_1thread_tests();
    p_case->start_Nthread_tests();
    p_case->stop();
  }
}  // do_tests_with_strings

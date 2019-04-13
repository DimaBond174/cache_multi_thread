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
#include "testers/testoncachem.h"


int main(int  argc,  char  **argv)  {
  std::shared_ptr<ISystem>  cur_system =
#if defined(Linux)
      std::make_shared<LinuxSystem>();
#elif defined(Windows)
      std::make_shared<WindowsSystem>();
#endif
  std::cout << "CACHE_LINE_SIZE = "
            << cur_system.get()->get_CPU_cache_line_size() << '\n';
  std::cout << "Size of shared_mutex =" << sizeof(std::shared_mutex) << '\n';


  //  Config uses Delegation pattern:
  // <- Command line args <- ./assets/settings.json <- Hardcoded settings
  std::shared_ptr<IConfig>  cur_config =
    std::make_shared<CmdConfig>();
  cur_config.get()->loadConfig(argc,  argv,  cur_system);
  //std::cout << cur_config.get()->getStringValue("TestResultsDir");

  //  Test cases:
  std::deque<std::shared_ptr<ITestCase>>  cases_list;
  cases_list.emplace_back(std::make_shared<TestCase1>());

  //  Testers:
  std::deque<std::shared_ptr<IAlgorithmTester>>  algorithms_list;
  algorithms_list.emplace_back(std::make_shared<TestOnCacheM>());

  //  Run tests:
  for (auto&& it_case :  cases_list) {
    ITestCase  *p_case  =  it_case.get();
    p_case->prepareTestCase(cur_config,  cur_system);
    for (auto&& it_algorithm :  algorithms_list) {
      p_case->addAlgorithmTester_1thread(it_algorithm);
    }
    //p_case->start_1thread_tests();
    p_case->start_Nthread_tests();
    p_case->stop();
  }

  std::cout << "\nEnd of tests!\n";
}

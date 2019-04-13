/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#include "testcase1.h"

#include <math.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include "spec/timetools.h"
#include "spec/spectools.h"

TestCase1::TestCase1 ()
{

}

void  TestCase1::stop()  {
  for (auto&& it : test_data) {
       delete it.second;
  }
  test_data.clear();
}

void  TestCase1::prepareTestCase(std::shared_ptr<IConfig>  &cfg, std::shared_ptr<ISystem>  &cur_system)  {
  l_cfg  =  cfg;
  l_cur_system  =  cur_system;
  int64_t  fromFile  =  l_cfg.get()->getLongValue("LOAD_DATA_FROM_TEST_CASE_FILE");
  if  (fromFile)  {
    loadCaseFromFile();
  }  else  {
    generateCase();
  }
}

void  TestCase1::loadCaseFromFile()  {
  const std::string  &testCase  =  l_cur_system.get()->
      load_text_file(l_cfg.get()->getStringValue("FileWithTestData").c_str());
  const char  *begin  =  testCase.c_str();
  const char  *end  =  begin  +  testCase.length();
  uint32_t  i  =  0;
  while (begin  <  end)  {
    int  r  =  stoll(begin,  end);
    Elem  *e  =  new Elem(r,  r,  r,  std::to_string(r));
    test_data.emplace(std::make_pair(e->key,  e));
    ++i;
  }
  std::cout << "Test data has loaded from file, items count = "
            << test_data.size() << '\n';
}

void  TestCase1::generateCase()  {
  srand(std::time(nullptr));
  int64_t  testSize  =  l_cfg.get()->getLongValue("TEST_MAXSIZE");
  std::string testCase;
  for (int64_t  i  =  0;  i  <=  testSize;  ++i)  {
    uint64_t  r  =  rand();
    testCase.append(std::to_string(r)).push_back(',');
    Elem  *e  =  new Elem(r,  r,  r,  std::to_string(r));
    test_data.emplace(std::make_pair(e->key,  e));
  }

  l_cur_system.get()->save_text_file(l_cfg.get()->getStringValue("FileWithTestData").c_str(),
      testCase.c_str(),  testCase.length() );

  std::cout << "Test data has random generated, items count = "
            << test_data.size() << '\n';
}

void  TestCase1::addAlgorithmTester_1thread(std::shared_ptr<IAlgorithmTester>  &tester)  {
  testers_1thread.push_back(tester);
}

void  TestCase1::addAlgorithmTester_Nthread(std::shared_ptr<IAlgorithmTester>  &tester)  {
  testers_Nthread.push_back(tester);
}

void  TestCase1::make_header(int64_t  startSize,  int64_t  testSize,  std::string  *str)  {
  str->clear();
  //  make header
  str->append("Test name");
  while (startSize  <=  testSize)  {
    str->append(",").append(std::to_string(startSize));
    startSize  *= 10;
  }
  str->push_back('\n');
}  // make_header


void  TestCase1::start_1thread_tests()  {
  std::string  str(l_cfg.get()->getStringValue("TestResultsDir"));
  auto  start  =  std::chrono::system_clock::now();
  int64_t  s  =  std::chrono::duration_cast<std::chrono::seconds>(
      start.time_since_epoch()).count();
  str.append("/compare.1thread.")
            .append(std::to_string(s))
            .append(".csv");
  l_cur_system.get()->mkdirs_for_file(str.c_str());
  std::ofstream  csvfile (str);
  int64_t  startSize  = 100;//l_cfg.get()->getLongValue("START_LOGPOINT");
  int64_t  testSize  = 1000;//l_cfg.get()->getLongValue("TEST_MAXSIZE");
  //int64_t  capacity  =  USHRT_MAX + 1;
  //l_cfg.get()->setValue("capacity",  std::to_string(capacity));
  make_header(startSize,  testSize,  &str);
  csvfile.write (str.c_str(), str.length());
  std::string  strFind;
  for (auto&&  it_tester  :  testers_1thread)  {    
    IAlgorithmTester  *p_tester  =  it_tester.get();    

    str.clear();
    str.append(p_tester->get_algorithm_name())
              .append(".1thread.insert (micros)");
    strFind.clear();
    strFind.append(p_tester->get_algorithm_name())
              .append(".1thread.find (micros)");
    int64_t  curSize  =  startSize;
    int64_t  capacity  =  curSize / 10;
    while (curSize  <=  testSize)  {
      l_cfg.get()->setValue("capacity",  std::to_string(capacity));
      p_tester->onStart(l_cfg);
      //Insert:
      int64_t  i  =  0;
      auto  start  =  std::chrono::high_resolution_clock::now();
      for (auto&&  it  :  test_data)  {
        Elem  *ptr  =  it.second;
    //    if (ptr->key.keyArray[0] == 215343422) {
    //       std::cout<<"\n";
    //    }
        p_tester->insert(ptr) ;
        ++i;
        if  (i  >=  curSize)  {
          break;
        }
      }  //  for
      auto  end  =  std::chrono::high_resolution_clock::now();
      int64_t  ms  =  std::chrono::duration_cast<std::chrono::microseconds>(
                      end - start).count();
      str.append(",").append(std::to_string(ms));

      //Find:
      i  =  0;
      int64_t  notFound  =  0;
      int64_t  found  =  0;
      start  =  std::chrono::high_resolution_clock::now();
      for (auto&&  it  :  test_data)  {
        Elem  *ptr  =  it.second;
        if  (p_tester->exist(ptr) )  {
          ++found;
        }  else  {
          ++notFound;
        }
        ++i;
        if  (i  >=  curSize)  {
          break;
        }
      }  //  for
      end  =  std::chrono::high_resolution_clock::now();
      ms  =  std::chrono::duration_cast<std::chrono::microseconds>(
                            end - start).count();
      strFind.append(",").append(std::to_string(ms));

      std::cout  <<  "found:"  <<  found  <<  " ,ERROR(NotFound):"  <<  notFound  <<  '\n';

      capacity  =  curSize;
      curSize  *= 10;
      p_tester->onStop();
    }  //while (curSize  <=  testSize)
    str.push_back('\n');
    strFind.push_back('\n');
    csvfile.write (str.c_str(), str.length());
    csvfile.write (strFind.c_str(), strFind.length());

  } // for (auto&&  it_tester
  csvfile.close();
  std::cout << "\nThe end of TestCase1.start_1thread_tests().\n";
} // start_1thread_tests

void  TestCase1::start_Nthread_tests()  {
  std::string  str(l_cfg.get()->getStringValue("TestResultsDir"));
  auto  start  =  std::chrono::system_clock::now();
  int64_t  s  =  std::chrono::duration_cast<std::chrono::seconds>(
      start.time_since_epoch()).count();
  str.append("/compare.Nthread.")
            .append(std::to_string(s))
            .append(".csv");
  l_cur_system.get()->mkdirs_for_file(str.c_str());
  std::ofstream  csvfile (str);
  int64_t  startSize  = 100;//l_cfg.get()->getLongValue("START_LOGPOINT");
  int64_t  testSize  = 1000;//l_cfg.get()->getLongValue("TEST_MAXSIZE");
  int64_t  cnt_insert_threads  = 2;//l_cfg.get()->getLongValue("The number of threads to insert");
  int64_t  cnt_find_threads  = 2;//l_cfg.get()->getLongValue("The number of threads to find");
  std::cout << "Multi threaded test:"
    <<"\nThe number of threads to insert = "  <<  cnt_insert_threads
    <<"\nThe number of threads to find = "  <<  cnt_find_threads  <<'\n';

  make_header(startSize,  testSize,  &str);
  csvfile.write (str.c_str(), str.length());

  for (auto&&  it_tester  :  testers_Nthread)  {
    IAlgorithmTester  *p_tester  =  it_tester.get();

    str.clear();
    str.append(p_tester->get_algorithm_name())
              .append(".Nthread.insert&find (micros)");

    int64_t  curSize  =  startSize;
    int64_t  capacity  =  curSize / 10;
    while (curSize  <=  testSize)  {
      l_cfg.get()->setValue("capacity",  std::to_string(capacity));
      p_tester->onStart(l_cfg);
      std::cout << "\n==========================\n"
                  <<  "Test for "  <<  curSize << " items started\n"
                  << "==========================\n";

      //WarmUp  &&  Insert:
      int64_t  i  =  0;      
      for (auto&&  it  :  test_data)  {
        Elem  *ptr  =  it.second;
        p_tester->insert(ptr) ;
        ++i;
        if  (i  >=  curSize)  {
          break;
        }
      }  //  for

    std::deque<std::future<InsertResults>>  fut_insert_results;
    std::deque<std::future<FindResults>>  fut_find_results;
    std::deque<std::thread>  threads;

    //Prepare insert threads:
    for (i  = cnt_insert_threads;  i;  --i)  {
      std::promise<InsertResults>  prom;
      fut_insert_results.emplace_back(prom.get_future());
      threads.emplace_back(std::thread (&TestCase1::insert_in_thread,
          this,  curSize,  std::move(prom),  p_tester));
    } // for insert

    //Prepare find threads:
    for (i  = cnt_find_threads;  i;  --i)  {
      std::packaged_task<FindResults(TestCase1  *i, int,  IAlgorithmTester  *)> ta(
            [](TestCase1  *i, int  count,  IAlgorithmTester  *p_tester){
         return i->find_in_thread(count,  p_tester);
      });

      fut_find_results.emplace_back(ta.get_future());
      threads.emplace_back(
        std::thread (std::move(ta),  this,  curSize,  p_tester));
    } // for insert

//    //Banzai!!!
    auto  start  =  std::chrono::high_resolution_clock::now();
    l_cur_system.get()->signal_semaphore(cnt_find_threads  +  cnt_insert_threads);
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
    auto  end  =  std::chrono::high_resolution_clock::now();

    //Show results:
    std::for_each(fut_insert_results.begin(), fut_insert_results.end(),
      [](auto&&  res){
        res.wait();
        auto&&  insert_result  =  res.get();
        std::cout << "Insert Thread id[" << insert_result.thread_id << "]:\n"
          << "started at: " << chronos_to_string(insert_result.start) << '\n'
          << "stopped at: " << chronos_to_string(insert_result.stop) << '\n'
          << "current time: " <<  return_current_time_and_date() << '\n'
          << "work time (microseconds): " <<  std::chrono::duration_cast<std::chrono::microseconds>(
               insert_result.stop  -  insert_result.start).count() << "\n\n";
    });
    std::cout << "-------------------------------------------\n";
    std::for_each(fut_find_results.begin(), fut_find_results.end(),
      [](auto&&  res){
        res.wait();
        auto&&  find_result  =  res.get();
        std::cout << "Find Thread id[" << find_result.thread_id << "]:\n"
          << "started at: " << chronos_to_string(find_result.start) << '\n'
          << "stopped at: " << chronos_to_string(find_result.stop) << '\n'
          << "work time (microseconds): " <<  std::chrono::duration_cast<std::chrono::microseconds>(
               find_result.stop  -  find_result.start).count() << '\n'
          << "found : " << find_result.found << '\n'
          << "not_found : " << find_result.not_found << "\n\n";
    });

    int64_t  ms   =  std::chrono::duration_cast<std::chrono::microseconds>(
                            end - start).count();
    str.append(",").append(std::to_string(ms));


      capacity  =  curSize;
      curSize  *= 10;
      p_tester->onStop();
    }  //while (curSize  <=  testSize)
    str.push_back('\n');    
    csvfile.write (str.c_str(), str.length());    

  } // for (auto&&  it_tester
  csvfile.close();
  std::cout << "\nThe end of TestCase1.start_Nthread_tests().\n";  
}  // start_Nthread_tests


FindResults  TestCase1::find_in_thread(int  count,
    IAlgorithmTester  *p_tester)  {
  FindResults  re;
  re.thread_id  =  spec::to_int64t(std::this_thread::get_id());
  l_cur_system.get()->wait_semaphore();
  re.start  =  std::chrono::system_clock::now();
  std::this_thread::yield();
  int64_t  notFound  =  0;
  int64_t  found  =  0;
  int64_t  i  =  0;
  for (auto&&  it  :  test_data)  {
    Elem  *ptr  =  it.second;
    if  (p_tester->exist(ptr) )  {
      ++found;
    }  else  {
      ++notFound;
    }
    ++i;
    if  (i  >=  count)  {
      break;
    }
  }  //  for
  re.stop  =  std::chrono::system_clock::now();
  re.found  =  found;
  re.not_found  =  notFound;
  return  re;
}


void  TestCase1::insert_in_thread(int  count,
    std::promise<InsertResults>  prom,  IAlgorithmTester  *p_tester)  {
  InsertResults  re;
  re.thread_id  =  spec::to_int64t(std::this_thread::get_id());    
  l_cur_system.get()->wait_semaphore();
  re.start  =  std::chrono::system_clock::now();
  std::this_thread::yield();
  int64_t  i  =  0;
  for (auto&&  it  :  test_data)  {
    Elem  *ptr  =  it.second;
    p_tester->insert(ptr) ;
    ++i;
    if  (i  >=  count)  {
      break;
    }
  }  //  for
  re.stop  =  std::chrono::system_clock::now();
  prom.set_value_at_thread_exit(re);
  return;
}

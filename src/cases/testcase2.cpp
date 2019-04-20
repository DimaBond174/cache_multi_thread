/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */
#include "testcase2.h"

#include <math.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include "spec/timetools.h"
#include "spec/spectools.h"

TestCase2::TestCase2()
{

}


void  TestCase2::stop()  {
  for (auto&& it : test_data) {
    ElemNSizeKey * ptr  =  it;
    delete  ptr;
  }
  test_data.clear();
}

void  TestCase2::prepareTestCase(std::shared_ptr<IConfig>  &cfg, std::shared_ptr<ISystem>  &cur_system)  {
  l_cfg  =  cfg;
  l_cur_system  =  cur_system;
  const  std::string  &fromFile  =  l_cfg.get()->getStringValue("TestCase2");  
  if  (!fromFile.empty())  {
    const std::string  &testCase  =  l_cur_system.get()->
        load_text_file(l_cfg.get()->getStringValue("TestCase2").c_str());
    const char *ch  =  testCase.c_str();
    const char *end  =  ch  +  testCase.length();
    std::string  str;
    str.reserve(ElemNSizeKey::key_ElemNSizeKey_size);
    int64_t  testSize  =  l_cfg.get()->getLongValue("TEST_MAXSIZE");
    int64_t  curSize  =  0;
    while (ch  <  end  &&  curSize < testSize) {
      if (0x0A  ==  *ch  ||  '.'  ==  *ch)  {
        if (str.length() > 0) {
          test_data.emplace_back(new ElemNSizeKey(str));
          str.clear();
          ++curSize;
        }
      }  else  {
        str.push_back(*ch);
      }
      ++ch;
    }
  }
  std::cout  <<  "Number of strings loaded: "  << test_data.size()   <<  '\n';
}


void  TestCase2::addAlgorithmTester_1thread(std::shared_ptr<IAlgorithmTester>  &tester)  {
  testers_1thread.push_back(tester);
}

void  TestCase2::addAlgorithmTester_Nthread(std::shared_ptr<IAlgorithmTester>  &tester)  {
  testers_Nthread.push_back(tester);
}

void  TestCase2::make_header(int64_t  startSize,  int64_t  testSize,  std::string  *str)  {
  str->clear();
  //  make header
  str->append("Test name");
  while (startSize  <=  testSize)  {
    str->append(",").append(std::to_string(startSize));
    startSize  *= 10;
  }
  str->push_back('\n');
}  // make_header


void  TestCase2::start_1thread_tests()  {
  std::string  str(l_cfg.get()->getStringValue("TestResultsDir"));
  auto  start  =  std::chrono::system_clock::now();
  int64_t  s  =  std::chrono::duration_cast<std::chrono::seconds>(
      start.time_since_epoch()).count();
  str.append("/compare.TestCase2.1thread.")
            .append(std::to_string(s))
            .append(".csv");
  l_cur_system.get()->mkdirs_for_file(str.c_str());
  std::ofstream  csvfile (str);
  int64_t  startSize  =  l_cfg.get()->getLongValue("START_LOGPOINT");
  int64_t  testSize  =  l_cfg.get()->getLongValue("TEST_MAXSIZE");
  if (testSize  >  test_data.size()) {  testSize  =  test_data.size();  }
  std::cout  <<  "TEST_MAXSIZE will be: "  << testSize  <<  '\n';

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
    std::cout << "\n@@@@@@@@@@@@@@@@@@@@@\n"
                <<  "Algorithm_name : "  <<  p_tester->get_algorithm_name()
                << "\n@@@@@@@@@@@@@@@@@@@@@\n";
    int64_t  curSize  =  startSize;
    int64_t  capacity  =  curSize / 10;
    while (curSize  <=  testSize)  {
      l_cfg.get()->setValue("capacity",  std::to_string(capacity));
      p_tester->onStart(l_cfg);
      //Insert:
      int64_t  i  =  0;
      auto  start  =  std::chrono::high_resolution_clock::now();
      for (auto&&  it  :  test_data)  {
        ElemNSizeKey  *ptr  =  it;
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
        ElemNSizeKey  *ptr  =  it;
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
  std::cout << "\nThe end of TestCase2.start_1thread_tests().\n";
} // start_1thread_tests

void  TestCase2::start_Nthread_tests()  {
  std::string  str(l_cfg.get()->getStringValue("TestResultsDir"));
  auto  start  =  std::chrono::system_clock::now();
  int64_t  s  =  std::chrono::duration_cast<std::chrono::seconds>(
      start.time_since_epoch()).count();
  str.append("/compare.TestCase2.Nthread.")
            .append(std::to_string(s))
            .append(".csv");
  l_cur_system.get()->mkdirs_for_file(str.c_str());
  std::ofstream  csvfile (str);
  int64_t  startSize  =  l_cfg.get()->getLongValue("START_LOGPOINT");
  int64_t  testSize  =  l_cfg.get()->getLongValue("TEST_MAXSIZE");
  if (testSize  >  test_data.size()) {  testSize  =  test_data.size();  }
  int64_t  cnt_insert_threads  =  l_cfg.get()->getLongValue("The number of threads to insert");
  int64_t  cnt_find_threads  =  l_cfg.get()->getLongValue("The number of threads to find");
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

    std::cout << "\n@@@@@@@@@@@@@@@@@@@@@\n"
                <<  "Algorithm_name : "  <<  p_tester->get_algorithm_name()
                << "\n@@@@@@@@@@@@@@@@@@@@@\n";

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
        ElemNSizeKey  *ptr  =  it;
        p_tester->insert(ptr) ;
        ++i;
        if  (i  >=  capacity)  {
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
      threads.emplace_back(std::thread (&TestCase2::insert_in_thread,
          this,  curSize,  std::move(prom),  p_tester));
    } // for insert

    //Prepare find threads:
    for (i  = cnt_find_threads;  i;  --i)  {
      std::packaged_task<FindResults(TestCase2  *i, int,  IAlgorithmTester  *)> ta(
            [](TestCase2  *i, int  count,  IAlgorithmTester  *p_tester){
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
  std::cout << "\nThe end of TestCase2.start_Nthread_tests().\n";
}  // start_Nthread_tests


TestCase2::FindResults  TestCase2::find_in_thread(int  count,
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
    ElemNSizeKey  *ptr  =  it;
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


void  TestCase2::insert_in_thread(int  count,
    std::promise<InsertResults>  prom,  IAlgorithmTester  *p_tester)  {
  InsertResults  re;
  re.thread_id  =  spec::to_int64t(std::this_thread::get_id());
  l_cur_system.get()->wait_semaphore();
  re.start  =  std::chrono::system_clock::now();
  std::this_thread::yield();
  int64_t  i  =  0;
  for (auto&&  it  :  test_data)  {
    ElemNSizeKey  *ptr  =  it;
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

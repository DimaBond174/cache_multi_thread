# Concurrent caching algorithms.

You are in the test bench repository where the fastest algorithms for multithreaded data caching are compared. If you have your own variant of a thread-safe cache, please send me a link to its C/C++ implementation (specnet.messenger@gmail.com) so that I can add it to the tests.

# Current test results.
## Key-string search :

![TestCase2.1thread](https://github.com/DimaBond174/cache_multi_thread/wiki/TestCase2.1thread.png)

## Key - number array search :

![TestCase1.1thread](https://github.com/DimaBond174/cache_multi_thread/wiki/TestCase1.1thread.png)


#### Step-by-step instructions on how to run a test using source code / ready-made compiled programs,
 bench description, description of caching algorithms [here: WiKi](https://github.com/DimaBond174/cache_multi_thread/wiki)

---

## Quick Start to use cache OnCache*.h :

To use OnCache * .h series templates, you need to  implement 2 methods used to search for an item by key:

1. Hash function:  inline uint64_t  get_hash(YourKey). For example, calculating a hash on a string, I cast the pointer std :: string.c_str() to an array[3] of 64-bit unsigned integers and put them together - these calculations are enough to decompose the elements into baskets of the hash-table:

```c++
  inline uint64_t  get_hash(const ElemNSizeKey  *lhv) {
    uint64_t re  =  0;
    if  (lhv->key.keyArray)  {
      for (int  i = 0;  i  <  lhv->key.keyLongSize;  ++i)  {
          re  += lhv->key.keyArray[i];
      }
    }
    return  (re < 9223372036854775807ll)?  re  :  (re >> 1);  
  }
```

2. Comparison function:  inline int compare (YourKey lhv,YourKey rhv). For exmaple, for std::string first compare casted to uint64_t[3] first letters and if they are equal, next call to std::string.compare(). So we are comparing by 8 bytes on each step versus 1 byte by 1 byte:


```c++
  inline int compare (const ElemNSizeKey  *lhv,
                      const ElemNSizeKey  *rhv)  {
    if  (lhv->key.keyArray)  {
      for (int  i  = lhv->key.keyLongSize_1;  i  >=  0;  --i) {
        if (lhv->key.keyArray[i]  >  rhv->key.keyArray[i])  return  1;
        if (lhv->key.keyArray[i]  <  rhv->key.keyArray[i])  return  -1;
      }
    }  else if (rhv->key.keyArray) {
      return  -1;
    }
    return lhv->data.compare(rhv->data);
  }
```

And the most important thing - your Key should work as a pointer (allows =nullptr):


```c++
    void  clear() {
       key  =  nullptr;
   // ...
    }
```

To make the key self-destruct when evicting a cached item, make it part of a data object or serve as an independent shared_ptr:

```c++
std::shared_ptr<YourKey> key;
```

You can see examples of use in tester classes:

Single threaded fastest:

[TestOnCacheSMRU](https://github.com/DimaBond174/cache_multi_thread/blob/master/src/testers/testoncachesmru.h)

[TestOnCacheSMRU2](https://github.com/DimaBond174/cache_multi_thread/blob/master/src/testers/testoncachesmru2.h)


Multithreaded thread safe:

[TestOnCacheMMRU](https://github.com/DimaBond174/cache_multi_thread/blob/master/src/testers/testoncachemmru.h)

[TestOnCacheMMRU2](https://github.com/DimaBond174/cache_multi_thread/blob/master/src/testers/testoncachemmru2.h)

[TestOnCacheMLRU](https://github.com/DimaBond174/cache_multi_thread/blob/master/src/testers/testoncachemlru.h)

[TestOnCacheMLRU2](https://github.com/DimaBond174/cache_multi_thread/blob/master/src/testers/testoncachemlru2.h)


How to call methods and work in parallel threads:

[TestCase1](https://github.com/DimaBond174/cache_multi_thread/blob/master/src/cases/testcase1.cpp)

[TestCase2](https://github.com/DimaBond174/cache_multi_thread/blob/master/src/cases/testcase2.cpp)


Graphs of the results of multi-threaded testing cite [here at the end of the WiKi page](https://github.com/DimaBond174/cache_multi_thread/wiki/3.-Development-of-caching-algorithms).

---
  Copyright (c) Dmitriy Bondarenko
  feel free to contact me: specnet.messenger@gmail.com



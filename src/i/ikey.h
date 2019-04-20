/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef IKEY_H
#define IKEY_H

#include <stdlib.h>
#include <stdint.h>
#include <string>

#define SKIPHEIGHT 5
#define SKIPHEIGHT_H 2
#define SIZEAllocLeaf 256


typedef void (*TDeletDataFunc)(const char *data);

//TODO any size key implementation:
template<typename T>
union AnySizeKey {
    T key;
    static constexpr int keyLongSize = ((sizeof(T)/sizeof(uint64_t))+ (sizeof(T)%sizeof(uint64_t)>0?1:0));
    uint64_t keyArray[keyLongSize];
    int cmp(AnySizeKey const & other ) {
        for (int i=0; i<keyLongSize; ++i) {
            if (keyArray[i]>other.keyArray[i]) return 1;
            if (keyArray[i]<other.keyArray[i]) return -1;
        }
        return 0;
    }
    uint64_t  hash() const  {
      const uint64_t re  =  0;
      for (int  i = 0;  i  <  keyLongSize;  ++i)  {
        re  += keyArray[i];
      }
      return  (re < 9223372036854775807ll)?  re  :  (re >> 1);
    }
};


class TKey  {
 public:  
  TKey(uint64_t  p0  =  0,  uint64_t  p1  =  0,  uint64_t  p2  =  0)  {
    keyArray[0]  =  p0;
    keyArray[1]  =  p1;
    keyArray[2]  =  p2;
  }

  TKey  & operator=(TKey const  &rhl)  {
    keyArray[0]  =  rhl.keyArray[0];
    keyArray[1]  =  rhl.keyArray[1];
    keyArray[2]  =  rhl.keyArray[2];
    return  *this;
  }

  TKey(const  TKey  &rhl)  {
    keyArray[0]  =  rhl.keyArray[0];
    keyArray[1]  =  rhl.keyArray[1];
    keyArray[2]  =  rhl.keyArray[2];
  }

  bool  operator==(TKey const  &rhl) const  {
    return  keyArray[0]  ==  rhl.keyArray[0]
            && keyArray[1]  ==  rhl.keyArray[1]
            && keyArray[2]  ==  rhl.keyArray[2];
  }

  static bool  do_worst_case;
  static constexpr int  keyLongSize  =  3;
  uint64_t  keyArray[keyLongSize];
  uint64_t  hash() const  {
    const uint64_t re  =  keyArray[0]  +  keyArray[1]  +  keyArray[2];
    if  (do_worst_case)  {
      return  (re % 1000);
    }
    return  (re < 9223372036854775807ll)?  re  :  (re >> 1);
  }

  int  cmp(TKey const  *other)  const  {
    if  (keyArray[2]  >  other->keyArray[2])  return  1;
    if  (keyArray[2]  <  other->keyArray[2])  return  -1;
    if  (keyArray[1]  >  other->keyArray[1])  return  1;
    if  (keyArray[1]  <  other->keyArray[1])  return  -1;
    if  (keyArray[0]  >  other->keyArray[0])  return  1;
    if  (keyArray[0]  <  other->keyArray[0])  return  -1;
    return 0;
  }

  bool operator<(const TKey  &r)  const  {
    if  (keyArray[0]  <  r.keyArray[0])  return true;
    if  (keyArray[0]  >  r.keyArray[0])  return false;
    if  (keyArray[1]  <  r.keyArray[1])  return true;
    if  (keyArray[1]  >  r.keyArray[1])  return false;
    if  (keyArray[2]  <  r.keyArray[2])  return true;
    if  (keyArray[2]  >  r.keyArray[2])  return false;
        // Otherwise both are equal
    return false;
  }
};  //  TKey



//inline uint qHash(const TKey *key, uint seed = 0)
//{
//    return key->hash() ^ seed;
//}

//inline uint qHash(TKey key, uint seed = 0)
//{
//    return key.hash() ^ seed;
//}

//struct THash {
//  _GLIBCXX_PURE size_t
//  operator()(const TKey& key) const noexcept {
//    return key.hash();
//  }
//};


  struct TEqual
  {
   bool operator()(const TKey&  l, const TKey&  r) const noexcept {
     return 0  ==  l.cmp(&r);
   }
  };

  struct  Elem  {
    Elem() {}
    Elem(uint64_t  p0,  uint64_t  p1,  uint64_t  p2,  std::string  &&_data)
          : key(p0,  p1,  p2),  data(std::move(_data)) { }
    Elem(const Elem  &other)  :  data(other.data),  key(other.key)  {  }
    Elem(const Elem  *other)  :  data(other->data),  key(other->key)  {  }
    Elem & operator=(const  Elem &rhv)  {
      if (&rhv  !=  this) {
        data  =  rhv.data;
        key  =  rhv.key;
      }
      return *this;
    }

    TKey key;
    std::string  data;
  };

  inline int compare (const TKey  *lhv,  const TKey  *rhv) {
    return  lhv->cmp(rhv);
  }

  inline uint64_t  get_hash(const TKey  *lhv) {
    return lhv->hash();
  }
#endif // IKEY_H

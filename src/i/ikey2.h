/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef IKEY2_H
#define IKEY2_H

#include <stdlib.h>
#include <stdint.h>
#include <string>

#define SKIPHEIGHT 5
#define SKIPHEIGHT_H 2
#define SIZEAllocLeaf 256

/*
 * NSizeKey must be stored with data on which it based.
 * N  =  bytes
 * void  *ptr == (Example:) std::string.data() where string.reserve(N)
*/
template<std::size_t N>
class  NSizeKey {
 public:
  NSizeKey() {  keyArray  =  nullptr;  }
  NSizeKey(void  *ptr) {
    setKey(ptr);
  }

  uint64_t  *keyArray;

  static constexpr int keyLongSize  =  ((N/sizeof(uint64_t))+ (N%sizeof(uint64_t)>0?1:0));
  static constexpr int keyLongSize_1  =  keyLongSize  -  1;
  void  setKey(void  *ptr)  {
    keyArray  =  static_cast<uint64_t  *>(ptr);
  }

  int cmp(NSizeKey const & other ) {
    if  (keyArray)  {
      for (int i=0; i<keyLongSize; ++i) {
        if (keyArray[i]  >  other.keyArray[i])  return  1;
        if (keyArray[i]  <  other.keyArray[i])  return  -1;
      }
    }  else if (other.keyArray) {
      return  -1;
    }
    return 0;
  }

  int cmp(NSizeKey const * other ) {
    if  (keyArray)  {
      for (int i=0; i<keyLongSize; ++i) {
        if (keyArray[i]  >  other->keyArray[i])  return  1;
        if (keyArray[i]  <  other->keyArray[i])  return  -1;
      }
    }  else if (other->keyArray) {
      return  -1;
    }
    return 0;
  }

  uint64_t  hash() const  {
    uint64_t re  =  0;
    if  (keyArray)  {
      for (int  i = 0;  i  <  keyLongSize;  ++i)  {
          re  += keyArray[i];
      }
    }
    return  (re < 9223372036854775807ll)?  re  :  (re >> 1);
  }
};


//class TKey  {
// public:
//  TKey(uint64_t  p0  =  0,  uint64_t  p1  =  0,  uint64_t  p2  =  0)  {
//    keyArray[0]  =  p0;
//    keyArray[1]  =  p1;
//    keyArray[2]  =  p2;
//  }

//  TKey  & operator=(TKey const  &rhl)  {
//    keyArray[0]  =  rhl.keyArray[0];
//    keyArray[1]  =  rhl.keyArray[1];
//    keyArray[2]  =  rhl.keyArray[2];
//    return  *this;
//  }

//  TKey(const  TKey  &rhl)  {
//    keyArray[0]  =  rhl.keyArray[0];
//    keyArray[1]  =  rhl.keyArray[1];
//    keyArray[2]  =  rhl.keyArray[2];
//  }

//  bool  operator==(TKey const  &rhl) const  {
//    return  keyArray[0]  ==  rhl.keyArray[0]
//            && keyArray[1]  ==  rhl.keyArray[1]
//            && keyArray[2]  ==  rhl.keyArray[2];
//  }

//  static bool  do_worst_case;
//  static constexpr int  keyLongSize  =  3;
//  uint64_t  keyArray[keyLongSize];
//  uint64_t  hash() const  {
//    const uint64_t re  =  keyArray[0]  +  keyArray[1]  +  keyArray[2];
//    if  (do_worst_case)  {
//      return  (re % 1000);
//    }
//    return  (re < 9223372036854775807ll)?  re  :  (re >> 1);
//  }

//  int  cmp(TKey const  *other)  const  {
//    if  (keyArray[2]  >  other->keyArray[2])  return  1;
//    if  (keyArray[2]  <  other->keyArray[2])  return  -1;
//    if  (keyArray[1]  >  other->keyArray[1])  return  1;
//    if  (keyArray[1]  <  other->keyArray[1])  return  -1;
//    if  (keyArray[0]  >  other->keyArray[0])  return  1;
//    if  (keyArray[0]  <  other->keyArray[0])  return  -1;
//    return 0;
//  }

//  bool operator<(const TKey  &r)  const  {
//    if  (keyArray[0]  <  r.keyArray[0])  return true;
//    if  (keyArray[0]  >  r.keyArray[0])  return false;
//    if  (keyArray[1]  <  r.keyArray[1])  return true;
//    if  (keyArray[1]  >  r.keyArray[1])  return false;
//    if  (keyArray[2]  <  r.keyArray[2])  return true;
//    if  (keyArray[2]  >  r.keyArray[2])  return false;
//        // Otherwise both are equal
//    return false;
//  }
//};  //  TKey



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


  struct  ElemNSizeKey  {
    ElemNSizeKey() {}
    ElemNSizeKey(const  std::string  &_data)
        : data(_data) {
      setKey();
    }

    ElemNSizeKey(const ElemNSizeKey  &other)  :  data(other.data)  {
      setKey();
    }

    ElemNSizeKey(const ElemNSizeKey  *other)  :  data(other->data)  {
      setKey();
    }

    ElemNSizeKey & operator=(const  ElemNSizeKey &rhv)  {
      if (&rhv  !=  this) {
        data  =  rhv.data;
        setKey();
      }
      return *this;
    }

    void setKey() {
      len = data.length();
      if (data.capacity()  <  key_ElemNSizeKey_size)  {
        data.reserve(key_ElemNSizeKey_size);
      }
      key.setKey(data.data());
    }

    static constexpr int key_ElemNSizeKey_size = 3 * sizeof(uint64_t);
    NSizeKey<key_ElemNSizeKey_size> key;
    std::string  data;
    int len;
  };

  //
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

  inline uint64_t  get_hash(const ElemNSizeKey  *lhv) {
    return lhv->key.hash();
  }
#endif // IKEY2_H

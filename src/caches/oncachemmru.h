/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef OnCacheMMRUMRU_H
#define OnCacheMMRUMRU_H

#include <stdlib.h>
#include <shared_mutex>
#include <mutex>
#include <atomic>
#include <climits>
#include <stdint.h>

/*
 * Most used cache,  multithreaded (thread safe)
 * Those who are more actively used are more likely to survive the recircle.
 *
 * Usage:
 *  OnCacheMMRU<>(uint32_t   capacity,  uint32_t  uselessness)
 *  capacity - how much items to store in RAM
 *  uselessness - limit of uselessness = do not evict if used count bigger
 *
 * it is better if TKey will be pointer to the part of TData for speed
 * If you want to keep the TKey separate, then make it as
 * typedef  std::shared_ptr<YourTKey> TKey
 * But default expected that TKey is a pointer: TKey key = nullptr;
 *
 * You will need 2 methods:
 * implementation of comparing keys:
  inline int compare (const TKey  &lhv,  const TKey  &rhv) {
    //Compare implementation
  }
 * and obtaining a key hash:
  inline uint64_t  get_hash(const TKey  &lhv) {
    //Hash implementation
  }
 *
*/
template<class  TKey,  class  TData>
class  OnCacheMMRU  {
 public:
  class  TONode  {
   public:
    // Does anyone need this data? (uint for overflow protect):
    std::atomic<uint32_t>  used  { 0 };
    TONode  *fwdPtrH;  // hash jumps
    TONode  *fwdPtrsL[3]; // cmp jumps

    TKey  key =  nullptr;
    std::shared_ptr<TData>  data;

    uint64_t  hash ;
    uint8_t  curHeight ;  // ==SKIPHEIGHT-1 to CPU economy

    // Lock on maintenance:
    std::atomic_flag in_use  =  ATOMIC_FLAG_INIT;

    void  clear() {
       key  =  nullptr;
       fwdPtrH  =  nullptr;
       fwdPtrsL[0]  =  nullptr;
       fwdPtrsL[1]  =  nullptr;
       fwdPtrsL[2]  =  nullptr;
       //hash  =  0;
       //curHeight  =  0;
       used.store(0,  std::memory_order_release);
    }
  };  //  TONode
    /*
     * capacity - how many elements can store
     * uselessness - limit of uselessness = do not evict if used count bigger
     * Key must be part of the stored Value - will deallocate Value only
    */    
  OnCacheMMRU(uint32_t   capacity,  uint32_t  uselessness)
      : l_capacity(capacity),  l_uselessness(uselessness)  {
    init();
  }


  ~OnCacheMMRU()  {
    clear();
  }


  std::shared_ptr<TData> getData(const TKey  &key)  {
    //return find(key);
    const uint64_t  l_hash  =  get_hash(key); //key->hash();
    const uint32_t  basketID  =  l_hash % l_capacity;
    std::shared_lock<std::shared_mutex> lk(basket_locks[basketID]);
    TONode  *cur  =  baskets[basketID];
    if (cur)  {
      while (cur  &&  cur->hash != l_hash) {
        cur  =  cur->fwdPtrH;
      }
      if (cur)  {
        int  cmp  =  compare(key,  cur->key); //key->cmp(cur->key);
        if (0  ==  cmp)  {
          ++cur->used;
          return cur->data;
        }
        if (cmp  <  0)  {
          //head is bigger, nothing to search
          return nullptr;
        }
        int  h  =  2;
        while (h  >=  0)  {
          while (cur->fwdPtrsL[h])  {
            cmp  =  compare(key,  cur->fwdPtrsL[h]->key); //key->cmp(cur->fwdPtrsL[h]->key);
            if (cmp  <  0)  {
              //found who bigger
              break;
            }  else  if (0  ==  cmp)  {
              ++cur->fwdPtrsL[h]->used;
              return cur->fwdPtrsL[h]->data;  // found
            }
            cur  =  cur->fwdPtrsL[h];  //step on it
          }
          --h;
        }
      }
    }
    return nullptr;
  }

  void  insertNode  (const TKey  &key,  std::shared_ptr<TData>  &&data)  {    
//    if (7953758402820723744  ==  key->key.keyArray[0]) {
//      assert(7953758402820723744  ==  key->key.keyArray[0]);
//    }
    const uint64_t  l_hash  =  get_hash(key); //key->hash();
    const uint32_t  basketID  =  l_hash % l_capacity;
    TONode  *new_node  =  allocNode();
    new_node->hash  =  l_hash;
    new_node->key  =  key;
    new_node->data  =  std::move(data);
//    if (5990907401477938266  ==  l_hash) {
//      assert( 5990907401477938266  ==  l_hash);
//    }
    insertN(new_node,  basketID);
    new_node->in_use.clear(std::memory_order_release);
  }  //  insertNode

 private:
  const  uint32_t   l_uselessness;
  const  uint32_t   l_capacity;
  TONode  **baskets; //[CACHE_CAPACITY];  //  [l_hash_baskets]
  std::shared_mutex  *basket_locks;
  std::allocator<std::shared_mutex> allocator_shared_mutex;

    //Allocations:
  std::atomic<uint32_t>  node_pool_id  { 0 };
  TONode  *node_pool;
  std::allocator<TONode> allocator_node_pool;

    //Landscapes    
  uint8_t  landscape_l[256];
  //static thread_local uint8_t  land_l_p;
  uint8_t & get_land_l_p()  {
      static thread_local uint8_t land_l_p = 0;
      return land_l_p;
  }

  void  init()  {    
    for (int  i  =  0;  i  <  255;  ++i)  {
      if  (0  ==  i % 25)  {
        landscape_l[i]  =  2;
      } else if (0  ==  i % 5)  {
        landscape_l[i]  =  1;
      }  else  {        
        landscape_l[i]  =  0;
      }
    }
    //new hash head always 2 hardcoded landscape_l[0]  =  2;
    landscape_l[255]  =  2;

        //init baskets:        
    size_t  size  =  l_capacity * sizeof(TONode  *);
    baskets  =  static_cast<TONode  **>(malloc(size));
    memset(baskets,  0,  size);
    node_pool  =  allocator_node_pool.allocate(l_capacity);
    basket_locks  = allocator_shared_mutex.allocate(l_capacity);
    for (int32_t  i = l_capacity - 1;  i >= 0;  --i) {
      allocator_shared_mutex.construct(basket_locks  +  i);
      allocator_node_pool.construct(node_pool  + i);
    }
  } //init

  void  clear()  {
//    for (uint64_t  i  =  0;  i  <  CACHE_CAPACITY;  ++i) {
//      if (node_pool[i].data)  {
//        l_f_delData(node_pool[i].data);
//      }
//    }
    for (int32_t  i = l_capacity - 1;  i >= 0;  --i) {
      allocator_shared_mutex.destroy(basket_locks  +  i);
      allocator_node_pool.destroy(node_pool  +  i);
    }
    allocator_shared_mutex.deallocate(basket_locks, l_capacity);
    allocator_node_pool.deallocate(node_pool, l_capacity);
    free(baskets);
  }


  TONode  *  allocNode()  {

    TONode  *re  =  nullptr;
    do {
      //uint32_t  id  =  node_pool_id.fetch_add(1);
      uint32_t  id  =  node_pool_id.fetch_add(1,  std::memory_order_relaxed);
      if  (id >= l_capacity )  {
        //node_pool_id.store(0, std::memory_order_release);
        node_pool_id.store(0, std::memory_order_relaxed);
        id  =  0; // data races will prevent through in_use.test_and_set
      }
      re  =  &node_pool[id];
      uint32_t  used  =  re->used.load(std::memory_order_acquire);
      if  (used  >  l_uselessness)  {
        //re->used.store(used - l_uselessness,  std::memory_order_release);
        re->used.store(used - l_uselessness,  std::memory_order_relaxed);
        continue;
      }
    } while (re->in_use.test_and_set(std::memory_order_acq_rel));    

    if (re->key) {
//      if (5990907401477938266  ==  re->hash) {
//        assert( 5990907401477938266  ==  re->hash);
//      }
      //in case of data races here will be serialization:
      {        
        uint32_t basketID  =  re->hash % l_capacity;
        std::unique_lock<std::shared_mutex> lk(basket_locks[basketID]);
        if (baskets[basketID]->hash  ==  re->hash)  {
          delOnBasketHead(re,  baskets[basketID],  basketID);
        }  else  {
          delWithOtherHash(re,  baskets[basketID]);
        }
      }
      re->clear();
    }
    //re->in_use.clear();
    return re;
  }  //  allocNode


//  //TONode * find(TKey const  *key)  {
//  std::shared_ptr<TData> find(const TKey  &key)  {
//    const uint64_t l_hash  =  get_hash(key); //key->hash();
//    const uint32_t basketID  =  l_hash % l_capacity;
//    std::shared_lock<std::shared_mutex> lk(basket_locks[basketID]);
//    TONode  *cur  =  baskets[basketID];
//    if (cur)  {
//      while (cur  &&  cur->hash != l_hash) {
//        cur  =  cur->fwdPtrH;
//      }
//      if (cur)  {
//        int  cmp  =  compare(key,  cur->key); //key->cmp(cur->key);
//        if (0  ==  cmp)  {
//          return cur->data;
//        }
//        if (cmp  <  0)  {
//          //head is bigger, nothing to search
//          return nullptr;
//        }
//        int  h  =  2;
//        while (h  >=  0)  {
//          while (cur->fwdPtrsL[h])  {
//            cmp  =  compare(key,  cur->fwdPtrsL[h]->key); //key->cmp(cur->fwdPtrsL[h]->key);
//            if (cmp  <  0)  {
//              //found who bigger
//              break;
//            }  else  if (0  ==  cmp)  {
//              return cur->fwdPtrsL[h]->data;  // found
//            }
//            cur  =  cur->fwdPtrsL[h];  //step on it
//          }
//          --h;
//        }
//      }
//    }
//    return nullptr;
//  }  //  find



  void  insertN(TONode  *new_node,  const uint32_t basketID)  {
      std::unique_lock<std::shared_mutex> lk(basket_locks[basketID]);
      TONode  *updatePathOutH  =  baskets[basketID];
      if (updatePathOutH) {
        if (updatePathOutH->hash  ==  new_node->hash) {
           setOnBasketHead(new_node,  updatePathOutH,  basketID);
        }  else if (updatePathOutH->hash  >  new_node->hash)  {
           baskets[basketID]  =  new_node;
           new_node->fwdPtrH  =  updatePathOutH;
           new_node->curHeight  =  2;
        }  else  {
           setOther(new_node,  updatePathOutH);
        }
      } else {
        baskets[basketID]  =  new_node;
        new_node->curHeight  =  2;
      }
      return;
  }  //  insertN

  void setOnBasketHead(TONode  *new_node, TONode  *cur,  uint32_t  basketID)  {
    TKey  p_key  =  new_node->key;
    int  cmp  =  compare(p_key,  cur->key);//key->cmp(cur->key);
    if (cmp  <  0)  {
      //updatePathOutL[0]  =  updatePathOutL[1] =  updatePathOutL[2] =  cur;
      //new_node->curHeight  =  landscape_l[get_land_l_p()++];
      //TONode  *prevHead  =  updatePathOutH->fwdPtrH;
      baskets[basketID]  =  new_node;
      new_node->fwdPtrH  =  cur->fwdPtrH;
      cur->fwdPtrH  =  nullptr;
      new_node->curHeight  =  2;
      cur->curHeight  =  landscape_l[get_land_l_p()++];
      for (int  i  =  0; i  <=  2;  ++i)  {
        if (cur->curHeight  >=  i)  {
          // catchNode(re, i, prevHead) ;
          new_node->fwdPtrsL[i]  =  cur;
        }  else  {
          // catchNode(re, i, prevHead->fwdPtrsL[i]) ;
          new_node->fwdPtrsL[i]  =  cur->fwdPtrsL[i];
          cur->fwdPtrsL[i]  =  nullptr;
        }
      }
      return;// 3; //must replace hash head in place
    }
    if (0  ==  cmp) {
      //l_f_delData(cur->data);
      cur->data  =  std::move(new_node->data);
      cur->key  =  new_node->key;
      //memset(new_node,  0,  sizeof(TONode));
      new_node->clear();
      return;
    }
    TONode  *updatePathOutL[3];
    int  h  =  2;
    while (h  >=  0)  {
      updatePathOutL[h]  =  cur;
      while (cur->fwdPtrsL[h])  {
        //assert (hash  ==  cur->fwdPtrsL[h]->hash);
        cmp  =  compare(p_key,  cur->fwdPtrsL[h]->key); //key->cmp(cur->fwdPtrsL[h]->key);
        if (cmp  <  0)  {
          //found who bigger
          break;
        }  else  if (0  ==  cmp)  {
          //updatePathOutL[0]  =  updatePathOutL[1] =  updatePathOutL[2]  =  cur->fwdPtrsL[h];
          //updatePathOutL[0]  =  cur->fwdPtrsL[h];
          cur  =  cur->fwdPtrsL[h];          
          cur->data  =  std::move(new_node->data);
          cur->key  =  new_node->key;
          //memset(new_node,  0,  sizeof(TONode));
          new_node->clear();
          return;// 0;  // must replace at place
        }
        cur  =  cur->fwdPtrsL[h];  //step on it
        updatePathOutL[h]  =  cur;
      }
      --h;
    }
    new_node->curHeight  =  landscape_l[get_land_l_p()++];
    for (int  i  =  0; i  <=  new_node->curHeight;  ++i)  {
      // catchNode(re, i, updatePathOutL[i]->fwdPtrsL[i]) ;
      new_node->fwdPtrsL[i]  =  updatePathOutL[i]->fwdPtrsL[i];
      // catchNode(updatePathOutL[i], i, re) ;
      updatePathOutL[i]->fwdPtrsL[i]  =  new_node;
    }
    return;// (cmp < 0) ? -1 : 1;
  }

  void  setOther(TONode  *new_node,  TONode  *cur)  {
      const  uint64_t  hash  =  new_node->hash;
      TKey  p_key  =  new_node->key;
      //TONode  *cur  =  updatePathOutH;
      while (cur->fwdPtrH  &&  hash > cur->fwdPtrH->hash)  {
        cur  =  cur->fwdPtrH;  //step on it        
      }

      if (cur->fwdPtrH  &&  cur->fwdPtrH->hash  ==  hash) {
        // same key jumps
        //cur  =  cur->fwdPtrH;  //step on it
        int  cmp  =  compare(p_key,  cur->fwdPtrH->key); //key->cmp(cur->key);
        if (cmp  <  0)  {
          //updatePathOutL[0]  =  updatePathOutL[1] =  updatePathOutL[2] =  cur;
          //3 == replace head of hash queue, use updatePathOutH
          //new_node->curHeight  =  landscape_l[land_l_p++];
          TONode  *prevHead  =  cur->fwdPtrH;
          cur->fwdPtrH  =  new_node;
          new_node->fwdPtrH  =  prevHead->fwdPtrH;
          prevHead->fwdPtrH  =  nullptr;
          new_node->curHeight  =  2;
          prevHead->curHeight  =  landscape_l[get_land_l_p()++];
          for (int  i  =  0; i  <=  2;  ++i)  {
            if (prevHead->curHeight  >=  i)  {
              // catchNode(re, i, prevHead) ;
              new_node->fwdPtrsL[i]  =  prevHead;
            }  else  {
              // catchNode(re, i, prevHead->fwdPtrsL[i]) ;
              new_node->fwdPtrsL[i]  =  prevHead->fwdPtrsL[i];
              prevHead->fwdPtrsL[i]  =  nullptr;
            }
          }
          return;// 3; //must replace hash head in place
        }
        cur  =  cur->fwdPtrH;  //step on it
        if (0  ==  cmp) {
          //updatePathOutL[0]  =  cur;
          //l_f_delData(cur->data);
          cur->data  =  std::move(new_node->data);
          cur->key  =  new_node->key;
          //memset(new_node,  0,  sizeof(TONode));
          new_node->clear();
          return;//  0;
        }
        int  h  =  2;
        TONode  *updatePathOutL[3];
        while (h  >=  0)  {
          updatePathOutL[h]  =  cur;
          while (cur->fwdPtrsL[h])  {
            //assert (hash  ==  cur->fwdPtrsL[h]->hash);
            cmp  =  compare(p_key,  cur->fwdPtrsL[h]->key);// key->cmp(cur->fwdPtrsL[h]->key);
            if (cmp  <  0)  {
              //found who bigger
              break;
            }  else  if (0  ==  cmp)  {
              cur  =  cur->fwdPtrsL[h];
              cur->data  =  std::move(new_node->data);
              cur->key  =  new_node->key;
              new_node->clear();
              return;// 0;  // must replace at place
            }
            cur  =  cur->fwdPtrsL[h];  //step on it
            updatePathOutL[h]  =  cur;
          }
          --h;
        }
        new_node->curHeight  =  landscape_l[get_land_l_p()++];
        for (int  i  =  0; i  <=  new_node->curHeight;  ++i)  {
          // catchNode(re, i, updatePathOutL[i]->fwdPtrsL[i]) ;
          new_node->fwdPtrsL[i]  =  updatePathOutL[i]->fwdPtrsL[i];
          // catchNode(updatePathOutL[i], i, re) ;
          updatePathOutL[i]->fwdPtrsL[i]  =  new_node;
        }
        return;// (cmp < 0) ? -1 : 1;
      }  else  {
        // here no a same hash
        //updatePathOutL[0] = updatePathOutL[1] = updatePathOutL[2] = nullptr;
        new_node->curHeight  =  2;
        new_node->fwdPtrH  =  cur->fwdPtrH;
        cur->fwdPtrH  =  new_node;
        return;// 4;
      }
      return;// 8;
    }  //  setOther

    void delOnBasketHead(TONode  *nodeToDel,  TONode  *cur,  uint32_t  basketID)  {
      if  (nodeToDel  ==  cur)  {
        //This is the head of hash queue:
        cur = nodeToDel->fwdPtrsL[0]; // new hash head
        if (cur)  {
          baskets[basketID] = cur;
          cur->fwdPtrH  =  nodeToDel->fwdPtrH;
          //assert (2 == nodeToDel->curHeight);
          for (int  h  =  2;  h  >  cur->curHeight;  --h)  {
            // catchNode(cur, h, nodeToDel->fwdPtrsL[h]) ;
            cur->fwdPtrsL[h]  =  nodeToDel->fwdPtrsL[h];  // new head see what nodeToDel see now
          }
          cur->curHeight  =  2;
        }  else  {
          baskets[basketID]  =  nodeToDel->fwdPtrH;
        }
      }  else  {
        //3. regular del:
        delInSameHashCmp3(nodeToDel,  cur) ;
      }
    }  //  delOnBasketHead

    void delWithOtherHash(TONode  *nodeToDel,  TONode  *startSearch)  {
      //1. must find same hash:
      const uint64_t  hash  =  nodeToDel->hash;
      TONode  *cur  =  startSearch;
      while (cur->fwdPtrH  &&  cur->fwdPtrH->hash != hash) {
        cur  =  cur->fwdPtrH;
      }

      //2. if nodeToDel is head of hash queue:
      if  (nodeToDel  ==  cur->fwdPtrH)  {
        //This is the head of hash queue:
        cur->fwdPtrH = nodeToDel->fwdPtrsL[0]; // new hash head
        if (cur->fwdPtrH)  {
          cur  =  cur->fwdPtrH;  // aka tmp for work
          //  passing pointers to a new head:
          cur->fwdPtrH  =  nodeToDel->fwdPtrH;
          //assert (2 == nodeToDel->curHeight);
          for (int  h  =  2;  h  >  cur->curHeight;  --h)  {
            // catchNode(cur, h, nodeToDel->fwdPtrsL[h]) ;
            cur->fwdPtrsL[h]  =  nodeToDel->fwdPtrsL[h];  // new head see what nodeToDel see now
          }
          cur->curHeight  =  2;
        }  else  {
          //nodeToDel was last with same hash
          cur->fwdPtrH  =  nodeToDel->fwdPtrH;
//          if (nodeToDel  ==  updatePathOutH)  {
//            updatePathOutH  =  cur;
//          }
        }
      }  else  {
        //3. regular del:
        delInSameHashCmp3(nodeToDel,  cur->fwdPtrH) ;
      }

      return;
    }


    // for setll cmp==3 or 4, no need  updatePathOutL
    void  delInSameHashCmp3(TONode  *nodeToDel, TONode  *hashHead)  {
      int  h  =  2;
      TONode * updatePath[3];
      TONode  *cur  =  hashHead;  //updatePathOutH->fwdPtrH;
      //int  cmp  =  1;
      TKey  p_key  =  nodeToDel->key;
      while (h  >=  0)  {
        updatePath[h]  =  cur;
        while (nodeToDel != cur->fwdPtrsL[h]  &&  cur->fwdPtrsL[h])  {
          //cmp  =  nodeToDel->key->cmp(cur->fwdPtrsL[h]->key);
          //assert (nodeToDel->hash  ==  cur->fwdPtrsL[h]->hash);
          //if (nodeToDel->key->cmp(cur->fwdPtrsL[h]->key)  <=  0)  {
          if (compare(p_key,  cur->fwdPtrsL[h]->key)  <=  0)  {
            //found who bigger or target, go level down
            break;
          }
          updatePath[h]  =  cur;
          cur  =  cur->fwdPtrsL[h];  //step on it
        }
        //if found exactly, put on path:
        if (nodeToDel == cur->fwdPtrsL[h])  {
          updatePath[h]  =  cur;
        }
        --h;
      }

      for (h  =  nodeToDel->curHeight;  h  >=  0;  --h)  {
        // catchNode(updatePath[h], h, nodeToDel->fwdPtrsL[h]);
        updatePath[h]->fwdPtrsL[h]  =  nodeToDel->fwdPtrsL[h];
      }
      return;
    } // delInSameHashCmp3

};



#endif // OnCacheMMRUMRU_H

/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef ONCACHESMRU_H
#define ONCACHESMRU_H

#include <stdlib.h>
#include <memory>
#include <iostream>

/*
 * Most used cache,  single threaded (thread UNsafe)
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
class  OnCacheSMRU  {
 public:
  class  TONode  {
   public:
    uint32_t  used  =  0;  // is it useful? need it in cache?
    TONode  *fwdPtrH;  // hash jumps
    TONode  *fwdPtrsL[3]; // cmp jumps

    TKey  key  =  nullptr;
    std::shared_ptr<TData>  data;

    uint64_t  hash;
    uint8_t  curHeight;  // ==SKIPHEIGHT-1 to CPU economy

    void  clear() {
       key  =  nullptr;
       fwdPtrH  =  nullptr;
       fwdPtrsL[0]  =  nullptr;
       fwdPtrsL[1]  =  nullptr;
       fwdPtrsL[2]  =  nullptr;
       used  =  0;
    }
  };  //  TONode

    /*
     * capacity - how many elements can store
     * uselessness - limit of uselessness - an element that does not collect even this is destroyed by old age
     * Key must be part of the stored Value - will deallocate Value only
    */
  OnCacheSMRU(uint32_t  capacity,  uint32_t  uselessness)
       :    l_capacity(capacity),  l_uselessness(uselessness)   {
    init();
  }

  ~OnCacheSMRU()  {
    clear();
  }


std::shared_ptr<TData> getData(const TKey  &key)  {
  const uint64_t  l_hash  =  get_hash(key);
  TONode  *cur  =  baskets[l_hash % l_capacity];
  if (cur)  {
    while (cur  &&  cur->hash != l_hash) {
      cur  =  cur->fwdPtrH;
    }
    if (cur)  {
      int  cmp  =  compare(key,  cur->key);
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
          cmp  =  compare(key,  cur->fwdPtrsL[h]->key);
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
//    if (key->keyArray[0]==139106
//        || key->keyArray[0]==139326
//        || key->keyArray[0]==142556) {
//        std::cout<<"\n";
//    }
    const uint64_t  hash  =  get_hash(key);
    const uint32_t  basketID  =  hash % l_capacity;
    updatePathOutH  =  &(baskets[basketID]);
    int  cmp  =  (*updatePathOutH)? setll(hash,  key) : 5;
    if  (0  ==  cmp)  {
      TONode  *cur  =  *updatePathOutH;
      cur->key  =  key;
      cur->data  =  std::move(data);
      cur->used  =  l_uselessness;
    }  else  {
    //insert new node:
      allocNode(hash,  key,  std::move(data),  basketID,  cmp);
    }
  }  //  insertNode

 private:
  const  uint32_t   l_uselessness;
  const  uint32_t   l_capacity;
  TONode  **baskets;
  TONode  **updatePathOutH;  //pointer to pointer
  TONode  *updatePathOutL[3];

    //Allocations:
  uint32_t  node_pool_id  =  0 ;
  TONode  *node_pool;
  std::allocator<TONode> allocator_node_pool;

    //Landscapes
  uint8_t  landscape_l[256];
  uint8_t  land_l_p  =  0;

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

    // init baskets:
    size_t  size  =  l_capacity * sizeof(TONode  *);
    baskets  =  static_cast<TONode  **>(malloc(size));
    memset(baskets,  0,  size);
    node_pool  =  allocator_node_pool.allocate(l_capacity);
    for (int32_t  i = l_capacity - 1;  i >= 0;  --i) {
      allocator_node_pool.construct(node_pool  + i);
    }
  } //init

  void  clear()  {
    for (int32_t  i = l_capacity - 1;  i >= 0;  --i) {
      allocator_node_pool.destroy(node_pool  +  i);
    }

    allocator_node_pool.deallocate(node_pool, l_capacity);
    free(baskets);
  }


  void  allocNode(const uint64_t  hash,  const TKey  &key,
       std::shared_ptr<TData>  &&data,  const uint32_t  basketID,  int  cmp)  {
    TONode  *re  =  nullptr;
    do {
      ++node_pool_id;
      if  (node_pool_id >= l_capacity )  {
        node_pool_id  =  0;
      }
      re  =  &node_pool[node_pool_id];
      if  (re->used  >=  l_uselessness)  {
        re->used -= l_uselessness;
        continue;
      }
    }  while (!re);

    if (re->key)  {
//      if (re->key->keyArray[0]==30810) {
//          std::cout<<"\n";
//      }
      if (hash  ==  re->hash)  {
        // Same  hash case        
        if (re  ==  *updatePathOutH)  {
          //re is head of hash queue
          if (3 == cmp  ||  updatePathOutL[0]  ==  re)  {
            //easy: key must be new head, just replace
            //cmp  =  777;
            re->key  =  key;
            re->data  =  std::move(data);
            return;
          }  else  {
            // swap re, re->fwdPtrsL[0]:
            TONode  *cur  =  re->fwdPtrsL[0];
            re->key  =  cur->key;
            re->data  =  std::move(cur->data);
            for (int  h  =  0;  h  <=  2;  ++h)  {
              if (h  <=  cur->curHeight) {                
                re->fwdPtrsL[h]  =  cur->fwdPtrsL[h];
                cur->fwdPtrsL[h]  =  nullptr;
              }
              if (updatePathOutL[h]  ==  cur) {
                updatePathOutL[h]  =  re;
              }
            }
            re  =  cur;            
            re->used  =  l_uselessness; // because we are here broke age
            //next do regular alhorithm..
          }
        }  else  {
          // re not a head of the hash queue          
          if (3  ==  cmp)  {            
            delInSameHashCmp3(re, *updatePathOutH);
          }  else  {
           // assert (4  !=  cmp);
            delInSameHash(re);
          }          
          re->clear();
        }
      }  else  {
        // Other hash case
        const uint32_t  re_basketID  =  re->hash % l_capacity;
        delWithOtherHash(re,
            (re->hash > hash  &&  re_basketID == basketID)  ?
              updatePathOutH  :  &baskets[re_basketID]);        
        re->clear();
      } // else

    }  //  if (!re)


    re->hash  =  hash;

    switch (cmp)  {
    case -1:
    case 1:
      //using update path
      // 1 == not found who bigger with the same hash, use updatePathOut
      // -1 == found who bigger at the queue end, use updatePathOut
      re->key  =  key;
      re->data  =  std::move(data);
      re->curHeight  =  landscape_l[land_l_p++];
      for (int  i  =  0; i  <=  re->curHeight;  ++i)  {        
        re->fwdPtrsL[i]  =  updatePathOutL[i]->fwdPtrsL[i];        
        updatePathOutL[i]->fwdPtrsL[i]  =  re;
      }
      break;
    case 3:
      //3 == replace head of hash queue, use updatePathOutH      
    {      
      TONode  *prevHead  =  *updatePathOutH;     
      *updatePathOutH  =  re;
      re->fwdPtrH  =  prevHead->fwdPtrH;
      prevHead->fwdPtrH  =  nullptr;
      re->key  =  key;
      re->data  =  std::move(data);
      re->curHeight  =  2;
      prevHead->curHeight  =  landscape_l[land_l_p++];
      for (int  i  =  0; i  <=  2;  ++i)  {
        if (prevHead->curHeight  >=  i)  {          
          re->fwdPtrsL[i]  =  prevHead;
        }  else  {          
          re->fwdPtrsL[i]  =  prevHead->fwdPtrsL[i];
          prevHead->fwdPtrsL[i]  =  nullptr;
        }
      }
    }
      break;
    case 4:
      //4 == here no a same hash, create new hash queue, use updatePathOutH
      re->key  =  key;
      re->data  =  std::move(data);
      re->curHeight  =  2;
      re->fwdPtrH  =  *updatePathOutH;
      *updatePathOutH  =  re;
      break;
    case 5:
      //New head of empty basket
      re->key  =  key;
      re->data  =  std::move(data);
      re->curHeight  =  2;
      baskets[basketID]  =  re;
      break;
    default:
      assert(false);
    }
    return;
  }  //  allocNode

    /*
     * return:
     * 0 == node with equal key found, use only updatePathOut[0]
     * 3 == replace head of hash queue, use updatePathOutH
     * 4 == here no a same hash, create new hash queue, use updatePathOutH
     * 1 == not found who bigger with the same hash, use updatePathOut
     * -1 == found who bigger at the queue end, use updatePathOut
   */
    int  setll(uint64_t  hash,  const TKey  &key)  {        
      int cmp;
      TONode  *cur  =  *updatePathOutH;
      if (cur->hash  ==  hash)  {
        cmp = compare(key,  cur->key) ;
        if (0  ==  cmp)  {  return  0;  }
        if (cmp  <  0) {  return  3;  }
      }  else  {
        while (*updatePathOutH   &&  hash > (*updatePathOutH)->hash) {
          updatePathOutH = &((*updatePathOutH)->fwdPtrH);
        }        
        if (*updatePathOutH  &&  (*updatePathOutH)->hash  ==  hash) {
          cmp  =  compare(key,  (*updatePathOutH)->key) ;
          if (cmp  <  0)  {            
            return 3; //must replace hash head in place
          }
          if (0  ==  cmp) {            
            return  0;
          }          
          cur  =  *updatePathOutH;  //step on it
        }  else  {
          cur  =  nullptr;
        }
      }

      if (cur)  {
        // same key jumps       
        int  h  =  2;
        while (h  >=  0)  {
          updatePathOutL[h]  =  cur;
          while (cur->fwdPtrsL[h])  {            
            cmp  =  compare(key,  cur->fwdPtrsL[h]->key) ; //key->cmp(cur->fwdPtrsL[h]->key);
            if (cmp  <  0)  {
              //found who bigger
              break;
            }  else  if (0  ==  cmp)  {              
              updatePathOutH  =  &(cur->fwdPtrsL[h]);
              return 0;  // must replace at place
            }
            cur  =  cur->fwdPtrsL[h];  //step on it
            updatePathOutL[h]  =  cur;
          }
          --h;
        }
        return (cmp < 0) ? -1 : 1;
      }  else  {
        // here no a same hash        
        return 4;
      }
      return 8;
    }  //  setll


    void delWithOtherHash(TONode  *nodeToDel,  TONode  **startSearch)  {
      //1. must find same hash:
      const uint64_t  hash  =  nodeToDel->hash;

      while (*startSearch  &&  (*startSearch)->hash != hash) {
        startSearch = &((*startSearch)->fwdPtrH);
      }

      //2. if nodeToDel is head of hash queue:
      if  (nodeToDel  ==  *startSearch)  {
        //This is the head of hash queue:        
        *startSearch = nodeToDel->fwdPtrsL[0];        
        if (*startSearch)  {          
          TONode  *cur  =  *startSearch;  // aka tmp for work
          //  passing pointers to a new head:
          cur->fwdPtrH  =  nodeToDel->fwdPtrH;          
          for (int  h  =  2;  h  >  cur->curHeight;  --h)  {            
            cur->fwdPtrsL[h]  =  nodeToDel->fwdPtrsL[h];  // new head see what nodeToDel see now
          }
          cur->curHeight  =  2;
        }  else  {
          //nodeToDel was last with same hash          
          *startSearch  =  nodeToDel->fwdPtrH;
          if (&(nodeToDel->fwdPtrH)  ==  updatePathOutH)  {
            updatePathOutH  =  startSearch;
          }
        }
      }  else  {
        //3. regular del:        
        delInSameHashCmp3(nodeToDel,  *startSearch) ;
      }
      return;
    }  //  delWithOtherHash

    void delInSameHashPathOut(TONode  *nodeToDel,  int  top_h)  {
        //Node to del in updatePathOut[h], need path to it for update
      //new path may be not the same      
      TONode  *updatePath[3];
      int  h  =  2;      
      TONode  *cur  =  *updatePathOutH;
      if  (nodeToDel->curHeight  >  top_h)  {
        top_h  =  nodeToDel->curHeight;
      }

      while (h  >  top_h)  {
        cur  =  updatePath[h]  =  updatePathOutL[h];
        --h;
      }

      while (h  >=  0)  {
        updatePath[h]  =  cur;
        while (nodeToDel != cur->fwdPtrsL[h]  &&  cur->fwdPtrsL[h])  {
          if (compare(nodeToDel->key,  cur->fwdPtrsL[h]->key) <=  0)  {
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

      for (h  =  top_h;  h  >=  0;  --h)  {        
        updatePath[h]->fwdPtrsL[h]  =  nodeToDel->fwdPtrsL[h];
        //replace deleted node :
        if (nodeToDel  ==  updatePathOutL[h])  {
          updatePathOutL[h]  =  updatePath[h];
        }
      }
      return;
    }  //  delInSameBasketPathOut

    void delInSameHashSuperFast(TONode  *nodeToDel,  int  top_h)  {
        //need path before to update pointers
      TONode * updatePath[3];
      if  (nodeToDel->curHeight  >  top_h)  {
        top_h  =  nodeToDel->curHeight;
      }
      updatePath[top_h]  =  updatePathOutL[top_h];
      TONode  *cur  =  updatePathOutL[top_h];
      int  h  =  top_h;
      while (h  >=  0)  {
        updatePath[h]  =  cur;
        while (nodeToDel != cur->fwdPtrsL[h]  &&  cur->fwdPtrsL[h])  {
          if (compare(nodeToDel->key,  cur->fwdPtrsL[h]->key)  <=  0)  {
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

      for (h  =  top_h;  h  >=  0;  --h)  {        
        updatePath[h]->fwdPtrsL[h]  =  nodeToDel->fwdPtrsL[h];
      }
      return;
    }  //  delInSameBasketSuperFast

    // for setll cmp==3 or 4, no need  updatePathOutL
    void  delInSameHashCmp3(TONode  *nodeToDel, TONode  *hashHead)  {
      int  h  =  2;
      TONode * updatePath[3];
      TONode  *cur  =  hashHead;  //updatePathOutH->fwdPtrH;      
      while (h  >=  0)  {
        updatePath[h]  =  cur;
        while (nodeToDel != cur->fwdPtrsL[h]  &&  cur->fwdPtrsL[h])  {          
          if (compare(nodeToDel->key,  cur->fwdPtrsL[h]->key)  <=  0)  {
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
        updatePath[h]->fwdPtrsL[h]  =  nodeToDel->fwdPtrsL[h];
      }
      return;
    } // delInSameHashCmp3

    // need to check if UpdatePath affected
    void  delInSameHash(TONode  *nodeToDel)  {
      for (int  h  =  2;  h  >=  0;  --h)  {
        if (nodeToDel  ==  updatePathOutL[h])  {
          //worst case - must do repath
          delInSameHashPathOut(nodeToDel, h);
          return;
        }
        if (nodeToDel  ==  updatePathOutL[h]->fwdPtrsL[h])  {
          //best case - know where and how to update
          //just check if not on Path:
          int h2  =  h  -  1;
          while (h2  >=  0)  {
            if (nodeToDel  ==  updatePathOutL[h2])  {
              //worst case - must do repath
              delInSameHashPathOut(nodeToDel, h2);
              return;
            }
            --h2;
          }
          delInSameHashSuperFast(nodeToDel, h);
          return;
        }
      }
      //not on path == path not affected      
      delInSameHashCmp3(nodeToDel,  *updatePathOutH) ;
    } // delInSameHash


//    void  catchNode(TONode  *node, int h, TONode  *in)  {
//      if (1 == h) {
//        if (node->key->keyArray[0] == 6693458) {
//          if (in) {
//          //if (in  &&  in->key->keyArray[0] == 2147339106) {
//            std::cout  <<  in->data << " : " << in->hash <<"\n";
//          }
//          //std::cout<<"\n";
//        }
//      }
//    }
};


#endif // ONCACHESMRU_H

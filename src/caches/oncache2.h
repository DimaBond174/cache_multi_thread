/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef ONCACHE2_H
#define ONCACHE2_H

#include <stdlib.h>
#include <string.h>
#include <cassert>
#include <cmath>
#include <memory>



template<class  TKey,  class  TData>
class  OnCache2  {
 public:
  class  TONode2  {
   public:
    TONode2  *fwdPtrH;  // hash jumps
    TONode2  *fwdPtrsL[3]; // cmp jumps
      //rating queue:
    TONode2  *mostUseful;

      //const uint8_t * key;
    std::shared_ptr<TKey>  key;
    std::shared_ptr<TData>  data;
    uint64_t  hash;
    uint8_t  curHeight;  // ==SKIPHEIGHT-1 to CPU economy
    uint32_t  used_count;  // is it useful? need it in cache?
  };  //  TONode2
    /*
     * capacity - how many elements can store
     * uselessness - limit of uselessness - an element that does not collect even this is destroyed by old age
     * Key must be part of the stored Value - will deallocate Value only
    */    
  OnCache2(uint64_t  capacity,  uint32_t  uselessness)
       :   l_hash_baskets(capacity),  l_uselessness(uselessness)  {
    init();
  }

  ~OnCache2()  {
    clear();
  }

  uint64_t  size()  {
    return  leafAllocCounter;
  }

  const char * getData(TKey const  *key)  {
    TONode2  *curFound  =  find(key) ;
    if  (curFound)  {
      //toTopUsage(curFound);
      ++curFound->used_count;
      return curFound->data;
    }
    return nullptr;
  }

  void  insertNode  (TKey const  *key,  const char  *data)  {
    const uint64_t  hash  =  key->hash();
    const uint64_t  basketID  =  hash % l_hash_baskets;
    int  cmp  =  setll(hash,  key,  basketID);
    if  (0  ==  cmp)  {
      TONode2  *cur  =  updatePathOutL[0];
      l_f_delData(cur->data);
      cur->data  =  data;
      cur->key  =  key;
      cur->used_count  =  l_uselessness;
    }  else  {
    //insert new node:
      allocNode(hash,  key,  data,  basketID,  cmp);
    }
  }  //  insertNode

//  void  find_bug()  {
//    for (uint64_t b  =  0;  b  <  l_hash_baskets;   ++b)  {
//      TONode2  *cur  =  &(baskets[b]);
//      if (cur->fwdPtrsL[0]) {
//        cur  =  cur->fwdPtrsL[0];
//      }  else  {
//        continue;
//      }
//      while (cur)  {
//        assert(cur->hash % l_hash_baskets  ==  b);
//        int  h_last  =  0;
//        for (int  h  =  4;  h  >=  0;  --h)  {
//          if (cur->fwdPtrsL[h]) {
//            assert(cur->fwdPtrs[h]->hash % _hash_baskets  ==  b);
//            h_last  =  h;
//          }  else  {
//            assert(0 == h_last);
//          }
//        }
//        cur  =  cur->fwdPtrs[0];
//      }
//    }
//  }  // find_bug
//  uint64_t  getRateCount()  {
//    TONode2  *cur  =  leastUseful;
//    uint64_t  re  =  1;
//    int cnt = 0;
//    while (cur->mostUseful )  {
////      if (341715 == cur->key->keyArray[0]) {
////        ++cnt;
////        assert(cnt < 2);
////      }
//      cur  =  cur->mostUseful;
//      ++re;
//    }
//    std::cout << "least="<<leastUseful->key->keyArray[0] << '\n';
//    assert (rating_len - 1 <= re  && cur == mostUseful);
//    rating_len  =  re;
//    if (341028 == leastUseful->key->keyArray[0]) {
//      std::cout << "\n";
//    }
//    return re;
//  }

 private:
  //const uint32_t  _capacity; == _hash_baskets
  const uint64_t  l_hash_baskets;

  //const unsigned short  leafSize; == _hash_baskets
  TONode2  *baskets;  //  [l_hash_baskets]
  TONode2  *updatePathOutH;
  TONode2  *updatePathOutL[3];
  uint64_t  hashOut  =  0;

    //Allocations:    
  TONode2  *curLeaf;  //  [l_hash_baskets];
  uint64_t  leafAllocCounter  =  0;

    //Deallocations:    
  TDeletDataFunc  l_f_delData;

    //Rating queue:    
//  TONode2  headNode;
  //rating queue:
TONode2  *mostUseful  =  nullptr;
TONode2  *leastUseful  =  nullptr;
const uint32_t  l_uselessness;
uint64_t  rating_len  =  1;

    //Landscapes    
//  uint8_t  landscape_h[256];
//  uint8_t  *land_h_p;
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

        //init baskets:        
    size_t  size  =  l_hash_baskets * sizeof(TONode2);
    baskets  =   (TONode2 *)malloc(size);
    memset(baskets,  0,  size);
    curLeaf  =   (TONode2 *)malloc(size);
    memset(curLeaf,  0,  size);
//    for  (int  i  =  0;  i < l_hash_baskets;  ++i)  {
//      baskets[i].curHeight  =  2;
//    }

        //init rating queue:                
//    memset(&headNode,  0,  sizeof(TONode2));
//    headNode.mostUseful  =  &headNode;
//    headNode.leastUseful  =  &headNode;
  } //init

  void  clear()  {
    for (uint64_t  i  =  0;  i  <  leafAllocCounter;  ++i) {
      if (curLeaf[i].data)  {
        l_f_delData(curLeaf[i].data);
      }
    }
    if (baskets)  {
      free(baskets);
      baskets  =  nullptr;
    }
    if (curLeaf)  {
      free(curLeaf);
      curLeaf  =  nullptr;
    }
  }


  void  allocNode(const uint64_t  hash,  TKey const  *key,
      const char  *data,  const uint64_t  basketID,  int  cmp)  {
    TONode2  *re  =  nullptr;
    //TONode2  *prevHead  =  updatePathOut[0];

    if (leafAllocCounter  <  l_hash_baskets)  {
      re  =  &curLeaf[leafAllocCounter];
      ++leafAllocCounter; 
      //New leader = new:
      if (mostUseful)  {
        mostUseful->mostUseful  =  re;
      }  else  {
        leastUseful  =  re;
      }
      mostUseful  =  re;
      //re->mostUseful  =  nullptr; //delete this
    }

    if (!re)  {
            //reuse an older node                  
      while (true) {
        re  =  leastUseful;
        leastUseful  =  re->mostUseful;
        //getRateCount();
        if (re->used_count  <  l_uselessness) {
          break;
        }
        re->used_count  =  0;
        mostUseful->mostUseful  =  re;
        mostUseful  =  re;
        //re->mostUseful  =  nullptr; //delete this
        //getRateCount() ;
      }  //  while

      if (re->key->keyArray[0]==86635) {
          std::cout<<"\n";
      }

      if (hash  ==  re->hash)  {
        // Same  hash case
        if (re  ==  updatePathOutH->fwdPtrH)  {
          //re is head of hash queue
          l_f_delData(re->data);
          mostUseful->mostUseful  =  re;
          mostUseful  =  re;
          //re->mostUseful  =  nullptr; //delete this
          //getRateCount() ;
          if (3 == cmp  ||  updatePathOutL[0]  ==  re)  {
            //easy: key must be new head, just replace
            //cmp  =  777;
            re->key  =  key;
            re->data  =  data;
            return;
          }  else  {
            // swap re, re->fwdPtrsL[0]:
            TONode2  *cur  =  re->fwdPtrsL[0];            
            re->key  =  cur->key;
            re->data  =  cur->data;
            for (int  h  =  0;  h  <=  2;  ++h)  {
              if (h  <=  cur->curHeight) {
                // catchNode(re, h, cur->fwdPtrsL[h]) ;
                re->fwdPtrsL[h]  =  cur->fwdPtrsL[h];
                cur->fwdPtrsL[h]  =  nullptr;
              }
              if (updatePathOutL[h]  ==  cur) {
                updatePathOutL[h]  =  re;
              }              
            }
            re  =  cur;            
            //memset(re,  0,  sizeof(TONode2));
            re->used_count  =  l_uselessness; // because we are here broke age
            //next do regular alhorithm..
          }
        }  else  {
          // re not a head of the hash queue
          //assert(updatePathOutH->fwdPtrH->hash == hash);
          if (3  ==  cmp)  {
            delInSameHashCmp3(re, updatePathOutH->fwdPtrH);
          }  else  {
           // assert (4  !=  cmp);
            delInSameHash(re);
          }
          l_f_delData(re->data);
          memset(re,  0,  sizeof(TONode2));
          //New leader = new:
          mostUseful->mostUseful  =  re;
          mostUseful  =  re;
          //getRateCount() ;
        }
      }  else  {
        // Other hash case
        const uint64_t  re_basketID  =  re->hash % l_hash_baskets;
        delWithOtherHash(re,
            (re->hash > hash  &&  re_basketID == basketID)  ?
              updatePathOutH  :  &(baskets[re_basketID]));
        l_f_delData(re->data);
        memset(re,  0,  sizeof(TONode2));
        //New leader = new:
        mostUseful->mostUseful  =  re;
        mostUseful  =  re;
        //getRateCount() ;
      } // else

    }  //  if (!re)


    re->hash  =  hash;

    switch (cmp) {
    case -1:
    case 1:
      //using update path
      // 1 == not found who bigger with the same hash, use updatePathOut
      // -1 == found who bigger at the queue end, use updatePathOut
      re->key  =  key;
      re->data  =  data;
      re->curHeight  =  landscape_l[land_l_p++];
      for (int  i  =  0; i  <=  re->curHeight;  ++i)  {
        // catchNode(re, i, updatePathOutL[i]->fwdPtrsL[i]) ;
        re->fwdPtrsL[i]  =  updatePathOutL[i]->fwdPtrsL[i];
        // catchNode(updatePathOutL[i], i, re) ;
        updatePathOutL[i]->fwdPtrsL[i]  =  re;
      }
      break;
    case 3:
      //3 == replace head of hash queue, use updatePathOutH
      //re->curHeight  =  landscape_l[land_l_p++];
    {
      TONode2  *prevHead  =  updatePathOutH->fwdPtrH;
      updatePathOutH->fwdPtrH  =  re;
      re->fwdPtrH  =  prevHead->fwdPtrH;
      prevHead->fwdPtrH  =  nullptr;
      re->key  =  key;
      re->data  =  data;
      re->curHeight  =  2;
      prevHead->curHeight  =  landscape_l[land_l_p++];
      for (int  i  =  0; i  <=  2;  ++i)  {
        if (prevHead->curHeight  >=  i)  {
          // catchNode(re, i, prevHead) ;
          re->fwdPtrsL[i]  =  prevHead;
        }  else  {
          // catchNode(re, i, prevHead->fwdPtrsL[i]) ;
          re->fwdPtrsL[i]  =  prevHead->fwdPtrsL[i];
          prevHead->fwdPtrsL[i]  =  nullptr;
        }
      }
    }
      break;
    case 4:
      //4 == here no a same hash, create new hash queue, use updatePathOutH
      re->key  =  key;
      re->data  =  data;
      re->curHeight  =  2;
      re->fwdPtrH  =  updatePathOutH->fwdPtrH;
      updatePathOutH->fwdPtrH  =  re;
      break;
    default:
      assert(false);
    }
    return;
  }  //  allocNode


  TONode2 * find(TKey const  *key)  {
    const uint64_t hash  =  key->hash();
    const uint64_t basketID  =  hash % l_hash_baskets;
    TONode2 * cur  =  &(baskets[basketID]);
    while (cur->fwdPtrH  &&  cur->fwdPtrH->hash != hash) {
      cur  =  cur->fwdPtrH;
    }

    if (cur->fwdPtrH) {
      cur  =  cur->fwdPtrH;
      int  cmp  =  key->cmp(cur->key);
      if (0  ==  cmp)  {
        return cur;
      }
      if (cmp  <  0)  {
        //head is bigger, nothing to search
        return nullptr;
      }
      int  h  =  2;
      while (h  >=  0)  {
        while (cur->fwdPtrsL[h])  {
          cmp  =  key->cmp(cur->fwdPtrsL[h]->key);
          if (cmp  <  0)  {
            //found who bigger
            break;
          }  else  if (0  ==  cmp)  {
            return cur->fwdPtrsL[h];  // found
          }
          cur  =  cur->fwdPtrsL[h];  //step on it
        }
        --h;
      }
    }
    return nullptr;
  }  //  find


    /*
     * return:
     * 0 == node with equal key found, use only updatePathOut[0]
     * 3 == replace head of hash queue, use updatePathOutH
     * 4 == here no a same hash, create new hash queue, use updatePathOutH
     * 1 == not found who bigger with the same hash, use updatePathOut
     * -1 == found who bigger at the queue end, use updatePathOut
   */
    int  setll(uint64_t  hash,
        TKey const  *key,  const uint64_t basketID)  {
      TONode2  *cur  =  &(baskets[basketID]);
      hashOut  =  hash;      
      updatePathOutH  =  cur;
      while (cur->fwdPtrH  &&  hash > cur->fwdPtrH->hash)  {
        cur  =  cur->fwdPtrH;  //step on it
        updatePathOutH  =  cur;
      }

      if (cur->fwdPtrH  &&  cur->fwdPtrH->hash  ==  hash) {
        // same key jumps
        cur  =  cur->fwdPtrH;  //step on it
        int  cmp  =  key->cmp(cur->key);
        if (cmp  <  0)  {
          //updatePathOutL[0]  =  updatePathOutL[1] =  updatePathOutL[2] =  cur;
          return 3; //must replace hash head in place
        }
        if (0  ==  cmp) {
          updatePathOutL[0]  =  cur;
          return  0;
        }
        int  h  =  2;
        while (h  >=  0)  {
          updatePathOutL[h]  =  cur;
          while (cur->fwdPtrsL[h])  {
            //assert (hash  ==  cur->fwdPtrsL[h]->hash);
            cmp  =  key->cmp(cur->fwdPtrsL[h]->key);
            if (cmp  <  0)  {
              //found who bigger
              break;
            }  else  if (0  ==  cmp)  {
              //updatePathOutL[0]  =  updatePathOutL[1] =  updatePathOutL[2]  =  cur->fwdPtrsL[h];
              updatePathOutL[0]  =  cur->fwdPtrsL[h];
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
        //updatePathOutL[0] = updatePathOutL[1] = updatePathOutL[2] = nullptr;
        return 4;
      }
      return 8;
    }


    void delWithOtherHash(TONode2  *nodeToDel,  TONode2  *startSearch)  {
      //1. must find same hash:
      const uint64_t  hash  =  nodeToDel->hash;
      TONode2  *cur  =  startSearch;
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
          if (nodeToDel  ==  updatePathOutH)  {
            updatePathOutH  =  cur;
          }
        }
      }  else  {
        //3. regular del:
        delInSameHashCmp3(nodeToDel,  cur->fwdPtrH) ;
      }

      return;
    }

    void delInSameHashPathOut(TONode2  *nodeToDel,  int  top_h)  {
        //Node to del in updatePathOut[h], need path to it for update
      //new path may be not the same
      //const uint64_t  hash  =  nodeToDel->hash;
      TONode2  *updatePath[3];
      int  h  =  2;
      TONode2  *cur  =  updatePathOutH->fwdPtrH;
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
          //int  cmp  =  nodeToDel->key->cmp(cur->fwdPtrsL[h]->key);
          //assert (nodeToDel->hash  ==  cur->fwdPtrsL[h]->hash);
          if (nodeToDel->key->cmp(cur->fwdPtrsL[h]->key)  <=  0)  {
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
        // catchNode(updatePath[h], h, nodeToDel->fwdPtrsL[h]) ;
        updatePath[h]->fwdPtrsL[h]  =  nodeToDel->fwdPtrsL[h];
        //replace deleted node :
        if (nodeToDel  ==  updatePathOutL[h])  {
          updatePathOutL[h]  =  updatePath[h];
        }        
      }
      return;
    }//delInSameBasketPathOut

    void delInSameHashSuperFast(TONode2  *nodeToDel,  int  top_h)  {
        //need path before to update pointers              
      TONode2 * updatePath[3];
      if  (nodeToDel->curHeight  >  top_h)  {
        top_h  =  nodeToDel->curHeight;
      }
      updatePath[top_h]  =  updatePathOutL[top_h];
      TONode2  *cur  =  updatePathOutL[top_h];
      int  h  =  top_h;
      while (h  >=  0)  {
        updatePath[h]  =  cur;
        while (nodeToDel != cur->fwdPtrsL[h]  &&  cur->fwdPtrsL[h])  {
          //int  cmp  =  nodeToDel->key->cmp(cur->fwdPtrsL[h]->key);
          //assert (nodeToDel->hash  ==  cur->fwdPtrsL[h]->hash);
          if (nodeToDel->key->cmp(cur->fwdPtrsL[h]->key)  <=  0)  {
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
        // catchNode(updatePath[h], h, nodeToDel->fwdPtrsL[h]);
        updatePath[h]->fwdPtrsL[h]  =  nodeToDel->fwdPtrsL[h];
      }
      return;
    }  //  delInSameBasketSuperFast

    // for setll cmp==3 or 4, no need  updatePathOutL
    void  delInSameHashCmp3(TONode2  *nodeToDel, TONode2  *hashHead)  {
      int  h  =  2;
      TONode2 * updatePath[3];
      TONode2  *cur  =  hashHead;  //updatePathOutH->fwdPtrH;
      //int  cmp  =  1;
      while (h  >=  0)  {
        updatePath[h]  =  cur;
        while (nodeToDel != cur->fwdPtrsL[h]  &&  cur->fwdPtrsL[h])  {
          //cmp  =  nodeToDel->key->cmp(cur->fwdPtrsL[h]->key);
          //assert (nodeToDel->hash  ==  cur->fwdPtrsL[h]->hash);
          if (nodeToDel->key->cmp(cur->fwdPtrsL[h]->key)  <=  0)  {
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

    // need to check if UpdatePath affected
    void  delInSameHash(TONode2  *nodeToDel)  {
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
      delInSameHashCmp3(nodeToDel, updatePathOutH->fwdPtrH) ;
    } // delInSameHash


    void  catchNode(TONode2  *node, int h, TONode2  *in)  {
      if (1 == h) {
        if (node->key->keyArray[0] == 6693458) {
          if (in) {
          //if (in  &&  in->key->keyArray[0] == 2147339106) {
            std::cout  <<  in->data << " : " << in->hash <<"\n";
          }
          //std::cout<<"\n";
        }
      }
    }
};


#endif // ONCACHE2_H

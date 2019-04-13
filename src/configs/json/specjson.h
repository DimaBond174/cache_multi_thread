/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef SPECJSON_H
#define SPECJSON_H

#include "spectools.h"

//must be greater 4:
#define  LEAF_SIZE  32

class  TNode  {
 public:
  char  type;
  const char  *pKey;
  unsigned int  lenKey;
  void  *pData;
  unsigned int  lenData;
  TNode  *nextNode;
};

class  TLeaf  {
 public:
  TNode  nodes[LEAF_SIZE];
};

static const unsigned char  whitespaces[256]  =  {
	// 0    1    2    3    4    5    6    7      8    9    A    B    C    D    E    F
	   0,   0,   0,   0,   0,   0,   0,   0,     0,   1,   1,   0,   0,   1,   0,   0, // 0
	   0,   0,   0,   0,   0,   0,   0,   0,     0,   0,   0,   0,   0,   0,   0,   0, // 1
	   1,   0,   0,   0,   0,   0,   0,   0,     0,   0,   0,   0,   1,   0,   0,   0, // 2
	   0,   0,   0,   0,   0,   0,   0,   0,     0,   0,   0,   0,   0,   0,   0,   0, // 3
	   0,   0,   0,   0,   0,   0,   0,   0,     0,   0,   0,   0,   0,   0,   0,   0, // 4
	   0,   0,   0,   0,   0,   0,   0,   0,     0,   0,   0,   0,   0,   0,   0,   0, // 5
	   0,   0,   0,   0,   0,   0,   0,   0,     0,   0,   0,   0,   0,   0,   0,   0, // 6
	   0,   0,   0,   0,   0,   0,   0,   0,     0,   0,   0,   0,   0,   0,   0,   0, // 7

	// 128-255
	   0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0
};

static const unsigned char  digits[256]  =  {
	// 0    1    2    3    4    5    6    7      8    9    A    B    C    D    E    F
	   0,   0,   0,   0,   0,   0,   0,   0,     0,   0,   0,   0,   0,   0,   0,   0, // 0
	   0,   0,   0,   0,   0,   0,   0,   0,     0,   0,   0,   0,   0,   0,   0,   0, // 1
	   0,   0,   0,   0,   0,   0,   0,   0,     0,   0,   0,   0,   0,   0,   1,   0, // 2
	   1,   1,   1,   1,   1,   1,   1,   1,     1,   1,   0,   0,   0,   0,   0,   0, // 3
	   0,   0,   0,   0,   0,   1,   0,   0,     0,   0,   0,   0,   0,   0,   0,   0, // 4
	   0,   0,   0,   0,   0,   0,   0,   0,     0,   0,   0,   0,   0,   0,   0,   0, // 5
	   0,   0,   0,   0,   0,   1,   0,   0,     0,   0,   0,   0,   0,   0,   0,   0, // 6
	   0,   0,   0,   0,   0,   0,   0,   0,     0,   0,   0,   0,   0,   0,   0,   0, // 7

	// 128-255
	   0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0
};
// 'z' = zero, not a node
// 'n' = next leaf pointer
// 's' = string
// 'a' = array
// 'o' = object
// 'd' = digit
// 'l'  = true, false, null.
class  SpecJson  {
 public:
  SpecJson(const char  *json,  int  len,  bool  doCopy)  :  _json_len(len)  {
    if  (doCopy)  {
      _jsonCopy  =  std::string(json,  len);
      _json  =  _jsonCopy.c_str();
    }  else  {
      _json  =  json;
		}
    state  =  0;
    curLeaf  =  &headLeaf;
    curNode  =  0;
    headLeaf.nodes[maxNode].type  =  'z';
	}

  ~SpecJson()  {
    if  ('n'  ==  headLeaf.nodes[maxNode].type)  {
			freeLeafs((TLeaf *)(headLeaf.nodes[maxNode].pData));
		}
	}

  bool  parse()  {
    const char  *p  =  _json;
    const char  *pEnd  =  _json  +  _json_len;
    for  (;  p  <  pEnd;  ++p)  {
      if  ('{'  ==  *p)  {
				++p;
        if  (parseObject(p,  pEnd))  {
          if  (-1  !=  state)  {  state = 1;  }
					return true;
				}
			}
		}
		return false;
	}

  std::string  getStrByKey(const char  *key,  unsigned int  keyLen)  {
    return  getStrByKey(key,  keyLen,  &(headLeaf.nodes[0]));
	}

  inline bool  keyCompare(const char  *lKey,  unsigned int  lKeyLen,
      const char  *rKey,  unsigned int  rKeyLen)  {
    if  (lKeyLen == rKeyLen  &&  rKeyLen > 0)  {
      do  {
				--rKeyLen;
        if  (lKey[rKeyLen]  !=  rKey[rKeyLen])  {  return false;  }
      }  while  (rKeyLen > 0);
      return  true;
		}
    return  false;
	}

  std::string  getStrByKey(const char  *key,  unsigned int  keyLen,
      TNode  *jsonObj)  {
    std::string  re;
    if  (1  ==  state)  {
      TNode  *cur  =  jsonObj;
      while  (cur)  {
        if  (keyCompare(key,  keyLen,  cur->pKey,  cur->lenKey))  {
          switch  (cur->type)  {
					case 's':
					case 'd':
            return  std::string(static_cast<const char *>(cur->pData),  cur->lenData);
					}					
				}
        cur  =  cur->nextNode;
			}			
		}
		return re;
	}

  TNode * getObjByKey(const char  *key,  unsigned int  keyLen,
      TNode  *jsonObj)  {
    TNode  *re  =  nullptr;
    if  (1  ==  state)  {
      TNode  *cur  =  jsonObj;
      while  (cur)  {
        if  (keyCompare(key,  keyLen,  cur->pKey,  cur->lenKey))  {
          if  ('o'  ==  cur->type)  {
            re  =  cur;
						break;
					}
				}
        cur  =  cur->nextNode;
			}
		}
    return  re;
	}

  TNode * getFirstNodeOfObject(TNode  *objNode)  {
    TNode  *re  =  nullptr;
    if  (1  ==  state)  {
      if  (!objNode)  {
        objNode  =  &(headLeaf.nodes[0]);
			}
      if  ('o'  ==  objNode->type)  {
        re  =  (TNode *)objNode->pData;
      }  else if ('z'  !=  objNode->type)  {
        re  =  objNode;
			}
		}
		return re;
 }

 private:
  const char  *_json;
  unsigned int  _json_len;
  std::string  _jsonCopy;
  int  state;
  TLeaf  headLeaf;
  TLeaf  *curLeaf;
  unsigned int  curNode;
  const unsigned int  maxNode  =  (LEAF_SIZE  -  1);

  void  freeLeafs(TLeaf  *n)  {
    if ('n'  ==  n->nodes[maxNode].type)  {
      freeLeafs(static_cast<TLeaf *>(n->nodes[maxNode].pData));
		}
		delete n;
	}

  TNode * getNewNode()  {
    TNode  *re  =  &(curLeaf->nodes[curNode]);
    re->type  =  'z';
    re->lenData  =  re->lenKey  =  0;
    re->pKey  =  nullptr;
    re->pData  =  nullptr;
    re->nextNode  =  nullptr;
		++curNode;
    if  (curNode  ==  maxNode)  {
      TLeaf  *nextLeaf  =  new TLeaf();
      nextLeaf->nodes[maxNode].type  =  'z';
      TNode  *n  =  &(curLeaf->nodes[curNode]);
      n->type  =  'n';
      n->pData  =  reinterpret_cast<void *>(nextLeaf);
      n->lenData  =  n->lenKey  =  0;
      n->pKey  =  nullptr;
      n->nextNode  =  nullptr;
      curLeaf  =  nextLeaf;
      curNode  =  0;
		}
		return re;
	}

  inline unsigned int  getStrLen(const char * &p,  const char  *pEnd)  {
    unsigned int  re  =  0;
    while  (p  <  pEnd)  {
      if  ('\\' == *p  &&  '"' == *(p + 1))  {
				++re;
				++p;
      } 	else if ('"'  ==  *(p))  {
				return re;
			}
			++re;
			++p;
    }  //  while
    state  =  -1;
    return  0;
	}

  inline unsigned int  getNumLen(const char * &p,  const char  *pEnd)  {
    unsigned int  re  =  0;
    while  (p < pEnd  &&  digits[static_cast<unsigned char>(*p)])  {
			++re;
			++p;
		}
		return re;
	}

  TNode * parseObject(const char * &p, const char  *pEnd)  {
    TNode  *reNode  =  nullptr;
    TNode  *preNode  =  nullptr;
    TNode  *curNode  =  nullptr;
    unsigned char  searchLvl  =  0; //key
        //bool allOk = true;
    while  (p  <  pEnd)  {
      if  (whitespaces[static_cast<unsigned char>(*p)])  {
				++p;
				continue; 
			}
      switch  (*p)  {
			case '"':
        if  (0  ==  searchLvl)  {
          curNode  =  getNewNode();
          curNode->pKey  =  ++p;
          curNode->lenKey  =  getStrLen(p,  pEnd);
          if  (-1  ==  state)  {  return nullptr;  }
          curNode->type  =  'z'; //zero
          if  (preNode)  {
            preNode->nextNode  =  curNode;
            preNode  =  curNode;
          } 	else  {
            reNode  =  preNode  =  curNode;
					}
					++searchLvl;
        } 	else if  (2  ==  searchLvl)  {
					++p;
          curNode->pData  =  (void *)(p);
          curNode->lenData  =  getStrLen(p,  pEnd);
          curNode->type  =  's'; //string
          searchLvl  =  0;
				}
				break;
			case ':':
        if  (1  ==  searchLvl)  {
					++searchLvl;
        }		else  {
          state  =  -1;
          return  nullptr;
				}
				break;
			case '}':
        if  (0  ==  searchLvl)  {
          return  reNode;
        } 	else  {
          state  =  -1;
          return  nullptr;
				}
				break;
			case '{':
        if  (2  ==  searchLvl)  {
          curNode->pData  =  parseObject(++p,  pEnd);
          curNode->lenData  =  0;
          curNode->type  =  'o'; //object
          searchLvl  =  0;
        }	 else  {
          state  =  -1;
          return  nullptr;
				}
				break;
				//case '[':
				//	if (2==searchLvl){
				//	curNode->pData = parseArray(++p, pEnd);
				//	curNode->lenData = 0;
				//	curNode->type = 'a'; //array
				//	searchLvl = 0;
				//	} else {
				//	state = -1;
				//	return nullptr;
				//	}
				//	break;
			default:
        if  (2  ==  searchLvl)  {
          if  (digits[static_cast<unsigned char>(*p)])  {
            curNode->pData  =  (void *)(p);
            curNode->lenData  =  getNumLen(p,  pEnd);
            curNode->type  =  'd'; //digital
            searchLvl  =  0;
						break;
					}
				}
        return  nullptr;
			}
			++p;
    }  //  while (p<pEnd)
    if (0  ==  searchLvl
        && (p  ==  pEnd)
        && ( '}' == *(p-1))) {
      return  reNode;
    }
    state  =  -1;
    return  nullptr;
  }  //  parseObject
};


#endif // SPECJSON_H

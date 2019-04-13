/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef IFILEADAPTER_H
#define IFILEADAPTER_H

#include <string>

class  IFileAdapter  {
 public:
  virtual  ~IFileAdapter()  {  }
  virtual std::string  load_text_file(const char  *path)  =  0;

  /*
   * -1 = exists
   * -2 = fail to save
   * 1 = OK, saved
  */
  virtual int  save_text_file(const char  *path,  const char  *data,  uint32_t  len)  =  0;
  virtual bool  save_text_file_over(const char  *path,  const char  *data,  uint32_t  len)  =  0;

  virtual void  mkdirs(const std::string  &filePath)  =  0;
  virtual std::string  getDir(const std::string  &filePath)  =  0;
  virtual void  delOld(const std::string  &dir,  unsigned int  keepCount)  =  0;  
  virtual uint64_t  removeAll(const char * path)  =  0;
  virtual bool file_exists(const char * path)  =  0;

};


#endif // IFILEADAPTER_H

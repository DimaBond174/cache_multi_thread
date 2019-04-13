/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef ISYSTEM_H
#define ISYSTEM_H

#include <memory>

//typedef bool (*TWaitFunc)(void  *ptr);

class  ISystem  {
 public:
  virtual ~ISystem()  { }
  virtual std::string  getExePath()  =  0;
  virtual std::string  toFullPath(const char  *path)  =  0;
  /*
   *  path - full or relative path to file
  */
  virtual std::string  load_text_file(const char  *path)  =  0;

  /*
   * path - full or relative path to file
   * -1 = exists
   * -2 = fail to save
   * 1 = OK, saved
  */
  virtual int  save_text_file(const char  *path,  const char  *data,  uint32_t  len)  =  0;
  virtual bool  save_text_file_over(const char  *path,  const char  *data,  uint32_t  len)  =  0;
  virtual std::string  execCmd(const char  *cmd)  =  0;
  virtual std::string  getSockPath(const char  *serviceName)  =  0;
  virtual std::string  sendCmd(const char  *serviceName,  const char  *cmd)  =  0;
 // virtual bool  waitForSUCCESS(TWaitFunc  f,  void  *ptr,
  //  int msRepeat,  int msTimeout)  =  0;
  virtual int get_CPU_cache_line_size()  =  0;
  virtual bool  mkdirs_for_file(const char  *file_path)  = 0;
  virtual void  wait_semaphore()  =  0;
  virtual void  signal_semaphore(unsigned int  count)  =  0;
};

#endif // ISYSTEM_H

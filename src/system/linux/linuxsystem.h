/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef LINUXSYSTEM_H
#define LINUXSYSTEM_H

#include <string>
#include "i/isystem.h"
#include "system/filesystem/fileadapter.h"
#include "linuxsemaphore.h"

class LinuxSystem  :  public ISystem  {
 public:
  LinuxSystem();

  std::string  getExePath()  override;
  std::string  toFullPath(const char  *path)  override;
  std::string  load_text_file(const char  *path)  override;
  int  save_text_file(const char  *path,  const char  *data,  uint32_t  len)  override;
  bool  save_text_file_over(const char  *path,  const char  *data,  uint32_t  len)  override;
  std::string  execCmd(const char  *cmd)  override;
  std::string  sendCmd(const char  *serviceName,  const char  *cmd)  override;
  std::string  getSockPath(const char  *serviceName)  override;
  //bool  waitForSUCCESS(TWaitFunc  f,  void  *ptr,
   //int  msRepeat,  int  msTimeout)  override;
  int get_CPU_cache_line_size()  override;
  bool  mkdirs_for_file(const char  *file_path) override;
  void  wait_semaphore()  override;
  void  signal_semaphore(unsigned int  count)  override;

    /* static direct calls from system related classes: */
  static std::string  getExePathS();
  static std::string  getExeS();
  static std::string  execCmdS(const char  *cmd);
  static std::string  sendCmdS(const char  *serviceName,  const char  *cmd);
  static std::string  getSockPathS(const char  *serviceName);

 private:
   FileAdapter  file_adapter;
   std::string  exePath;
   Semaphore  semaphor;
};

#endif // LINUXSYSTEM_H

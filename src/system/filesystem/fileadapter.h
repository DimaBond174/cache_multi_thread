/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#ifndef FILEADAPTER_H
#define FILEADAPTER_H

#include "i/ifileadapter.h"
#include <experimental/filesystem>

namespace  fs  =  std::experimental::filesystem ;

struct file_info {
    fs::path path ;
    fs::file_time_type last_write_time ;
   // std::uintmax_t size ;
} typedef t_file_info;

class  FileAdapter  :  public IFileAdapter  {
 public:
  FileAdapter();
/* interface methods */
  std::string  load_text_file(const char  *path)  override;
  int  save_text_file(const char  *path,  const char  *data,  uint32_t  len)  override;
  bool  save_text_file_over(const char  *path,  const char  *data,  uint32_t  len)  override;

  void  mkdirs(const std::string  &filePath)  override;
  std::string  getDir(const std::string  &filePath)  override;
  void  delOld(const std::string  &dir,  unsigned int  keepCount)  override;
  uint64_t  removeAll(const char  *path)  override;
  bool file_exists(const char * path) override;

  bool  mkdirs_for_file(const char  *file_path);

/* static members */
  static std::string  load_text_file_S(const char  *filePath);
  static void  mkdirsS(const std::string  &filePath);
  static std::string  getDirS (const std::string  &filePath);
  static void  delOldS(const std::string  &dir,  unsigned int  keepCount);
  static int  save_text_file_S(const char  *path,  const char  *data,  uint32_t  len) ;
  static bool  save_text_file_over_S(const char  *filePath,  const char  *data,  uint32_t  len);
    //https://en.cppreference.com/w/cpp/filesystem/create_symlink
  static bool  createSymlink(const std::string  &target,  const std::string  &link);
  static uint64_t  removeAllS(const char  *path);
  static bool file_existsS(const char * path);

 private:  
   static std::vector<t_file_info>  file_list(const fs::path  &dir);
   static bool  mkdir_p(const char  *filePath);
};

#endif // FILEADAPTER_H

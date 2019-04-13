/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#include "fileadapter.h"
#include <fstream>
#include <streambuf>
#include <sys/types.h>

#include <stdio.h>

#include <string.h>
#include <limits.h>     /* PATH_MAX */
#include <sys/stat.h>   /* mkdir(2) */
#include <errno.h>
#include <algorithm>
#include <vector>

#include "spec/specstatic.h"
#include <iostream>

#if defined(Windows)
#include <direct.h>
#ifndef PATH_MAX 
#define PATH_MAX _MAX_PATH
#endif // !

#endif

static thread_local std::error_code  ec;

FileAdapter::FileAdapter()  {  }

std::string  FileAdapter::load_text_file(const char  *path)  {
  return FileAdapter::load_text_file_S(path);
}

std::string  FileAdapter::load_text_file_S(const char  *filePath)  {
  std::string  str;
  if  (fs::exists(filePath, ec))  {
    std::ifstream  t(filePath);
    t.seekg(0,  std::ios::end);
    str.reserve(t.tellg());
    t.seekg(0,  std::ios::beg);
    str.assign((std::istreambuf_iterator<char>(t)),
        std::istreambuf_iterator<char>());
  }
  return str;
}

//bool FileAdapter::file_exists(const char * path) {
//    struct stat fileStat;
//    if ( stat(path, &fileStat) )
//    {
//        return false;
//    }
//    if ( !S_ISREG(fileStat.st_mode) )
//    {
//        return false;
//    }
//    return true; //Есть такой и это файл
//}

void  FileAdapter::mkdirs(const std::string  &filePath)  {
  FileAdapter::mkdirsS(filePath);
}

void  FileAdapter::mkdirsS(const std::string  &filePath)  {
    /* Adapted from http://stackoverflow.com/a/2336245/119527 */
    //const size_t len = strlen(path);
  const  size_t  len  =  filePath.length();
  char  _path[PATH_MAX];
  char  *p;
  errno  =  0;
  if  (len  >  sizeof(_path)  -  1)  {
    errno  =  ENAMETOOLONG;
    return;
  }
    //strcpy(_path, path);
  strcpy(_path,  filePath.c_str());
    /* Iterate the string */
  for  (p  =  _path  +  1;  *p;  p++)  {
    if  (*p == '/'  ||  *p == '\\')  {
            /* Temporarily truncate */
      *p  =  '\0';
#if defined(Windows)
      if (_mkdir(_path)  !=  0) {
#else
      if  (mkdir(_path,  S_IRWXU)  !=  0)  {
#endif
        if  (errno  !=  EEXIST)  {
          return;
        }
      }
      *p  =  '/';
    }
  }  //  for

#if defined(Windows)
  if  (_mkdir(_path)  !=  0)  {
#else
  if  (mkdir(_path,  S_IRWXU)  !=  0)  {
#endif    
    if  (errno  !=  EEXIST)  {
            return;
    }
  }
  return;
}  //  mkdirsS

std::string FileAdapter::getDir (const std::string  &filePath)  {
  return FileAdapter::getDirS(filePath);
}

std::string FileAdapter::getDirS(const std::string  &filePath)  {
  size_t found  =  filePath.find_last_of("/\\");
  return(filePath.substr(0, found));
}

std::vector<t_file_info>  FileAdapter::file_list(const fs::path  &dir)  {
  std::vector<t_file_info>  result ;
  for  (const auto  &p  :  fs::recursive_directory_iterator(dir) )  {
    const auto  &path  =  p.path() ;
    if  (fs::is_regular_file(path))
      result.push_back( t_file_info  {
        path, fs::last_write_time(path)
        //,fs::file_size( path )
        } ) ;
    }
    return result ;
}

void  FileAdapter::delOld(const std::string  &dir,  unsigned int  keepCount)  {
    delOldS(dir,  keepCount);
}

void  FileAdapter::delOldS(const std::string  &dir,  unsigned int  keepCount)  {
  try  {
    auto&&  flist  =  file_list(dir) ;
    static const auto  cmp_times  =  [] (const t_file_info  &a,  const t_file_info  &b)
      {  return  (a.last_write_time  >  b.last_write_time);  } ;
    std::sort( std::begin( flist ),  std::end( flist ),  cmp_times ) ;
    for  (std::size_t  i  =  flist.size();  i  >  keepCount ;  --i)  {
      fs::remove( flist[i-1].path ) ;
    }
  }  catch  (const std::exception& ) {  }
}  //  delOldS

int  FileAdapter::save_text_file(const char  *path,  const char  *data,  uint32_t  len)  {
  return save_text_file_S(path,  data,  len);
}

int  FileAdapter::save_text_file_S(const char  *path,  const char  *data,  uint32_t  len)  {
  int  re  =  -1;
  //try {
    if  (!fs::exists(path, ec))  {
      if  (save_text_file_over_S(path,  data,  len))  {
        re  =  1;
      }  else  {
        re  =  -2;
      }
    }
  //} catch (...) {}

  return re;
}

bool  FileAdapter::save_text_file_over(const char  *path,  const char  *data,  uint32_t  len)  {
  return  save_text_file_over_S(path,  data,  len) ;
}

bool  FileAdapter::save_text_file_over_S(const char  *filePath,  const char  *data,  uint32_t  len)  {
  bool  re  =  false;
  if  (mkdir_p(filePath))  {
    std::ofstream  outfile(filePath);
    outfile.write(data,  len);
    outfile.close();
    re  =  true;
  }
  return re;
}

bool FileAdapter::mkdirs_for_file(const char  *file_path) {
  return mkdir_p(file_path);
}

/* Local usage for saveFile to enshure file path exists */
bool  FileAdapter::mkdir_p(const char  *filePath)  {
  if  (!(filePath  &&  *filePath) )  {  return  false;  }
  char  dir_path[PATH_MAX];
  char  *p  =  dir_path;
  char  *end  =  dir_path  +  PATH_MAX;
  char  *last_slash  =  dir_path;
  while  (*filePath  &&  p < end)  {
    *p  =  *filePath;
    if  ('/' == *filePath  ||  '\\' == *filePath)  {
      last_slash  =  p;
    }
    ++p;
    ++filePath;
  }
  *last_slash  = '\0';
  if (fs::is_directory(dir_path, ec))  {
    return true;
  }
  return fs::create_directories(dir_path, ec);
}  //  mkdir_p

bool  FileAdapter::createSymlink(const std::string  &target,  const std::string  &link)  {
  fs::create_symlink(fs::path(target),  fs::path(link),  ec);
  return !ec;
}

uint64_t  FileAdapter::removeAll(const char  *path)  {
    return  removeAllS(path);
}

uint64_t  FileAdapter::removeAllS(const char  *path)  {
  uint64_t  re  =  0;
 // try  {
    re  =  fs::remove_all(path, ec);
 // }  catch  (...)  {  }
  return re;
}

bool FileAdapter::file_exists(const char * path) {
  return file_existsS(path);
}

bool FileAdapter::file_existsS(const char * path)  {
 // bool  re = false;
  //try {
  //  re  =  fs::exists(path, ec);  //fs::exists(fs::path(path))  ;
  //} catch(...) {}
  return fs::exists(path, ec);
}

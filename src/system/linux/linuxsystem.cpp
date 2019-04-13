/*
 * This is the source code of SpecNet project
 * It is licensed under MIT License.
 *
 * Copyright (c) Dmitriy Bondarenko
 * feel free to contact me: specnet.messenger@gmail.com
 */

#include "linuxsystem.h"
#include <unistd.h>
#include <limits.h>
#include <linux/limits.h>
#include <dlfcn.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <array>
#include <sys/socket.h>
#include <sys/un.h>
#include <thread>
#include <algorithm>

#define BUFFER_SIZE 256

LinuxSystem::LinuxSystem()  {
  exePath = getExePath();
}

std::string  LinuxSystem::getExePath()  {
  return  getExePathS();
}


std::string LinuxSystem::getExePathS(){    
#if defined(Linux)
    char  buf[PATH_MAX];
    ssize_t  len  =  0;
    len = readlink("/proc/self/exe",  buf,  PATH_MAX);
    if (len>0) {
        for(--len;  len>0;  --len) {
            if ('/'  ==  buf[len]){
              return  std::string(buf,  len + 1);
            }
        }
    }
#endif
    return std::string();
}

std::string  LinuxSystem::toFullPath(const char  *path)  {
  std::string  re(exePath);
  if('.' == path[0]  && '/' == path[1])  {
    re.append((path + 2));
  }  else  {
    re.append(path);
  }
  return re;
}

bool  LinuxSystem::save_text_file_over(const char  *path,  const char  *data,  uint32_t  len)  {
  if (path) {
    if ('/'  ==  path[0]) {
      return file_adapter.save_text_file_over(path, data,  len);
    } else {
      return file_adapter.save_text_file_over(toFullPath(path).c_str(),  data,  len);
    }
  }
  return  false;
}

int  LinuxSystem::save_text_file(const char  *path,  const char  *data,  uint32_t  len)  {
  if (path) {
    if ('/'  ==  path[0]) {
      return file_adapter.save_text_file(path, data,  len);
    } else {
      return file_adapter.save_text_file(toFullPath(path).c_str(),  data,  len);
    }
  }
  return  -2;
}

std::string  LinuxSystem::load_text_file(const char  *path)  {
  if (path) {
    if ('/'  ==  path[0]) {
      return FileAdapter::load_text_file_S(path);
    } else {
      return FileAdapter::load_text_file_S(toFullPath(path).c_str());
    }
  }
  return std::string();
}

std::string LinuxSystem::getExeS(){
    std::string _exePath;
#if defined(Linux)
    char buf[PATH_MAX];
    ssize_t len = 0;
    len = readlink("/proc/self/exe", buf, PATH_MAX);
    if (len>0) {
        _exePath = std::string(buf, len);
    }
#endif
    return _exePath;
}

std::string LinuxSystem::execCmd(const char * cmd) {
    return execCmdS(cmd);
}

std::string LinuxSystem::execCmdS(const char * cmd){
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    //in deleter of shared_ptr: pclose(pipe);
    return result;
}

std::string LinuxSystem::sendCmd(const char * serviceName, const char * cmd) {
    return sendCmdS(serviceName, cmd);
}


std::string LinuxSystem::sendCmdS(const char * serviceName, const char * cmd) {
    std::string re;
    int sock = -1;
    //faux loop
    do {
        //int sock = ::socket(PF_UNIX, SOCK_STREAM, 0);
        int sock = ::socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if (-1 == sock) {
            //std::cerr << "Error: LinuxService::sendCmd  -1 == ::socket(" << std::endl;
            break;
        }
        struct sockaddr_un addr;
        ::memset(&addr, 0, sizeof(struct sockaddr_un));
        addr.sun_family = AF_UNIX;
        const std::string & sock_path = LinuxSystem::getSockPathS(serviceName);
        ::memcpy(addr.sun_path, sock_path.c_str(), sock_path.length());

        int res = ::connect(sock, (struct sockaddr *)&addr, SUN_LEN(&addr));
        if (-1 == res ) {
            //std::cerr << "Error: LinuxService::sendCmd  ::connect(sock to "
            //          << sock_path << std::endl;
            break;
        }

        /* Send. */
        res = write(sock, cmd, strlen(cmd) + 1);
        if (-1 == res ) {
            //std::cerr << "Error: LinuxService::sendCmd  ::write(sock to "
            //          << sock_path << std::endl;
            break;
        }

        char buffer[BUFFER_SIZE];
        /* Receive result. */
        res = read(sock, buffer, BUFFER_SIZE);
        if ( res < 0 ) {
            //std::cerr << "Error: LinuxService::sendCmd  ::read(sock from "
             //         << sock_path << std::endl;
            break;
        }

        //buffer[res] = 0;
        re = std::string(buffer, res);
        //printf("Server answer: %s\n", buffer);

        //re = true;
    } while (false);
    if (-1 != sock) { ::close(sock); }

    return re;
}

std::string LinuxSystem::getSockPath(const char * serviceName) {
    return getSockPathS(serviceName);
}

std::string LinuxSystem::getSockPathS(const char * serviceName) {
   std::string legal (serviceName);
   std::transform(legal.begin(), legal.end(), legal.begin(), [](char ch){
       const char * legal = "abcdefghijklmnopqrstuvwxyz1234567890";
       for (const char *p = legal; *p; ++p) {
           if (*p==ch) { return ch; }
       }
       return 'a'; } );
   std::string re ("/var/tmp/");
   re.append(legal);
   return re;
}

//bool LinuxSystem::waitForSUCCESS(TWaitFunc f, void * ptr,
//                    int msRepeat,
//                    int msTimeout) {
//    auto start = std::chrono::system_clock::now();
//    while (std::chrono::system_clock::now() - start < std::chrono::milliseconds(msTimeout)) {
//        if ((*f)(ptr)) {
//            return true;
//        }
//        std::this_thread::sleep_for(std::chrono::milliseconds(msRepeat));
//    }//while
//    return false;
//}


int LinuxSystem::get_CPU_cache_line_size()  {
  //you can check it from command: ~$ getconf LEVEL1_DCACHE_LINESIZE
  //64
  //https://stackoverflow.com/questions/794632/programmatically-get-the-cache-line-size
  return sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
}

bool  LinuxSystem::mkdirs_for_file(const char  *file_path)  {
  if (file_path) {
    if ('/'  ==  file_path[0]) {
      return file_adapter.mkdirs_for_file(file_path);
    } else {
      return file_adapter.mkdirs_for_file(toFullPath(file_path).c_str());
    }
  }
  return  false;
}

void  LinuxSystem::wait_semaphore()  {
  semaphor.wait();
}

void  LinuxSystem::signal_semaphore(unsigned int  count)  {
  semaphor.signal(count);
}

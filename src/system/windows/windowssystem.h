#ifndef WINDOWSSYSTEM_H
#define WINDOWSSYSTEM_H

#include <string>
#include "i/isystem.h"
#include <windows.h>

typedef bool(*TMailCallback)(const char * str);

class WindowsSystem : public ISystem
{
public:
    WindowsSystem();

    std::string getExePath() override;
	std::string execCmd(const char * cmd) override;
	std::string sendCmd(const char * serviceName, const char * cmd) override;
	std::string getSockPath(const char * serviceName) override;
	bool waitForSUCCESS(TWaitFunc f, void * ptr,
		int msRepeat,
		int msTimeout) override;
	std::shared_ptr<ILib> openSharedLib(const char * libPath) override;
	void closeSharedLib(const std::shared_ptr<ILib> &iLib) override;
  int get_CPU_cache_line_size()  override

	/* static direct calls from system related classes: */
	static std::string getExePathS();
	static std::string getExeS();
	static std::string execCmdS(const char * cmd);
	static std::string sendCmdS(const char * serviceName, const char * cmd);
	static std::string getSockPathS(const char * serviceName);
	static std::shared_ptr<ILib> openSharedLibS(const char * libPath);
	static void closeSharedLibS(const std::shared_ptr<ILib> &iLib);
	static std::string getParentDir(const char * path);
	static bool writeMailSlot(const std::string & slot, const std::string & msg);  
};

#endif // WINDOWSSYSTEM_H

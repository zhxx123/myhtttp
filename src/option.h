#ifndef __OPTION__
#define __OPTION__
#include <stdint.h>
#include <string>
#include <vector>
#include <cstdio>
#include <stdarg.h>
#include <fstream>
#include <string>
#include <iostream>
#include <map>
#include <ctime>
#include <cstring>
#include "utilstrencodings.h"
#define DBG( x ) x

enum LoggerLevel {
    DISABLE=0,
    ERROR,
    WARNING,
    INFO,
    DEBUG
};
#define LogPrintf(format,...) fprintf(stdout,format, ##__VA_ARGS__)
#define __func__ __FILE__
const bool DEFAULT_FALSE=false;
const bool DEFAULT_TRUE=true;
static bool InterpretBool(const std::string& strValue);
static void InterpretNegativeSetting(std::string& strKey, std::string& strValue);
void ParseParameters(int argc, const char* const argv[]);
std::string GetArg(const std::string& strArg, const std::string& strDefault);
int64_t GetArg(const std::string& strArg, int64_t nDefault);
bool GetBoolArg(const std::string& strArg, bool fDefault);
bool SoftSetArg(const std::string& strArg, const std::string& strValue);
bool SoftSetBoolArg(const std::string& strArg, bool fValue);
std::string HelpMessageGroup(const std::string &message);
std::string HelpMessageOpt(const std::string &option, const std::string &message);
// void setLog(bool fDefault);
bool LogPrint(const int level, const char* format,...);
std::string GetSystemTime();
#endif //__OPTION__
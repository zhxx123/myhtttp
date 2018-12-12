#include "option.h"
std::map<std::string, std::string> mapArgs;
std::map<std::string, std::vector<std::string> > mapMultiArgs;

bool fplog=false;
bool AllisNum(const std::string str)
{
    int size=str.size();
    for(int i=0;i<size;i++)
    {
        int tmp=(int)str[i];
        if(tmp>=48 && tmp<=57)
            continue;
        else 
            return false;
    }
    return true;
}
/** Interpret string as boolean, for argument parsing */
static bool InterpretBool(const std::string& strValue)
{
    if (strValue.empty())
        return true;
    return (atoi(strValue) != 0);
}

/** Turn -noX into -X=0 */
static void InterpretNegativeSetting(std::string& strKey, std::string& strValue)
{
    if (strKey.length()>3 && strKey[0]=='-' && strKey[1]=='n' && strKey[2]=='o')
    {
        strKey = "-" + strKey.substr(3);
        strValue = InterpretBool(strValue) ? "0" : "1";
    }
}
void ParseParameters(int argc, const char* const argv[])
{
    mapArgs.clear();
    mapMultiArgs.clear();

    for (int i = 1; i < argc; i++)
    {
        std::string str(argv[i]);
        std::string strValue;
        size_t is_index = str.find('=');
        if (is_index != std::string::npos)
        {
            strValue = str.substr(is_index+1);
            str = str.substr(0, is_index);
        }
#ifdef WIN32
        boost::to_lower(str);
        if (boost::algorithm::starts_with(str, "/"))
            str = "-" + str.substr(1);
#endif

        if (str[0] != '-')
            break;

        // Interpret --foo as -foo.
        // If both --foo and -foo are set, the last takes effect.
        if (str.length() > 1 && str[1] == '-')
            str = str.substr(1);
        InterpretNegativeSetting(str, strValue);

        mapArgs[str] = strValue;
        mapMultiArgs[str].push_back(strValue);
    }
}

std::string GetArg(const std::string& strArg, const std::string& strDefault)
{
    if (mapArgs.count(strArg))
        return mapArgs[strArg];
    return strDefault;
}

int64_t GetArg(const std::string& strArg, int64_t nDefault)
{
    if (mapArgs.count(strArg))
        return atoi64(mapArgs[strArg]);
    return nDefault;
}

bool GetBoolArg(const std::string& strArg, bool fDefault)
{
    if (mapArgs.count(strArg))
    {
        return InterpretBool(mapArgs[strArg]);
    }
    return fDefault;
}

bool SoftSetArg(const std::string& strArg, const std::string& strValue)
{
    if (mapArgs.count(strArg))
        return false;
    mapArgs[strArg] = strValue;
    return true;
}

bool SoftSetBoolArg(const std::string& strArg, bool fValue)
{
    if (fValue)
        return SoftSetArg(strArg, std::string("1"));
    else
        return SoftSetArg(strArg, std::string("0"));
}

static const int screenWidth = 79;
static const int optIndent = 2;
static const int msgIndent = 7;

std::string HelpMessageGroup(const std::string &message) 
{
    return std::string(message) + std::string("\n\n");
}

std::string HelpMessageOpt(const std::string &option, const std::string &message) 
{
    return std::string(optIndent,' ') +std::string(option) +
           std::string("\n") + std::string(msgIndent,' ') +message+
           std::string("\n");
}
bool logLevel(const int level,std::string &info)
{
    if(level<1 || level>4) return false;
    int pLogLevel=GetArg("-log",LoggerLevel::WARNING);
    if(level>pLogLevel) return false;
    switch(level)
    {
        case 1: info="ERROR";break;
        case 2: info="WARNING";break;
        case 3: info="INFO ";break;
        case 4: info="DEBUG";break;
        default:info="WARNING";break;
    }
    return true;
}
bool LogPrint(const int level, const char* format,...)
{
    std::string info;
    if(!logLevel(level,info)) return false;
    char logbuff[65530]={0};
    printf("%s%s:: ",GetSystemTime().c_str(),info.c_str());
    va_list argptr;
    va_start(argptr,format);
    vsprintf(logbuff,format,argptr);
    printf("%s",logbuff);
    va_end(argptr);
    return true;
}
std::string GetSystemTime()
{ 
    time_t tNowTime;
    time(&tNowTime);
    tm* tLocalTime = localtime(&tNowTime);   
    char szTime[30] = {'\0'};    
    strftime(szTime, 30, "[%Y-%m-%d %H:%M:%S] ", tLocalTime);   
    std::string strTime = szTime;
    return strTime;    
}
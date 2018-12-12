#include <iostream>
#include <string>
#include "httpevent.h"
#include "http.h" 
#include "dockerothers.h"
#include "option.h"
#include "color.h"
std::string HelpMessage()
{
    //When adding new options to the categories, please keep and ensure alphabetical ordering.
    string strUsage = HelpMessageGroup(("Options:"));
    strUsage += HelpMessageOpt("-h", ("This help message"));
    strUsage += HelpMessageOpt("-host", ("server http ip,default <-ip=192.168.0.86:2375>"));
    strUsage += HelpMessageOpt("-method", ("Execute the get or post method,default<-method=get>"));
    strUsage += HelpMessageOpt("-data", ("Data for the post method,default <-data=\"\">"));
    strUsage += HelpMessageOpt("-url", ("Http request path,default <-url=/version>"));
    strUsage += HelpMessageOpt("-json", ("json print level,default <-json=0>"));
    strUsage += HelpMessageOpt("-log", ("Log print level,default <-log=2>"));
    return strUsage;
}
bool init(int argc,char **argv)
{
    if(argc<2)
    {
        LogPrintf("USAGE:\n    %s -h (for more help)\n",argv[0]);
        return  false;
    }
    ParseParameters(argc,argv);
    if(GetBoolArg("-h",DEFAULT_FALSE))
    {
        std::cout<<HelpMessage()<<std::endl;
        return false;
    }
    int pLogLevel=GetArg("-log",2);
    std::string message="";
    if(pLogLevel==-1){
        SoftSetArg("-log",std::to_string(LoggerLevel::WARNING));
        pLogLevel=2;
        message="LogLevel init, Use default LogLevel, ";
    }
    LogPrintf("%sthis current LogLevel: %d\n",message.c_str(),pLogLevel);
    return true;
}
void GetDataFromJson(std::string& JSDataTmp,std::string &url)
{
    // std::cout<<JSDataTmp<<std::endl;
    // std::cout<<"*********************************\n";
    if(JSDataTmp.empty() || JSDataTmp.size()==0){
        return;
    }
    if(GetBoolArg("-json",false)){
        std::string otherData;
        dockerothers(JSDataTmp,otherData);
        std::cout<<otherData<<std::endl;
    }
}
int http(std::string method,std::string data,std::string ip,int port,std::string url)
{
    int ret=0;
    HttpRequest http(ip,port,url,data);
    LogPrintf("http info:  -host=%s:%d  -url=%s  -data=%s\n",ip.c_str(),port,url.c_str(),data.c_str());
    if(method =="get") {
        if((ret=http.HttpGet()) != 0) {
            std::cerr << "error!! error_code:" << ret << std::endl;
            return -1;
        }
        std::string reponse_data=http.getReponseData();
        GetDataFromJson(reponse_data,http.url);

    }else if(method == "delete"){
        if(data.empty()||data.size()==0){
            std::cout << "error!! post of data cann't be empty\n";
            return -2;
        }
        if((ret=http.HttpDelete()) != 0) {
            std::cerr << "error!! error_code:" << ret << std::endl;
            return -3;
        }
        std::string reponse_data=http.getReponseData();
        std::cout << reponse_data << std::endl;  
    }else if(method == "post"){
        std::cout<<data<<std::endl;
        if(data.empty()||data.size()==0){
            std::cout << "error!! post of data cann't be empty\n";
            return -2;
        }
        if((ret=http.HttpPost()) != 0) {
            std::cerr << "error!! error_code:" << ret << std::endl;
            return -3;
        }
        std::string reponse_data=http.getReponseData();
        std::cout << "response: " <<reponse_data << std::endl;  
    }else{
        std::cerr<<"unkonw http method\n";
        return -4;
    }
    return 0;
}
bool getIPPort(std::string host,std::string &ip,int &port)
{
    int index=host.find(":");
    ip=host.substr(0,index);
    if(index!=-1){
        std::string sport=host.substr(index+1);
        port=std::stoi(sport.c_str(),nullptr,10);
    }else{
        port=80;
    }
    return true;
}
int main(int argc,char **argv)
{
    if(!init(argc,argv)) return 0;
    string method=GetArg("-method","get");
    string data=GetArg("-data","");
    string host = GetArg("-host","132.232.79.195:2375");
    std::string ip="";
    int port;
    getIPPort(host,ip,port);
    string url =GetArg("-url","");
    if(http(method,data,ip,port,url)!=0){
        return 0;
    }
    // httpevent(ip,port,url);
    return 0;  
}

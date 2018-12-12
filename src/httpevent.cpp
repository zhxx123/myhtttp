#include "httpevent.h"
#include "http.h"

void req_cb(struct evhttp_request *req, void *arg)
{ 
    char buf[4096];
    struct evbuffer* evbuf = evhttp_request_get_input_buffer(req);
    int n = 0;
    std::string strResponse="";
    while ((n = evbuffer_remove(evbuf, buf, 4096)) > 0)
    {
        fwrite(buf, n, 1, stdout);
        strResponse+=string(buf,n);
    }
    std::cout<<strResponse<<std::endl;
}
void httpevent(std::string ips,int ports,std::string url)
{
    std::string ip=ips;
    int port=ports;
    printf("ip:%s port:%d\n",ip.c_str(),port);
    struct event_base* base; 
    struct evhttp_connection* conn; 
    struct evhttp_request* req; base = event_base_new(); 
    conn = evhttp_connection_new(ip.c_str(), port); 
    evhttp_connection_set_base(conn, base); 
    req = evhttp_request_new(req_cb, base); 
    evhttp_add_header(req->output_headers, "Host", ip.c_str());
    evhttp_add_header(req->output_headers, "Connection", "close");
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, url.c_str()); 
    evhttp_connection_set_timeout(req->evcon, 600); 
    event_base_dispatch(base); 
    evhttp_connection_free(conn); 
    event_base_free(base); 
    printf("run over...\n"); 
    return; 
}
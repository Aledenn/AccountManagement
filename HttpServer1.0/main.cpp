#include <iostream>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/http.h>
#include <string.h>
#include <mutex>
#include <thread>
#include <unistd.h>

#include "HttpCommon.h"
#include "Login.h"
#include "Play.h"
#include "Interface.h"
#include "ThreadPool.h"
#define TNUM 70
using namespace std;

CThreadPool *pMypoll;
bool startHttpServer(const char* ip, int port, void(*cb)(struct evhttp_request *, void *), void *arg)
{
	//����event_base��evhttp
	event_base* base = event_base_new();
	evhttp* http_server = evhttp_new(base);
	if (!http_server) {
		return false;
	}
	//�󶨵�ָ����ַ��
	int ret = evhttp_bind_socket(http_server, ip, port & 0xFFFF);
	if (ret != 0) {
		return false;
	}
	//�����¼�������
	evhttp_set_gencb(http_server, cb, arg);
	//һ�α����
//	event_base_loopexit(base,NULL);
	//�����¼�ѭ��������http�����ʱ������ָ���Ļص�
	//unique_lock<mutex> lck(mtx);
	//while(1)
	//{
//		cout<<"a"<<endl;
	  event_base_dispatch(base);
//	  cout<<"ba"<<endl;
	 // cv.wait(lck);
	  //}
	evhttp_free(http_server);
	return true;
}

void MyHttpServerHandler(struct evhttp_request* req );

void start_thread(struct evhttp_request* req,void *arg)
{
	//thread a(MyHttpServerHandler,req);
	//a.detach();
	pMypoll->AddTask(MyHttpServerHandler,req);

	}
	
void MyHttpServerHandler(struct evhttp_request* req)
{
	//cv.notify_one();
	//����Ҫʹ�õ�buffer����
	evbuffer* buf = evbuffer_new();
	if (!buf) {
		return;
	}
	//��ȡ�����URI
	string sURI = evhttp_request_get_uri(req);
	//const char* uri = (char*)evhttp_request_get_uri(req);
	//��Ӷ�Ӧ��HTTP����
	evbuffer_add_printf(buf, "<html>");
	evbuffer_add_printf(buf, "<head><title>MyHttpServer</title></head>");
	evbuffer_add_printf(buf, "<body>");

	//��ȡһ���ӿ���
	string sInterfaceName;
	int nSubIndex = sURI.find('//', 1);
	if (-1 != nSubIndex)
		sInterfaceName = sURI.substr(1, nSubIndex-1);
	else
		sInterfaceName = sURI.substr(1, sURI.size()-1);

	std::cout << "Interface Name Is : " << sInterfaceName << endl;

	//���ɽӿڶ��󲢴���
	CHttpCommon * myHttp = (CHttpCommon *)ObjectFactory::CreateObject(sInterfaceName);
	myHttp->Handle();
	evbuffer_add_printf(buf, "<p>");
	evbuffer_add_printf(buf, myHttp->GetContant().c_str());
	evbuffer_add_printf(buf, "</p>");
	delete myHttp;
	
	evbuffer_add_printf(buf, "</body>");
	evbuffer_add_printf(buf, "</html>");
	//�ظ����ͻ���
	evhttp_send_reply(req, HTTP_OK, "OK", buf);
	evbuffer_free(buf);
	sleep(20);
}


void startse()
{
	
	pMypoll=new CThreadPool(10);
	startHttpServer("0.0.0.0", 80, start_thread, NULL);
	//startHttpServer("0.0.0.0", 80, MyHttpServerHandler, NULL);
	}
int main(int argc, char** argv)
{
	std::cout << "Server Start" << std::endl;
	//Windows ƽ̨�׽��ֿ�ĳ�ʼ��
#ifdef WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
	//���߳�
/*	thread threads[TNUM];
	for(int i=0;i<TNUM;i++)
		threads[i]=thread(startse);
	for(auto &th:threads)
		th.join();*/
	//���������ڵ�ַ 127.0.0.1:9000 ��
	startse();
#ifdef WIN32
	WSACleanup();
#endif
	return 0;
}

#include "HttpCommon.h"


CHttpCommon::CHttpCommon(PREQ pReq)
{
	this->pReq = pReq;
}

CHttpCommon::~CHttpCommon()
{
}

void CHttpCommon::SetContant(string sContant)
{
	m_sContant = sContant;
}

string CHttpCommon::GetContant()
{
	return m_sContant;
}

bool CHttpCommon::ParseParam(Json::Value &jRoot)
{
	string sURI = evhttp_request_get_uri(pReq);

	int nPos;
	string sParamList;
	
	//截取出参数表
	nPos = sURI.find('?', (unsigned int)1);
	sParamList = sURI.substr(nPos+1, sURI.size() - nPos);

	//把参数列表格式化成Json
	sParamList = "{\"" + sParamList + "\"}";
	int nIndex;
	while(-1 != (nIndex = sParamList.find("&")))
	{
		sParamList.replace(nIndex, 1, "\",\"");
	}
	while(-1 != (nIndex = sParamList.find("=")))
	{
		sParamList.replace(nIndex, 1, "\":\"");
	}

	Json::Reader jReader;
	if ( ! jReader.parse(sParamList.c_str(), jRoot))
		return false;
	
	if ( ! jRoot.isMember("tm"))
		return false;
	if ( ! jRoot.isMember("data"))
		return false;
	if ( ! jRoot.isMember("token"))
		return false;
	//std::cout << "参数json格式为：" << jRoot << std::endl;

	return true;
}

void CHttpCommon::Send(int nCode, string sMessage)
{
	evbuffer* pBuf = evbuffer_new();
	evbuffer_add_printf(pBuf, sMessage.c_str());
	evhttp_send_reply(pReq, nCode, "OK", pBuf);
	evbuffer_free(pBuf);
}

void CHttpCommon::Send_Error(int nCode, string sMessage)
{
	//evbuffer* pBuf = evbuffer_new();
	//evbuffer_add_printf(pBuf, sMessage.c_str());
	//evhttp_send_reply(pReq, nCode, "ERROR", pBuf);
	evhttp_send_error(pReq,nCode,sMessage.c_str()); 
}
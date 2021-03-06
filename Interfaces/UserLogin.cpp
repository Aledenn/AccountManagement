#include "UserLogin.h"

#define USER_LOGIN_STR(sName, sPasswd)                          \
        "SELECT * FROM AccountManagement_User WHERE             \
        user_name=\'" + (sName) + "\' AND                       \
        user_password=\'" + (sPasswd) + "\';"  

CUserLogin::CUserLogin(PREQ pReq) : CHttpCommon(pReq)
{ }

CUserLogin::~CUserLogin()
{ }

void CUserLogin::Handle()
{
    Json::Value jData;
    if(!ParseParam(jData))
        return;
    
    if(!jData.isMember("name") || !jData.isMember("passwd"))
    {
        Send_Error(404, Error_Data_Param_Miss);
		return;
    }
    
    string sValues = USER_LOGIN_STR(jData["name"].asString(), jData["passwd"].asString());

    Json::Value jRoot = CMySQL_Client::GetInstance()->Select(sValues);
    if(jRoot.isMember("error"))
    {
        Send_Fail(502, Error_Operation_Rrror(2, "User Login Failure"));
		return;
    }
    
    int ret = jRoot["array"].size();
    if(ret <= 0)
    {
        Send_Fail(502, Error_Operation_Rrror(2, "User Login Failure"));
		return;
    }

    Json::Value jContant;
    jContant["ticket"] = CTicketManager::GetInstance()->GetTicket(jData["name"].asString());
    jContant["data"] = "User Login Success";

    Json::Value jRet;
    jRet["code"] = 1;
    jRet["contant"] = jContant;

    Send(200, jRet.toStyledString());
}
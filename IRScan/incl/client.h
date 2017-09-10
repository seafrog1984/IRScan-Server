#ifndef CLIENT__H__
#define CLIENT__H__
#pragma once


#include "../incl/protocol.h"
#include "../utils/socket/Socket.h"
#include "../utils/string/string_utils.h"


#define MAX_DATA 1024*100


class _client_t
{
public:
	_client_t(): bConnection(false) {}
	~_client_t()
	{
		if (bConnection)
			sock.Close();

		bConnection = false;
	}

	std::string get_msg() { return sMsg; }

	std::string get_auth() { return sAuth; }

	//初始化
	bool init(const std::string &host, short port);

	//获取授权
	bool login_auth();
    //验证授权
	bool check_auth();
	//发送图片
	bool send_png(const std::string &id, char *data, int len, std::vector<std::string> &vecPngID);
	//发送个人信息
	bool send_info(std::map<std::string, std::string> &mapUserInfo);
	//获取个人信息
	int get_info(const std::string &id, std::map<std::string, std::string> &mapUserInfo);
	//获取图片
	bool get_png(const std::string &id, const std::string &png_id, char *data);
	//发送结果图片
	bool send_result(const std::string &id, char *data, int len);


private:
	void set_req_head(req_head_t &head, const int &cmd);

	//验证授权
	std::string get_local_auth(std::string &mac, std::string &ip, std::string &device);

	bool bConnection; //连接状态
	CSocket sock;     //连接句柄

	std::string sAuth;
	std::string sMsg;
};


typedef _client_t client_t;


#endif
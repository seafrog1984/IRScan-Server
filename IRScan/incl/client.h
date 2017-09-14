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
	_client_t() : bConnection(false) {}
	~_client_t()
	{
		close();
	}

	void close()
	{
		if (bConnection)
			sock.Close();

		bConnection = false;
	}

	bool get_conn() { return bConnection; }

	std::string get_msg() { return sMsg; }

	std::string get_auth() { return sAuth; }

	//初始化
	bool init(const std::string &host, short port);

	//获取授权
	int login_auth(CString user, CString passwd, int flag = 1);
	//验证授权
	int check_auth(int flag = 1);
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

	//获取用户列表
	bool get_users(CString &user, CString &passwd, std::map<std::string, std::string> &mapUserInfo, int &level);
	//新增用户
	bool new_user(CString &user, CString &passwd, int &level, int &permissions, CString &loginuser, CString &loginpasswd);
	//删除用户
	bool del_user(CString &user, CString &loginuser, CString &loginpasswd);
	//修改用户
	bool update_user(CString &user, CString &passwd, int &permissions, CString &loginuser, CString &loginpasswd);

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
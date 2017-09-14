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

	//��ʼ��
	bool init(const std::string &host, short port);

	//��ȡ��Ȩ
	int login_auth(CString user, CString passwd, int flag = 1);
	//��֤��Ȩ
	int check_auth(int flag = 1);
	//����ͼƬ
	bool send_png(const std::string &id, char *data, int len, std::vector<std::string> &vecPngID);
	//���͸�����Ϣ
	bool send_info(std::map<std::string, std::string> &mapUserInfo);
	//��ȡ������Ϣ
	int get_info(const std::string &id, std::map<std::string, std::string> &mapUserInfo);
	//��ȡͼƬ
	bool get_png(const std::string &id, const std::string &png_id, char *data);
	//���ͽ��ͼƬ
	bool send_result(const std::string &id, char *data, int len);

	//��ȡ�û��б�
	bool get_users(CString &user, CString &passwd, std::map<std::string, std::string> &mapUserInfo, int &level);
	//�����û�
	bool new_user(CString &user, CString &passwd, int &level, int &permissions, CString &loginuser, CString &loginpasswd);
	//ɾ���û�
	bool del_user(CString &user, CString &loginuser, CString &loginpasswd);
	//�޸��û�
	bool update_user(CString &user, CString &passwd, int &permissions, CString &loginuser, CString &loginpasswd);

private:
	void set_req_head(req_head_t &head, const int &cmd);

	//��֤��Ȩ
	std::string get_local_auth(std::string &mac, std::string &ip, std::string &device);

	bool bConnection; //����״̬
	CSocket sock;     //���Ӿ��

	std::string sAuth;
	std::string sMsg;
};


typedef _client_t client_t;


#endif
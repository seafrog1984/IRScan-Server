#include <stdafx.h>
#include "./utils/pc/pc_info.h"
#include "./utils/md5/md5.h"
#include "./incl/client.h"

#include <ctime>


bool _client_t::init(const std::string &host, short port)
{
	if (!sock.Connect(host.c_str(), port))
	{
		char buf[128] = {0};
		sprintf_s(buf, "connect %s:%d fail", host.c_str(), port);
		sMsg = buf;
		return false;
	}

	bConnection = true;
	return true;
}


void _client_t::set_req_head(req_head_t &head, const int &cmd)
{
	head.version = 10001;
	head.cmd     = cmd;
	time_t now_time = time(NULL);
	head.timestamp = (int)now_time;
}


std::string _client_t::get_local_auth(std::string &mac, std::string &ip, std::string &device)
{
	int len = device.length();

	char re[2] = {0};
	re[0] = mac[0];
	device.replace(0, 1, re);
	re[0] = mac[4];
	device.replace(4, 1, re);
	int pos1 = len - 1;
	re[0] = mac[mac.length() - 1];
	device.replace(pos1, 1, re);
	pos1 = len - 4;
	re[0] = mac[mac.length() - 4];
	device.replace(pos1, 1, re);
	re[0] = ip[ip.length() - 1];
    device.insert(0, re);
	re[0] = ip[0];
	device.append(re);
	//58eb053ee0bd03425dc91f1d612df7ba

	return MD5(device).toString();
}


int _client_t::login_auth(CString user, CString passwd, int flag)
{
	if(!bConnection)
	{
		sMsg = "no init connection";
		return -1;
	}

	//��ȡ��Ȩ����ʵ��
	req_head_t tHead;
	set_req_head(tHead, REQ_CMD_GET_AUTH);

	//����ͻ��˱�ʶ
	//mac+ip�ļ��ܴ�(��ϣ|md5|�����㷨)
	std::string sMac = GetMAC();
	std::string sIP = GetIP();
	std::string sDevice = sMac + "&" + sIP;
	sDevice = bytesToHexstring(sDevice.c_str(), sDevice.length());
	int len = sDevice.length();

	string sUser = user.GetBuffer();
	int len2 = sUser.length();
	string sPasswd = passwd.GetBuffer();
	int len3 = sPasswd.length();

	//��ȡ��Ȩ��������
	//ͷ��+�ͻ��˱�ʶ���ݴ�С(int)+�ͻ��˱�ʶ����
	tHead.length = 4*sizeof(int) + len + len2 + len3 + 1;
	//�������ݿ���
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sDevice.c_str(), len);
	pos += len;
	//�û���
	memcpy(&data[pos], &len2, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sUser.c_str(), len2);
	pos += len2;
	//����
	memcpy(&data[pos], &len3, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sPasswd.c_str(), len3);
	pos += len3;
	//0��ʾ���ԣ�1��ʾ��ʽ
	string sFlag = (flag == 0)?"0":"1";
    len = 1;
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sFlag.c_str(), len);
	pos += len;

	//�������δ��Ȩ�򷵻ش�����Ϣ
	//�����������Ȩ�򷵻���Ȩ��Ϣ����Ȩ����ܺ󱣴浽����
	//��Ȩ��Ϊ���ݿͻ��˱�ʶ���ݼ��ܵõ��ģ���ͻ��˱�ʶһһ��Ӧ
	if(!sock.Send(data, pos))
	{
		sMsg = "send auth msg fail";
		return -1;
	}
	char resp[MAX_DATA] = {0};
	int recv = sock.Read(resp, sizeof(resp_head_t));
	if(0 >= recv)
	{
		sMsg = "send auth resp fail";
		return -1;
	}
	//����ͷ��
	resp_head_t *resp_head = new resp_head_t();
	memcpy(resp_head, resp, sizeof(resp_head_t));
	//printf("anth head: %d, %d, %d\n", 
	//	resp_head->length, resp_head->result, resp_head->timestamp);

	recv = sock.Read(&resp[sizeof(resp_head_t)], resp_head->length);
	if(0 >= recv)
	{
		sMsg = "send auth resp fail";
		return -1;
	}
	//����body
	int data_len;
	pos = sizeof(resp_head_t);
	//for(int i=0; i<20; ++i)
	//{
	//	printf("#%c\n#", resp[pos+i]);
	//}
	memcpy(&data_len, &resp[pos], sizeof(int));
	pos += sizeof(int);
	std::string msg(&resp[pos], data_len);
	pos += data_len;
	//printf("auth body: %d, %s\n", data_len, msg.c_str());

	if(0 != resp_head->result)
	{
		sMsg = "auth fail, " + msg;
		return -1;
	}

	if("test case" == msg)
	{
        sMsg = msg;
		memcpy(&data_len, &resp[pos], sizeof(int));
		pos += sizeof(int);
		std::string msg(&resp[pos], data_len);
		if(msg != get_local_auth(sMac, sIP, sDevice))
		{
			sMsg = "auth fail";
			return -1;
		}
		sAuth = msg;
		return 0;
	}

	if(msg != get_local_auth(sMac, sIP, sDevice))
	{
		sMsg = "auth fail";
		return -1;
	}
	sAuth = msg;
	return 1;
}


int _client_t::check_auth(int flag)
{
	if(!bConnection)
	{
		sMsg = "no init connection";
		return -1;
	}

	if("" == sAuth)
	{
		sMsg = "invalid auth";
		return -1;
	}

	//��֤��Ȩ����ʵ��
	req_head_t tHead;
	set_req_head(tHead, REQ_CMD_CHECK_AUTH);

	//����ͻ��˱�ʶ
	std::string sMac = GetMAC();
	std::string sDevice = bytesToHexstring(sMac.c_str(), sMac.length());
	int len = sDevice.length();
	//��֤��Ȩ��������
	//ͷ��+�ͻ��˱�ʶ���ݴ�С(int)+�ͻ��˱�ʶ����
	tHead.length = 3*sizeof(int) + len + sAuth.length() + 1;
	//�������ݿ���
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sDevice.c_str(), len);
	pos += len;
	len = sAuth.length();
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sAuth.c_str(), len);
	pos += len;
	//0��ʾ���ԣ�1��ʾ��ʽ
	string sFlag = (flag == 0)?"0":"1";
	len = 1;
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sFlag.c_str(), len);
	pos += len;
	//�������δ��Ȩ�򷵻ش�����Ϣ
	if(!sock.Send(data, pos))
	{
		sMsg = "send check msg fail";
		return -1;
	}
	char resp[MAX_DATA] = {0};
	int recv = sock.Read(resp, sizeof(resp_head_t));
	if(0 >= recv)
	{
		sMsg = "send check msg resp fail";
		return -1;
	}
	//����ͷ��
	resp_head_t *resp_head = new resp_head_t();
	memcpy(resp_head, resp, sizeof(resp_head_t));
	//printf("anth head: %d, %d, %d\n", 
	//	resp_head->length, resp_head->result, resp_head->timestamp);

	recv = sock.Read(&resp[sizeof(resp_head_t)], resp_head->length);
	if(0 >= recv)
	{
		sMsg = "send check msg resp fail";
		return -1;
	}
	//����body
	int data_len;
	pos = sizeof(resp_head_t);
	memcpy(&data_len, &resp[pos], sizeof(int));
	pos += sizeof(int);
	std::string msg(&resp[pos], data_len);
	//printf("auth body: %d, %s\n", data_len, msg.c_str());

	if(0 != resp_head->result)
	{
		sMsg = "check auth fail, " + msg;
		return -1;
	}

	if("test case" == msg)
	{
		sMsg = msg;
		return 0;
	}	
	return 1;
}

bool _client_t::send_png(const std::string &id, char *png, int png_len, std::vector<std::string> &vecPngID)
{
	if(!bConnection)
	{
		sMsg = "no init connection";
		return false;
	}

	if("" == sAuth)
	{
		sMsg = "invalid auth";
		return false;
	}

	//ͼƬ����ʵ��
	req_head_t tHead;
	set_req_head(tHead, REQ_CMD_SEND_PNG);

	std::string sPng = bytesToHexstring(png, png_len);
	png_len = sPng.length();

	int len = id.length();
	//ͼƬ��������
	//ͷ��+�ͻ��˱�ʶ���ݴ�С(int)+�ͻ��˱�ʶ����
	tHead.length = 2*sizeof(int) + png_len + len;
	//�������ݿ���
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], id.c_str(), len);
	pos += len;
	//data
	memcpy(&data[pos], &png_len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sPng.c_str(), png_len);
	pos += png_len;
	//�������δ��Ȩ�򷵻ش�����Ϣ
	if(!sock.Send(data, pos))
	{
		sMsg = "send png msg fail";
		return false;
	}
	char resp[MAX_DATA] = {0};
	int recv = sock.Read(resp, sizeof(resp_head_t));
	if(0 >= recv)
	{
		sMsg = "send png resp fail";
		return false;
	}
	//����ͷ��
	resp_head_t *resp_head = new resp_head_t();
	memcpy(resp_head, resp, sizeof(resp_head_t));
	//printf("anth head: %d, %d, %d\n", 
	//	resp_head->length, resp_head->result, resp_head->timestamp);

	recv = sock.Read(&resp[sizeof(resp_head_t)], resp_head->length);
	if(0 >= recv)
	{
		sMsg = "send png fail";
		return false;
	}
	//����body
	int data_len;
	pos = sizeof(resp_head_t);
	memcpy(&data_len, &resp[pos], sizeof(int));
	pos += sizeof(int);
	std::string msg(&resp[pos], data_len);
	//printf("auth body: %d, %s\n", data_len, msg.c_str());

	if(0 != resp_head->result)
	{
		sMsg = "png send fail, " + msg;
		return false;
	}

	vecPngID.push_back(msg);
	return true;
}


bool _client_t::send_info(std::map<std::string, std::string> &mapUserInfo)
{
	if(!bConnection)
	{
		sMsg = "no init connection";
		return false;
	}

	if("" == sAuth)
	{
		sMsg = "invalid auth";
		return false;
	}

	//������Ϣ����ʵ��
	req_head_t tHead;
	set_req_head(tHead, REQ_CMD_SEND_DATA);

	std::string sData = map_join(mapUserInfo, '&', '=');
	int len = sData.length();
	//ͼƬ��������
	//ͷ��+�ͻ��˱�ʶ���ݴ�С(int)+�ͻ��˱�ʶ����
	tHead.length = sizeof(int) + len;
	//�������ݿ���
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sData.c_str(), len);
	pos += len;
	//�������δ��Ȩ�򷵻ش�����Ϣ
	if(!sock.Send(data, pos))
	{
		sMsg = "send user msg fail";
		return false;
	}
	char resp[MAX_DATA] = {0};
	int recv = sock.Read(resp, sizeof(resp_head_t));
	if(0 >= recv)
	{
		sMsg = "send user msg resp fail";
		return false;
	}
	//����ͷ��
	resp_head_t *resp_head = new resp_head_t();
	memcpy(resp_head, resp, sizeof(resp_head_t));
	//printf("anth head: %d, %d, %d\n", 
	//	resp_head->length, resp_head->result, resp_head->timestamp);

	recv = sock.Read(&resp[sizeof(resp_head_t)], resp_head->length);
	if(0 >= recv)
	{
		sMsg = "send user msg resp fail";
		return false;
	}
	//����body
	int data_len;
	pos = sizeof(resp_head_t);
	memcpy(&data_len, &resp[pos], sizeof(int));
	pos += sizeof(int);
	std::string msg(&resp[pos], data_len);
	//printf("auth body: %d, %s\n", data_len, msg.c_str());

	if(0 != resp_head->result)
	{
		sMsg = "user info send fail, " + msg;
		return false;
	}
	return true;
}


int _client_t::get_info(const std::string &id, std::map<std::string, std::string> &mapUserInfo)
{
	if(!bConnection)
	{
		sMsg = "no init connection";
		return -1;
	}

	if("" == sAuth)
	{
		sMsg = "invalid auth";
		return -1;
	}

	//������Ϣ����ʵ��
	req_head_t tHead;
	set_req_head(tHead, REQ_CMD_GET_DATA);

	int len = id.length();
	//ͼƬ��������
	//ͷ��+�ͻ��˱�ʶ���ݴ�С(int)+�ͻ��˱�ʶ����
	tHead.length = sizeof(int) + len;
	//�������ݿ���
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], id.c_str(), len);
	pos += len;
	//�������δ��Ȩ�򷵻ش�����Ϣ
	if(!sock.Send(data, pos))
	{
		sMsg = "get user msg fail";
		return -1;
	}
	char resp[MAX_DATA] = {0};
	int recv = sock.Read(resp, sizeof(resp_head_t));
	if(0 >= recv)
	{
		sMsg = "get user msg resp fail";
		return -1;
	}
	//����ͷ��
	resp_head_t *resp_head = new resp_head_t();
	memcpy(resp_head, resp, sizeof(resp_head_t));
	//printf("anth head: %d, %d, %d\n", 
	//	resp_head->length, resp_head->result, resp_head->timestamp);

	if(0 == resp_head->length)
	{
		return 0;
	}

	recv = sock.Read(&resp[sizeof(resp_head_t)], resp_head->length);
	if(0 >= recv)
	{
		sMsg = "get user msg resp fail";
		return -1;
	}
	//����body
	int data_len;
	pos = sizeof(resp_head_t);
	memcpy(&data_len, &resp[pos], sizeof(int));
	pos += sizeof(int);
	std::string msg(&resp[pos], data_len);
	//printf("auth body: %d, %s\n", data_len, msg.c_str());

	if(0 != resp_head->result)
	{
		sMsg = "get info send fail, " + msg;
		return -1;
	}

	split_map(msg.c_str(), mapUserInfo, '=', '&');
	return 1;
}

bool _client_t::get_png(const std::string &id, const std::string &png_id, char *png)
{
	if(!bConnection)
	{
		sMsg = "no init connection";
		return false;
	}

	if("" == sAuth)
	{
		sMsg = "invalid auth";
		return false;
	}

	//ͼƬ����ʵ��
	req_head_t tHead;
	set_req_head(tHead, REQ_CMD_GET_PNG);

	//ͼƬ��������
	//ͷ��+�ͻ��˱�ʶ���ݴ�С(int)+�ͻ��˱�ʶ����
	tHead.length = 2*sizeof(int) + id.length() + png_id.length();
	//�������ݿ���
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	int len = id.length();
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], id.c_str(), len);
	pos += len;
	//data
	len = png_id.length();
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], png_id.c_str(), len);
	pos += len;
	//�������δ��Ȩ�򷵻ش�����Ϣ
	if(!sock.Send(data, pos))
	{
		sMsg = "get png msg fail";
		return false;
	}
	char resp[MAX_DATA] = {0};
	int recv = sock.Read(resp, sizeof(resp_head_t));
	if(0 >= recv)
	{
		sMsg = "get png resp fail";
		return false;
	}
	//����ͷ��
	resp_head_t *resp_head = new resp_head_t();
	memcpy(resp_head, resp, sizeof(resp_head_t));
	//printf("anth head: %d, %d, %d\n", 
	//	resp_head->length, resp_head->result, resp_head->timestamp);

	recv = sock.Read(&resp[sizeof(resp_head_t)], resp_head->length);
	if(0 >= recv)
	{
		sMsg = "get png fail";
		return false;
	}
	//����body
	int data_len;
	pos = sizeof(resp_head_t);
	memcpy(&data_len, &resp[pos], sizeof(int));
	pos += sizeof(int);
	std::string msg(&resp[pos], data_len);
	//printf("auth body: %d, %s\n", data_len, msg.c_str());

	if(0 != resp_head->result)
	{
		sMsg = "png get fail, " + msg;
		return false;
	}

	hexstringToBytes(msg.c_str(), msg.length(), png);

	return true;
}


bool _client_t::send_result(const std::string &id, char *png, int png_len)
{
	if(!bConnection)
	{
		sMsg = "no init connection";
		return false;
	}

	if("" == sAuth)
	{
		sMsg = "invalid auth";
		return false;
	}

	//ͼƬ����ʵ��
	req_head_t tHead;
	set_req_head(tHead, REQ_CMD_SEND_RESULT);

	std::string sPng = bytesToHexstring(png, png_len);
	png_len = sPng.length();

	int len = id.length();
	//ͼƬ��������
	//ͷ��+�ͻ��˱�ʶ���ݴ�С(int)+�ͻ��˱�ʶ����
	tHead.length = 2*sizeof(int) + png_len + len;
	//�������ݿ���
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], id.c_str(), len);
	pos += len;
	//data
	memcpy(&data[pos], &png_len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sPng.c_str(), png_len);
	pos += png_len;
	//�������δ��Ȩ�򷵻ش�����Ϣ
	if(!sock.Send(data, pos))
	{
		sMsg = "send result png msg fail";
		return false;
	}
	char resp[MAX_DATA] = {0};
	int recv = sock.Read(resp, sizeof(resp_head_t));
	if(0 >= recv)
	{
		sMsg = "send result png resp fail";
		return false;
	}
	//����ͷ��
	resp_head_t *resp_head = new resp_head_t();
	memcpy(resp_head, resp, sizeof(resp_head_t));
	//printf("anth head: %d, %d, %d\n", 
	//	resp_head->length, resp_head->result, resp_head->timestamp);

	recv = sock.Read(&resp[sizeof(resp_head_t)], resp_head->length);
	if(0 >= recv)
	{
		sMsg = "send result png fail";
		return false;
	}
	//����body
	int data_len;
	pos = sizeof(resp_head_t);
	memcpy(&data_len, &resp[pos], sizeof(int));
	pos += sizeof(int);
	std::string msg(&resp[pos], data_len);
	//printf("auth body: %d, %s\n", data_len, msg.c_str());

	if(0 != resp_head->result)
	{
		sMsg = "result png send fail, " + msg;
		return false;
	}
	return true;
}

//��ȡ�û��б�
bool _client_t::get_users(CString &user, CString &passwd, std::map<std::string, std::string> &mapUserInfo, int &iLevel)
{
	if(!bConnection)
	{
		sMsg = "no init connection";
		return false;
	}

	//��ȡ��Ȩ����ʵ��
	req_head_t tHead;
	set_req_head(tHead, REQ_CONTROL_GET_LIST);

	string sUser = user.GetBuffer();
	int len1 = sUser.length();
	string sPasswd = passwd.GetBuffer();
	int len2 = sPasswd.length();

	//��ȡ��Ȩ��������
	//ͷ��+�ͻ��˱�ʶ���ݴ�С(int)+�ͻ��˱�ʶ����
	tHead.length = 2*sizeof(int) + len1 + len2;
	//�������ݿ���
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	//�û���
	memcpy(&data[pos], &len1, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sUser.c_str(), len1);
	pos += len1;
	//����
	memcpy(&data[pos], &len2, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sPasswd.c_str(), len2);
	pos += len2;
	//�������δ��Ȩ�򷵻ش�����Ϣ
	//�����������Ȩ�򷵻���Ȩ��Ϣ����Ȩ����ܺ󱣴浽����
	//��Ȩ��Ϊ���ݿͻ��˱�ʶ���ݼ��ܵõ��ģ���ͻ��˱�ʶһһ��Ӧ
	if(!sock.Send(data, pos))
	{
		sMsg = "send get userlist msg fail";
		return false;
	}
	
	char resp[MAX_DATA] = {0};
	int recv = sock.Read(resp, sizeof(resp_head_t));
	if(0 >= recv)
	{
		sMsg = "send get userlist resp fail";
		return false;
	}
	//����ͷ��
	resp_head_t *resp_head = new resp_head_t();
	memcpy(resp_head, resp, sizeof(resp_head_t));
	//printf("anth head: %d, %d, %d\n", 
	//	resp_head->length, resp_head->result, resp_head->timestamp);
   
	recv = sock.Read(&resp[sizeof(resp_head_t)], resp_head->length);
	if(0 >= recv)
	{
		sMsg = "send get userlist resp fail";
		return false;
	}
	//����body
	int data_len;
	pos = sizeof(resp_head_t);
	//for(int i=0; i<20; ++i)
	//{
	//	printf("#%c\n#", resp[pos+i]);
	//}
    
	memcpy(&data_len, &resp[pos], sizeof(int));
	pos += sizeof(int);
	std::string msg(&resp[pos], data_len);
	pos += data_len;
	//printf("auth body: %d, %s\n", data_len, msg.c_str());
 
	split_map(msg.c_str(), mapUserInfo, '=', '&');

	memcpy(&data_len, &resp[pos], sizeof(int));
	pos += sizeof(int);
	std::string level(&resp[pos], data_len);
	iLevel = atoi(level.c_str());

	if(0 != resp_head->result)
	{
		sMsg = "get userlist fail, " + msg;
		return false;
	}
	return true;
}

//�����û�
bool _client_t::new_user(CString &user, CString &passwd,int &level, int &permissions, CString &loginuser, CString &loginpasswd)
{
	if(!bConnection)
	{
		sMsg = "no init connection";
		return false;
	}

	//��ȡ��Ȩ����ʵ��
	req_head_t tHead;
	set_req_head(tHead, REQ_CONTROL_NEW_USER);

	string sUser = user.GetBuffer();
	int len1 = sUser.length();
	string sPasswd = passwd.GetBuffer();
	int len2 = sPasswd.length();
	CString sTmp;
	sTmp.Format("%d", level);
	string sLevel = sTmp.GetBuffer();
	int len3 = sLevel.length();
	sTmp.Format("%d", permissions);
	string sPermissions = sTmp.GetBuffer();
	int len4 = sPermissions.length();
	string sLoginUser = loginuser.GetBuffer();
	int len5 = sLoginUser.length();
	string sLoginPasswd = loginpasswd.GetBuffer();
	int len6 = sLoginPasswd.length();
	//��ȡ��Ȩ��������
	//ͷ��+�ͻ��˱�ʶ���ݴ�С(int)+�ͻ��˱�ʶ����
	tHead.length = 6*sizeof(int) + len1 + len2 + len3 + len4 + len5 + len6;
	//�������ݿ���
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	//�û���
	memcpy(&data[pos], &len1, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sUser.c_str(), len1);
	pos += len1;
	//����
	memcpy(&data[pos], &len2, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sPasswd.c_str(), len2);
	pos += len2;
	//����
	memcpy(&data[pos], &len3, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sLevel.c_str(), len3);
	pos += len3;
	memcpy(&data[pos], &len4, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sPermissions.c_str(), len4);
	pos += len4;
	//��¼
	memcpy(&data[pos], &len5, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sLoginUser.c_str(), len5);
	pos += len5;
	//����
	memcpy(&data[pos], &len6, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sLoginPasswd.c_str(), len6);
	pos += len6;
	//�������δ��Ȩ�򷵻ش�����Ϣ
	//�����������Ȩ�򷵻���Ȩ��Ϣ����Ȩ����ܺ󱣴浽����
	//��Ȩ��Ϊ���ݿͻ��˱�ʶ���ݼ��ܵõ��ģ���ͻ��˱�ʶһһ��Ӧ
	if(!sock.Send(data, pos))
	{
		sMsg = "send new user msg fail";
		return false;
	}

	char resp[MAX_DATA] = {0};
	int recv = sock.Read(resp, sizeof(resp_head_t));
	if(0 >= recv)
	{
		sMsg = "send new user resp fail";
		return false;
	}
	//����ͷ��
	resp_head_t *resp_head = new resp_head_t();
	memcpy(resp_head, resp, sizeof(resp_head_t));
	//printf("anth head: %d, %d, %d\n", 
	//	resp_head->length, resp_head->result, resp_head->timestamp);

	recv = sock.Read(&resp[sizeof(resp_head_t)], resp_head->length);
	if(0 >= recv)
	{
		sMsg = "send new user resp fail";
		return false;
	}
	//����body
	int data_len;
	pos = sizeof(resp_head_t);
	//for(int i=0; i<20; ++i)
	//{
	//	printf("#%c\n#", resp[pos+i]);
	//}

	memcpy(&data_len, &resp[pos], sizeof(int));
	pos += sizeof(int);
	std::string msg(&resp[pos], data_len);
	pos += data_len;
	//printf("auth body: %d, %s\n", data_len, msg.c_str());

	if(0 != resp_head->result)
	{
		sMsg = "new user fail, " + msg;
		return false;
	}

	return true;
}

//ɾ���û�
bool _client_t::del_user(CString &user, CString &loginuser, CString &loginpasswd)
{
	if(!bConnection)
	{
		sMsg = "no init connection";
		return false;
	}

	//��ȡ��Ȩ����ʵ��
	req_head_t tHead;
	set_req_head(tHead, REQ_CONTROL_DEL_USER);

	string sUser = user.GetBuffer();
	int len1 = sUser.length();
	string sLoginUser = loginuser.GetBuffer();
	int len2 = sLoginUser.length();
	string sLoginPasswd = loginpasswd.GetBuffer();
	int len3 = sLoginPasswd.length();
	//��ȡ��Ȩ��������
	//ͷ��+�ͻ��˱�ʶ���ݴ�С(int)+�ͻ��˱�ʶ����
	tHead.length = 3*sizeof(int) + len1 + len2 + len3;
	//�������ݿ���
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	//�û���
	memcpy(&data[pos], &len1, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sUser.c_str(), len1);
	pos += len1;
	//��¼
	memcpy(&data[pos], &len2, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sLoginUser.c_str(), len2);
	pos += len2;
	//����
	memcpy(&data[pos], &len3, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sLoginPasswd.c_str(), len3);
	pos += len3;
	//�������δ��Ȩ�򷵻ش�����Ϣ
	//�����������Ȩ�򷵻���Ȩ��Ϣ����Ȩ����ܺ󱣴浽����
	//��Ȩ��Ϊ���ݿͻ��˱�ʶ���ݼ��ܵõ��ģ���ͻ��˱�ʶһһ��Ӧ
	if(!sock.Send(data, pos))
	{
		sMsg = "send new user msg fail";
		return false;
	}

	char resp[MAX_DATA] = {0};
	int recv = sock.Read(resp, sizeof(resp_head_t));
	if(0 >= recv)
	{
		sMsg = "send del user resp fail";
		return false;
	}
	//����ͷ��
	resp_head_t *resp_head = new resp_head_t();
	memcpy(resp_head, resp, sizeof(resp_head_t));
	//printf("anth head: %d, %d, %d\n", 
	//	resp_head->length, resp_head->result, resp_head->timestamp);

	recv = sock.Read(&resp[sizeof(resp_head_t)], resp_head->length);
	if(0 >= recv)
	{
		sMsg = "send del user resp fail";
		return false;
	}
	//����body
	int data_len;
	pos = sizeof(resp_head_t);
	//for(int i=0; i<20; ++i)
	//{
	//	printf("#%c\n#", resp[pos+i]);
	//}

	memcpy(&data_len, &resp[pos], sizeof(int));
	pos += sizeof(int);
	std::string msg(&resp[pos], data_len);
	pos += data_len;
	//printf("auth body: %d, %s\n", data_len, msg.c_str());

	if(0 != resp_head->result)
	{
		sMsg = "del user fail, " + msg;
		return false;
	}

	return true;
}

//�޸��û�
bool _client_t::update_user(CString &user, CString &passwd, int &permissions, CString &loginuser, CString &loginpasswd)
{
	if(!bConnection)
	{
		sMsg = "no init connection";
		return false;
	}

	//��ȡ��Ȩ����ʵ��
	req_head_t tHead;
	set_req_head(tHead, REQ_CONTROL_UPD_USER);

	string sUser = user.GetBuffer();
	int len1 = sUser.length();
	string sPasswd = passwd.GetBuffer();
	int len2 = sPasswd.length();
	CString sTmp;
	sTmp.Format("%d", permissions);
	string sPermissions = sTmp.GetBuffer();
	int len3 = sPermissions.length();
	string sLoginUser = loginuser.GetBuffer();
	int len4 = sLoginUser.length();
	string sLoginPasswd = loginpasswd.GetBuffer();
	int len5 = sLoginPasswd.length();
	//��ȡ��Ȩ��������
	//ͷ��+�ͻ��˱�ʶ���ݴ�С(int)+�ͻ��˱�ʶ����
	tHead.length = 5*sizeof(int) + len1 + len2 + len3 + len4 + len5;
	//�������ݿ���
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	//�û���
	memcpy(&data[pos], &len1, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sUser.c_str(), len1);
	pos += len1;
	//����
	memcpy(&data[pos], &len2, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sPasswd.c_str(), len2);
	pos += len2;
	//����
	memcpy(&data[pos], &len3, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sPermissions.c_str(), len3);
	pos += len3;
	//��¼
	memcpy(&data[pos], &len4, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sLoginUser.c_str(), len4);
	pos += len4;
	//����
	memcpy(&data[pos], &len5, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sLoginPasswd.c_str(), len5);
	pos += len5;
	//�������δ��Ȩ�򷵻ش�����Ϣ
	//�����������Ȩ�򷵻���Ȩ��Ϣ����Ȩ����ܺ󱣴浽����
	//��Ȩ��Ϊ���ݿͻ��˱�ʶ���ݼ��ܵõ��ģ���ͻ��˱�ʶһһ��Ӧ
	if(!sock.Send(data, pos))
	{
		sMsg = "send update user msg fail";
		return false;
	}

	char resp[MAX_DATA] = {0};
	int recv = sock.Read(resp, sizeof(resp_head_t));
	if(0 >= recv)
	{
		sMsg = "send update user resp fail";
		return false;
	}
	//����ͷ��
	resp_head_t *resp_head = new resp_head_t();
	memcpy(resp_head, resp, sizeof(resp_head_t));
	//printf("anth head: %d, %d, %d\n", 
	//	resp_head->length, resp_head->result, resp_head->timestamp);

	recv = sock.Read(&resp[sizeof(resp_head_t)], resp_head->length);
	if(0 >= recv)
	{
		sMsg = "send update user resp fail";
		return false;
	}
	//����body
	int data_len;
	pos = sizeof(resp_head_t);
	//for(int i=0; i<20; ++i)
	//{
	//	printf("#%c\n#", resp[pos+i]);
	//}

	memcpy(&data_len, &resp[pos], sizeof(int));
	pos += sizeof(int);
	std::string msg(&resp[pos], data_len);
	pos += data_len;
	//printf("auth body: %d, %s\n", data_len, msg.c_str());

	if(0 != resp_head->result)
	{
		sMsg = "update user fail, " + msg;
		return false;
	}

	return true;
}
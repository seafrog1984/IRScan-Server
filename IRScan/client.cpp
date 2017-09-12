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


bool _client_t::login_auth()
{
	if(!bConnection)
	{
		sMsg = "no init connection";
		return false;
	}

	//获取授权请求串实现
	req_head_t tHead;
	set_req_head(tHead, REQ_CMD_GET_AUTH);

	//定义客户端标识
	//mac+ip的加密串(哈希|md5|加密算法)
	std::string sMac = GetMAC();
	std::string sIP = GetIP();
	std::string sDevice = sMac + "&" + sIP;
	sDevice = bytesToHexstring(sDevice.c_str(), sDevice.length());
	int len = sDevice.length();
	//获取授权请求数据
	//头部+客户端标识数据大小(int)+客户端标识数据
	tHead.length = sizeof(int) + len;
	//请求数据拷贝
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sDevice.c_str(), len);
	pos += len;
	//若服务端未授权则返回错误信息
	//若服务端已授权则返回授权信息，授权码加密后保存到本地
	//授权码为根据客户端标识数据加密得到的，与客户端标识一一对应
	if(!sock.Send(data, pos))
	{
		sMsg = "send auth msg fail";
		return false;
	}
	char resp[MAX_DATA] = {0};
	int recv = sock.Read(resp, sizeof(resp_head_t));
	if(0 >= recv)
	{
		sMsg = "send auth resp fail";
		return false;
	}
	//解析头部
	resp_head_t *resp_head = new resp_head_t();
	memcpy(resp_head, resp, sizeof(resp_head_t));
	//printf("anth head: %d, %d, %d\n", 
	//	resp_head->length, resp_head->result, resp_head->timestamp);

	recv = sock.Read(&resp[sizeof(resp_head_t)], resp_head->length);
	if(0 >= recv)
	{
		sMsg = "send auth resp fail";
		return false;
	}
	//解析body
	int data_len;
	pos = sizeof(resp_head_t);
	//for(int i=0; i<20; ++i)
	//{
	//	printf("#%c\n#", resp[pos+i]);
	//}
	memcpy(&data_len, &resp[pos], sizeof(int));
	pos += sizeof(int);
	std::string msg(&resp[pos], data_len);
	//printf("auth body: %d, %s\n", data_len, msg.c_str());

	if(0 != resp_head->result)
	{
		sMsg = "auth fail, " + msg;
		return false;
	}
	if(msg != get_local_auth(sMac, sIP, sDevice))
	{
		sMsg = "auth fail";
		return false;
	}
	sAuth = msg;
	return true;
}


bool _client_t::check_auth()
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

	//验证授权请求串实现
	req_head_t tHead;
	set_req_head(tHead, REQ_CMD_CHECK_AUTH);

	//定义客户端标识
	std::string sMac = GetMAC();
	std::string sDevice = bytesToHexstring(sMac.c_str(), sMac.length());
	int len = sDevice.length();
	//验证授权请求数据
	//头部+客户端标识数据大小(int)+客户端标识数据
	tHead.length = 2*sizeof(int) + len + sAuth.length();
	//请求数据拷贝
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
	//若服务端未授权则返回错误信息
	if(!sock.Send(data, pos))
	{
		sMsg = "send check msg fail";
		return false;
	}
	char resp[MAX_DATA] = {0};
	int recv = sock.Read(resp, sizeof(resp_head_t));
	if(0 >= recv)
	{
		sMsg = "send check msg resp fail";
		return false;
	}
	//解析头部
	resp_head_t *resp_head = new resp_head_t();
	memcpy(resp_head, resp, sizeof(resp_head_t));
	//printf("anth head: %d, %d, %d\n", 
	//	resp_head->length, resp_head->result, resp_head->timestamp);

	recv = sock.Read(&resp[sizeof(resp_head_t)], resp_head->length);
	if(0 >= recv)
	{
		sMsg = "send check msg resp fail";
		return false;
	}
	//解析body
	int data_len;
	pos = sizeof(resp_head_t);
	memcpy(&data_len, &resp[pos], sizeof(int));
	pos += sizeof(int);
	std::string msg(&resp[pos], data_len);
	//printf("auth body: %d, %s\n", data_len, msg.c_str());

	if(0 != resp_head->result)
	{
		sMsg = "check auth fail, " + msg;
		return false;
	}
	return true;
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

	//图片请求串实现
	req_head_t tHead;
	set_req_head(tHead, REQ_CMD_SEND_PNG);

	std::string sPng = bytesToHexstring(png, png_len);
	png_len = sPng.length();

	int len = id.length();
	//图片请求数据
	//头部+客户端标识数据大小(int)+客户端标识数据
	tHead.length = 2*sizeof(int) + png_len + len;
	//请求数据拷贝
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
	//若服务端未授权则返回错误信息
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
	//解析头部
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
	//解析body
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

	//个人信息请求串实现
	req_head_t tHead;
	set_req_head(tHead, REQ_CMD_SEND_DATA);

	std::string sData = map_join(mapUserInfo, '&', '=');
	int len = sData.length();
	//图片请求数据
	//头部+客户端标识数据大小(int)+客户端标识数据
	tHead.length = sizeof(int) + len;
	//请求数据拷贝
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sData.c_str(), len);
	pos += len;
	//若服务端未授权则返回错误信息
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
	//解析头部
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
	//解析body
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

	//个人信息请求串实现
	req_head_t tHead;
	set_req_head(tHead, REQ_CMD_GET_DATA);

	int len = id.length();
	//图片请求数据
	//头部+客户端标识数据大小(int)+客户端标识数据
	tHead.length = sizeof(int) + len;
	//请求数据拷贝
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], id.c_str(), len);
	pos += len;
	//若服务端未授权则返回错误信息
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
	//解析头部
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
	//解析body
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

	//图片请求串实现
	req_head_t tHead;
	set_req_head(tHead, REQ_CMD_GET_PNG);

	//图片请求数据
	//头部+客户端标识数据大小(int)+客户端标识数据
	tHead.length = 2*sizeof(int) + id.length() + png_id.length();
	//请求数据拷贝
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
	//若服务端未授权则返回错误信息
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
	//解析头部
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
	//解析body
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

	//图片请求串实现
	req_head_t tHead;
	set_req_head(tHead, REQ_CMD_SEND_RESULT);

	std::string sPng = bytesToHexstring(png, png_len);
	png_len = sPng.length();

	int len = id.length();
	//图片请求数据
	//头部+客户端标识数据大小(int)+客户端标识数据
	tHead.length = 2*sizeof(int) + png_len + len;
	//请求数据拷贝
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
	//若服务端未授权则返回错误信息
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
	//解析头部
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
	//解析body
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
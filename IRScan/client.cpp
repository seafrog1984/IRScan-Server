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


bool _client_t::recv_data(char *buf, int buf_len)
{
	int recv_len = sock.Read(buf, buf_len);
	if(0 >= recv_len)
		return false;

	int tmp_len = 0;
	while(recv_len < buf_len)
	{
		tmp_len = sock.Read(&buf[recv_len], buf_len-recv_len);
		if(0 >= tmp_len)
			return false;

		recv_len += tmp_len;
	}
	return true;
}


int _client_t::recv_resp(char *buf)
{
	char resp[MAX_DATA] = {0};
	if(!recv_data(resp, sizeof(resp_head_t)))
	{
		sMsg = "get resp head fail 1";
		return -1;
	}
	//解析头部
	resp_head_t *resp_head = new resp_head_t();
	memcpy(resp_head, resp, sizeof(resp_head_t));
	if (10000000 < resp_head->length)
	{
		sMsg = "get resp head fail 2";
		return -1;
	}
	//printf("anth head: %d, %d, %d\n", 
	//	resp_head->length, resp_head->result, resp_head->timestamp);

	//char tmp[26] = {0};
	//sprintf(tmp, "%d-%d-%d", resp_head->result, resp_head->length, resp_head->timestamp);
	//sprintf(tmp, "%d-%d-%d", resp_head->length, data_len, recv);
	//sMsg = tmp;
	//return -1;

	if(0 == resp_head->length)
		return 2;

	if(!recv_data(buf, resp_head->length))
	{
		sMsg = "get resp data fail";
		return -1;
	}

	if(0 != resp_head->result)
		return 0;
	else
		return 1;
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

	string sUser = user.GetBuffer();
	int len2 = sUser.length();
	string sPasswd = passwd.GetBuffer();
	int len3 = sPasswd.length();

	//获取授权请求数据
	//头部+客户端标识数据大小(int)+客户端标识数据
	tHead.length = 4*sizeof(int) + len + len2 + len3 + 1;
	//请求数据拷贝
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sDevice.c_str(), len);
	pos += len;
	//用户名
	memcpy(&data[pos], &len2, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sUser.c_str(), len2);
	pos += len2;
	//密码
	memcpy(&data[pos], &len3, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sPasswd.c_str(), len3);
	pos += len3;
	//0表示测试，1表示正式
	string sFlag = (flag == 0)?"0":"1";
    len = 1;
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sFlag.c_str(), len);
	pos += len;

	//若服务端未授权则返回错误信息
	//若服务端已授权则返回授权信息，授权码加密后保存到本地
	//授权码为根据客户端标识数据加密得到的，与客户端标识一一对应
	if(!sock.Send(data, pos))
	{
		sMsg = "send auth msg fail";
		return -1;
	}
	char resp[MAX_DATA] = {0};
	int ret = recv_resp(resp);
	if (0 > ret)
	{
		return false;
	}

	//解析body
	int data_len;
	memcpy(&data_len, resp, sizeof(int));
	pos = sizeof(int);

	std::string msg(&resp[pos], data_len);
	pos += data_len;
	//printf("auth body: %d, %s\n", data_len, msg.c_str());

	if(0 == ret)
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

	//验证授权请求串实现
	req_head_t tHead;
	set_req_head(tHead, REQ_CMD_CHECK_AUTH);

	//定义客户端标识
	std::string sMac = GetMAC();
	std::string sDevice = bytesToHexstring(sMac.c_str(), sMac.length());
	int len = sDevice.length();
	//验证授权请求数据
	//头部+客户端标识数据大小(int)+客户端标识数据
	tHead.length = 3*sizeof(int) + len + sAuth.length() + 1;
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
	//0表示测试，1表示正式
	string sFlag = (flag == 0)?"0":"1";
	len = 1;
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sFlag.c_str(), len);
	pos += len;
	//若服务端未授权则返回错误信息
	if(!sock.Send(data, pos))
	{
		sMsg = "send check msg fail";
		return -1;
	}
	char resp[MAX_DATA] = {0};
	int ret = recv_resp(resp);
	if (0 > ret)
	{
		return false;
	}

	//解析body
	int data_len;
	memcpy(&data_len, resp, sizeof(int));
	pos = sizeof(int);
	std::string msg(&resp[pos], data_len);
	pos += data_len;

	if(0 == ret)
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


int _client_t::get_scanid(const std::string &card_id, std::string &scan_id, const std::string &user)
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
	set_req_head(tHead, REQ_CMD_GET_SCANID);

	//验证授权请求数据
	//头部+客户端标识数据大小(int)+客户端标识数据
	tHead.length = 3*sizeof(int) + card_id.length() + user.length();
	//请求数据拷贝
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	int len = card_id.length();
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], card_id.c_str(), len);
	pos += len;
	len = user.length();
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], user.c_str(), len);
	pos += len;
	//若服务端未授权则返回错误信息
	if(!sock.Send(data, pos))
	{
		sMsg = "get cardid msg fail";
		return -1;
	}
	char resp[MAX_DATA] = {0};
	int ret = recv_resp(resp);
	if (0 > ret)
	{
		return false;
	}

	//解析body
	int data_len;
	memcpy(&data_len, resp, sizeof(int));
	pos = sizeof(int);
	std::string msg(&resp[pos], data_len);
	pos += data_len;

	if(0 == ret)
	{
		sMsg = "get cardid fail, " + msg;
		return -1;
	}
	scan_id = msg;

	return 1;
}


bool _client_t::send_png(const std::string &scan_id, unsigned short *png, int png_len, std::vector<std::string> &vecPngID)
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


	//std::string sPng = bytesToHexstring(png, png_len);
	//png_len *= sizeof(short);

	int len1 = scan_id.length();
	int len2 = png_len * sizeof(short);

	//图片请求数据
	//头部+客户端标识数据大小(int)+客户端标识数据
	tHead.length = 2*sizeof(int) + len1 + len2;
	//请求数据拷贝
	char data[MAX_DATA+PIC_SIZE*2] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	memcpy(&data[pos], &len1, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], scan_id.c_str(), len1);
	pos += len1;
	//data
	memcpy(&data[pos], &png_len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], png, len2);
	pos += len2;

	//若服务端未授权则返回错误信息
	if(!sock.Send(data, pos))
	{
		sMsg = "send png msg fail";
		return false;
	}
	char resp[MAX_DATA] = {0};
	int ret = recv_resp(resp);
	if (0 > ret)
	{
		return false;
	}

	//解析body
	int data_len;
	memcpy(&data_len, resp, sizeof(int));
	pos = sizeof(int);
	std::string msg(&resp[pos], data_len);
	//pos += data_len;

	if(0 == ret)
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
	int ret = recv_resp(resp);
	if (0 > ret)
	{
		return false;
	}

	//解析body
	int data_len;
	memcpy(&data_len, resp, sizeof(int));
	pos = sizeof(int);
	std::string msg(&resp[pos], data_len);
	pos += data_len;

	if(0 == ret)
	{
		sMsg = "user info send fail, " + msg;
		return false;
	}
	return true;
}


int _client_t::get_info(const std::string &scan_id, std::map<std::string, std::string> &mapUserInfo)
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

	int len = scan_id.length();
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
	memcpy(&data[pos], scan_id.c_str(), len);
	pos += len;
	//若服务端未授权则返回错误信息
	if(!sock.Send(data, pos))
	{
		sMsg = "get user msg fail";
		return -1;
	}
	char resp[MAX_DATA] = {0};

	int ret = recv_resp(resp);
	if (0 > ret)
	{
		return false;
	}
    if (2 == ret)
    {
		return 0;
    }

	//解析body
	int data_len;
	memcpy(&data_len, resp, sizeof(int));
	pos = sizeof(int);
	std::string msg(&resp[pos], data_len);
	pos += data_len;

	if(0 == ret)
	{
		sMsg = "get info send fail, " + msg;
		return -1;
	}
	split_map(msg.c_str(), mapUserInfo, '=', '&');

	return 1;
}

bool _client_t::get_png(const std::string &scan_id, const std::string &png_id, unsigned short *png)
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
	tHead.length = 2*sizeof(int) + scan_id.length() + png_id.length();
	//请求数据拷贝
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	int len = scan_id.length();
	memcpy(&data[pos], &len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], scan_id.c_str(), len);
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

	char resp[MAX_DATA+PIC_SIZE*2] = {0};
	int ret = recv_resp(resp);
	if (0 > ret)
	{
		return false;
	}

	//解析body
	int data_len;
	memcpy(&data_len, resp, sizeof(int));
    pos = sizeof(int);

	if (0 == ret)
	{
		//std::string msg(&resp[pos], data_len);
		sMsg = "png get fail";
		return false;
	}
	
	int cnt = data_len/sizeof(unsigned short);

	std::vector <unsigned short> vecPicData;
	for (int i=0; i<cnt; ++i) 
	{
		vecPicData.push_back(*((unsigned short*)(&resp[pos] + i * sizeof(unsigned short))));
	}

	std::ofstream outfile((scan_id+png_id+".txt").c_str(), std::ios::binary);
	//std::ofstream outfile((scan_id+png_id+".txt").c_str());
	if(outfile.is_open())
	{
		if(0 < vecPicData.size())
		{
			outfile << vecPicData[0];
			for (int i=1; i<cnt; ++i) 
			{
				outfile << " " << vecPicData[i];
			}
		}
		//outfile.write(&resp[pos], resp_head->length-8);
		outfile.close();
	}
	else
	{
		std::cout<<"不能打开文件!"<<std::endl;
	}

	return true;
}


bool _client_t::send_result(const std::string &scan_id, unsigned short *png, int png_len, const std::string &user)
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

	//std::string sPng = bytesToHexstring(png, png_len);
	//png_len *= sizeof(short);

	int len1 = scan_id.length();
	int len2 = user.length();
	int len3 = png_len * sizeof(short);
	//图片请求数据
	//头部+客户端标识数据大小(int)+客户端标识数据
	tHead.length = 3*sizeof(int) + len1 + len2 + len3;
	//请求数据拷贝
	char data[MAX_DATA+PIC_SIZE*2] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	memcpy(&data[pos], &len1, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], scan_id.c_str(), len1);
	pos += len1;
	//user
	memcpy(&data[pos], &len2, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], user.c_str(), len2);
	pos += len2;
	//data
	memcpy(&data[pos], &png_len, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], png, len3);
	pos += len3;

	//若服务端未授权则返回错误信息
	if(!sock.Send(data, pos))
	{
		sMsg = "send result png msg fail";
		return false;
	}
	char resp[MAX_DATA] = {0};
	int ret = recv_resp(resp);
	if (0 > ret)
	{
		return false;
	}

	//解析body
	int data_len;
	memcpy(&data_len, resp, sizeof(int));
	pos = sizeof(int);
	std::string msg(&resp[pos], data_len);
	pos += data_len;

	if(0 == ret)
	{
		sMsg = "result png send fail, " + msg;
		return false;
	}
	return true;
}

//获取用户列表
bool _client_t::get_users(CString &user, CString &passwd, std::map<std::string, std::string> &mapUserInfo, int &iLevel)
{
	if(!bConnection)
	{
		sMsg = "no init connection";
		return false;
	}

	//获取授权请求串实现
	req_head_t tHead;
	set_req_head(tHead, REQ_CONTROL_GET_LIST);

	string sUser = user.GetBuffer();
	int len1 = sUser.length();
	string sPasswd = passwd.GetBuffer();
	int len2 = sPasswd.length();

	//获取授权请求数据
	//头部+客户端标识数据大小(int)+客户端标识数据
	tHead.length = 2*sizeof(int) + len1 + len2;
	//请求数据拷贝
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	//用户名
	memcpy(&data[pos], &len1, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sUser.c_str(), len1);
	pos += len1;
	//密码
	memcpy(&data[pos], &len2, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sPasswd.c_str(), len2);
	pos += len2;
	//若服务端未授权则返回错误信息
	//若服务端已授权则返回授权信息，授权码加密后保存到本地
	//授权码为根据客户端标识数据加密得到的，与客户端标识一一对应
	if(!sock.Send(data, pos))
	{
		sMsg = "send get userlist msg fail";
		return false;
	}
	
	char resp[MAX_DATA] = {0};
	int ret = recv_resp(resp);
	if (0 > ret)
	{
		return false;
	}

	//解析body
	int data_len;
	memcpy(&data_len, resp, sizeof(int));
	pos = sizeof(int);
	std::string msg(&resp[pos], data_len);
	pos += data_len;

	if(0 == ret)
	{
		sMsg = "get userlist fail, " + msg;
		return false;
	}

	split_map(msg.c_str(), mapUserInfo, '=', '&');

	memcpy(&data_len, &resp[pos], sizeof(int));
	pos += sizeof(int);
	std::string level(&resp[pos], data_len);
	iLevel = atoi(level.c_str());

	return true;
}

//新增用户
bool _client_t::new_user(CString &user, CString &passwd,int &level, int &permissions, CString &loginuser, CString &loginpasswd)
{
	if(!bConnection)
	{
		sMsg = "no init connection";
		return false;
	}

	//获取授权请求串实现
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
	//获取授权请求数据
	//头部+客户端标识数据大小(int)+客户端标识数据
	tHead.length = 6*sizeof(int) + len1 + len2 + len3 + len4 + len5 + len6;
	//请求数据拷贝
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	//用户名
	memcpy(&data[pos], &len1, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sUser.c_str(), len1);
	pos += len1;
	//密码
	memcpy(&data[pos], &len2, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sPasswd.c_str(), len2);
	pos += len2;
	//其他
	memcpy(&data[pos], &len3, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sLevel.c_str(), len3);
	pos += len3;
	memcpy(&data[pos], &len4, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sPermissions.c_str(), len4);
	pos += len4;
	//登录
	memcpy(&data[pos], &len5, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sLoginUser.c_str(), len5);
	pos += len5;
	//密码
	memcpy(&data[pos], &len6, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sLoginPasswd.c_str(), len6);
	pos += len6;
	//若服务端未授权则返回错误信息
	//若服务端已授权则返回授权信息，授权码加密后保存到本地
	//授权码为根据客户端标识数据加密得到的，与客户端标识一一对应
	if(!sock.Send(data, pos))
	{
		sMsg = "send new user msg fail";
		return false;
	}

	char resp[MAX_DATA] = {0};
	int ret = recv_resp(resp);
	if (0 > ret)
	{
		return false;
	}

	//解析body
	int data_len;
	memcpy(&data_len, resp, sizeof(int));
	pos = sizeof(int);
	std::string msg(&resp[pos], data_len);
	pos += data_len;

	if(0 == ret)
	{
		sMsg = "new user fail, " + msg;
		return false;
	}

	return true;
}

//删除用户
bool _client_t::del_user(CString &user, CString &loginuser, CString &loginpasswd)
{
	if(!bConnection)
	{
		sMsg = "no init connection";
		return false;
	}

	//获取授权请求串实现
	req_head_t tHead;
	set_req_head(tHead, REQ_CONTROL_DEL_USER);

	string sUser = user.GetBuffer();
	int len1 = sUser.length();
	string sLoginUser = loginuser.GetBuffer();
	int len2 = sLoginUser.length();
	string sLoginPasswd = loginpasswd.GetBuffer();
	int len3 = sLoginPasswd.length();
	//获取授权请求数据
	//头部+客户端标识数据大小(int)+客户端标识数据
	tHead.length = 3*sizeof(int) + len1 + len2 + len3;
	//请求数据拷贝
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	//用户名
	memcpy(&data[pos], &len1, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sUser.c_str(), len1);
	pos += len1;
	//登录
	memcpy(&data[pos], &len2, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sLoginUser.c_str(), len2);
	pos += len2;
	//密码
	memcpy(&data[pos], &len3, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sLoginPasswd.c_str(), len3);
	pos += len3;
	//若服务端未授权则返回错误信息
	//若服务端已授权则返回授权信息，授权码加密后保存到本地
	//授权码为根据客户端标识数据加密得到的，与客户端标识一一对应
	if(!sock.Send(data, pos))
	{
		sMsg = "send new user msg fail";
		return false;
	}

	char resp[MAX_DATA] = {0};
	int ret = recv_resp(resp);
	if (0 > ret)
	{
		return false;
	}

	//解析body
	int data_len;
	memcpy(&data_len, resp, sizeof(int));
	pos = sizeof(int);
	std::string msg(&resp[pos], data_len);
	pos += data_len;

	if(0 == ret)
	{
		sMsg = "del user fail, " + msg;
		return false;
	}

	return true;
}

//修改用户
bool _client_t::update_user(CString &user, CString &passwd, int &permissions, CString &loginuser, CString &loginpasswd)
{
	if(!bConnection)
	{
		sMsg = "no init connection";
		return false;
	}

	//获取授权请求串实现
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
	//获取授权请求数据
	//头部+客户端标识数据大小(int)+客户端标识数据
	tHead.length = 5*sizeof(int) + len1 + len2 + len3 + len4 + len5;
	//请求数据拷贝
	char data[MAX_DATA] = {0};
	int pos=0;
	memcpy(&data[pos], &tHead, sizeof(req_head_t));
	pos += sizeof(req_head_t);
	//用户名
	memcpy(&data[pos], &len1, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sUser.c_str(), len1);
	pos += len1;
	//密码
	memcpy(&data[pos], &len2, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sPasswd.c_str(), len2);
	pos += len2;
	//其他
	memcpy(&data[pos], &len3, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sPermissions.c_str(), len3);
	pos += len3;
	//登录
	memcpy(&data[pos], &len4, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sLoginUser.c_str(), len4);
	pos += len4;
	//密码
	memcpy(&data[pos], &len5, sizeof(int));
	pos += sizeof(int);
	memcpy(&data[pos], sLoginPasswd.c_str(), len5);
	pos += len5;
	//若服务端未授权则返回错误信息
	//若服务端已授权则返回授权信息，授权码加密后保存到本地
	//授权码为根据客户端标识数据加密得到的，与客户端标识一一对应
	if(!sock.Send(data, pos))
	{
		sMsg = "send update user msg fail";
		return false;
	}

	char resp[MAX_DATA] = {0};
	int ret = recv_resp(resp);
	if (0 > ret)
	{
		return false;
	}

	//解析body
	int data_len;
	memcpy(&data_len, resp, sizeof(int));
	pos = sizeof(int);
	std::string msg(&resp[pos], data_len);
	pos += data_len;

	if(0 == ret)
	{
		sMsg = "update user fail, " + msg;
		return false;
	}

	return true;
}
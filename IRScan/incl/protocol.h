#ifndef PROTOCOL__H__
#define PROTOCOL__H__



#define   REQ_CMD_GET_AUTH      1
#define   REQ_CMD_CHECK_AUTH    2
#define   REQ_CMD_SEND_PNG      3
#define   REQ_CMD_SEND_DATA     4
#define   REQ_CMD_GET_PNG       5
#define   REQ_CMD_GET_DATA      6
#define   REQ_CMD_SEND_RESULT   7

#define   REQ_CONTROL_GET_LIST  8
#define   REQ_CONTROL_NEW_USER  9
#define   REQ_CONTROL_DEL_USER  10
#define   REQ_CONTROL_UPD_USER  11



#pragma pack(1)

//����ͷ
//cmd: 1��ȡ��Ȩ��2��֤��Ȩ��3�ϴ�ͼƬ���ݣ�4�ϴ������5���������6����ͼƬ��7�������
typedef struct _req_head
{
    int length;     //������ͷ���ĳ���
    int version;    //Э��汾��ĿǰΪ10001
    int cmd;        //��������;
    int timestamp;  //��ǰʱ�䣬ʱ�����ʽ
} req_head_t;

//Ӧ��ͷ
typedef struct _resp_head
{
    int length;     //������ͷ���ĳ���
    int result;     //���
    int timestamp;  //��ǰʱ�䣬ʱ�����ʽ
} resp_head_t;

/*
//������Ϣ����
typedef struct _usr_info
{
	std::string id;
	std::string name;
	std::string sex;
	std::string age;
	std::string identity;
	std::string pics;
} usr_info_t;
*/

#pragma pack()


#endif

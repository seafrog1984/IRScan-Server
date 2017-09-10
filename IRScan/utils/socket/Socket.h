#ifndef _MG_SOCKET_H
#define _MG_SOCKET_H


#include <winsock2.h> 
#include <iostream>
#pragma comment(lib,"ws2_32.lib")


class CSocket
{
public:
	CSocket();
	virtual ~CSocket();

	virtual bool Connect( const char* server, int port ); 
#ifdef WIN32
	int tConnect( SOCKET sock, const struct sockaddr *addr, unsigned int len );
#else
	int tConnect( int sock, const struct sockaddr *addr, unsigned int len );
#endif
	bool Send( const void* buf, int buflen );
	int     Read( void* buf, int buflen );
	bool SetTimeOut( unsigned int nsec );
	void SetConnectTimeOut( unsigned int nsec );

	SOCKET getSocket(void) {return m_Sock;}
	void  setSocket(SOCKET &conSocket) {m_Sock = conSocket;}
	int     GetLastError(){return m_nLastError;};
	bool Create();
	void Close();

protected:
	int m_nLastError;  
	bool m_bConnected; 
	unsigned int m_nConnectTimeOut;
	unsigned int m_nTimeOut;

private:
#ifdef WIN32
	SOCKET m_Sock;
#else
	int       m_Sock;
#endif
	bool  m_bSockValid; 
};


#endif
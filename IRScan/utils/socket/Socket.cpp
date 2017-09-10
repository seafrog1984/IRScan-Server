#include <stdafx.h>
#include "Socket.h"


const int gConnectTimeOut = 10;
const int gReceiveTimeOut =  30;


CSocket::CSocket()
{
#ifdef WIN32
	WSADATA wsadata;
	WSAStartup( MAKEWORD( 2, 0 ), &wsadata );
#endif

	m_bConnected = false;
	m_bSockValid = false;
	m_nLastError = 0;
	m_nConnectTimeOut = gConnectTimeOut;
	m_nTimeOut = gReceiveTimeOut;
}

CSocket::~CSocket()
{
	Close();

#ifdef WIN32
	WSACleanup();
#endif
}

bool CSocket::Create()
{
#ifdef WIN32
	m_Sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	if ( m_Sock == INVALID_SOCKET )
	{
#else
	m_Sock = socket( AF_INET, SOCK_STREAM, 0 );
	if ( m_Sock == -1 ) 
	{
#endif
			m_nLastError = 0x01;
			return false;
		}
		else 
		{
			m_bSockValid = true;
			return true;
		}
	}

bool CSocket::Connect( const char * server, int port )
{
		Close();
		if ( !Create() )
			return false;

		struct hostent *hp;
		hp = gethostbyname( server );
		if ( !hp ) {
			m_nLastError = 0x02;
			return false;
		}

		if ( hp->h_addrtype != AF_INET && hp->h_addrtype != AF_INET6 ) { //neither AF_INET nor AF_INET6
			m_nLastError = 0x03;
			return false;
		}
		struct sockaddr_in servaddr;
		memset( &servaddr, 0, sizeof( servaddr ) );
		servaddr.sin_family = hp->h_addrtype;
		servaddr.sin_port = htons( ( short ) port );
		servaddr.sin_addr = *( ( struct in_addr* ) ( hp->h_addr ) );
		//try connect
		if ( tConnect( m_Sock, ( struct sockaddr* ) & servaddr, sizeof( servaddr ) ) < 0 ) {
			m_nLastError = 0x04;
			return false;
		}
		else {
			m_bConnected = true;
#ifdef WIN32
			int winout=m_nTimeOut*1000;
			setsockopt( m_Sock, SOL_SOCKET, SO_SNDTIMEO, ( char* ) & winout, sizeof( int ) );
			setsockopt( m_Sock, SOL_SOCKET, SO_RCVTIMEO, ( char* ) & winout, sizeof( int ) );
#else
			struct timeval tv;
			tv.tv_sec = m_nTimeOut;
			tv.tv_usec = 0;
			setsockopt( m_Sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof( tv ) );
			setsockopt( m_Sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof( tv ) );
#endif
			return true;
		}
}

#ifdef WIN32
int CSocket::tConnect( SOCKET sock, const struct sockaddr * addr, unsigned int len )
#else
int CSocket::tConnect( int sock, const struct sockaddr * addr, unsigned int len )
#endif
{
			int ret, val;
			struct timeval tv;
			fd_set sset;
#ifdef WIN32
			int lon;
			u_long iMode = 1;
			ioctlsocket( sock, FIONBIO, &iMode );	//iMode = 0×èÈû£»1·Ç×èÈû
#else
			int flags, old_flags;
			socklen_t lon;
			flags = old_flags = fcntl( sock, F_GETFL, 0 );
			flags |= O_NONBLOCK;

			if ( fcntl( sock, F_SETFL, flags ) == -1 ) {
				return -1;
			}
#endif
			ret = connect( sock, addr, len );
			if ( ret < 0 ) {
#ifdef WIN32
				if ( WSAEWOULDBLOCK == WSAGetLastError() )
#else
					if ( errno == EINPROGRESS )
#endif
					{
						tv.tv_sec = m_nConnectTimeOut;
						tv.tv_usec = 0;
						FD_ZERO( &sset );
						FD_SET( sock, &sset );
						if ( select( sock + 1, NULL, &sset, NULL, &tv ) > 0 )
						{
							lon = sizeof( int );
#ifdef WIN32
							getsockopt( sock, SOL_SOCKET, SO_ERROR, ( char* ) ( &val ), &lon );
#else
							getsockopt( sock, SOL_SOCKET, SO_ERROR, ( void* ) ( &val ), &lon );
#endif
							if ( val != 0 ) {
								ret = -1;
							}
							else {
								ret = 0;
							}
						}
						else {
							ret = -1;
						}
					}
					else {
						ret = -1;
					}
			}

#ifdef WIN32
			iMode = 0;
			ioctlsocket( sock, FIONBIO, &iMode );	//enable the blocking mode 
#else
			fcntl( sock, F_SETFL, old_flags );
#endif
			return ret;
}

bool CSocket::Send( const void * buf, int buflen )
{
	int pos = 0;
	int nRet = 0;
	char* movehead = ( char* ) buf;
	if ( !m_bConnected ) {
		m_nLastError = 0x07;
		return false;
	}

	while ( pos < buflen ) {
#ifdef WIN32
		nRet = send ( m_Sock, ( char* ) ( movehead + pos ), buflen - pos, 0 /*flags*/ );
#else
		nRet = send ( m_Sock, ( void* ) ( movehead + pos ), buflen - pos, 0 /*flags*/ );
#endif
		if ( nRet == 0 ) {
			m_nLastError = 0x08;
			std::cout<<"0x08: send find connection closed;"<<std::endl;
			Close();
			return false;
		}
		else if ( nRet < 0 ) {
			m_nLastError = 0x09;
			std::cout<<"0x09: send find network error;"<<std::endl;
			Close();
			return false;
		}
		else {
			pos += nRet;
			if ( pos != buflen ) {
				continue;
			}
			else {
				return true;
			}
		}
	}

	m_nLastError = 0x0A;
	return false;
}

int CSocket::Read( void * buf, int buflen )
{
	if ( !m_bConnected ) {
		m_nLastError = 0x0B;
		return -1;
	}

	if ( buflen <= 0 || buf == NULL ) {
		m_nLastError = 0x0C;
		return -1;
	}

#ifdef WIN32
	int nret = recv( m_Sock, ( char* ) buf, buflen, 0 );
#else
	int nret = recv( m_Sock, buf, buflen, 0 );
#endif
	if ( nret < 0 ) {
		m_nLastError = 0x0D;
		Close();
		return -1;
	}
	else if ( nret == 0 ) {
		m_nLastError = 0x0E;
		Close();
		return 0;
	}
	return nret;
}

void CSocket::Close() 
{
	if ( m_bSockValid ) {
		if ( m_bConnected )
			shutdown( m_Sock, 0 );
#ifdef WIN32
		closesocket( m_Sock );
#else
		close( m_Sock );
#endif
		m_bSockValid = false;
		m_bConnected = false;
	}
}

void CSocket::SetConnectTimeOut( unsigned int nsec )
{
		if ( nsec < 5 || nsec > 300 )
			m_nConnectTimeOut = 30;
		else
			m_nConnectTimeOut = nsec;
}

bool CSocket::SetTimeOut( unsigned int nsec )
{
	if ( !m_bSockValid ) {
		m_nLastError = 0x05;
		return false;
	}
	if ( nsec <= 5 || nsec > 300 ) {
		m_nLastError = 0x06;
		return false;
	}

	m_nTimeOut = nsec;
	return true;
}
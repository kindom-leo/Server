#pragma once
#ifdef _WIN32

#include <WinSock2.h>
#include <process.h>
typedef int  socklen_t;
typedef  void THREAD_RET;

#else
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

typedef int SOCKET;
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT* PFLOAT;
typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int* PUINT;
typedef char* LPSTR;
typedef const char* LPCSTR;
#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
typedef  void* THREAD_RET;

#define closesocket(x) close(x)
inline int WSAGetLastError() {
	return errno;
}
#endif //Win32
class CSockLeo {
	SOCKET m_hSocket;
public:
	CSockLeo();
	~CSockLeo();
	operator SOCKET() {
		return m_hSocket;
	}

	BOOL Create(LPCTSTR strIp=nullptr, UINT nPort=0, int nType = SOCK_STREAM);
	int SendTo(LPCTSTR strIp, UINT nPort, void* buf, int len);
	int Receive(void* buf, int len, int flags = 0);
	int Send(void* buf, int len, int flags = 0) {
		return send(m_hSocket, (const char*)buf, len, flags);
	}

	BOOL Listen(int backlog = 5) {
		return listen(m_hSocket, backlog);
	}
	BOOL Connect(LPCTSTR strIp, UINT nPort);
	BOOL Accept(CSockLeo& sock, LPTSTR bufIp = NULL, UINT* nPort = NULL);
	
	void Close() {
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}

	int ReceiveFrom(LPTSTR strIp, UINT* nPort, void* buf, int len);
		
	static int GetLastError() {
		return WSAGetLastError();
	}

	BOOL GetSockName(char* sIp, UINT& nPort);
	BOOL GetPeerName(char* sIp, UINT& nPort);
};


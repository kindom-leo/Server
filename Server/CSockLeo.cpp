#include "CSockLeo.h"
#include <WS2tcpip.h>
#include <assert.h>

#pragma comment(lib,"ws2_32.lib")
CSockLeo::CSockLeo():m_hSocket(INVALID_SOCKET) {
    WSADATA data;
    WORD ver = MAKEWORD(2, 1);
    WSAStartup(ver, &data);
}

CSockLeo::~CSockLeo() {
    closesocket(m_hSocket);
    WSACleanup();
}

BOOL CSockLeo::Create(LPCTSTR strIp, UINT nPort, int nType) {
    m_hSocket = socket(AF_INET, nType, 0);
    if (m_hSocket == INVALID_SOCKET) {
        return FALSE;
    }

    sockaddr_in sa = { AF_INET,htons(nPort) };
    if (strIp) {
        InetPton(AF_INET, strIp, &sa.sin_addr.S_un.S_addr);
    }
    int ret = bind(m_hSocket, (const sockaddr*)&sa, sizeof(sa));
    if (ret == SOCKET_ERROR) {
        perror("bind");
        closesocket(m_hSocket);
        return FALSE;
    }
    return TRUE;
}

int CSockLeo::SendTo(LPCTSTR strIp, UINT nPort, void* buf, int len) {
    assert(strIp);
    sockaddr_in sa = { AF_INET,htons(nPort) };
    InetPton(AF_INET, strIp, &sa.sin_addr.S_un.S_addr);
    return sendto(m_hSocket, (LPCSTR)buf, len, 0, (const sockaddr*)&sa, sizeof(sa));
}

int CSockLeo::Receive(void* buf, int len, int flags) {
    return recv(m_hSocket,(char*)buf,len,flags);
}

BOOL CSockLeo::Connect(LPCTSTR strIp, UINT nPort) {
    sockaddr_in sa = { AF_INET,htons(nPort) };
    InetPton(AF_INET, strIp, &sa.sin_addr.S_un.S_addr);

    return !connect(m_hSocket, (const sockaddr*)&sa, sizeof(sa));
}

BOOL CSockLeo::Accept(CSockLeo& sock, LPTSTR bufIp, UINT* nPort) {
    sockaddr_in sa;
    int n = sizeof(sa);
    SOCKET fd = accept(m_hSocket, (sockaddr*)&sa, &n);
    if (fd == INVALID_SOCKET) {
        perror("accept()");
        return FALSE;
    }
    sock.m_hSocket = fd;
    if (nPort) {
        *nPort = htons(sa.sin_port);
    }
    if (bufIp) {
        InetNtop(AF_INET, &sa.sin_addr.S_un.S_addr, bufIp, sizeof(bufIp));
    }
    return TRUE;
}

int CSockLeo::ReceiveFrom(LPTSTR strIp, UINT* nPort, void* buf, int len) {
    if (strIp || nPort) {
        sockaddr_in sa;
        int n = sizeof(sa);
        n = recvfrom(m_hSocket, (char*)buf, len, 0, (sockaddr*)&sa, &n);
        if (n > 0) {
            if (nPort) {
                *nPort = htons(sa.sin_port);
            }
            if (strIp) {
                InetNtopA(AF_INET, &sa.sin_addr.S_un.S_addr, (char*)buf, sizeof(buf));
            }
        }
        return n;
    } else {
        recv(m_hSocket, (char*)buf, len, 0);
    }
    return 0;
}

BOOL CSockLeo::GetSockName(char* sIp, UINT& nPort) {
    assert(sIp);
    sockaddr_in sa;
    int n = sizeof(sa);
    n = getsockname(m_hSocket, (sockaddr*)&sa, &n);
    if (!n) {
        nPort = htons(sa.sin_port);
        InetNtopA(AF_INET, &sa.sin_addr.S_un.S_addr, sIp, sizeof(sIp));
        return TRUE;
    }
    return FALSE;
}

BOOL CSockLeo::GetPeerName(char* sIp, UINT& nPort) {
    assert(sIp);
    sockaddr_in sa;
    int n = sizeof(sa);
    n = getpeername(m_hSocket, (sockaddr*)&sa, &n);
    if (n) {
        return FALSE;
    }
    nPort = htons(sa.sin_port);
    InetNtopA(AF_INET, &sa.sin_addr.S_un.S_addr, sIp, sizeof(sIp));
    return TRUE;
}

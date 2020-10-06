#include "os_specific.h"

#if defined(__CPPSOCKET_WINDOWS__)

int writev(const int sock, const struct iovec *iov, int nvecs)
{
    DWORD ret;
    if (WSASend(sock, (LPWSABUF)iov, nvecs, &ret, 0, NULL, NULL) == 0)
    {
        return ret;
    }
    return -1;
}

int readv(const int sock, const struct iovec *iov, int nvecs)
{
    DWORD ret;
    if (WSARecv(sock, (LPWSABUF)iov, nvecs, &ret, 0, NULL, NULL) == 0)
    {
        return ret;
    }
    return -1;
}

#endif

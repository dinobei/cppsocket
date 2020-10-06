#include "native_socket.h"

cppsocket::NativeSocket::NativeSocket(int sockId) : sockId(sockId)
{
    assignPeerInfo(sockId);
}

cppsocket::NativeSocket::NativeSocket(ProtocolFamily family, SocketType type) : family(family), type(type) {}

void cppsocket::NativeSocket::close()
{
#if defined(__CPPSOCKET_WINDOWS__)
    closesocket(this->sockId);
#else
    ::close(this->sockId);
#endif
}

bool cppsocket::NativeSocket::close(enum CloseType type)
{
#if defined(__CPPSOCKET_WINDOWS__)
    switch (type)
    {
    case read:
        shutdown(this->sockId, SD_RECEIVE);
        break;
    case write:
        shutdown(this->sockId, SD_SEND);
        break;
    default:
        shutdown(this->sockId, SD_BOTH);
        break;
    }
#else
    switch (type)
    {
    case read:
        shutdown(this->sockId, SHUT_RD);
        break;
    case write:
        shutdown(this->sockId, SHUT_WR);
        break;
    default:
        shutdown(this->sockId, SHUT_RDWR);
        break;
    }
#endif
    return true;
}

bool cppsocket::NativeSocket::safeSend(char *buffer, const int bytes)
{
    return safeSend(buffer, 0, bytes);
}

bool cppsocket::NativeSocket::safeSend(char *buffer, const int offset, const int bytes, const int opt)
{
    int totalSendBytes = 0, sendBytes = 0;
    while (1)
    {
        sendBytes = ::send(sockId,
                           &buffer[offset + totalSendBytes],
                           bytes - totalSendBytes,
                           opt);
        if (sendBytes <= 0)
        {
            return false;
        }

        totalSendBytes += sendBytes;
        if (totalSendBytes >= bytes)
        {
            break;
        }
    }

    return true;
}

bool cppsocket::NativeSocket::safeRecv(char *buffer, const int bytes)
{
    return safeRecv(buffer, 0, bytes);
}

bool cppsocket::NativeSocket::safeRecv(char *buffer, const int offset, const int bytes, const int opt)
{
    int totalRecvBytes = 0, recvBytes = 0;
    while (1)
    {
        recvBytes = ::recv(sockId,
                           &buffer[offset + totalRecvBytes],
                           bytes - totalRecvBytes,
                           opt);
        if (recvBytes <= 0)
        {
            return false;
        }

        totalRecvBytes += recvBytes;
        if (totalRecvBytes >= bytes)
        {
            break;
        }
    }

    return true;
}

int cppsocket::NativeSocket::writev(const struct iovec *iov, const int iovcount)
{
    return ::writev(sockId, iov, iovcount);
}

int cppsocket::NativeSocket::readv(const struct iovec *iov, const int iovcount)
{
    return ::readv(sockId, iov, iovcount);
}

std::string cppsocket::NativeSocket::getSockIP()
{
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int ret = getsockname(sockId, (struct sockaddr *)&addr, &addr_size);
    if (ret != 0)
        return std::string();

    return inet_ntoa(addr.sin_addr);
}

std::string cppsocket::NativeSocket::getSockPort()
{
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int ret = getsockname(sockId, (struct sockaddr *)&addr, &addr_size);
    if (ret != 0)
        return std::string();

    return std::to_string(ntohs(addr.sin_port));
}

std::string cppsocket::NativeSocket::getPeerIP()
{
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int ret = getpeername(sockId, (struct sockaddr *)&addr, &addr_size);
    if (ret != 0)
        return std::string();

    return inet_ntoa(addr.sin_addr);
}

std::string cppsocket::NativeSocket::getPeerPort()
{
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int ret = getpeername(sockId, (struct sockaddr *)&addr, &addr_size);
    if (ret != 0)
        return std::string();

    return std::to_string(ntohs(addr.sin_port));
}

cppsocket::SocketType cppsocket::NativeSocket::getSocketType()
{
    return type;
}

cppsocket::ProtocolFamily cppsocket::NativeSocket::getProtocolFamily()
{
    return family;
}

int cppsocket::NativeSocket::Domain2address(const char *host, NSAddress_D *addr)
{
    if (host == NULL || addr == NULL)
    {
        return NS_INVALID_PARAMETERS;
    }

    int i;
    struct addrinfo hints, *result, *rp;
    struct sockaddr_in *sin;
    struct sockaddr_in6 *sin6;
    char buf[80] = {
        0,
    };

    memset(addr, 0, sizeof(NSAddress_D));

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM; // Only TCP

    if (getaddrinfo(host, "http", &hints, &result) != 0)
    {
        return NS_INVALID_HOST_NAME;
    }

    for (rp = result, i = 0; rp != NULL; rp = rp->ai_next, i++)
    {
        if (rp->ai_family == AF_INET6)
        {
            sin6 = (sockaddr_in6 *)rp->ai_addr;
            inet_ntop(rp->ai_family, &sin6->sin6_addr, buf, sizeof(buf));

            memcpy(addr->host, host, strlen(host));
            memcpy(addr->address, buf, strlen(buf));

            addr->ns_family = NS_FAMILY_INET6;

            freeaddrinfo(result);
            return NS_SUCCESS;
        }
    }

    for (rp = result, i = 0; rp != NULL; rp = rp->ai_next, i++)
    {
        if (rp->ai_family == AF_INET)
        {
            sin = (sockaddr_in *)rp->ai_addr;
            inet_ntop(rp->ai_family, &sin->sin_addr, buf, sizeof(buf));

            memcpy(addr->host, host, strlen(host));
            memcpy(addr->address, buf, strlen(buf));

            addr->ns_family = NS_FAMILY_INET;

            freeaddrinfo(result);
            return NS_SUCCESS;
        }
    }

    freeaddrinfo(result);
    return NS_UNKNOWN_ERROR;
}

bool cppsocket::NativeSocket::option(SocketOptionType type, void *arg)
{
    switch (type)
    {
    case SOCK_REUSE:
    {
        int option = *(int *)arg;
        setsockopt(this->sockId, SOL_SOCKET, SO_REUSEADDR, (char *)&option, (int)sizeof(option));
    }
    break;
    case SOCK_NODELAY:
    {
        int option = *(int *)arg;
        setsockopt(this->sockId, IPPROTO_TCP, TCP_NODELAY, (char *)&option, (int)sizeof(option));
    }
    break;
    case SOCK_LINGER:
    {
        int option = *(int *)arg;
        struct linger lng;
        lng.l_onoff = option;
        lng.l_linger = 0;
        setsockopt(this->sockId, SOL_SOCKET, SO_LINGER, (char *)&lng, sizeof(lng));
    }
    break;
    case SOCK_RCVTIMEO_MS:
    {
        int option = *(int *)arg;
#if defined(__CPPSOCKET_WINDOWS__)
        setsockopt(sockId, SOL_SOCKET, SO_RCVTIMEO, (char *)&option, sizeof(option));
#else
        struct timeval tv_timeo;
        tv_timeo.tv_sec = option / 1000;
        tv_timeo.tv_usec = (option % 1000) * 1000;
        setsockopt(this->sockId, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv_timeo, sizeof(tv_timeo));
#endif
    }
    break;
    case SOCK_SNDTIMEO_MS:
    {
        int option = *(int *)arg;
#if defined(__CPPSOCKET_WINDOWS__)
        setsockopt(sockId, SOL_SOCKET, SO_SNDTIMEO, (char *)&option, sizeof(option));
#else
        struct timeval tv_timeo;
        tv_timeo.tv_sec = option / 1000;
        tv_timeo.tv_usec = (option % 1000) * 1000;
        setsockopt(this->sockId, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv_timeo, sizeof(tv_timeo));
#endif
    }
    break;
    case SOCK_RCVBUFFER:
    {
        int option = *(int *)arg;
        setsockopt(this->sockId, SOL_SOCKET, SO_RCVBUF, (char *)&option, sizeof(int));
    }
    break;
    case SOCK_SNDBUFFER:
    {
        int option = *(int *)arg;
        setsockopt(this->sockId, SOL_SOCKET, SO_SNDBUF, (char *)&option, sizeof(int));
    }
    break;
    case SOCK_BROADCAST:
    {
        int option = *(int *)arg;
        setsockopt(this->sockId, SOL_SOCKET, SO_BROADCAST, (char *)&option, sizeof(option));
    }
    break;
    case SOCK_IP_MULTICAST_TTL:
    {
        int ttl = *(int *)arg;
        setsockopt(this->sockId, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl));
    }
    break;
    case SOCK_IP_ADD_MEMBERSHIP:
    {
        auto option = (char *)arg;

        struct ip_mreq join_adr;
        join_adr.imr_multiaddr.s_addr = inet_addr(option); // multicast group address
        join_adr.imr_interface.s_addr = htons(INADDR_ANY);
        setsockopt(this->sockId, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&join_adr, sizeof(join_adr));
    }
    break;
    }

    return true;
}

void cppsocket::NativeSocket::setSocketIdentifier(int socketId)
{
    assignPeerInfo(socketId);
}

bool cppsocket::NativeSocket::connect(std::string hostname, std::string service, int ms)
{
#if defined(__CPPSOCKET_WINDOWS__)
    createSocket();

    struct sockaddr_in serv_addr;

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(service.c_str()));
    inet_pton(AF_INET, hostname.c_str(), &serv_addr.sin_addr.s_addr);

    ULONG nonBlk = TRUE;

    if (ioctlsocket(this->sockId, FIONBIO, &nonBlk) == SOCKET_ERROR)
        return false;

    int ret = ::connect(this->sockId, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (ret == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
        return false;

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(this->sockId, &fdset);

    struct timeval timevalue;
    timevalue.tv_sec = ms / 1000;
    timevalue.tv_usec = (ms % 1000) * 1000;

    if (select(0, NULL, &fdset, NULL, &timevalue) == SOCKET_ERROR)
        return false;

    if (!FD_ISSET(sockId, &fdset))
    {
        this->close();
        sockId = INVALID_SOCKET;
        return false;
    }

    nonBlk = FALSE;
    if (ioctlsocket(sockId, FIONBIO, &nonBlk) == SOCKET_ERROR)
        return false;

    return true;

#else

    struct addrinfo hints, *res, *ressave;
    int n;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;

    hints.ai_socktype = SOCK_STREAM; // default is tcp
    switch (this->type)
    {
    case tcp:
        hints.ai_socktype = SOCK_STREAM;
        break;
    case udp:
        hints.ai_socktype = SOCK_DGRAM;
        break;
    default:
        break;
    }

    n = getaddrinfo(hostname.c_str(), service.c_str(), &hints, &res);
    if (n != 0)
        return false;

    ressave = res;
    do
    {
        struct sockaddr_in *ts;
        sockId = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockId < 0)
            continue;

        int sr_ms = 500;
        option(SOCK_RCVTIMEO_MS, (void *)&sr_ms);
        option(SOCK_SNDTIMEO_MS, (void *)&sr_ms);

        ts = (struct sockaddr_in *)res->ai_addr;

        if (connectNonblock((struct sockaddr *)res->ai_addr, res->ai_addrlen, ms) == 0)
            break;

        ::close(sockId);
    } while ((res = res->ai_next) != NULL);

    if (res == NULL)
        return false;

    freeaddrinfo(ressave);

    return true;

#endif
}

bool cppsocket::NativeSocket::bind(int port, cppsocket::ProtocolFamily family, std::string hostaddr)
{
    int retv;
    struct sockaddr_in serv_addr;

    memset(&serv_addr, 0, sizeof(serv_addr));

    if (family == cppsocket::IPv4)
        serv_addr.sin_family = AF_INET;
    else
        serv_addr.sin_family = AF_INET6;

    if (hostaddr.empty())
    {
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        inet_pton(serv_addr.sin_family,
                  hostaddr.c_str(),
                  &serv_addr.sin_addr.s_addr);
    }

    serv_addr.sin_port = htons(port);

    retv = ::bind(sockId, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (retv != 0)
    {
        std::cout << "Bind Failed - " << retv << std::endl;
        return false;
    }

    return true;
}

void cppsocket::NativeSocket::assignPeerInfo(int sockId)
{
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int ret = getpeername(sockId, (struct sockaddr *)&addr, &addr_size);
    assert(ret == 0);

    sockIP = getSockIP();
    sockPort = getSockPort();
    peerIP = inet_ntoa(addr.sin_addr);
    peerPort = std::to_string(ntohs(addr.sin_port));

    if (addr.sin_family == AF_INET)
    {
        this->family = cppsocket::IPv4;
    }
    else if (addr.sin_family == AF_INET6)
    {
        this->family = cppsocket::IPv6;
    }
    else
    {
        assert(false);
    }

    int socketType;
    socklen_t socketTypeLength = sizeof(socketType);
    ret = getsockopt(sockId, SOL_SOCKET, SO_TYPE, reinterpret_cast<SOCKOPT_TYPE *>(&socketType), &socketTypeLength);
    assert(ret == 0);
    if (socketType == SOCK_DGRAM)
    {
        this->type = cppsocket::udp;
    }
    else if (socketType == SOCK_STREAM)
    {
        this->type = cppsocket::tcp;
    }
    else
    {
        assert(false);
    }
}

ssize_t cppsocket::NativeSocket::sendTo(Peer *peer, char *buffer, const unsigned int bytes, const int opt)
{
    return ::sendto(this->getSocketIdentifier(), buffer, bytes, opt, (struct sockaddr *)&peer->sockAddr, peer->sockLen);
}

ssize_t cppsocket::NativeSocket::sendTo(Peer *peer, char *buffer, const unsigned int offset, const unsigned int bytes, const int opt)
{
    return ::sendto(this->getSocketIdentifier(), &buffer[offset], bytes, opt, (struct sockaddr *)&peer->sockAddr, peer->sockLen);
}

ssize_t cppsocket::NativeSocket::recvFrom(Peer *peer, char *buffer, const unsigned int bytes, const int opt)
{
    return ::recvfrom(this->getSocketIdentifier(), buffer, bytes, opt, (struct sockaddr *)&peer->sockAddr, &peer->sockLen);
}

ssize_t cppsocket::NativeSocket::recvFrom(Peer *peer, char *buffer, const unsigned int offset, const unsigned int bytes, const int opt)
{
    return ::recvfrom(this->getSocketIdentifier(), &buffer[offset], bytes, opt, (struct sockaddr *)&peer->sockAddr, &peer->sockLen);
}

ssize_t cppsocket::NativeSocket::send(char *buffer, const unsigned int bytes, const int opt)
{
    return ::send(this->getSocketIdentifier(), buffer, bytes, opt);
}

ssize_t cppsocket::NativeSocket::send(char *buffer, const int offset, const int unsigned bytes, const int opt)
{
    return ::send(this->getSocketIdentifier(), &buffer[offset], bytes, opt);
}

ssize_t cppsocket::NativeSocket::recv(char *buffer, const unsigned int bytes, const int opt)
{
    return ::recv(this->getSocketIdentifier(), buffer, bytes, opt);
}

ssize_t cppsocket::NativeSocket::recv(char *buffer, const int offset, const unsigned int bytes, const int opt)
{
    return ::recv(this->getSocketIdentifier(), &buffer[offset], bytes, opt);
}

#if !defined(__CPPSOCKET_WINDOWS__)

using namespace std::chrono;
void cppsocket::NativeSocket::delayIfNeeded(long int timeout_ms)
{
    auto timeout = milliseconds(timeout_ms);
    auto current = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    auto passed_time = duration_cast<milliseconds>(current - last);
    if (passed_time < timeout)
    {
        __msleep((timeout - passed_time).count());
    }

    last = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
}

int cppsocket::NativeSocket::connectNonblock(const struct sockaddr *saptr, int salen, const int ms)
{
    int flags, n, error;
    socklen_t len;
    fd_set rset, wset;
    struct timeval tval;

    flags = fcntl(this->sockId, F_GETFL, 0);
    fcntl(this->sockId, F_SETFL, flags | O_NONBLOCK);

    error = 0;

    if ((n = ::connect(this->sockId, (struct sockaddr *)saptr, salen)) < 0)
    {
        if (errno != EINPROGRESS)
        {
            std::cout << "errno is not EINPROGRESS" << std::endl;
            delayIfNeeded(ms);
            return -1;
        }
    }

    if (n == 0)
        goto done; /* connect completed immediately */

    FD_ZERO(&rset);
    FD_SET(this->sockId, &rset);
    wset = rset;

    tval.tv_sec = ms / 1000;
    tval.tv_usec = (ms % 1000) * 1000;
    if ((n = select(this->sockId + 1, &rset, &wset, NULL, &tval)) == 0)
    {
        this->close(); /* timeout */
        errno = ETIMEDOUT;
        return -1;
    }
    if (FD_ISSET(this->sockId, &rset) || FD_ISSET(this->sockId, &wset))
    {
        len = sizeof(error);
        if (getsockopt(this->sockId, SOL_SOCKET, SO_ERROR, reinterpret_cast<SOCKOPT_TYPE *>(&error), &len) < 0)
        {
            delayIfNeeded(ms);
            return -1; /* Solaris pending error */
        }
    }
    else
    {
        // printf("select error: sockfd not set\n");
        // printf("* select error\n");
        delayIfNeeded(ms);
        return -1;
    }

done:
    fcntl(this->sockId, F_SETFL, flags); /* restore file status flags */
    if (error)
    {
        this->close(); /* just in case */
        errno = error;
        delayIfNeeded(ms);
        return -1;
    }

    return 0;
}

#endif

void cppsocket::NativeSocket::createSocket()
{
    this->sockId = socket((this->family == IPv4) ? AF_INET : AF_INET6,
                          (this->type == cppsocket::tcp) ? SOCK_STREAM : SOCK_DGRAM,
                          (this->type == cppsocket::tcp) ? IPPROTO_TCP : IPPROTO_UDP);
}

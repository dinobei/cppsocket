#include "native_socket.h"

cppsocket::native_socket::native_socket(int sock_id) : sock_id(sock_id)
{
    assign_peer_info(sock_id);
}

cppsocket::native_socket::native_socket(protocol_family family, socket_type type) : family(family), type(type) {}

void cppsocket::native_socket::close()
{
#if defined(__CPPSOCKET_WINDOWS__)
    closesocket(this->sock_id);
#else
    ::close(this->sock_id);
#endif
}

bool cppsocket::native_socket::close(enum close_type type)
{
#if defined(__CPPSOCKET_WINDOWS__)
    switch (type)
    {
    case read:
        shutdown(this->sock_id, SD_RECEIVE);
        break;
    case write:
        shutdown(this->sock_id, SD_SEND);
        break;
    default:
        shutdown(this->sock_id, SD_BOTH);
        break;
    }
#else
    switch (type)
    {
    case read:
        shutdown(this->sock_id, SHUT_RD);
        break;
    case write:
        shutdown(this->sock_id, SHUT_WR);
        break;
    default:
        shutdown(this->sock_id, SHUT_RDWR);
        break;
    }
#endif
    return true;
}

bool cppsocket::native_socket::safe_send(char *buffer, const int bytes)
{
    return safe_send(buffer, 0, bytes);
}

bool cppsocket::native_socket::safe_send(char *buffer, const int offset, const int bytes, const int opt)
{
    int totalSendBytes = 0, sendBytes = 0;
    while (1)
    {
        sendBytes = ::send(sock_id,
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

bool cppsocket::native_socket::safe_recv(char *buffer, const int bytes)
{
    return safe_recv(buffer, 0, bytes);
}

bool cppsocket::native_socket::safe_recv(char *buffer, const int offset, const int bytes, const int opt)
{
    int totalRecvBytes = 0, recvBytes = 0;
    while (1)
    {
        recvBytes = ::recv(sock_id,
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

int cppsocket::native_socket::writev(const struct iovec *iov, const int iovcount)
{
    return ::writev(sock_id, iov, iovcount);
}

int cppsocket::native_socket::readv(const struct iovec *iov, const int iovcount)
{
    return ::readv(sock_id, iov, iovcount);
}

std::string cppsocket::native_socket::get_sock_ip()
{
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int ret = getsockname(sock_id, (struct sockaddr *)&addr, &addr_size);
    if (ret != 0)
        return std::string();

    return inet_ntoa(addr.sin_addr);
}

std::string cppsocket::native_socket::get_sock_port()
{
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int ret = getsockname(sock_id, (struct sockaddr *)&addr, &addr_size);
    if (ret != 0)
        return std::string();

    return std::to_string(ntohs(addr.sin_port));
}

std::string cppsocket::native_socket::get_peer_ip()
{
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int ret = getpeername(sock_id, (struct sockaddr *)&addr, &addr_size);
    if (ret != 0)
        return std::string();

    return inet_ntoa(addr.sin_addr);
}

std::string cppsocket::native_socket::get_peer_port()
{
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int ret = getpeername(sock_id, (struct sockaddr *)&addr, &addr_size);
    if (ret != 0)
        return std::string();

    return std::to_string(ntohs(addr.sin_port));
}

cppsocket::socket_type cppsocket::native_socket::get_socket_type()
{
    return type;
}

cppsocket::protocol_family cppsocket::native_socket::get_protocol_family()
{
    return family;
}

int cppsocket::native_socket::domain2address(const char *host, address *addr)
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

    memset(addr, 0, sizeof(address));

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

bool cppsocket::native_socket::option(socket_option_type type, void *arg)
{
    switch (type)
    {
    case SOCK_REUSE:
    {
        int option = *(int *)arg;
        setsockopt(this->sock_id, SOL_SOCKET, SO_REUSEADDR, (char *)&option, (int)sizeof(option));
    }
    break;
    case SOCK_NODELAY:
    {
        int option = *(int *)arg;
        setsockopt(this->sock_id, IPPROTO_TCP, TCP_NODELAY, (char *)&option, (int)sizeof(option));
    }
    break;
    case SOCK_LINGER:
    {
        int option = *(int *)arg;
        struct linger lng;
        lng.l_onoff = option;
        lng.l_linger = 0;
        setsockopt(this->sock_id, SOL_SOCKET, SO_LINGER, (char *)&lng, sizeof(lng));
    }
    break;
    case SOCK_RCVTIMEO_MS:
    {
        int option = *(int *)arg;
#if defined(__CPPSOCKET_WINDOWS__)
        setsockopt(sock_id, SOL_SOCKET, SO_RCVTIMEO, (char *)&option, sizeof(option));
#else
        struct timeval tv_timeo;
        tv_timeo.tv_sec = option / 1000;
        tv_timeo.tv_usec = (option % 1000) * 1000;
        setsockopt(this->sock_id, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv_timeo, sizeof(tv_timeo));
#endif
    }
    break;
    case SOCK_SNDTIMEO_MS:
    {
        int option = *(int *)arg;
#if defined(__CPPSOCKET_WINDOWS__)
        setsockopt(sock_id, SOL_SOCKET, SO_SNDTIMEO, (char *)&option, sizeof(option));
#else
        struct timeval tv_timeo;
        tv_timeo.tv_sec = option / 1000;
        tv_timeo.tv_usec = (option % 1000) * 1000;
        setsockopt(this->sock_id, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv_timeo, sizeof(tv_timeo));
#endif
    }
    break;
    case SOCK_RCVBUFFER:
    {
        int option = *(int *)arg;
        setsockopt(this->sock_id, SOL_SOCKET, SO_RCVBUF, (char *)&option, sizeof(int));
    }
    break;
    case SOCK_SNDBUFFER:
    {
        int option = *(int *)arg;
        setsockopt(this->sock_id, SOL_SOCKET, SO_SNDBUF, (char *)&option, sizeof(int));
    }
    break;
    case SOCK_BROADCAST:
    {
        int option = *(int *)arg;
        setsockopt(this->sock_id, SOL_SOCKET, SO_BROADCAST, (char *)&option, sizeof(option));
    }
    break;
    case SOCK_IP_MULTICAST_TTL:
    {
        int ttl = *(int *)arg;
        setsockopt(this->sock_id, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl));
    }
    break;
    case SOCK_IP_ADD_MEMBERSHIP:
    {
        auto option = (char *)arg;

        struct ip_mreq join_adr;
        join_adr.imr_multiaddr.s_addr = inet_addr(option); // multicast group address
        join_adr.imr_interface.s_addr = htons(INADDR_ANY);
        setsockopt(this->sock_id, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&join_adr, sizeof(join_adr));
    }
    break;
    }

    return true;
}

void cppsocket::native_socket::set_socket_identifier(int sock_id)
{
    assign_peer_info(sock_id);
}

bool cppsocket::native_socket::connect(std::string hostname, std::string service, int ms)
{
#if defined(__CPPSOCKET_WINDOWS__)
    create_socket();

    struct sockaddr_in serv_addr;

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(service.c_str()));
    inet_pton(AF_INET, hostname.c_str(), &serv_addr.sin_addr.s_addr);

    ULONG nonblk = TRUE;

    if (ioctlsocket(this->sock_id, FIONBIO, &nonblk) == SOCKET_ERROR)
        return false;

    int ret = ::connect(this->sock_id, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (ret == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
        return false;

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(this->sock_id, &fdset);

    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;

    if (select(0, NULL, &fdset, NULL, &tv) == SOCKET_ERROR)
        return false;

    if (!FD_ISSET(sock_id, &fdset))
    {
        this->close();
        sock_id = INVALID_SOCKET;
        return false;
    }

    nonblk = FALSE;
    if (ioctlsocket(sock_id, FIONBIO, &nonblk) == SOCKET_ERROR)
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
        sock_id = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sock_id < 0)
            continue;

        int sr_ms = 500;
        option(SOCK_RCVTIMEO_MS, (void *)&sr_ms);
        option(SOCK_SNDTIMEO_MS, (void *)&sr_ms);

        ts = (struct sockaddr_in *)res->ai_addr;

        if (connect_nonblock((struct sockaddr *)res->ai_addr, res->ai_addrlen, ms) == 0)
            break;

        ::close(sock_id);
    } while ((res = res->ai_next) != NULL);

    if (res == NULL)
        return false;

    freeaddrinfo(ressave);

    return true;

#endif
}

bool cppsocket::native_socket::bind(int port, cppsocket::protocol_family family, std::string hostaddr)
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

    retv = ::bind(sock_id, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (retv != 0)
    {
        std::cout << "Bind Failed - " << retv << std::endl;
        return false;
    }

    return true;
}

void cppsocket::native_socket::assign_peer_info(int sock_id)
{
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int ret = getpeername(sock_id, (struct sockaddr *)&addr, &addr_size);
    assert(ret == 0);

    sock_ip = get_sock_ip();
    sock_port = get_sock_port();
    peer_ip = inet_ntoa(addr.sin_addr);
    peer_port = std::to_string(ntohs(addr.sin_port));

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

    int socket_type;
    socklen_t socket_type_length = sizeof(socket_type);
    ret = getsockopt(sock_id, SOL_SOCKET, SO_TYPE, reinterpret_cast<SOCKOPT_TYPE *>(&socket_type), &socket_type_length);
    assert(ret == 0);
    if (socket_type == SOCK_DGRAM)
    {
        this->type = cppsocket::udp;
    }
    else if (socket_type == SOCK_STREAM)
    {
        this->type = cppsocket::tcp;
    }
    else
    {
        assert(false);
    }
}

ssize_t cppsocket::native_socket::sendto(peer *peer, char *buffer, const unsigned int bytes, const int opt)
{
    return ::sendto(this->get_socket_identifier(), buffer, bytes, opt, (struct sockaddr *)&peer->sock_addr, peer->sock_len);
}

ssize_t cppsocket::native_socket::sendto(peer *peer, char *buffer, const unsigned int offset, const unsigned int bytes, const int opt)
{
    return ::sendto(this->get_socket_identifier(), &buffer[offset], bytes, opt, (struct sockaddr *)&peer->sock_addr, peer->sock_len);
}

ssize_t cppsocket::native_socket::recvfrom(peer *peer, char *buffer, const unsigned int bytes, const int opt)
{
    return ::recvfrom(this->get_socket_identifier(), buffer, bytes, opt, (struct sockaddr *)&peer->sock_addr, &peer->sock_len);
}

ssize_t cppsocket::native_socket::recvfrom(peer *peer, char *buffer, const unsigned int offset, const unsigned int bytes, const int opt)
{
    return ::recvfrom(this->get_socket_identifier(), &buffer[offset], bytes, opt, (struct sockaddr *)&peer->sock_addr, &peer->sock_len);
}

ssize_t cppsocket::native_socket::send(char *buffer, const unsigned int bytes, const int opt)
{
    return ::send(this->get_socket_identifier(), buffer, bytes, opt);
}

ssize_t cppsocket::native_socket::send(char *buffer, const int offset, const int unsigned bytes, const int opt)
{
    return ::send(this->get_socket_identifier(), &buffer[offset], bytes, opt);
}

ssize_t cppsocket::native_socket::recv(char *buffer, const unsigned int bytes, const int opt)
{
    return ::recv(this->get_socket_identifier(), buffer, bytes, opt);
}

ssize_t cppsocket::native_socket::recv(char *buffer, const int offset, const unsigned int bytes, const int opt)
{
    return ::recv(this->get_socket_identifier(), &buffer[offset], bytes, opt);
}

#if !defined(__CPPSOCKET_WINDOWS__)

using namespace std::chrono;
void cppsocket::native_socket::delay_if_needed(long int timeout_ms)
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

int cppsocket::native_socket::connect_nonblock(const struct sockaddr *saptr, int salen, const int ms)
{
    int flags, n, error;
    socklen_t len;
    fd_set rset, wset;
    struct timeval tv;

    flags = fcntl(this->sock_id, F_GETFL, 0);
    fcntl(this->sock_id, F_SETFL, flags | O_NONBLOCK);

    error = 0;

    if ((n = ::connect(this->sock_id, (struct sockaddr *)saptr, salen)) < 0)
    {
        if (errno != EINPROGRESS)
        {
            std::cout << "errno is not EINPROGRESS" << std::endl;
            delay_if_needed(ms);
            return -1;
        }
    }

    if (n == 0)
        goto done; /* connect completed immediately */

    FD_ZERO(&rset);
    FD_SET(this->sock_id, &rset);
    wset = rset;

    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    if ((n = select(this->sock_id + 1, &rset, &wset, NULL, &tv)) == 0)
    {
        this->close(); /* timeout */
        errno = ETIMEDOUT;
        return -1;
    }
    if (FD_ISSET(this->sock_id, &rset) || FD_ISSET(this->sock_id, &wset))
    {
        len = sizeof(error);
        if (getsockopt(this->sock_id, SOL_SOCKET, SO_ERROR, reinterpret_cast<SOCKOPT_TYPE *>(&error), &len) < 0)
        {
            delay_if_needed(ms);
            return -1; /* Solaris pending error */
        }
    }
    else
    {
        // printf("select error: sockfd not set\n");
        // printf("* select error\n");
        delay_if_needed(ms);
        return -1;
    }

done:
    fcntl(this->sock_id, F_SETFL, flags); /* restore file status flags */
    if (error)
    {
        this->close(); /* just in case */
        errno = error;
        delay_if_needed(ms);
        return -1;
    }

    return 0;
}

#endif

void cppsocket::native_socket::create_socket()
{
    this->sock_id = socket((this->family == IPv4) ? AF_INET : AF_INET6,
                          (this->type == cppsocket::tcp) ? SOCK_STREAM : SOCK_DGRAM,
                          (this->type == cppsocket::tcp) ? IPPROTO_TCP : IPPROTO_UDP);
}

#include <stdio.h>
#include <stdlib.h>
#include "cppsocket/tcp_socket.h"

int main(int argc, char *argv[])
{
    int reuse = 1;
    cppsocket::tcp_socket sock((ushort)9000);
    sock.option(cppsocket::SOCK_REUSE, (void *)&reuse);
    std::cout << "tcp server-socket info, " << sock.sock_ip << ":" << sock.sock_port << std::endl;

    auto client = sock.accept();
    std::cout << "Connected client ip address: " << client->peer_ip << ":" << client->peer_port << std::endl;

    int ms = 1000;
    client->option(cppsocket::SOCK_SNDTIMEO_MS, (void *)&ms);
    client->option(cppsocket::SOCK_RCVTIMEO_MS, (void *)&ms);

    char buf[255] = {
        0,
    };

    if (!client->safe_recv(buf, 10))
    {
        std::cout << "recv error" << std::endl;
        return -1;
    }
    std::cout << "msg from client : " << buf << std::endl;

    if (!client->safe_send(buf, sizeof(buf)))
    {
        std::cout << "send error" << std::endl;
        return -1;
    }
    std::cout << "msg echo success : " << buf << std::endl;

    sock.close();
    return 0;
}

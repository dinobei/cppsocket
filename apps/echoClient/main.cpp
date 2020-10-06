#include <stdio.h>
#include <stdlib.h>
#include "cppsocket/tcp_socket.h"

int main(int argc, char *argv[])
{
    int tryCnt = 0;
    cppsocket::TCPSocket sock;
    while (true)
    {
        if (sock.connect("127.0.0.1", "9000", 1000))
        {
            std::cout << "Connect Success" << std::endl;
            break;
        }
        else
        {
            std::cout << "Connect Error - (try: " << (tryCnt++) << ")" << std::endl;
        }
    }

    char sendBuf[11] = "1234567890";
    char recvBuf[11] = {
        0,
    };

    if (!sock.safeSend(sendBuf, 10))
    {
        std::cout << "send error" << std::endl;
        return -1;
    }
    std::cout << "msg send success: " << sendBuf << std::endl;

    if (!sock.safeRecv(recvBuf, 10))
    {
        std::cout << "recv error" << std::endl;
        return -1;
    }
    std::cout << "msg from server: " << recvBuf << std::endl;

    sock.close();
    return 0;
}

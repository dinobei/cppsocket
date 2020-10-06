#include <stdio.h>
#include <stdlib.h>
#include "cppsocket/tcp_socket.h"

int main(int argc, char *argv[])
{
    int tryCnt = 0;
    cppsocket::tcp_socket sock;
    while (true)
    {
        if (sock.connect("127.0.0.1", "9000"))
        {
            std::cout << "Connected" << std::endl;
            break;
        }
        else
        {
            std::cout << "Connect Error - (try: " << (tryCnt++) << ")" << std::endl;
        }
    }

    char send_buf[11] = "1234567890";
    char recv_buf[11] = {
        0,
    };

    if (!sock.safe_send(send_buf, 10))
    {
        std::cout << "send error" << std::endl;
        return -1;
    }
    std::cout << "msg send success: " << send_buf << std::endl;

    if (!sock.safe_recv(recv_buf, 10))
    {
        std::cout << "recv error" << std::endl;
        return -1;
    }
    std::cout << "msg from server: " << recv_buf << std::endl;

    sock.close();
    return 0;
}

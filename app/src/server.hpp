#pragma once

#include <netinet/in.h>
#include <vector>
#include <string>

std::vector<std::string> split(const std::string &s, char delimiter);

class Server
{
public:
    Server() = default;
    ~Server() = default;
    void init();
    std::string getMessage();
private:
    int server_fd, new_socket, valread;
    struct sockaddr_in addr;
    int opt = 1;
    int addrlen = sizeof(addr);
    char buffer[1024] = {0};
};

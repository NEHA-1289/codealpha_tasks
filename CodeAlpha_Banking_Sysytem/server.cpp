
#include <iostream>
#include <string>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) return 1;

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(
        std::getenv("PORT") ? std::atoi(std::getenv("PORT")) : 10000
    );

    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0)
        return 1;

    if (listen(server_fd, 10) < 0) return 1;

    while (true) {
        int client = accept(server_fd, nullptr, nullptr);
        if (client < 0) continue;

        std::string body =
            "<html><head><title>Banking System</title></head>"
            "<body style='font-family:Arial;text-align:center;margin-top:80px'>"
            "<h1>Banking System</h1>"
            "<p>Banking System service is running successfully.</p>"
            "<p>C++ application deployed using Docker and Render.</p>"
            "</body></html>";

        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=UTF-8\r\n"
            "Content-Length: " + std::to_string(body.size()) +
            "\r\nConnection: close\r\n\r\n" + body;

        send(client, response.c_str(), response.size(), 0);
        close(client);
    }

    close(server_fd);
    return 0;
}

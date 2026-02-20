#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

bool check_port(const std::string& host, int port, int timeout_ms = 500) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return false;

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) {
        close(sock);
        return false;
    }

    // Set non-blocking
    fcntl(sock, F_SETFL, O_NONBLOCK);

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);

    struct timeval tv{};
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    if (select(sock + 1, nullptr, &fdset, nullptr, &tv) == 1) {
        int so_error;
        socklen_t len = sizeof so_error;
        getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
        close(sock);
        return so_error == 0;
    }

    close(sock);
    return false;
}

std::string identify_service(int port) {
    switch (port) {
        case 22: return "SSH";
        case 80: return "HTTP";
        case 443: return "HTTPS";
        case 3306: return "MySQL";
        case 21: return "FTP";
        case 25: return "SMTP";
        case 3389: return "RDP";
        default: return "Unknown";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./net_inspector <target_ip>\n";
        return 1;
    }

    std::string target = argv[1];

    std::cout << "Network Inspection Report\n";
    std::cout << "=========================\n\n";
    std::cout << "Target: " << target << "\n\n";

    std::vector<int> common_ports = {21,22,25,53,80,110,143,443,3306,3389};
    std::vector<int> open_ports;

    for (int port : common_ports) {
        if (check_port(target, port)) {
            open_ports.push_back(port);
        }
    }

    if (open_ports.empty()) {
        std::cout << "No common ports open.\n";
    } else {
        std::cout << "Open Ports:\n";
        for (int port : open_ports) {
            std::cout << "  " << port << " (" << identify_service(port) << ")\n";
        }
    }

    std::cout << "\nRisk Summary:\n";
    for (int port : open_ports) {
        if (port == 22) std::cout << "  - SSH exposed\n";
        if (port == 80 || port == 443) std::cout << "  - Web server detected\n";
        if (port == 3306) std::cout << "  - Database exposed\n";
        if (port == 3389) std::cout << "  - RDP exposed\n";
    }

    std::cout << "\nDone.\n";
    return 0;
}

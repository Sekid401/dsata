#pragma once
#include <string>
#include <cstdint>

// DSataServer — serves a dSATA volume over TCP
// One connection at a time; client mounts a volume, then issues READ/WRITE commands.
class DSataServer {
public:
    explicit DSataServer(uint16_t port);
    ~DSataServer();

    void start();   // blocking: accept → serve → repeat
    void stop();

    bool isRunning() const;
    uint16_t port() const;

private:
    uint16_t port_;
    int listenfd_;
    bool running_;

    void serveClient(int clientfd);
    void sendLine(int fd, const std::string& line);
    std::string recvLine(int fd);
    void sendRaw(int fd, const uint8_t* data, size_t size);
    void recvRaw(int fd, uint8_t* buffer, size_t size);
};

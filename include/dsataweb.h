#pragma once
#include <string>
#include <cstdint>

#define DSATA_WEB_SPEED_LIMIT 125000000ULL  // 1 Gbps = 125 MB/s

class DSataThrottle;

class DSataWeb {
public:
    DSataWeb();
    ~DSataWeb();

    void connect(const std::string& host, uint16_t port);
    void disconnect();
    void mount(const std::string& volumeName);
    void unmount();

    void write(const uint8_t* data, size_t size, uint64_t offset);
    void read(uint8_t* buffer, size_t size, uint64_t offset);

    // raw transport (internal use)
    void send(const uint8_t* data, size_t size);
    void recv(uint8_t* buffer, size_t size);

    bool isConnected() const;
    bool isMounted() const;
    const std::string& host() const;
    uint16_t port() const;
    const std::string& mountedVolume() const;

private:
    std::string host_;
    uint16_t port_;
    std::string mountedVolume_;
    bool connected_;
    bool mounted_;
    int sockfd_;

    DSataThrottle* webThrottle_;

    void sendLine(const std::string& line);
    std::string recvLine();
};

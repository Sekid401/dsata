#include "../include/dsataweb.h"
#include "../src/throttle.cpp"
#include <stdexcept>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

DSataWeb::DSataWeb()
    : port_(0), connected_(false), mounted_(false),
      sockfd_(-1), webThrottle_(new DSataThrottle(DSATA_WEB_SPEED_LIMIT)) {}

DSataWeb::~DSataWeb() {
    if (connected_) disconnect();
    delete webThrottle_;
}

void DSataWeb::connect(const std::string& host, uint16_t port) {
    if (connected_) throw std::runtime_error("dSATA web: already connected");

    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ < 0) throw std::runtime_error("dSATA web: socket failed");

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0)
        throw std::runtime_error("dSATA web: invalid host: " + host);

    if (::connect(sockfd_, (sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("dSATA web: connection failed to " + host);

    host_ = host;
    port_ = port;
    connected_ = true;
}

void DSataWeb::disconnect() {
    if (mounted_) unmount();
    if (sockfd_ >= 0) { ::close(sockfd_); sockfd_ = -1; }
    connected_ = false;
}

void DSataWeb::mount(const std::string& volumeName) {
    if (!connected_) throw std::runtime_error("dSATA web: not connected");
    if (mounted_)    throw std::runtime_error("dSATA web: already mounted");

    // handshake: tell server which volume to open
    sendLine("MOUNT " + volumeName);
    std::string resp = recvLine();
    if (resp != "OK")
        throw std::runtime_error("dSATA web: mount rejected: " + resp);

    mountedVolume_ = volumeName;
    mounted_ = true;
}

void DSataWeb::unmount() {
    if (!mounted_) return;
    sendLine("UNMOUNT");
    recvLine(); // consume OK
    mountedVolume_.clear();
    mounted_ = false;
}

void DSataWeb::write(const uint8_t* data, size_t size, uint64_t offset) {
    if (!mounted_) throw std::runtime_error("dSATA web: no volume mounted");

    // send command header
    std::string cmd = "WRITE " + std::to_string(offset) + " " + std::to_string(size) + "\n";
    send((const uint8_t*)cmd.data(), cmd.size());

    // send payload
    send(data, size);

    // wait for ack
    std::string resp = recvLine();
    if (resp != "OK")
        throw std::runtime_error("dSATA web: write failed: " + resp);
}

void DSataWeb::read(uint8_t* buffer, size_t size, uint64_t offset) {
    if (!mounted_) throw std::runtime_error("dSATA web: no volume mounted");

    std::string cmd = "READ " + std::to_string(offset) + " " + std::to_string(size) + "\n";
    send((const uint8_t*)cmd.data(), cmd.size());

    recv(buffer, size);
}

void DSataWeb::send(const uint8_t* data, size_t size) {
    if (!connected_) throw std::runtime_error("dSATA web: not connected");
    webThrottle_->consume(size);
    size_t sent = 0;
    while (sent < size) {
        ssize_t n = ::send(sockfd_, data + sent, size - sent, 0);
        if (n < 0) throw std::runtime_error("dSATA web: send failed");
        sent += n;
    }
}

void DSataWeb::recv(uint8_t* buffer, size_t size) {
    if (!connected_) throw std::runtime_error("dSATA web: not connected");
    webThrottle_->consume(size);
    size_t received = 0;
    while (received < size) {
        ssize_t n = ::recv(sockfd_, buffer + received, size - received, 0);
        if (n <= 0) throw std::runtime_error("dSATA web: recv failed");
        received += n;
    }
}

void DSataWeb::sendLine(const std::string& line) {
    std::string msg = line + "\n";
    send((const uint8_t*)msg.data(), msg.size());
}

std::string DSataWeb::recvLine() {
    std::string result;
    char c;
    while (true) {
        ssize_t n = ::recv(sockfd_, &c, 1, 0);
        if (n <= 0) throw std::runtime_error("dSATA web: connection lost");
        if (c == '\n') break;
        result += c;
    }
    return result;
}

bool DSataWeb::isConnected() const { return connected_; }
bool DSataWeb::isMounted() const { return mounted_; }
const std::string& DSataWeb::host() const { return host_; }
uint16_t DSataWeb::port() const { return port_; }
const std::string& DSataWeb::mountedVolume() const { return mountedVolume_; }

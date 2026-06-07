#include "../include/dsataserver.h"
#include "../include/dsata.h"
#include "../dsata/core.cpp"
#include <stdexcept>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

DSataServer::DSataServer(uint16_t port)
    : port_(port), listenfd_(-1), running_(false) {}

DSataServer::~DSataServer() {
    stop();
}

void DSataServer::start() {
    listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd_ < 0) throw std::runtime_error("dSATA server: socket failed");

    int opt = 1;
    setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port_);

    if (bind(listenfd_, (sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("dSATA server: bind failed on port " + std::to_string(port_));

    if (listen(listenfd_, 4) < 0)
        throw std::runtime_error("dSATA server: listen failed");

    running_ = true;

    while (running_) {
        int clientfd = accept(listenfd_, nullptr, nullptr);
        if (clientfd < 0) {
            if (!running_) break;
            continue;
        }
        serveClient(clientfd);
        ::close(clientfd);
    }
}

void DSataServer::stop() {
    running_ = false;
    if (listenfd_ >= 0) { ::close(listenfd_); listenfd_ = -1; }
}

bool DSataServer::isRunning() const { return running_; }
uint16_t DSataServer::port() const { return port_; }

void DSataServer::serveClient(int clientfd) {
    DSata* vol = nullptr;

    try {
        while (true) {
            std::string line = recvLine(clientfd);
            if (line.empty()) break;

            if (line.substr(0, 6) == "MOUNT ") {
                std::string volName = line.substr(6);
                if (vol) { delete vol; vol = nullptr; }
                try {
                    vol = new DSata(volName);
                    sendLine(clientfd, "OK");
                } catch (std::exception& e) {
                    sendLine(clientfd, std::string("ERR ") + e.what());
                }

            } else if (line == "UNMOUNT") {
                if (vol) { vol->flush(); delete vol; vol = nullptr; }
                sendLine(clientfd, "OK");
                break;

            } else if (line.substr(0, 6) == "WRITE ") {
                // WRITE <offset> <size>
                std::istringstream ss(line.substr(6));
                uint64_t offset; size_t size;
                ss >> offset >> size;

                std::vector<uint8_t> buf(size);
                recvRaw(clientfd, buf.data(), size);

                if (vol) {
                    vol->write(buf.data(), size, offset);
                    sendLine(clientfd, "OK");
                } else {
                    sendLine(clientfd, "ERR no volume mounted");
                }

            } else if (line.substr(0, 5) == "READ ") {
                // READ <offset> <size>
                std::istringstream ss(line.substr(5));
                uint64_t offset; size_t size;
                ss >> offset >> size;

                std::vector<uint8_t> buf(size, 0);
                if (vol) {
                    vol->read(buf.data(), size, offset);
                } // if no vol, sends zeroes — client will get ERR on next op
                sendRaw(clientfd, buf.data(), size);

            } else {
                sendLine(clientfd, "ERR unknown command");
            }
        }
    } catch (...) {
        // client disconnected or error — clean up
    }

    if (vol) { vol->flush(); delete vol; }
}

void DSataServer::sendLine(int fd, const std::string& line) {
    std::string msg = line + "\n";
    size_t sent = 0;
    while (sent < msg.size()) {
        ssize_t n = ::send(fd, msg.data() + sent, msg.size() - sent, 0);
        if (n < 0) throw std::runtime_error("dSATA server: send failed");
        sent += n;
    }
}

std::string DSataServer::recvLine(int fd) {
    std::string result;
    char c;
    while (true) {
        ssize_t n = ::recv(fd, &c, 1, 0);
        if (n <= 0) return "";
        if (c == '\n') break;
        result += c;
    }
    return result;
}

void DSataServer::sendRaw(int fd, const uint8_t* data, size_t size) {
    size_t sent = 0;
    while (sent < size) {
        ssize_t n = ::send(fd, data + sent, size - sent, 0);
        if (n < 0) throw std::runtime_error("dSATA server: send failed");
        sent += n;
    }
}

void DSataServer::recvRaw(int fd, uint8_t* buffer, size_t size) {
    size_t received = 0;
    while (received < size) {
        ssize_t n = ::recv(fd, buffer + received, size - received, 0);
        if (n <= 0) throw std::runtime_error("dSATA server: recv failed");
        received += n;
    }
}

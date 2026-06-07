#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>

#define DSATA_READ_SPEED_LIMIT  803000000ULL  // 803 MB/s
#define DSATA_WRITE_SPEED_LIMIT 557000000ULL  // 557 MB/s

class DSataCache;
class DSataStorage;
class DSataThrottle;

class DSata {
public:
    explicit DSata(const std::string& volumeName);
    ~DSata();

    void write(const uint8_t* data, size_t size, uint64_t offset);
    void read(uint8_t* buffer, size_t size, uint64_t offset);
    void flush();
    void close();

    uint64_t size() const;
    const std::string& name() const;
    bool isOpen() const;

private:
    std::string volumeName_;
    std::string volumePath_;
    bool open_;

    DSataCache*    cache_;
    DSataStorage*  storage_;
    DSataThrottle* readThrottle_;
    DSataThrottle* writeThrottle_;

    std::string resolvePath() const;
};

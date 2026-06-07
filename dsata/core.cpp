#include "../include/dsata.h"
#include "storage.cpp"
#include "cache.cpp"
#include "../src/throttle.cpp"
#include <cstdlib>
#include <stdexcept>

DSata::DSata(const std::string& volumeName)
    : volumeName_(volumeName), open_(false),
      cache_(nullptr), storage_(nullptr),
      readThrottle_(nullptr), writeThrottle_(nullptr)
{
    volumePath_ = resolvePath();
    cache_        = new DSataCache();
    storage_      = new DSataStorage(volumePath_);
    readThrottle_ = new DSataThrottle(DSATA_READ_SPEED_LIMIT);
    writeThrottle_= new DSataThrottle(DSATA_WRITE_SPEED_LIMIT);
    open_ = true;
}

DSata::~DSata() {
    if (open_) flush();
    delete cache_;
    delete storage_;
    delete readThrottle_;
    delete writeThrottle_;
}

void DSata::write(const uint8_t* data, size_t size, uint64_t offset) {
    if (!open_) throw std::runtime_error("dSATA: volume not open");
    writeThrottle_->consume(size); // memory write
    cache_->write(data, size, offset);
    writeThrottle_->consume(size); // storage write
    storage_->write(data, size, offset);
}

void DSata::read(uint8_t* buffer, size_t size, uint64_t offset) {
    if (!open_) throw std::runtime_error("dSATA: volume not open");
    readThrottle_->consume(size); // memory read
    if (!cache_->read(buffer, size, offset)) {
        readThrottle_->consume(size); // storage read
        storage_->read(buffer, size, offset);
    }
}

void DSata::flush() {
    for (auto& [block, data] : cache_->blocks()) {
        if (cache_->isDirty(block)) {
            writeThrottle_->consume(data.size()); // memory read of dirty block
            writeThrottle_->consume(data.size()); // storage write
            storage_->write(data.data(), data.size(), block * 4096ULL);
            cache_->clearDirty(block);
        }
    }
    storage_->flush();
}

void DSata::close() {
    flush();
    open_ = false;
}

uint64_t DSata::size() const { return storage_->size(); }
const std::string& DSata::name() const { return volumeName_; }
bool DSata::isOpen() const { return open_; }

std::string DSata::resolvePath() const {
    const char* home = std::getenv("DSATAHOME");
    if (!home)
        throw std::runtime_error("dSATA: $DSATAHOME not set");
    return std::string(home) + "/" + volumeName_ + ".dsata";
}


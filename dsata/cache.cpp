#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include <cstdint>
#include <cstring>

#define DSATA_CACHE_BLOCK 4096ULL  // 4KB blocks

class DSataCache {
public:
    explicit DSataCache(size_t maxBytes = 256 * 1024 * 1024)
        : maxBytes_(maxBytes), usedBytes_(0) {}

    void write(const uint8_t* data, size_t size, uint64_t offset) {
        uint64_t block = offset / DSATA_CACHE_BLOCK;
        uint64_t blockOffset = offset % DSATA_CACHE_BLOCK;

        size_t written = 0;
        while (written < size) {
            auto& entry = cache_[block];
            if (entry.empty()) {
                entry.resize(DSATA_CACHE_BLOCK, 0);
                usedBytes_ += DSATA_CACHE_BLOCK;
                lruList_.push_front(block);
                lruMap_[block] = lruList_.begin();
            } else {
                // move to front (most recently used)
                lruList_.erase(lruMap_[block]);
                lruList_.push_front(block);
                lruMap_[block] = lruList_.begin();
            }

            size_t canWrite = std::min(size - written, (size_t)(DSATA_CACHE_BLOCK - blockOffset));
            std::memcpy(entry.data() + blockOffset, data + written, canWrite);
            dirty_.insert(block);

            written += canWrite;
            block++;
            blockOffset = 0;
        }

        evict();
    }

    bool read(uint8_t* buffer, size_t size, uint64_t offset) {
        uint64_t block = offset / DSATA_CACHE_BLOCK;
        uint64_t blockOffset = offset % DSATA_CACHE_BLOCK;

        size_t read = 0;
        while (read < size) {
            auto it = cache_.find(block);
            if (it == cache_.end()) return false;

            // move to front
            lruList_.erase(lruMap_[block]);
            lruList_.push_front(block);
            lruMap_[block] = lruList_.begin();

            size_t canRead = std::min(size - read, (size_t)(DSATA_CACHE_BLOCK - blockOffset));
            std::memcpy(buffer + read, it->second.data() + blockOffset, canRead);

            read += canRead;
            block++;
            blockOffset = 0;
        }
        return true;
    }

    bool isDirty(uint64_t block) const { return dirty_.count(block) > 0; }
    void clearDirty(uint64_t block) { dirty_.erase(block); }

    std::unordered_map<uint64_t, std::vector<uint8_t>>& blocks() { return cache_; }
    std::unordered_set<uint64_t>& dirtyBlocks() { return dirty_; }

private:
    size_t maxBytes_;
    size_t usedBytes_;

    std::unordered_map<uint64_t, std::vector<uint8_t>> cache_;
    std::unordered_set<uint64_t> dirty_;

    // LRU tracking
    std::list<uint64_t> lruList_;
    std::unordered_map<uint64_t, std::list<uint64_t>::iterator> lruMap_;

    void evict() {
        // evict clean blocks from LRU end until under limit
        auto it = lruList_.end();
        while (usedBytes_ > maxBytes_ && it != lruList_.begin()) {
            --it;
            uint64_t block = *it;
            if (dirty_.count(block)) continue; // skip dirty, can't evict
            cache_.erase(block);
            lruMap_.erase(block);
            it = lruList_.erase(it);
            usedBytes_ -= DSATA_CACHE_BLOCK;
        }
    }
};

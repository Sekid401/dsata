#include <string>
#include <fstream>
#include <stdexcept>
#include <cstdint>

class DSataStorage {
public:
    explicit DSataStorage(const std::string& path) : path_(path) {
        file_.open(path_, std::ios::in | std::ios::out | std::ios::binary);
        if (!file_.is_open()) {
            file_.open(path_, std::ios::out | std::ios::binary);
            file_.close();
            file_.open(path_, std::ios::in | std::ios::out | std::ios::binary);
        }
        if (!file_.is_open())
            throw std::runtime_error("dSATA: failed to open backing store: " + path_);
    }

    ~DSataStorage() {
        if (file_.is_open()) file_.close();
    }

    void write(const uint8_t* data, size_t size, uint64_t offset) {
        file_.seekp(offset);
        file_.write(reinterpret_cast<const char*>(data), size);
        if (!file_) throw std::runtime_error("dSATA: storage write failed");
    }

    void read(uint8_t* buffer, size_t size, uint64_t offset) {
        file_.seekg(offset);
        file_.read(reinterpret_cast<char*>(buffer), size);
        if (!file_ && !file_.eof())
            throw std::runtime_error("dSATA: storage read failed");
    }

    void flush() { file_.flush(); }

    uint64_t size() {
        file_.seekg(0, std::ios::end);
        return static_cast<uint64_t>(file_.tellg());
    }

private:
    std::string path_;
    std::fstream file_;
};

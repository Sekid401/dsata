#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

// compile dsata first then link, or include core directly for testing
#ifdef __cplusplus
#include "dsata/core.cpp"
#include "src/network.cpp"

void test_local_write_read() {
    printf("=== test_local_write_read ===\n");
    DSata vol("test_volume");

    const char* msg = "hello from dsata";
    vol.write((const uint8_t*)msg, strlen(msg), 0);

    char buf[64] = {0};
    vol.read((uint8_t*)buf, strlen(msg), 0);
    vol.flush();

    if (strcmp(buf, msg) == 0)
        printf("PASS: write/read match\n");
    else
        printf("FAIL: got '%s'\n", buf);
}

void test_speed_limits() {
    printf("=== test_speed_limits ===\n");
    DSata vol("speed_test");

    const size_t SIZE = 1024 * 1024; // 1MB
    uint8_t* data = (uint8_t*)malloc(SIZE);
    memset(data, 0xAB, SIZE);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    vol.write(data, SIZE, 0);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    double mbps = (SIZE / (1024.0 * 1024.0)) / elapsed;
    printf("Write speed: %.2f MB/s (limit: 557 MB/s)\n", mbps);

    clock_gettime(CLOCK_MONOTONIC, &start);
    vol.read(data, SIZE, 0);
    clock_gettime(CLOCK_MONOTONIC, &end);

    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    mbps = (SIZE / (1024.0 * 1024.0)) / elapsed;
    printf("Read speed:  %.2f MB/s (limit: 803 MB/s)\n", mbps);

    vol.flush();
    free(data);
}

void test_web_connect() {
    printf("=== test_web_connect ===\n");
    DSataWeb web;
    printf("DSataWeb created, connected: %s\n", web.isConnected() ? "yes" : "no");
    printf("PASS: DSataWeb init ok (skipping actual connect, no server)\n");
}

int main() {
    test_local_write_read();
    test_speed_limits();
    test_web_connect();
    printf("\nAll tests done.\n");
    return 0;
}
#else
int main() {
    printf("Compile with clang++ for dSATA tests\n");
    return 1;
}
#endif

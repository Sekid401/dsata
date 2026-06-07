#include "src/server.cpp"
#include <cstdio>

int main(int argc, char* argv[]) {
    uint16_t port = 5454; // default dSATA port
    if (argc > 1) port = (uint16_t)atoi(argv[1]);

    printf("dSATA server starting on port %d\n", port);
    DSataServer srv(port);
    srv.start();
    return 0;
}

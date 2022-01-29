#include "pipe_networking.h"


int main() {
    int to_server;
    int from_server;

    from_server = client_handshake( &to_server );

    while(1) {
        char a[BUFFER_SIZE];
        fgets(a, sizeof(a), stdin);
        write(to_server, a, sizeof(a));
        read(from_server, a, sizeof(a));
        printf("%s\n", a);
    }
}

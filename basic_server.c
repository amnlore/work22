#include "pipe_networking.h"


int main() {
    int to_client;
    int from_client;

    while(1) {
        from_client = server_setup();
        int fk = fork();
        if (fk) {
            remove(WKP);
        }
        else {
            to_client = server_connect(from_client);
            char a[BUFFER_SIZE];
            int i = read(from_client, a, sizeof(a));
            if (i == 0) break;
            a[strlen(a) - 1] = '\0';
            strcat(a, "random text");
            write(to_client, a, sizeof(a));
        }
    }
}

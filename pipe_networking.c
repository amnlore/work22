#include "pipe_networking.h"

/*=========================
  server_setup
  args:
  creates the WKP (upstream) and opens it, waiting for a
  connection.
  removes the WKP once a connection has been made
  returns the file descriptor for the upstream pipe.
  =========================*/
int server_setup() {
    int b;
    int from_client;

    printf("[server] handshake: making wkp\n");
    b = mkfifo(WKP, 0600);

    from_client = open(WKP, O_RDONLY, 0);

    remove(WKP);

    printf("[server] handshake: removed wkp\n");

    return from_client;
}

/*=========================
  server_connect
  args: int from_client
  handles the subserver portion of the 3 way handshake
  returns the file descriptor for the downstream pipe.
  =========================*/
int server_connect(int from_client) {
    int to_client  = 0;
    int b;
    char buffer[HANDSHAKE_BUFFER_SIZE];

    b = read(from_client, buffer, sizeof(buffer));
    printf("[server] handshake received: -%s-\n", buffer);

    to_client = open(buffer, O_WRONLY, 0);

    srand(time(NULL));
    int r = rand() % HANDSHAKE_BUFFER_SIZE;
    sprintf(buffer, "%d", r);

    write(to_client, buffer, sizeof(buffer));

    read(from_client, buffer, sizeof(buffer));
    int ra = atoi(buffer);
    if (ra != r + 1) {
        printf("[server] handshake received bad ACK: -%s-\n", buffer);
        exit(0);
    }
    printf("[server] handshake received: -%s-\n", buffer);

    return to_client;
}

/*=========================
  server_handshake
  args: int * to_client
  Performs the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.
  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
    int b, from_client;
    char buffer[HANDSHAKE_BUFFER_SIZE];

    printf("[server] handshake: making wkp\n");
    b = mkfifo(WKP, 0600);
    if ( b == -1 ) {
      printf("mkfifo error %d: %s\n", errno, strerror(errno));
      exit(-1);
    }

    from_client = open(WKP, O_RDONLY, 0);
    remove(WKP);

    printf("[server] handshake: removed wkp\n");

    b = read(from_client, buffer, sizeof(buffer));
    printf("[server] handshake received: -%s-\n", buffer);


    *to_client = open(buffer, O_WRONLY, 0);

    srand(time(NULL));
    int r = rand() % HANDSHAKE_BUFFER_SIZE;
    sprintf(buffer, "%d", r);

    write(*to_client, buffer, sizeof(buffer));

    read(from_client, buffer, sizeof(buffer));
    int ra = atoi(buffer);
    if (ra != r+1) {
      printf("[server] handshake received bad ACK: -%s-\n", buffer);
      exit(0);
    }
    printf("[server] handshake received: -%s-\n", buffer);

    return from_client;
}


/*=========================
  client_handshake
  args: int * to_server
  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.
  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
    int from_server;
    char buffer[HANDSHAKE_BUFFER_SIZE];
    char ppname[HANDSHAKE_BUFFER_SIZE];

    printf("[client] handshake: making pp\n");
    sprintf(ppname, "%d", getpid() );
    mkfifo(ppname, 0600);

    printf("[client] handshake: connecting to wkp\n");
    *to_server = open( WKP, O_WRONLY, 0);
    if ( *to_server == -1 ) {
      printf("open error %d: %s\n", errno, strerror(errno));
      exit(1);
    }

    write(*to_server, ppname, sizeof(buffer));
    from_server = open(ppname, O_RDONLY, 0);

    read(from_server, buffer, sizeof(buffer));

    printf("[client] handshake: received -%s-\n", buffer);

    remove(ppname);
    printf("[client] handshake: removed pp\n");


    int r = atoi(buffer) + 1;
    sprintf(buffer, "%d", r);
    write(*to_server, buffer, sizeof(buffer));

    return from_server;
}

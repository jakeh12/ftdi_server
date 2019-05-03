#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "spi.h"

#define BUFFER_SIZE 65539 // 1 byte target + 2 bytes length + 65536 bytes data
#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }

int main (int argc, char *argv[]) {
  if (argc < 2) on_error("Usage: %s [port]\n", argv[0]);
  
  int port = atoi(argv[1]);
  
  // TODO: initialize ftdi spi chip

  int server_fd, client_fd, err;
  struct sockaddr_in server, client;
  char tx_buf[BUFFER_SIZE];
  char rx_buf[BUFFER_SIZE];

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) on_error("Could not create socket\n");

  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  int opt_val = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);

  err = bind(server_fd, (struct sockaddr *) &server, sizeof(server));
  if (err < 0) on_error("Could not bind socket\n");

  err = listen(server_fd, 128);
  if (err < 0) on_error("Could not listen on socket\n");

  printf("Server is listening on %d\n", port);

  while (1) {
    socklen_t client_len = sizeof(client);
    client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);

    if (client_fd < 0) on_error("Could not establish new connection\n");

    while (1) {
      int read = recv(client_fd, tx_buf, BUFFER_SIZE, 0); // receiving into tx_buf

      if (!read) break;
      if (read < 0) on_error("Client read failed\n");
      
      // TODO: process data
      
      // packet format
      // +-----+-------+-------------------------------------------------+
      // | tgt | len   | data                                            |
      // +-----+-------+-------------------------------------------------+
      // |  01 | 10 00 | 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 |
      // +-----+-------+-------------------------------------------------+
      
      uint8_t tgt = tx_buf[0];
      uint16_t len = tx_buf[2] << 8 | tx_buf[1];
      uint8_t* data = (uint8_t*) &tx_buf[3];
      
      if (len != read-3) {
        // TODO: packet error
      }
      
      memset(rx_buf, BUFFER_SIZE, sizeof(uint8_t));
      
      err = spi_shift(tgt, len, data, (uint8_t*) rx_buf);
      if (err < 0) on_error("SPI shift failed\n");
      
      // send back reply
      err = send(client_fd, rx_buf, len, 0);
      if (err < 0) on_error("Client write failed\n");
    }
  }
  
  // TODO: cleanup spi chip

  return 0;
}
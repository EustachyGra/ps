#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Użycie: %s <host> <wiadomość>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *host = argv[1];
    const char *message = argv[2];
    const char *port = "5005"; // Numer portu
    int sockfd;
    struct addrinfo hints, *res;
    char buffer[BUFFER_SIZE];

    // Konfiguracja getaddrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     // Obsługa IPv4 i IPv6
    hints.ai_socktype = SOCK_DGRAM; // Protokół UDP

    // Pobranie informacji o adresie
    if (getaddrinfo(host, port, &hints, &res) != 0) {
        perror("Błąd getaddrinfo");
        exit(EXIT_FAILURE);
    }

    // Tworzenie gniazda
    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        perror("Błąd tworzenia gniazda");
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }

    // Wysłanie wiadomości do serwera
    if (sendto(sockfd, message, strlen(message), 0, res->ai_addr, res->ai_addrlen) < 0) {
        perror("Błąd wysyłania wiadomości");
        freeaddrinfo(res);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Wiadomość wysłana do %s:%s\n", host, port);

    // Odbiór odpowiedzi
    memset(buffer, 0, BUFFER_SIZE);
    if (recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL) < 0) {
        perror("Błąd odbioru odpowiedzi");
        freeaddrinfo(res);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Odpowiedź od serwera: %s\n", buffer);

    freeaddrinfo(res);
    close(sockfd);
    return 0;
}


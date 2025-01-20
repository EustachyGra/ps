#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Użycie: %s <host>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *host = argv[1];
    const char *port = "5005"; // Numer portu
    int sockfd;
    struct addrinfo hints, *res;
    char buffer[BUFFER_SIZE];

    // Konfiguracja getaddrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     // Obsługa IPv4 i IPv6
    hints.ai_socktype = SOCK_DGRAM; // Protokół UDP
    hints.ai_flags = AI_PASSIVE;    // Użycie lokalnego adresu

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

    // Powiązanie gniazda z adresem
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("Błąd bindowania");
        freeaddrinfo(res);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(res);

    printf("Serwer UDP nasłuchuje na %s:%s\n", host, port);

    struct sockaddr_storage client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        // Odbieranie wiadomości
        if (recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len) < 0) {
            perror("Błąd odbioru");
            continue;
        }

        printf("Otrzymano wiadomość: %s\n", buffer);

        // Wysłanie odpowiedzi
        const char *response = "Wiadomość odebrana";
        if (sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&client_addr, addr_len) < 0) {
            perror("Błąd wysyłania odpowiedzi");
        }
    }

    close(sockfd);
    return 0;
}


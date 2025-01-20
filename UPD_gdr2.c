#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Użycie: %s <host>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *host = argv[1];
    const char *message = argv[2];
    const char *port = "5005"; // Port docelowy
    int sockfd;
    struct addrinfo hints, *res;

    // Konfiguracja struktury hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;     // IPv4 lub IPv6
    hints.ai_socktype = SOCK_DGRAM; // Protokół UDP

    // Rozwiązywanie nazwy hosta na adres IP
    if (getaddrinfo(host, port, &hints, &res) != 0) {
        perror("Błąd getaddrinfo");
        exit(EXIT_FAILURE);
    }

    // Tworzenie gniazda UDP
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        perror("Błąd tworzenia gniazda");
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    printf("Podaj wiadomosc: ");
    fgets(buffer,BUFFER_SIZE,stdin);
    // Wysyłanie wiadomości do serwera
    buffer[strlen(buffer)-1]='\0';
    if (sendto(sockfd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen) < 0) {
        perror("Błąd wysyłania wiadomości");
        freeaddrinfo(res);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Wysłano: %s\n", buffer);

    // Odbieranie odpowiedzi od serwera
    memset(buffer, 0, BUFFER_SIZE);
    struct sockaddr_storage server_addr;
    socklen_t addr_len = sizeof(server_addr);

    int bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, 
                                  (struct sockaddr *)&server_addr, &addr_len);
    if (bytes_received < 0) {
        perror("Błąd odbioru odpowiedzi");
    } else {
        buffer[bytes_received] = '\0'; // Null-terminator
        printf("Otrzymano od serwera: %s\n", buffer);
    }

    // Czyszczenie i zamykanie zasobów
    freeaddrinfo(res);
    close(sockfd);
    return 0;
}


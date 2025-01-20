#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>

#define BUFFER_SIZE 1024

int sockfd; // Główne gniazdo serwera

// Funkcja obsługująca sygnał SIGINT
void handle_sigint(int sig) {
    printf("\nZatrzymywanie serwera...\n");
    if (sockfd >= 0) {
        close(sockfd);
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Użycie: %s <host>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *host = argv[1];
    const char *port = "5005"; // Numer portu
    int clientfd;
    struct addrinfo hints, *res;
    char buffer[BUFFER_SIZE];

    // Rejestracja handlera dla SIGINT
    signal(SIGINT, handle_sigint);

    // Konfiguracja getaddrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     // Obsługa IPv4 i IPv6
    hints.ai_socktype = SOCK_STREAM; // Protokół TCP
    hints.ai_flags = AI_PASSIVE;    // Użycie lokalnego adresu

    // Pobranie informacji o adresie
    if (getaddrinfo(host, port, &hints, &res) != 0) {
        perror("Błąd getaddrinfo");
        exit(EXIT_FAILURE);
    }

    // Tworzenie gniazda
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
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

    // Nasłuchiwanie na gnieździe
    if (listen(sockfd, 10) < 0) {
        perror("Błąd nasłuchiwania");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Serwer TCP nasłuchuje na %s:%s\n", host, port);

    struct sockaddr_storage client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while (1) {
        // Akceptowanie połączenia
        clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);
        if (clientfd < 0) {
            perror("Błąd akceptowania połączenia");
            continue;
        }

        printf("Połączenie nawiązane z klientem.\n");

        memset(buffer, 0, BUFFER_SIZE);

        // Odbieranie danych
        int bytes_received = recv(clientfd, buffer, BUFFER_SIZE, 0);
        if (bytes_received > 0) {
            printf("Otrzymano od klienta: %s\n", buffer);

            // Odpowiedź do klienta
            const char *response = "Wiadomość odebrana. Serwer zakończy działanie.";
            if (send(clientfd, response, strlen(response), 0) < 0) {
                perror("Błąd wysyłania odpowiedzi");
            }
        } else if (bytes_received == 0) {
            printf("Klient zakończył połączenie.\n");
        } else {
            perror("Błąd odbioru");
        }

        close(clientfd);
        printf("Zamknięto połączenie z klientem.\n");

        // Zamykamy serwer po obsłużeniu pierwszego klienta
        break;
    }

    printf("Zamykam serwer...\n");
    close(sockfd);
    return 0;
}


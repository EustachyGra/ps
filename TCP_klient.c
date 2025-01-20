#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define PORT 5005
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Tworzenie gniazda
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Błąd tworzenia gniazda");
        exit(EXIT_FAILURE);
    }

    // Konfiguracja adresu serwera
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Błąd konwersji adresu IP");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Połączenie z serwerem
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Błąd połączenia");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Połączono z serwerem.\n");

    // Wysłanie wiadomości
    const char *message = "Witaj, serwerze!";
    if (send(sockfd, message, strlen(message), 0) < 0) {
        perror("Błąd wysyłania wiadomości");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Odbiór odpowiedzi
    memset(buffer, 0, BUFFER_SIZE);
    if (recv(sockfd, buffer, BUFFER_SIZE, 0) < 0) {
        perror("Błąd odbioru odpowiedzi");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Odpowiedź od serwera: %s\n", buffer);

    close(sockfd);
    return 0;
}


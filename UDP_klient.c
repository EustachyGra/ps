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
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Błąd tworzenia gniazda");
        exit(EXIT_FAILURE);
    }

    // Konfiguracja adresu serwera
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Błąd konwersji adresu IP");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Wysłanie wiadomości
    const char *message = "Witaj, serwerze!";
    if (sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Błąd wysyłania wiadomości");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Wiadomość wysłana do serwera.\n");

    // Odbiór odpowiedzi
    socklen_t addr_len = sizeof(server_addr);
    memset(buffer, 0, BUFFER_SIZE);
    if (recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len) < 0) {
        perror("Błąd odbioru odpowiedzi");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Odpowiedź od serwera: %s\n", buffer);

    close(sockfd);
    return 0;
}


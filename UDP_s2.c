#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    // Tworzenie gniazda UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Błąd tworzenia gniazda");
        exit(EXIT_FAILURE);
    }

    // Konfiguracja adresu serwera
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Nasłuch na wszystkich interfejsach
    server_addr.sin_port = htons(5005);

    // Bindowanie gniazda
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Błąd bindowania");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Serwer UDP nasłuchuje na porcie 5005\n");

    while (1) {
        // Odbieranie danych od klienta
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                                      (struct sockaddr *)&client_addr, &addr_len);
        if (bytes_received < 0) {
            perror("Błąd odbioru danych");
            continue;
        }

        printf("Otrzymano od klienta: %s\n", buffer);

        // Wysyłanie odpowiedzi do klienta
        const char *response = "Wiadomość odebrana przez serwer UDP";
        if (sendto(sockfd, response, strlen(response), 0,
                   (struct sockaddr *)&client_addr, addr_len) < 0) {
            perror("Błąd wysyłania odpowiedzi");
        }
    }

    close(sockfd);
    return 0;
}


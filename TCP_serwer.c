#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5005
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    // Tworzenie gniazda
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Błąd tworzenia gniazda");
        exit(EXIT_FAILURE);
    }

    // Konfiguracja adresu serwera
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Powiązanie gniazda z adresem
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Błąd bindowania");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Nasłuchiwanie
    if (listen(server_fd, 5) < 0) {
        perror("Błąd nasłuchiwania");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Serwer TCP nasłuchuje na porcie %d...\n", PORT);

    // Akceptowanie połączenia
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len)) < 0) {
        perror("Błąd akceptacji połączenia");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Klient połączony.\n");

    // Odbieranie wiadomości
    memset(buffer, 0, BUFFER_SIZE);
    if (recv(client_fd, buffer, BUFFER_SIZE, 0) < 0) {
        perror("Błąd odbioru");
        close(client_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Otrzymano wiadomość: %s\n", buffer);

    // Wysłanie odpowiedzi
    const char *response = "Witaj, kliencie!";
    if (send(client_fd, response, strlen(response), 0) < 0) {
        perror("Błąd wysyłania odpowiedzi");
    }

    close(client_fd);
    close(server_fd);
    return 0;
}


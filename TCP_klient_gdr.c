#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Użycie: %s <host>>\n", argv[0]);
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
    hints.ai_socktype = SOCK_STREAM; // Protokół TCP

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

    // Nawiązywanie połączenia z serwerem
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("Błąd łączenia z serwerem");
        freeaddrinfo(res);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(res);

    printf("Połączono z serwerem TCP.\n");
	
   // Wysyłanie wiadomości
	fgets(buffer,BUFFER_SIZE,stdin);
	buffer[strlen(buffer)-1]='\0';
    if (send(sockfd,buffer, strlen(buffer), 0) < 0) {
        perror("Błąd wysyłania wiadomości");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Wysłano: %s\n", buffer);
    // Odbiór odpowiedzi
    memset(buffer, 0, BUFFER_SIZE);
    int bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);
    if (bytes_received > 0) {
        printf("Otrzymano od serwera: %s\n", buffer);
    } else {
        perror("Błąd odbioru odpowiedzi");
    }

    close(sockfd);
    return 0;
}


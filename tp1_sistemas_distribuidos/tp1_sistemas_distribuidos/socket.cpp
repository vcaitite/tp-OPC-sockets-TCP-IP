/*
 * Servidor com WinSock 2
 *
 * Autor: Vítor Gabriel Reis Caitité &
 *        Joao Pedro Antunes Ferreira
 *
 * NOTA: compilado com cl.exe (compilador do Visual Studio)
 */

#include "socket.h"

void __cdecl socketServer(void)
{
    HANDLE handle;
    // Obtém handle para a saída da console
    handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle == INVALID_HANDLE_VALUE) {
        printf("Erro ao obter o handle para saida da console\n");
    }
    SetConsoleTextAttribute(handle, WHITE);
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    
    printf("\n\t-> Inicializando a biblioteca de Winsock...");
    /*  
    *    Inicializando a biblioteca de Winsock. Se a biblioteca não for inicializada
    *    o programa encerra retornando 1.
    */
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("\t\t[-] WSASturtup falhou! Erro: %d\n\n", iResult);
        return;
    }
    printf("\t\t[+] Biblioteca inicializada com sucesso!\n");


    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    printf("\t-> Resolvendo endereco e porta do servidor...");
    // Resolvendo o endereco e porta do servidor
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return;
    }printf("\t\t[+] Sucesso!\n");


    printf("\t-> Criando SOCKET para conexao...");
    // Criando Socket
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("\t\t\t[-] Socket falhou! Erro: %ld\n\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return;
    }
    printf("\t\t\t[+] Socket criado com sucesso!\n");

    printf("\t-> Setando TCP listening socket...\n\n");
    printf("\t-> bind()...");
    // Setup TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("\t\t\t\t\t\t[-] bind() falhou! Erro: %d\n\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }
    printf("\t\t\t\t\t\t[+] Sucesso!\n");

    freeaddrinfo(result);

    printf("\t-> listen()...");
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("\t\t\t\t\t\t[-] listen() falhou! Erro: %d\n\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }
    printf("\t\t\t\t\t\t[+] Sucesso!\n");

    printf("\t-> Aguardando conexoes! accept()...");
    // Aceitando um cliente socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("\t\t\t[-] accept() falhou! Erro: %d\n\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }
    printf("\t\t\t[+] Sucesso!\n");

    closesocket(ListenSocket);

    printf("\n\t[+] Servidor pronto para receber dados!!!\n\n");
    // Recebe ate o cliente encerrar a conexao
    char* sendMsg;
    do {
        //printf("\t-> Aguardando...");
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            if (!check_sequencial_number(recvbuf)) {
                SetConsoleTextAttribute(handle, HLRED);
                printf("\t\t\t\t\t[-] numero sequencial invalido\n");
                SetConsoleTextAttribute(handle, WHITE);
                closesocket(ClientSocket);
                WSACleanup();
            }
            sendMsg = socketMsgTreatment(recvbuf, recvbuflen);
            //printf("Send: %s", sendMsg);
            SetConsoleTextAttribute(handle, WHITE);
            printf("\t\t[+] Bytes recebidos: %d\n\n", iResult);
            SetConsoleTextAttribute(handle, WHITE);

            if (iResult == 8) {
                SetConsoleTextAttribute(handle, HLGREEN);
                printf("\t<- Enviando msg de posicionamento do vagao...");
                SetConsoleTextAttribute(handle, WHITE);
                iSendResult = send(ClientSocket, &sendMsg[0], (POSITION_MSG_LENGHT - 1), 0);
            }
            else if (iResult == 22) {
                SetConsoleTextAttribute(handle, HLGREEN);
                printf("\t<- Enviando msg de ACK ao cliente TCP/IP...");
                SetConsoleTextAttribute(handle, WHITE);
                iSendResult = send(ClientSocket, &sendMsg[0], (ACK_MSG_LENGHT - 1), 0);
            }
            if (iSendResult == SOCKET_ERROR) {
                SetConsoleTextAttribute(handle, HLRED);
                printf("\t\t\t[-] send() falhou! Erro: %d\n", WSAGetLastError());
                SetConsoleTextAttribute(handle, WHITE);
                closesocket(ClientSocket);
                WSACleanup();
                return;
            }
            SetConsoleTextAttribute(handle, WHITE);
            printf("\t\t[+] Sucesso! Bytes enviados: %d\n\n", iSendResult);
            SetConsoleTextAttribute(handle, WHITE);
        }
        else if (iResult == 0) {
            SetConsoleTextAttribute(handle, HLRED);
            printf("\t\t\t\t\t[-] Conexao fexada pelo cliente!\n");
            SetConsoleTextAttribute(handle, WHITE);
        }
        else {
            SetConsoleTextAttribute(handle, HLRED);
            printf("\t\t\t\t\t[-] recv() falhou! Erro: %d\n", WSAGetLastError());
            SetConsoleTextAttribute(handle, WHITE);
            closesocket(ClientSocket);
            WSACleanup();
            return;
        }

    } while (1);

    printf("\t-> Encerrando...");
    // Fexando a conexao quando terminarmos
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("\t\t\t\t\t[-] shutdown() falhou! Erro: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return;
    }
    printf("\t\t\t\t\t[+] Encerrado!\n");
    printf("\n\n\t### Bye!!! ###\n");
    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return;

}
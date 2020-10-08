#include "socket.h"
#include <thread>
 

int main() {
    HANDLE handle;
    // Obtém handle para a saída da console
    handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle == INVALID_HANDLE_VALUE) {
        printf("Erro ao obter o handle para saida da console\n");
    }
    SetConsoleTextAttribute(handle, HLRED);

    printf("\n\n\t\t\t\t Trabalho Pratico sobre OPC e Socket TCP/IP by Vitor & Joao\n");
	std::thread socket_server(socketServer);
    // Teste Git - adiciona comentário
	socket_server.join();
	return 1;
}

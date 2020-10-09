#include "socket.h"
#include "opcclient.h"
#include <thread>

int main(void)
{
	HANDLE handle;
	HANDLE handle_2;
	// Obtém handle para a saída da console
	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (handle == INVALID_HANDLE_VALUE) {
		printf("Erro ao obter o handle para saida da console\n");
	}
	SetConsoleTextAttribute(handle, HLRED);

	printf("\n\n\t\t\t\t Trabalho Pratico sobre OPC e Socket TCP/IP by Vitor & Joao\n");
	std::thread socket_server(socketServer);
	std::thread opc_client(opcClient);
	// Teste Git - adiciona comentário
	socket_server.join();
	opc_client.join();
	return 1;
}

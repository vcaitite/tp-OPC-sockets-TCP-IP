#include "socket.h"
#include "opcclient.h"
#include <thread>

/*
* Initial C++ file that start the application
* 
* Main task responsible for starting the socket server process and the opc client process on separate
* threads.
* 
* (by: João P. A. Ferreira and Vitor G. R. Caitité)
*/

int main(void)
{
	HANDLE handle;
	//HANDLE handle_2;
	// Obtém handle para a saída da console
	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (handle == INVALID_HANDLE_VALUE) {
		printf("Erro ao obter o handle para saida da console\n");
	}
	SetConsoleTextAttribute(handle, HLRED);
	
	printf("\n\n\t\t\t\t Trabalho Pratico sobre OPC e Socket TCP/IP by Vitor & Joao\n");
	// Define and start socket server and opc client processes
	std::thread socket_server(socketServer);

	Sleep(50);
	std::thread opc_client_reader(opcClient);

	socket_server.join();
	opc_client_reader.join();
	return 1;
}

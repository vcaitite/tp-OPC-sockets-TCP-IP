#pragma once
#ifndef SOCKET_H
#define SOCKET_H

/*
 * Servidor de sockets utilizando WinSock 2
 *
 * Autor: Vítor Gabriel Reis Caitité &
 *        Joao Pedro Antunes Ferreira
 *
 * NOTA: compilado com cl.exe (compilador do Visual Studio)
 */

#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "msg_treatment.h"
#include <thread>


// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "4045"


void __cdecl socketServer(void);
#endif
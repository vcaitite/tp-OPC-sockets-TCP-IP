#pragma once
#ifndef MSG_TREATMENT_H
#define MSG_TREATMENT_H


/*
* Script where the treatment of messages received via
* TCP / IP sockets are defined.
*
* Autor: Vítor Gabriel Reis Caitité &
*        Joao Pedro Antunes Ferreira
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include "global_variables.h"
#include "socket.h"

char* socketMsgTreatment(char* recvbuf, int recvbuflen);
bool check_sequencial_number(char* recvbuf);

#endif

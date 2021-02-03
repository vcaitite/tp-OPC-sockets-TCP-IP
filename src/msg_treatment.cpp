#include "msg_treatment.h"

/*
* Script where the treatment of messages received via 
* TCP / IP sockets are defined.
* 
* Autor: Vítor Gabriel Reis Caitité &
*        Joao Pedro Antunes Ferreira
*/

char* socketMsgTreatment(char* recvbuf, int recvbuflen) {
	char field_type[3];
	field_type[2] = '\0';
	memcpy(field_type, recvbuf, 2 * sizeof(char));
	int field_type_int = atoi(field_type);
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (handle == INVALID_HANDLE_VALUE) {
		printf("\n\nError getting the identifier for the console\n");
	}
	switch (field_type_int) {
	case 55:
		dataMsg[0] = '5';
		dataMsg[1] = '5';
		dataMsg[2] = '|';
		SetConsoleTextAttribute(handle, HLBLUE);
		printf("\n\t[SOCKETSERVER] -> Data request message received...\t");
		//SetConsoleTextAttribute(handle, WHITE);
		mtx_PositioningParameters.lock();
		char bufInt[6];
		snprintf(bufInt, 6, "%05d", ++countMessages);
		memcpy(&dataMsg[3], bufInt, 5);
		snprintf(bufInt, 6, "%05d", positionParameters.wagonSpeed);
		memcpy(&dataMsg[9], bufInt, 5);
		if(positionParameters.startSensorStatus > 0)
			snprintf(bufInt, 6, "%05d", 1);
		else
			snprintf(bufInt, 6, "%05d", 0);
		memcpy(&dataMsg[15], bufInt, 5);
		snprintf(bufInt, 6, "%05d", positionParameters.endSensorStatus);
		if (positionParameters.endSensorStatus > 0)
			snprintf(bufInt, 6, "%05d", 1);
		else
			snprintf(bufInt, 6, "%05d", 0);
		memcpy(&dataMsg[21], bufInt, 5);
		char bufFloat[8];
		snprintf(bufFloat, 8, "%07.2f", positionParameters.load_weight);
		memcpy(&dataMsg[27], bufFloat, 7);
		mtx_PositioningParameters.unlock();
		//printf("MSG: %s", dataMsg);
		return &dataMsg[0];
		break;
	case 99:
		while (SHOULD_WRITE == TRUE) {
			// Ensuring that every message that arrives will be written to the OPC server!
		}
		SHOULD_WRITE = true;
		mtx_LoadingParameters.lock();
		SetConsoleTextAttribute(handle, ORANGE);
		printf("\n\t[SOCKETSERVER] -> Charging parameters received...");
		//SetConsoleTextAttribute(handle, WHITE);
		// Lendo dados:
		sscanf_s(&recvbuf[9], "%5d", &loadingParameters.openTime);
		sscanf_s(&recvbuf[15], "%7f", &loadingParameters.oreQuantity);
		printf("\n\t* Opening time of silo gate: %d sec.", loadingParameters.openTime);
		printf("\n\t* Amount of ore to be loaded : %7.2f kg.\t\t", loadingParameters.oreQuantity);
		// Escrevendo dados na mensagem: 
		snprintf(bufInt, 6, "%05d", ++countMessages);
		memcpy(&ackMsg[3], bufInt, 5);
		mtx_LoadingParameters.unlock();
		return ackMsg;
		break;
	default:
		SetConsoleTextAttribute(handle, HLRED);
		printf("\tUnrecognized message!\n");
		SetConsoleTextAttribute(handle, WHITE);
		return NULL;
		break;
	}
}


bool check_sequencial_number(char* recvbuf) {
	int nSeq;
	sscanf_s(&recvbuf[3], "%5d", &nSeq);
	countMessages++;
	if (nSeq == countMessages) {
		return true;
	}
	else {
		return false;
	}
}
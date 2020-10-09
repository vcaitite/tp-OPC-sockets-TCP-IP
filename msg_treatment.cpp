#include "msg_treatment.h"

char* socketMsgTreatment(char* recvbuf, int recvbuflen) {
	char field_type[3];
	field_type[2] = '\0';
	memcpy(field_type, recvbuf, 2 * sizeof(char));
	int field_type_int = atoi(field_type);
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (handle == INVALID_HANDLE_VALUE) {
		printf("\n\nErro ao obter o handle para saida da console\n");
	}
	switch (field_type_int) {
	case 55:
		dataMsg[0] = '5';
		dataMsg[1] = '5';
		dataMsg[2] = '|';
		SetConsoleTextAttribute(handle, HLBLUE);
		printf("\t-> Mensagem de solicitacao de dados recebida!");
		SetConsoleTextAttribute(handle, WHITE);
		char bufInt[6];
		snprintf(bufInt, 6, "%05d", ++countMessages);
		memcpy(&dataMsg[3], bufInt, 5);
		snprintf(bufInt, 6, "%05d", positionParameters.wagonSpeed);
		memcpy(&dataMsg[9], bufInt, 5);
		snprintf(bufInt, 6, "%05d", positionParameters.startSensorStatus);
		memcpy(&dataMsg[15], bufInt, 5);
		snprintf(bufInt, 6, "%05d", positionParameters.endSensorStatus);
		memcpy(&dataMsg[21], bufInt, 5);
		char bufFloat[8];
		snprintf(bufFloat, 8, "%07.2f", positionParameters.load_weight);
		memcpy(&dataMsg[27], bufFloat, 7);
		//printf("MSG: %s", dataMsg);
		return &dataMsg[0];
		break;
	case 99:
		SHOULD_WRITE = true;
		SetConsoleTextAttribute(handle, ORANGE);
		printf("\t-> Parametros de carregamento recebidoss!");
		SetConsoleTextAttribute(handle, WHITE);
		// Lendo dados:
		sscanf_s(&recvbuf[9], "%5d", &loadingParameters.openTime);
		sscanf_s(&recvbuf[15], "%7f", &loadingParameters.oreQuantity);
		printf("\n\t* Tempo de abertura da comporta do silo: %d seg.", loadingParameters.openTime);
		printf("\n\t* Quantidade de minerio a carregar: %7.2f kg", loadingParameters.oreQuantity);
		// Escrevendo dados na mensagem: 
		snprintf(bufInt, 6, "%05d", ++countMessages);
		memcpy(&ackMsg[3], bufInt, 5);
		return ackMsg;
		break;
	default:
		SetConsoleTextAttribute(handle, HLRED);
		printf("\tMensagem não reconhecida!\n");
		SetConsoleTextAttribute(handle, WHITE);
		//return NULL;
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
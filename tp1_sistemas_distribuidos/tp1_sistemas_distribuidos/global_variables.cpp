#include "global_variables.h"

position_parameters_t positionParameters;
loading_parameters_t loadingParameters;
int countMessages = 0;
char dataMsg[POSITION_MSG_LENGHT] = "55|NNNNN|NNNNN|NNNNN|NNNNN|NNNN.NN";
char ackMsg[ACK_MSG_LENGHT] = "00|NNNNN";

int seq_number = 0;
#pragma once
#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include "data_types.h"
#include "msg_treatment.h"

#define POSITION_MSG_LENGHT 35
#define LOADING_PARAMETERS_MSG_LENGHT 23
#define ACK_MSG_LENGHT 9


#define WHITE   FOREGROUND_RED   | FOREGROUND_GREEN      | FOREGROUND_BLUE
#define HLGREEN FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define HLRED   FOREGROUND_RED   | FOREGROUND_INTENSITY
#define HLBLUE  FOREGROUND_BLUE  | FOREGROUND_INTENSITY
#define YELLOW  FOREGROUND_RED   | FOREGROUND_GREEN
#define ORANGE  YELLOW			 | FOREGROUND_RED
#define LBLUE   HLBLUE  | WHITE

extern int countMessages;
extern position_parameters_t positionParameters;
extern loading_parameters_t loadingParameters;
extern char dataMsg[POSITION_MSG_LENGHT];
extern char ackMsg[ACK_MSG_LENGHT];

#endif
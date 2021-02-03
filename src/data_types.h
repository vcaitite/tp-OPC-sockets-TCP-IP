#pragma once
#ifndef DATA_TYPES_H
#define DATA_TYPES_H
/*
* Script where the project data structures are defined.
* Autor: V�tor Gabriel Reis Caitit� &
*        Joao Pedro Antunes Ferreira
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*	Estrutura dedados contendo os par�metros de posicionamento do vag�o	*/
typedef struct position_parameters_s {
	int wagonSpeed;			// Velocidade do vag�o (cm/s)
	int startSensorStatus;	// Estado do sensor de in�cio de carga (Ligado/Desligao)
	int endSensorStatus;	// Estado do sensor de fim de carga	(Ligado/Desligado)
	float load_weight;		// Leitura da balan�a de carga no vag�o (kg)
}position_parameters_t;


/*	Estrutura dedados contendo os par�metros de carregamento do vag�o	*/
typedef struct loading_parameters {
	int openTime;			// Tempo de abertura da comporta do silo (seg.)
	float oreQuantity;		// Quantidade de min�rio a carregar (kg)
}loading_parameters_t;

#endif
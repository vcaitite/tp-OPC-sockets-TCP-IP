#pragma once
#ifndef DATA_TYPES_H
#define DATA_TYPES_H
/*
* Script where the project data structures are defined.
* Autor: Vítor Gabriel Reis Caitité &
*        Joao Pedro Antunes Ferreira
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*	Estrutura dedados contendo os parÂmetros de posicionamento do vagão	*/
typedef struct position_parameters_s {
	int wagonSpeed;			// Velocidade do vagão (cm/s)
	int startSensorStatus;	// Estado do sensor de início de carga (Ligado/Desligao)
	int endSensorStatus;	// Estado do sensor de fim de carga	(Ligado/Desligado)
	float load_weight;		// Leitura da balança de carga no vagão (kg)
}position_parameters_t;


/*	Estrutura dedados contendo os parÂmetros de carregamento do vagão	*/
typedef struct loading_parameters {
	int openTime;			// Tempo de abertura da comporta do silo (seg.)
	float oreQuantity;		// Quantidade de minério a carregar (kg)
}loading_parameters_t;

#endif
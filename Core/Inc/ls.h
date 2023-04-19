/*
 * ls.h
 *
 *  Created on: 11 abr. 2023
 *      Author: ferna
 */

#ifndef INC_LS_H_
#define INC_LS_H_

#define Q_ORDER         2

// Order of matrix to calculate identify
#define M_SIZE          (( 2 * Q_ORDER ) + 1)

// Max values
#define M_VALUES      200


/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

struct type_ILSdata
{
	uint32_t ts_Ms;

	void (*p_receive) (float32_t*);	        // Recibir señal de la planta

	uint32_t N;						        // Número de muestras del algoritmo (máx definido por M_VALUES)

	uint32_t i;

	float32_t buffer_E[1];			        // Error 1x1

	float32_t buffer_T[M_SIZE];			    // Buffer del vector de parametros estimados Theta (M_SIZE x 1)
	float32_t buffer_F[M_SIZE * M_VALUES];  // Buffer del vector de salidas y entradas Phi (M_SIZE x M_VALUES)

	float32_t buffer_FT[M_SIZE * M_VALUES]; // Buffer del vector traspuesto de salidas y entradas (M_VALUES x M_SIZE)

	float32_t buffer_aux0[M_SIZE * M_SIZE]; // Buffer de la matriz aux0 = Phi' Phi (M_SIZE x M_SIZE)
	float32_t buffer_aux1[M_SIZE * M_SIZE];	// Buffer de la matriz aux1 = aux0^(-1) (M_SIZE x M_SIZE)
	float32_t buffer_aux2[M_SIZE];          // Buffer de la matriz aux2 = Phi' Y (M_SIZE x 1)

	float32_t buffer_Y[M_VALUES];		// Buffer de la matriz Y (1 x M_VALUES)
	float32_t buffer_U[M_VALUES];		// Buffer de la matriz Y (1 x M_VALUES)
};

typedef struct type_ILSdata t_ILSdata;

/*=====[Prototypes (declarations) of public functions]=======================*/

// Configura la estructura de datos del Identificador
void ILS_Init (t_ILSdata* iData, uint32_t n, uint32_t ts_Ms, void (*pfR)(float32_t*));

// Ejecucion recurrente del Identificador
void ILS_Run(t_ILSdata* iData);


#endif /* INC_LS_H_ */

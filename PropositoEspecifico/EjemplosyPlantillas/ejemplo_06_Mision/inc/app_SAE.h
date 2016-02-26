/**************************************************************************
 *  Desarrollado por SUR Emprendimientos Tecnologicos S.R.L.
 *  Email: contacto@suremptec.com.ar
 *  Web: http://www.suremptec.com
 **************************************************************************
 *
 * Firmware SAE (c) 2015 CONAE
 *
 **************************************************************************
 * Esta obra se encuentra protegida por derechos de autor, queda prohibida
 * su reproducción, modificacion y/o publicacion, por cualquier medio,
 * sin autorizacion previa de CONAE.
 ***************************************************************************/

#ifndef _APP_SAE_H_
#define _APP_SAE_H_
#include "../inc/plataforma.h"

enum
{
	SAE_DISPOSITIVO_NO_INICIALIZADO,
	SAE_DISPOSITIVO_INICIALIZADO,
	SAE_DISPOSITIVO_ACELEROMETRO_ERROR,
	SAE_DISPOSITIVO_ACELEROMETRO_OK,
	SAE_DISPOSITIVO_BAROMETRO_ERROR,
	SAE_DISPOSITIVO_BAROMETRO_OK,
};

typedef struct sae_s
{
	uint8_t		estado_dispositivo;
	uint8_t		estado_acelerometro;
	uint8_t		estado_barometro;
	int			tiempo;
} sae_u;

extern sae_u sae;

#endif /* #ifndef _APP_SAE_H_ */

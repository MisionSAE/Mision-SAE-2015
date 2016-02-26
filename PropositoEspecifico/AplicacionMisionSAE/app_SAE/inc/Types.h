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

#ifndef TYPES_H_
#define TYPES_H_

#include "os.h"						/* <= operating system header */
#include "ciaaPOSIX_stdio.h"		/* <= device handler header */
#include "ciaaPOSIX_string.h"		/* <= string header */
#include "ciaak.h"					/* <= ciaa kernel header */
#include "chip.h"

typedef	unsigned char		uint8_t;
typedef	unsigned short		uint16_t;
//typedef	unsigned int		uint32_t;
typedef	unsigned long long	uint64_t;

//typedef	unsigned char		bool;
#define	true				1
#define	false				0

#endif /* TYPES_H_ */

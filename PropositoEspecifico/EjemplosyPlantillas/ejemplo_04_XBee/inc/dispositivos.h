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

#ifndef _DISPOSITIVOS_H_
#define _DISPOSITIVOS_H_
#include "os.h"
#include "ciaaPOSIX_stdio.h"
#include "ciaaPOSIX_string.h"
#include "ciaak.h"

extern int32_t teclado;
extern int32_t display;
extern int32_t uartUSB;
extern int32_t uartExpansion;
extern int32_t i2cExpansion;

#endif /* _DISPOSITIVOS_H_ */

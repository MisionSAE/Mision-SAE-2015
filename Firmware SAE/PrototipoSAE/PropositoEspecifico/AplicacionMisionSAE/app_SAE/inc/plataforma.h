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

#ifndef _PLATAFORMA_H_
#define _PLATAFORMA_H_

#include "os.h"
#include "ciaaPOSIX_stdio.h"
#include "ciaaPOSIX_string.h"
#include "ciaak.h"
#include "chip.h"
#include "../inc/dispositivos.h"
#include "../inc/XBee.h"
#include "../inc/mpu6050.h"
#include "../inc/bmp085.h"
#include "../inc/at24c512.h"
#include "../inc/definiciones.h"
#include "../inc/app_sensores.h"
#include "../inc/app_medicion.h"
#include "../inc/app_SAE.h"

#define	CANAL						(0x0c)										//! Canal de comunicaciones
#define	DIRECCION_COHETE			(0x1111)									//! Dirección origen
#define	DIRECCION_RED				(0x3332)									//! PAN ID. Ver si se usa o no
#define	DIRECCION_TERRENA			(0x2222)									//! Dirección destino (dirección origen de la estación terrena)

#endif /* #ifndef _PLATAFORMA_H_ */

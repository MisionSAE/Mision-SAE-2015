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

/**
 * @file	at24c5215_portable.c
 * @author	SUR Emprendimientos Tecnologicos S.R.L.
 * @date	Mayo, 2015
 * @brief	Driver AT24C512 - Portable.
 *
 * Funciónes portables de la biblioteca de driver para memoria EEPROM AT24C512
 *
 */

/*
 * at24c5215_portable.c
 */

/**
 * @addtogroup Memoria
 * @{
 */

/**
 * @addtogroup Portable
 * @{
 */

#include "../inc/at24c512_portable.h"

int AT24C512_I2C_Escribir(uint8_t addr, uint8_t *pBuf, uint32_t len)
{
	ciaaPOSIX_ioctl(i2cExpansion, ciaaPOSIX_IOCTL_SET_SLAVEADD, 			(void *)addr);
	ciaaPOSIX_ioctl(i2cExpansion, ciaaPOSIX_IOCTL_SET_REGISTERADDWIDTH, 	(void *)ciaaREGISTERADDWIDTH_0bits);
	ciaaPOSIX_ioctl(i2cExpansion, ciaaPOSIX_IOCTL_SET_REGISTERADD, 			(void *)0x00);
	len = ciaaPOSIX_write(i2cExpansion, pBuf, len);
	return len;
}

int AT24C512_I2C_Leer(uint8_t addr, uint8_t *pBuf_out, uint32_t len_out, uint8_t *pBuf_in, uint32_t len_in)
{
	uint32_t reg = 0UL;
	reg |=  ((uint32_t)pBuf_out[0]) << 8;
	reg |=  ((uint32_t)pBuf_out[1]) << 0;

	ciaaPOSIX_ioctl(i2cExpansion, ciaaPOSIX_IOCTL_SET_SLAVEADD, 			(void *)addr);
	ciaaPOSIX_ioctl(i2cExpansion, ciaaPOSIX_IOCTL_SET_REGISTERADDWIDTH, 	(void *)len_out);
	ciaaPOSIX_ioctl(i2cExpansion, ciaaPOSIX_IOCTL_SET_REGISTERADD, 			(void *)reg);
	len_in = ciaaPOSIX_read(i2cExpansion, pBuf_in, len_in);

	return len_in;
}

/** @}*/

/** @}*/

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


/*
 * bmp085_portable.c
 */

#include "../inc/bmp085_portable.h"

int BMP085_I2C_Escribir(uint8_t addr, uint8_t *pBuf, uint32_t len)
{
	ciaaPOSIX_ioctl(i2cExpansion, ciaaPOSIX_IOCTL_SET_SLAVEADD, 			(void *)addr);
	ciaaPOSIX_ioctl(i2cExpansion, ciaaPOSIX_IOCTL_SET_REGISTERADDWIDTH, 	(void *)ciaaPOSIX_IOCTL_REGISTERADDWIDTH_0bits);
	ciaaPOSIX_ioctl(i2cExpansion, ciaaPOSIX_IOCTL_SET_REGISTERADD, 			(void *)0x00);
	len = ciaaPOSIX_write(i2cExpansion, pBuf, len);
	return len;
}

int BMP085_I2C_Leer(uint8_t addr, uint8_t *pBuf_out, uint32_t len_out, uint8_t *pBuf_in, uint32_t len_in)
{
	uint32_t reg = 0UL;
	int i;
	for(i = 0; i < len_out; i++)
	{
		reg |=  pBuf_out[i] << (8*i);
	}

	ciaaPOSIX_ioctl(i2cExpansion, ciaaPOSIX_IOCTL_SET_SLAVEADD, 			(void *)addr);
	ciaaPOSIX_ioctl(i2cExpansion, ciaaPOSIX_IOCTL_SET_REGISTERADDWIDTH, 	(void *)len_out);
	ciaaPOSIX_ioctl(i2cExpansion, ciaaPOSIX_IOCTL_SET_REGISTERADD, 			(void *)reg);
	len_in = ciaaPOSIX_read(i2cExpansion, pBuf_in, len_in);

	return len_in;
}

int BMP085_Esperar(uint8_t tiempo)
{
	return 1;
}

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

#include "../inc/plataforma.h"
#include "../inc/app_SAE.h"

/***********************************************************************
 *
 ***********************************************************************/

void Decimal_a_Binario(memoria_dato_u * pBinario, VariablesMision * pDecimal)
{
	ciaaPOSIX_memset(pBinario, MEMORIA_DATO_RESERVADO, MEMORIA_DATO_LON);

	pBinario->nroMedicion 				= (uint16_t)pDecimal->nroMedicion;
	pBinario->presion_Pascal 			= pDecimal->presion_Pascal;
	pBinario->temperatura_Centigrados 	= pDecimal->temperatura_Centigrados;
	pBinario->aceleracion_x 			= pDecimal->aceleracion_x;
	pBinario->aceleracion_y 			= pDecimal->aceleracion_y;
	pBinario->aceleracion_z 			= pDecimal->aceleracion_z;
	pBinario->crc						= CalcularCRC(pBinario, MEMORIA_DATO_LON - 2);
	pBinario->marca						= MEMORIA_DATO_MARCA;
}

void Binario_a_Decimal(VariablesMision * pDecimal, memoria_dato_u * pBinario)
{
	pDecimal->nroMedicion 				= pBinario->nroMedicion;
	pDecimal->presion_Pascal 			= pBinario->presion_Pascal;
	pDecimal->temperatura_Centigrados 	= pBinario->temperatura_Centigrados;
	pDecimal->aceleracion_x 			= pBinario->aceleracion_x;
	pDecimal->aceleracion_y 			= pBinario->aceleracion_y;
	pDecimal->aceleracion_z 			= pBinario->aceleracion_z;
}

void ArmarTrama(char* buffer, char* trama, VariablesMision * pVariables)
{
	static char		buffer1[128];
	static char		buffer2[128];
//	int i;

	sprintf_float( buffer1, trama,		pVariables->nroMedicion );
	sprintf_float( buffer2, buffer1,	pVariables->presion_Pascal );
	sprintf_float( buffer1, buffer2,	pVariables->temperatura_Centigrados );
	sprintf_float( buffer2, buffer1,	pVariables->aceleracion_x );
	sprintf_float( buffer1, buffer2,	pVariables->aceleracion_y );
	sprintf_float( buffer2, buffer1,	pVariables->aceleracion_z );

	ciaaPOSIX_memcpy(buffer, buffer2, ciaaPOSIX_strlen(buffer2));
//	for (i = 0; i < 128; i++) buffer[i] = buffer2[i];
}

uint8_t CalcularCRC(uint8_t * pData, int len)
{
	uint8_t crc = 0;

	while(len > 0)
	{
		crc += *pData;
		pData++;
		len--;
	}

	return crc;
}

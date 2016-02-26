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
 * sprintf.c
 *
 *  Created on: 13/05/2015
 *      Author: Mirko Serra para Sur Emprendimientos Tecnológicos
 */

char* sprintf_float(char* buffer, const char* formato, float dato)
{
	int posicion_origen		= 0;
	int posicion_destino	= 0;
	int espacios			= 0;
	int poner_ceros			= 0;
	int decimales			= 6;
	int caracteres			= 0;
	int division			= 0;
	int	divisor_entero		= 0;
	int	signo				= 0;
	int necesarios			= 0;
	int variable_puesta		= 0;
	float divisor			= 0;
	float resto;
	int i;

	// Si es negativo
	if (dato < 0)
	{
		// Mostrar el signo menos
		signo = -1;
		dato = dato * -1;
	}

	do
	{
		if (!variable_puesta && (formato[posicion_origen] == '%'))
		{
			posicion_origen++;
			// Si hay un + a continuación del %
			if (formato[posicion_origen] == '+')
			{
				// Si es positivo, mostrar el signo
				// Si es negativo, ya lo mostrábamos de antes
				if (signo == 0) signo = 1;
				posicion_origen++;
			}
			// Si sigue un cero, poner ceros adelante
			if (formato[posicion_origen] == '0')
			{
				poner_ceros = 1;
				posicion_origen++;
			}
			// Si sigue un número, es la cantidad de cifras
			while (formato[posicion_origen] >= '0' && formato[posicion_origen] <= '9')
			{
				espacios *= 10;
				espacios += formato[posicion_origen] - '0';
				posicion_origen++;
			}
			// Si sigue un punto, buscamos el especificador de 			
			if (formato[posicion_origen] == '.')
			{
				decimales = 0;
				posicion_origen++;
				// Buscamos el número de decimales a mostrar
				while (formato[posicion_origen] >= '0' && formato[posicion_origen] <= '9')
				{
					decimales *= 10;
					decimales += formato[posicion_origen] - '0';
					posicion_origen++;
				}
			}
			if (formato[posicion_origen] == 'f')
			{
				variable_puesta++;
				resto = dato;
				caracteres = 1;
				while (resto >= 10)
				{
					resto /= 10;
					caracteres++;
				}

				necesarios = caracteres;
				if (signo != 0) necesarios++;
				if (decimales) necesarios += decimales + 1;

				if (signo)
				{
					if (poner_ceros) buffer[posicion_destino++] = (signo < 0) ? '-' : '+';
				}
				while (necesarios < espacios)
				{
					buffer[posicion_destino++] = poner_ceros ? '0' : ' ';
					espacios--;
				}
				if (signo)
				{
					if (!poner_ceros) buffer[posicion_destino++] = (signo < 0) ? '-' : '+';
				}

				divisor_entero = 1;
				for (i = 0; i < caracteres-1; i++)
				{
					divisor_entero *= 10;
				}
				resto = dato;
				while (divisor_entero >= 1)
				{
					division = resto / divisor_entero;
					buffer[posicion_destino++] = division + '0';
					resto = resto - (division * divisor_entero);
					divisor_entero /= 10;
				}
				if (decimales)
				{
					buffer[posicion_destino++] = '.';
					divisor = 0.1;
					for (i = 0; i < decimales; i++)
					{
						division = resto / divisor;
						buffer[posicion_destino++] = division + '0';
						resto = resto - (division * divisor);
						divisor /= 10;
					}
				}
			}
			else buffer[posicion_destino++] = formato[posicion_origen];
		}
		else buffer[posicion_destino++] = formato[posicion_origen];
	} while (formato[posicion_origen++]);

	return buffer;
}


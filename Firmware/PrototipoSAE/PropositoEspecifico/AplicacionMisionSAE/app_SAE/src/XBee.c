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

/** @file */

/*==================[Includes]===============================================*/
#include "XBee.h"
#include "Types.h"

//#define _XBEE_TX_CONECTADO

/*==================[internal data definition]===============================*/

static const char	comandoSecuenciaEscape[]		= "+++";				//! Secuencia con la que se entra en modo comandos
static const char	finDeLinea[]					= "\x0d";				//! Fin de línea, verificar si se trabaja con este

static const char	comandoAtencion[]				= "AT";
static const char	comandoPANID[]					= "ATID";
static const char	comandoDireccionPropiaAlta[]	= "ATSH";
static const char	comandoDireccionPropiaBaja[]	= "ATSL";
static const char	comandoDireccionDestinoAlta[]	= "ATDH";
static const char	comandoDireccionDestinoBaja[]	= "ATDL";
static const char	comandoGetVersionTextual[]		= "ATVL";
static const char	comandoGetFirmwareVersion[]		= "ATVR";
static const char	comandoDireccionOrigen[]		= "ATMY";
static const char	comandoSalirModoComandos[]		= "ATCN";
static const char	comandoGrabarConfiguracion[]	= "ATWR";
static const char	comandoConfigPinD[]				= "ATD";
static const char	comandoConfigPinP[]				= "ATP";
static const char	comandoConfigCanal[]			= "ATCH";
static const char	comandoConfigBaudRate[]			= "ATBD";
static const char	comandoTasaIO[]					= "ATIR";
static const char	comandoIteracionesIO[]			= "ATIT";
static const char	comandoAcceso[]					= "ATIA";
static const char	comandoVoltaje[]				= "AT\%V";
static const char	comandoPullup[]					= "ATPR";
static const char	comandoReset[]					= "ATRE";
static const char	comandoPotenciaEntrante[]		= "ATDB";

static const char	respuestaOK[]					= "OK";
static const char	respuestaError[]				= "ERROR";

static char			bufferUltimo					[XBEE_TAMANIO_LINEA];	//! Buffer de propósito general. Usualmente, último comando recibido
static int			posicionUltimo;											//! Posicion de dicho buffer en las lecturas no bloqueantes
static int			XBeeFileDescriptor				= -1;					//! Descriptor del puerto serie al que conectamos el XBee


/*==================[internal functions definition]==========================*/

/** \brief Envía un string al módulo XBee
 *
 * \param cadena Texto a enviar
 * \return true si pudo escribir
 *
 */
bool escribirCadena(const char* cadena)
{
	return (-1 != ciaaPOSIX_write(XBeeFileDescriptor, cadena, ciaaPOSIX_strlen(cadena)));
}

/** \brief Convierte un número en string hexadecimal de 2 caracteres
 *
 * \param numero Número a convertir
 * \param buffer Destino
 * \return true
 *
 */
bool toHex8(uint8_t numero, char* buffer)
{
	buffer[0] = (numero >> 4) & 0x0f;
	buffer[1] = numero & 0x0f;

	buffer[0] += (buffer[0] > 9) ? 'A'-10 : '0';
	buffer[1] += (buffer[1] > 9) ? 'A'-10 : '0';

	buffer[2] = 0;
	return true;
}

/** \brief Convierte un número en string hexadecimal de 4 caracteres
 *
 * \param numero Número a convertir
 * \param buffer Destino
 * \return true
 *
 */
bool toHex16(uint16_t numero, char* buffer)
{
	buffer[0] = (numero >> 12) & 0x0f;
	buffer[1] = (numero >> 8) & 0x0f;
	buffer[2] = (numero >> 4) & 0x0f;
	buffer[3] = numero & 0x0f;

	buffer[0] += (buffer[0] > 9) ? 'A'-10 : '0';
	buffer[1] += (buffer[1] > 9) ? 'A'-10 : '0';
	buffer[2] += (buffer[2] > 9) ? 'A'-10 : '0';
	buffer[3] += (buffer[3] > 9) ? 'A'-10 : '0';

	buffer[4] = 0;
	return true;
}

/** \brief Convierte un número en string hexadecimal
 *
 * \param numero Número a convertir
 * \param buffer Destino
 * \param suprimirCero true para que la longitud sea variable; si es false serán 8 cifras
 * \return true
 *
 */
bool toHexVariable(uint32_t numero, char* buffer, bool suprimirCero)
{
	int iteraciones;
	int posicion = 0;

	if (suprimirCero && numero == 0)
	{
		buffer[0] = '0';
		buffer[1] = 0;
		return true;
	}

	for (iteraciones = 0; iteraciones < 8; iteraciones++)
	{
		buffer[posicion] = (numero >> 28) & 0x0f;
		buffer[posicion] += (buffer[posicion] > 9) ? 'A'-10 : '0';
		if (suprimirCero && buffer[posicion] != '0')
		{
			suprimirCero = false;
		}
		if (!suprimirCero) posicion++;
		numero = numero << 4;
	}
	buffer[posicion] = 0;
	return true;
}

/** \brief Convierte un string hexadecimal en número
 *
 * \param cadena Texto a convertir en número
 * \return valor numérico
 *
 */
uint32_t fromHex(const char* cadena)
{
	uint32_t	retVal = 0;
	uint8_t		letra;
	int			puntero = 0;

	// Repetimos (se sale con un ret interno)
	while (1)
	{
		// Buscamos una letra
		letra = cadena[puntero++];
		// Si es un número, le restamos el 0
		// Y lo agregamos al valor a retornar
		if (letra >= '0' && letra <= '9')
		{
			retVal = retVal << 4;
			retVal += letra - '0';
		}
		// Si es una letra minúscula, le restamos la 'a' y le sumamos 10
		// Y lo agregamos al valor a retornar
		else if (letra >= 'a' && letra <= 'f')
		{
			retVal = retVal << 4;
			retVal += letra - 'a' + 10;
		}
		// Si es una letra mayúscula, le restamos la 'A' y le sumamos 10
		// Y lo agregamos al valor a retornar
		else if (letra >= 'A' && letra <= 'F')
		{
			retVal = retVal << 4;
			retVal += letra - 'A' + 10;
		}
		// Si es cualquier otra cosa (espacio, enter, coma, fin de línea),
		// terminamos la conversión
		else return retVal;
	}
}

/** \brief Espera por una línea de respuesta
 *
 * \return true solo si hubo respuesta y es igual a OK
 *
 */
bool esperarRespuesta()
{
	// Leemos desde el inicio
	posicionUltimo = 0;
	// Leemos una línea
	if (XBeeLeerLinea(bufferUltimo, &posicionUltimo))
	{
		// Si la leímos bien y es OK, devolvemos true
		return XBeeEsOK(bufferUltimo);
	}
	else return false;
}

/** \brief Ejecuta un comando y devuelve un valor numérico
 *
 * \return valor devuelto por el comando
 *
 */
int genericGet(const char* comando)
{
	// Si no está iniciado, volvemos
	if (XBeeFileDescriptor < 0) return 0;
	// Enviamos el comando
	escribirCadena(comando);
	// Enviamos el fin de línea
	escribirCadena(finDeLinea);
	// Nueva lectura
	posicionUltimo = 0;
	// Leemos una línea
	XBeeLeerLinea(bufferUltimo, &posicionUltimo);
	// Devolvemos el valor de respuesta
	return fromHex(bufferUltimo);
}

/** \brief Envía un comando
 *
 * \param comando Comando a enviar. Se anexa fin de línea.
 * \return true si la respuesta es OK
 *
 */
bool genericSend(const char* comando)
{
	// Si no está iniciado, volvemos
	if (XBeeFileDescriptor < 0) return 0;
	// Escribimos el comando
	escribirCadena(comando);
	// Escribimos el fin de línea
	escribirCadena(finDeLinea);
	// Devolvemos true si vino un OK
	return esperarRespuesta();
}

/** \brief Envía un comando
 *
 * \param comando Comando a enviar. Se anexa fin de línea.
 * \param valor Nuevo valor (se convierte en hexa)
 * \return true en éxito, false de lo contrario
 *
 */
bool genericSendVal(const char* comando, uint32_t valor)
{
	// Si no está iniciado, volvemos
	if (XBeeFileDescriptor < 0) return 0;
	// Enviamos el comando
	escribirCadena(comando);
	// Enviamos el valor
	toHexVariable(valor, bufferUltimo, true);
	escribirCadena(bufferUltimo);
	// Enviamos el fin de línea
	escribirCadena(finDeLinea);
	// Devolvemos true si vino un OK
	return esperarRespuesta();
}


/*==================[external functions definition]==========================*/

/** \brief Lee una línea del módulo XBee
 *
 * Lee hasta encontrar cualquier caracter menor que ' ' (en general, retorno de carro)
 *
 * \param destino Buffer donde almacenar lo leído
 * \param posicion Posicion relativa dentro del buffer, se actualiza con el tamaño parcial/total
 *
 * \return true si la línea terminó; false si debe llamarse la función nuevamente (no bloqueante)
 *
 */
bool		XBeeLeerLinea(char* destino, int* posicion)
{
	int ret;

	// Leemos
	do
	{
		// Leemos un solo byte
		ret = ciaaPOSIX_read(XBeeFileDescriptor, destino + (*posicion), 1);

		// Si no hay datos, devolvemos false
		if (ret < 1) return false;
		// Si es menor que ' ' (por ejemplo, un enter), finalizamos la cadena
		if (destino[*posicion] < ' ') destino[*posicion] = 0;
	}
	// Mientras no se termine la cadena
	while (destino[(*posicion)++]);
	// Indicamos que se leyó la línea entera
	return true;
}

/** \brief Dice si una línea equivale a OK
 *
 * \param destino Buffer de destino a comparar
 * \return true si es OK
 *
 */
bool		XBeeEsOK(const char* destino)
{
	return (ciaaPOSIX_strcmp(destino, respuestaOK) == STRING_IGUALES);
}

/** \brief Dice si una línea equivale a ERROR
 *
 * \param destino Buffer de destino a comparar
 * \return true si es ERROR
 *
 */
bool		XBeeEsERROR(const char* destino)
{
	return (ciaaPOSIX_strcmp(destino, respuestaError) == STRING_IGUALES);
}

/** \brief Inicia el sistema XBee
 *
 * \param fileDescriptor descriptor de donde se encuentra el módulo XBee conectado
 * \return true si el descriptor es válido
 *
 */
bool		XBeeIniciar(int fileDescriptor)
{
	XBeeFileDescriptor		= fileDescriptor;

	return XBeeFileDescriptor >= 0;
}

/** \brief Cierra el sistema XBee
 *
 * \return true
 *
 */
bool		XBeeCerrar()
{
	XBeeFileDescriptor		= -1;

	return true;
}

/** \brief Entra en modo comandos
 *
 * Luego de XBeeIniciar() y antes de poder enviar comandos se debe llamar a esta función varias veces
 * Se la debe llamar una vez por segundo (o más espaciado) con una variable inicializada a cero.
 * Dicha variable se va incrementando con cada llamado para cumplir los pasos de la inicialización
 * y devolveremos true cuando la inicialización se completó (y no es necesario continuar llamando a esta función).
 * XBeeSalirModoComandos() debe ser enviada luego para poder transmitir datos.
 *
 * \param contadorEtapa estado de la máquina de estados
 * \return false = llamar nuevamente dentro de un segundo. true = se entró en modo comandos, no llamar más
 *
 */
bool		XBeeEntrarModoComandos(int* contadorEtapa)
{
	switch (*contadorEtapa)
	{
	// Esperar un segundo
	case 0:
		break;

	// Enviar +++
	case 1:
		escribirCadena(comandoSecuenciaEscape);
		break;

	// Esperar otro segundo
	case 2:
		break;

	// Leer y esperar OK
	case 3:
#ifdef _XBEE_TX_CONECTADO
		if (esperarRespuesta())
		{
			return true;
		}
#else
		return true;
#endif
		break;

	case 4:
		escribirCadena(comandoAtencion);
		escribirCadena(finDeLinea);
		break;

	case 5:
#ifdef _XBEE_TX_CONECTADO
		if (esperarRespuesta())
		{
			return true;
		}
#else
		return true;
#endif
		break;

	default:
		*contadorEtapa = 0;
	}
	(*contadorEtapa)++;

	return false;
}

/** \brief Transmite datos por el XBee
 *
 * \param datos Puntero a los datos
 * \param cantidad Cantidad de datos
 * \return true en éxito
 *
 */
bool		XBeeTransmitirDatos(const char* datos, uint8_t cantidad)
{
	return (-1 != ciaaPOSIX_write(XBeeFileDescriptor, datos, cantidad));
}

/** \brief Devuelve el serie del módulo
 *
 * \return Número de serie del módulo (0 en error)
 *
 */
uint64_t	XBeeGetSerial()
{
	uint64_t	numeroDeSerie = 0;
	uint32_t	parteBaja = 0;
	uint32_t	parteAlta = 0;

	parteBaja = genericGet(comandoDireccionPropiaBaja);
	if (parteBaja && esperarRespuesta())
	{
		parteAlta = genericGet(comandoDireccionPropiaAlta);
		if (parteAlta && esperarRespuesta())
		{
			numeroDeSerie = parteAlta;
			numeroDeSerie <<= 32;
			numeroDeSerie += parteBaja;
		}
	}

	return numeroDeSerie;
}

/** \brief Setea el baud rate del módulo
 *
 * \param baudRate Nuevo baud rate, puede ser cualquier constante como XBEE_BAUD_1200, XBEE_BAUD_2400, XBEE_BAUD_4800, XBEE_BAUD_9600, XBEE_BAUD_19200, XBEE_BAUD_38400, XBEE_BAUD_57600, XBEE_BAUD_115200
 * \return true en éxito, false de lo contrario
 *
 */
bool		XBeeSetBaudRate(int baudRate)
{
	return genericSendVal(comandoConfigBaudRate, baudRate);
}

/** \brief Setea el canal de comunicaciones
 *
 * \param canal Canal a usar (desde XBEE_CANAL_MINIMO hasta XBEE_CANAL_MAXIMO)
 * \return true en éxito, false de lo contrario
 *
 */
bool		XBeeSetChannel(int canal)
{
	if ((canal < XBEE_CANAL_MINIMO) || (canal > XBEE_CANAL_MAXIMO)) return false;
	return genericSendVal(comandoConfigCanal, canal);
}

/** \brief Sale del modo comandos y vuelve al modo datos
 *
 * \return true en éxito, false de lo contrario
 *
 */
bool		XBeeSalirModoComandos()
{
	return genericSend(comandoSalirModoComandos);
}

/** \brief Devuelve la potencia del último paquete recibido
 *
 * \return valor en dBm del último paquete recibido o 0 en error
 *
 */
int			XBeeGetPotenciaRecepcion()
{
	return genericGet(comandoPotenciaEntrante) * -1;
}

/** \brief Setea el PAN ID
 *
 * \param PAN_ID nuevo pan id de la red
 * \return true en éxito, false de lo contrario
 *
 */
bool		XBeeSetPANID(uint16_t PAN_ID)
{
	return genericSendVal(comandoPANID, PAN_ID);
}

/** \brief Consulta el PAN ID actual
 *
 * \return PAN ID actual o 0 en error
 *
 */
uint16_t	XBeeGetPANID()
{
	return genericGet(comandoPANID);
}

/** \brief Setea la dirección propia de red
 *
 * \param newAddress nueva dirección a adoptar
 * \return true en éxito, false de lo contrario
 *
 */
bool		XBeeSetOwnAddress(uint16_t newAddress)
{
	return genericSendVal(comandoDireccionOrigen, newAddress);
}

/** \brief Consulta la dirección propia de red
 *
 * \return dirección propia o 0 en error
 *
 */
uint16_t	XBeeGetOwnAddress()
{
	return genericGet(comandoDireccionOrigen);
}

/** \brief Setea la dirección de destino de los datos
 *
 * \param newAddress Nueva dirección de 16 bits a la que enviar los datos
 * \return true en éxito, false de lo contrario
 *
 */
bool		XBeeSetDestinationAddress(uint16_t newAddress)
{
	return genericSendVal(comandoDireccionDestinoAlta, 0) && genericSendVal(comandoDireccionDestinoBaja, newAddress);
}

/** \brief Salva la configuración en la memoria del módulo
 *
 * \return true en éxito, false de lo contrario
 *
 */
bool		XBeeSaveConfig()
{
	return genericSend(comandoGrabarConfiguracion);
}

/** \brief Resetea el módulo
 *
 * \return true en éxito, false de lo contrario
 *
 */
bool		XBeeReset()
{
	return genericSend(comandoReset);
}

/** \brief Devuelve la dirección de destino del módulo
 *
 * \return true en éxito, false de lo contrario
 *
 */
uint64_t	XBeeGetDestinationAddress()
{
	uint64_t	DireccionDestino = 0;
	uint32_t	parteBaja = 0;
	uint32_t	parteAlta = 0;

	parteBaja = genericGet(comandoDireccionDestinoBaja);
	if (parteBaja && esperarRespuesta())
	{
		parteAlta = genericGet(comandoDireccionDestinoAlta);
		if (parteAlta && esperarRespuesta())
		{
			DireccionDestino = (parteAlta << 32) + parteBaja;
		}
	}

	return DireccionDestino;
}

/** \brief Consulta la versión del dispositivo
 *
 * \return puntero al buffer con la info del módulo
 *
 */
const char*	XBeeGetVersionTextual()
{
	int posicionAnterior;
	const char* texto;

	if (XBeeFileDescriptor < 0) return 0;

	escribirCadena(comandoGetVersionTextual);
	escribirCadena(finDeLinea);

	posicionUltimo = 0;
	posicionAnterior = 0;
	while (XBeeLeerLinea(bufferUltimo, &posicionUltimo))
	{
		texto = bufferUltimo + posicionAnterior;
		if (XBeeEsERROR(bufferUltimo + posicionAnterior))
		{
			return bufferUltimo;
		}
		if (XBeeEsOK(bufferUltimo + posicionAnterior))
		{
			return bufferUltimo;
		}
		bufferUltimo[posicionUltimo++] = '\x0d';
		posicionAnterior = posicionUltimo;
	}
	return bufferUltimo;
}

/** \brief Consulta la versión del firmware
 *
 * \return Versión del firmware o 0 en error
 *
 */
uint16_t	XBeeGetFirmwareVersion()
{
	return genericGet(comandoGetFirmwareVersion);
}

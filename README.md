# Mision-SAE-2015

En este repositorio se presenta y deja a disposici�n de los interesados material generado en la edici�n 2015 del Proyecto Misi�n SAE. En el mismo se podr� encontrar el firmware y hardware desarrollados para el vuelo del prototipo del dispositivo SAE, as� como plantillas y ejemplos de trabajo para la programaci�n de los dispositivos.

<h2>Estructura del repositorio</h2>

* **Prototipo SAE:** C�digo fuente y dise�o de hardware utilizados en la construcci�n del prototipo del dispositivo SAE.

	* **_Firmware:_** C�digo fuente del firmware del prototipo SAE y ejemplos de desarrollo.

		* _PrototipoSAE:_ Firmware utilizado en los lanzamientos de validaci�n del prototipo SAE
		
			* PropositoGeneral: Firmware de prop�sito general del prototipo SAE, conformado por la versi�n 0.6.1 del firmware del Proyecto CIAA (https://github.com/ciaa/Firmware)

			* PropositoEspecifico: C�digo fuente de las aplicaciones necesarias para llevar adelante la misi�n primar�a de la Misi�n SAE. Contiene biblioteca para el uso del transmisor XBee, la memoria externa y los sensores de presi�n, aceleraci�n y temperatura.

		* _MaterialdeTrabajo:_ Plantilla y ejemplos de desarrollo brindados a los participantes de la edici�n 2015 del proyecto Misi�n SAE.

	* **_Hardware:_** Bibliotecas, esquem�ticos y PCB para el dise�o de la placa de integraci�n o poncho que se utiliz� como interfaz entre la placa EDU-CIAA y los componentes del Kit SAE. Para su dise�o se utiliz� la versi�n (2013-07-07 BZR 4022)-stable de KiCad.
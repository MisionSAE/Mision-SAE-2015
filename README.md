# Mision-SAE-2015

En este repositorio se presenta y deja a disposición de los interesados material generado en la edición 2015 del Proyecto Misión SAE. En el mismo se podrá encontrar el firmware y hardware desarrollados para el vuelo del prototipo del dispositivo SAE, así como plantillas y ejemplos de trabajo para la programación de los dispositivos.

<h2>Estructura del repositorio</h2>

* **Prototipo SAE:** Código fuente y diseño de hardware utilizados en la construcción del prototipo del dispositivo SAE.

	* **_Firmware:_** Código fuente del firmware del prototipo SAE y ejemplos de desarrollo.

		* _PrototipoSAE:_ Firmware utilizado en los lanzamientos de validación del prototipo SAE
		
			* PropositoGeneral: Firmware de propósito general del prototipo SAE, conformado por la versión 0.6.1 del firmware del Proyecto CIAA (https://github.com/ciaa/Firmware)

			* PropositoEspecifico: Código fuente de las aplicaciones necesarias para llevar adelante la misión primaría de la Misión SAE. Contiene biblioteca para el uso del transmisor XBee, la memoria externa y los sensores de presión, aceleración y temperatura.

		* _MaterialdeTrabajo:_ Plantilla y ejemplos de desarrollo brindados a los participantes de la edición 2015 del proyecto Misión SAE.

	* **_Hardware:_** Bibliotecas, esquemáticos y PCB para el diseño de la placa de integración o poncho que se utilizó como interfaz entre la placa EDU-CIAA y los componentes del Kit SAE. Para su diseño se utilizó la versión (2013-07-07 BZR 4022)-stable de KiCad.
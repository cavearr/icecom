# Log de investigación sobre la librería libusb


## Ejercicio 1 - Compilación test

gcc  testlibusb.c `pkg-config --libs --cflags libusb-1.0` -o testlibusb

Se ha modificado el testlibusb.c incluido  en la distro oficial, simplemente modificando el archivo de cabecera libusb.h de comillas a <>

El ejemplo funciona perfectamente detectando la alhambra y resto de dispositivos usb, obteniendo en mi caso:

```
Dev (bus 2, device 1): 1D6B - 0003
Dev (bus 1, device 8): 04F2 - B604
Dev (bus 1, device 7): 0BDA - B812
Dev (bus 1, device 9): 0BDA - B023
Dev (bus 1, device 12): Mareldem - Alhambra II v1.0A - B06-158
Dev (bus 1, device 1): 1D6B - 0002
```


## Ejercicio 2 - Hotplug con libusb

Al conectar y desconectar la alhambra se muestra el mensaje



## Ejercicio 3 - libFTDI

Conexión serie con la UART de la FTDI usando libftdi

gcc  test3.c  -I /home/carl/workspace/github/icecom/libs/libftdi -lftdi1 `pkg-config --libs --cflags libusb-1.0` -o test3


## Ejercicio 4 - Detección de FTDIs

gcc  test4.c  -I /home/carl/workspace/github/icecom/libs/libftdi -lftdi1 `pkg-config --libs --cflags libusb-1.0` -o test4

## Ejercicio 5 - Stream desde FTDI

gcc  test5.c  -I/home/carl/workspace/github/icecom/libs/libftdi/include/libftdi1/ -lftdi1 `pkg-config --libs --cflags libusb-1.0` -o test5

El ejemplo utiliza el modo de "stream" para alcanzar velocidadades de 60Mhz, hace falta modificar la eeprom de la FTDI, se adjunta en los documentos una nota de aplicación. 

El ejemplo compila y se ejecuta pero dejamos este modo para futuras investigaciones (muy interesante para establecer un canal de alta velocidad).

## Ejercicio 6 - serial test

gcc  test6.c  -I/home/carl/workspace/github/icecom/libs/libftdi/include/libftdi1/ -lftdi1 `pkg-config --libs --cflags libusb-1.0` -o test6D

Ejecutamos como ./test6 -i 2   

2 es la uart a usar en nuestro caso con el módulo de la colección jedi el 2, hay que probar si en otros equipos y alhambras es la 2 también.



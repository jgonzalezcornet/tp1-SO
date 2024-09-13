# Sistemas Operativos - Trabajo Práctico 1

El presente trabajo consiste en un sistema que distribuye el cómputo del hash md5 de múltiples archivos entre varios procesos.

## Requisitos:

Para utilizar el sistema, se debe contar con Docker instalado y utilizar la imagen provista por la cátedra: ```agodio/itba-so-multi-platform:3.0```

## Instalación:

Clonar el repositorio:

- HTTPS:
  ```sh
  git clone https://github.com/jgonzalezcornet/tp1-SO.git
  ```
- SSH:
  ```sh
  git clone git@github.com:jgonzalezcornet/tp1-SO.git
  ```

Instalar la imagen de docker provista:
 ```sh
docker pull agodio/itba-so-multi-platform:3.0
  ```

Luego, para compilar el código se debe inicializar el contenedor de docker y ejecutar:
``` sh 
make
```


## Instrucciones:

Una vez compilado el código, el sistema ofrece una serie de opciones de ejecución. Si se corre únicamente el proceso app junto con una serie de archivos, se obtiene el resultado de los hash en un archivo result.txt. Para esto, se debe ejecutar:
``` sh 
./app <NOMBRE DE LOS ARCHIVOS ...>
```
Mientras que se ejecuta este proceso, se puede a su vez correr la vista para ir obteniendo los resultados a medida que se generan los hash. Para esto, se debe correr:
``` sh 
./view
```
Si se quiere ejecutar ambos proceos a la vez, se los puede conectar mediante un pipe de la siguiente forma:
``` sh 
./app <NOMBRE DE LOS ARCHIVOS ...> | ./view
```
Es posible a su vez correr el proceso esclavo, encargado de generar los hash md5. Para esto se debe ejecutar el siguiente comando e ingresar por la terminal los nombres de los archivos a ser procesados. Se termina la ejecución cuando recibe un ```EOF``` o ```CTRL + D```.
``` sh 
./slave
```

## Integrantes:

Josefina Gonzalez Cornet (64550) - jgonzalezcornet@itba.edu.ar

Ignacio Pedemonte Berthoud (64908) - ipedemonteberthoud@itba.edu.ar

Agustín Tomás Romero (64268) - agromero@itba.edu.ar

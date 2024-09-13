# Sistemas Operativos - Trabajo Práctico 1

El presente trabajo consiste en un programa que permite el dibujo y manipulamiento de figuras al estilo Paint. Algunas de estas funcionalidades disponibles son:

- <b>Selección Múltiple, Agrupar y Desagrupar Figuras</b>: Permite dibujar un recuadro para seleccionar todas las figuras contenidas en él, moverlas y agruparlas o desagruparlas.

- <b>Sombra, Gradiente y Biselado</b>: Para la/s figura/s y/o grupos de figuras seleccionados, permite aplicar cualquiera de estos tres efectos; sombra, gradiente o biselado.

- <b>Girar, Voltear y Escalar</b>: Permite realizar dichas manipulaciones a una selección de figuras o grupo de figuras (múltiple o simple).

- <b>Etiquetas de figuras (tags)</b>: Permite asignar etiquetas (tags) a una figura o grupo de figuras, para luego realizar una visualización de todas o las que tengan una etiqueta especificada.


<details>
  <summary>Contenidos</summary>
  <ol>
    <li><a href="#instalación">Instalación</a></li>
    <li><a href="#instrucciones">Instrucciones</a></li>
    <li><a href="#integrantes">Integrantes</a></li>
  </ol>
</details>

## Instalación:

Clonar el repositorio:

- HTTPS:
  ```sh
  git clone https://github.com/cgarros33/FINAL-POO-2Q2023.git
  ```
- SSH:
  ```sh
  git clone git@github.com:cgarros33/FINAL-POO-2Q2023.git
  ```

Luego, compilar y correr el programa desde el IDE elegido.

## Instrucciones:

Una vez ejecutado el código, se despliega el canvas vacío con todas las funcionalidades.

### Dibujo de Figuras:

Para el dibujo de figuras, se debe seleccionar en la barra izquierda la deseada y luego hacer click en el canvas y arrastrar para determinar el tamaño.

- En el caso del rectángulo o cuadrado se debe dibujar comenzando por el punto superior izquierdo hasta el punto inferior derecho. 
- Para la elipse se debe dibujar un rectángulo al igual que en el caso anterior que contendrá la elipse.
- Para el círculo se debe dibujar una linea desde el punto central en dirección derecha y hacia abajo que representa el radio asignado.

En el inferior de la barra izquierda, se encuentran los colores posibles para el dibujo de figuras.

Por otro lado, en caso de querer borrar figuras, se debe simplemente seleccionarla/s y apretar el botón de "Borrar".


### Selección Múltiple, Agrupado y Desagrupado:

Para la selección múltiple, se debe apretar el botón de "Selección" y luego dibujar un rectángulo al igual que con la figura. Todas las que se encuentren <b>completamente</b> dentro de este, serán las seleccionadas.

Luego, el botón de "Agrupar" permite crear un grupo imaginario de figuras con las que se encuentren seleccionadas. La agrupación se puede realizar tanto entre figuras como entre grupos de figuras o ambas.

En caso de querer desarmar un grupo, se debe seleccionar el grupo y utilizar el botón de "Desagrupar", obteniendo así todas las figuras por separado.

### Efectos y Manipulación:

Para manipular una o más figuras, en la barra izquierda se encuentran 5 botones que realizan las siguientes acciones:

- GirarD: gira la figura noventa grados.
- VoltearH: Voltea la figura horizontalmente.
- VoltearV: Voltea la figura verticalmente.
- Escala+: Incrementa el tamaño de la figura.
- Escala-: Decrmenta el tamaño de la figura.

Por otro lado, en la barra superior se encuentra una serie de efectos aplicables a las figuras, para las cuales se puede seleccionar uno o más. Estos consisten en gradiente, sombra y biselado. 

### Etiquetas:

En la barra izquierda se encuentra un cuadro de texto que permite escribir etiquetas que luego se guardan con el botón "Guardar" para una o más figuras. Los "tags" se separan según los espacios o por las distintas líneas.

En caso de tener dos figuras con distintos tags y agruparlas, los mismos se fusionan y el grupo pasa a tener todos los tags. Lo mismo entre grupos de figuras.

Luego, en la barra inferior se encuentra la opción para mostrar todas las figuras o únicamente las que contienen el tag ingresado en el cuadro de texto encontrado en la misma.

## Integrantes:

Josefina Gonzalez Cornet (64550) - jgonzalezcornet@itba.edu.ar

Ignacio Pedemonte Berthoud (64908) - ipedemonteberthoud@itba.edu.ar

Agustín Tomás Romero (64268) - agromero@itba.edu.ar

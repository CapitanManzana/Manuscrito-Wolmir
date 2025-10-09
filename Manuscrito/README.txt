# Plantilla para proyectos con SDL en Visual Studio

Instrucciones
-------------

* Si quieres añadir una nueva biblioteca, copia su directorio en el directorio libs de la solución y añade los nuevos elementos al archivo ProyectoSDL.props: IncludePath, LibraryPath, AdditionalDependencies y LocalDebuggerEnvironment.

* Los proyectos se deberían ejecutar sin problemas desde Visual Studio, pero los programas compilados que quedan en la carpeta bin (*Debug.exe y *Release.exe) al ejecutarse directamente mostrarán un mensaje de error indicando que la biblioteca SDL3.dll u otras no están disponibles. Esto se puede evitar copiando los archivos .dll en SDL3* a la carpeta bin del proyecto.

* El proyecto tiene tres configuraciones: Debug, Sanitize y Release. Las dos primeras compilan el programa para depuración y son las que conviene usar durante el desarrollo. La diferencia entre ambas es que Debug comprueba fugas de memoria con checkML.h mientras que Sanitize comprueba errores de acceso a memoria con AddressSanitizer (ambos herramientas son incompatibles). La configuración Release compila una versión optimizada para su uso corriente.

* Si quieres crear nuevos proyectos en la misma solución recuerda añadir la página de propiedades ProyectosSDL.props al nuevo proyecto: en el menú Ver -> Otras ventanas -> Administrador de propiedades -> Agregar hoja de propiedades existente, busca el archivo ProyectosSDL.props para incorporarlo. Sin hacer esto es posible que el compilador no encuentre la biblioteca SDL o se produzcan otros errores de compilación.

Bibliotecas
-----------

* SDL3 3.2.24
* SDL3_image 3.2.4

Más información sobre las bibliotecas en https://www.libsdl.org/.

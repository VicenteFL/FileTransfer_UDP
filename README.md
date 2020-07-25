# FileTransfer_UDP
_File transfer system between two machines_

## Compilado 🔧

El sistema tiene dos archivos .c alojados cada uno en una carpeta distinta. En cada carpeta hay un makefile, por lo que lo único que hay que hacer tanto para compilar cliente como servidor es ejecutar en consola el comando make en las carpetas correspondientes, /Server y /Cliente.
   
      cd Server
      make
      
      cd Cliente
      make


## Ejecución 🚀

Una vez compilados, en la carpeta Server ejecutaremos el archivo server que se ha creado escribiendo ./server. Para el cliente lo mismo, en la carpeta Client ejecutaremos el archivo client que se ha creado escribiendo ./client.

      ./server
      
      ./client

Ahora, el servidor mostrará información dependiendo de las operaciones que vaya realizando, pero toda la acción del usuario se realiza mediante la consola del cliente. Éste nos pedirá un comando para ejecutar una acción.

* Con el comando **list**, el cliente mostrará por consola la lista de archivos dentro del directorio Server, enviada por el servidor. 
      
* Con el comando **download**, el cliente nos pedirá un nombre de archivo para descargarlo del servidor. Si el archivo introducido existe, el servidor enviará dicho archivo al cliente, que lo almacenará en su directorio con el mismo nombre, además de mostrarlo por pantalla.
      
* Con el comando **upload**, el cliente nos pedirá también un nombre de archivo para alojarlo en el servidor. Si el archivo introducido existe en el cliente, se enviará al servidor y aparecerá un mensaje informando de la finalización de la tarea. 
      
* Con el comando **exit**, el cliente se cerrará.

Una vez completada una acción, tanto de forma exitosa como fallida, el cliente volverá a solicitar otra acción al usuario automáticamente, listando los posibles comandos.

## Autor ✒️

* **Vicente Fernández Lledó** - [VicenteFL](https://github.com/VicenteFL)

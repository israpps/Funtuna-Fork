


## CFG.ELF se congela cuando intento asignar una app a `mass:/` 

   hay dos respuestas, dependiendo de su respuesta a esta pregunta: `Descargó funtuna después de 28/1/2021?`
   - `si`: __inicia un nuevo [reporte](https://github.com/israpps/Funtuna-Fork/issues)__
   - `no`: actualiza funtuna desde [aqui](https://github.com/israpps/Funtuna-Fork/releases), este error se resolvió en el commit [71b2dd8](https://github.com/israpps/Funtuna-Fork/commit/b9e3d8f4ff48df03c371a23702f78e8c52444e97)

***
## Tenía otro OPL antes de instalar Funtuna, y ahora el OPL nuevo se cangela 

Probablemente es un conflicto con el archivo de configuración.

borra el archivo de configuracion y ejecuta el nuevo OPL para recrear el archivo

***

## el icono invisible desapareció / no es el primero del navegador


algo (probablemente **vos**, si es que tocaste algo en la carpeta `BXEXEC-FUNTUNA`) afectó la fecha de la carpeta de fortuna

esta es la solución:
 1. ejecuta [uLaunchELF 4.43x_ISR](https://github.com/israpps/Funtuna-Fork/blob/main/__Release/ULE_psu.ELF) (incluido en el paquete de funtuna como `ULE_psu.ELF`)
 2. entre a filebrowser
 3. entre en la memory card (`mc0:/` para slot 1 y `mc1:/` para slot 2)
 4. seleccione la carpeta `BXEXEC-FUNTUNA`, presione R1 y elija la opción `Fortuna Date`.
 5. mire si la fecha de la carpeta `BXEXEC-FUNTUNA` cambió a `2099`





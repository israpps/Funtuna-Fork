---
layout: default
title: Como Instalar FunTuna Fork
description: ESPAÑOL
---

1  - copiar todos los archivos a una USB __(formato FAT32)__

2  - lanzar uLaunchELF __(Ver nota 1)__ desde la consola.

3  - en uLaunchELF dirijase a _"File Browser"_, luego a _"mass:/"_, dirijase a donde colocó los archivos incluidos junto a este y elija uno de los paquetes __(las carpetas que empiezan con '!')__ __(funtuna basic se recomienda para principiantes, funtuna-clean tiene los archivos Basicos + [uLaunchELF 4.43x_isr](https://github.com/israpps/wLaunchELF_ISR) )__

4  - una vez elegido el paquete entre a la carpeta del mismo, presione ![Cuadrado](https://github.com/israpps/Funtuna-Fork/tree/gh-pages/PAD/SQUARE.PNG "cuadrado") , luego presione R1, luego en copy __(ver nota 2)__

5  - salga de la USB presionando ![Triangulo](https://israpps.github.io/Funtuna-Fork/logos%20%26%20others/PAD/TRIANGLE.PNG) e ingrese en _mc0:/_

6  - una vez alli presione __R1__ y luego en __paste__

7  - regrese a la USB dirijase a donde colocó los archivos incluidos junto a este

8  - situese sobre el archivo _"FUNTUNA-LAUNCHER.PSU"_, presione R1 y luego en copy __(ver nota 2)__

9  - dirijase a la mc0 y presione __R1__, luego __PSUPaste__. si todo salió bien deberia aparecer una carpeta llamada "BXEXEC-FUNTUNA". __(Ver nota 3)__

10 - si en la MC0 hay una carpeta llamada _"FORTUNA"_ __eliminela inmediatamente__ situandose sobre ella, presionando R1, luego en delete

11 - si todo salio bien la "instalacion" ya está completa








*******************************************************************************************************************************************************
Notas | _ 
---------|-------------------------
**1** | Se recomienda utilizar el [ulaunchelf incluido en este paquete](https://github.com/israpps/Funtuna-Fork/blob/main/__Release/ULE_psu.ELF) (ULE_psu.ELF), se recomienda utilizarlo para evitar errores al realizar el paso 9
**2** | si lo hizo bien debería aparecer un cuadrado verde en la esquina superior derecha de la pantalla
**3** | NUNCA TOQUE ESTA CARPETA, tras crearla asegurese de que la fecha indicada para esa carpeta sea 2099

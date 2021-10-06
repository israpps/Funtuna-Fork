FUNTUNA-FORK:
Moded by: | modeado por:
Matías Israelson
https://github.com/israpps


WIKI:
https://github.com/israpps/Funtuna-Fork/wiki

LICENSE: | LICENCIA:
https://github.com/israpps/Funtuna-Fork/blob/main/LICENSE.MD

DOWNLOADS: | DESCARGAS:
https://github.com/israpps/Funtuna-Fork/releases

PACKAGE CONTENTS: | CONTENIDO DE LOS PAQUETES:
https://github.com/israpps/Funtuna-Fork#current-package-contents

REPORT ISSUES: | REPORTAR BUGS:
https://github.com/israpps/Funtuna-Fork/issues

DISCUSIONS: | DISCUCIONES:
https://github.com/israpps/Funtuna-Fork/discussions


 Differences compared to original FunTuna:

- Hacked icon (Opentuna/fortuna) is stored on an independent Folder to avoid timestamp changes when updating software
- Every OSDSYS element is customizable (original FunTuna had top and bottom delimiters hardcoded)
- Fixed bugs on FreeMcBoot
- changed FreeMcBoot settings filename to keep FunTuna settings separated from FreeMcBoot settings on usb
- FreeMcBoot and configurator can load USB drivers from `SYS-CONF` if not found on `BOOT`
- Replaced fortuna with latest Opentuna
- bundled with wLaunchELF mod  capable of fixing fortuna/Opentuna icon date (quite stable and 101kb smaller than the original one)
- Bundled with latest OPL release 
- Installer based on Opentuna installer
- FreeMcBoot and configurator are slightly smaller
- Automatically updated if uLaunchELF or OPL are updated

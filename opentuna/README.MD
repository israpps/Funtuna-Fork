# OPENTUNA PAYLOAD

modified versions of [opentuna source code](https://github.com/parrado/opentuna-payload) will reside here as a backup

when this opentuna variant reachs stability it will be used to update [FUNTUNA-LAUNCHER.PSU](https://github.com/israpps/Funtuna-Fork/blob/main/__Release/FUNTUNA-LAUNCHER.psu)



by now i'm planning to make two variants:
 + one without launch keys, just `mc?:/BOOT/BOOT.ELF`

 + one with the following launch-keys:
		- circle: `mc?:/APPS/OPNPS2LD.ELF`
		- cross: `mc?:/BOOT/ULE.ELF`
		- none: `mc?:/BOOT/BOOT.ELF`
		- if app not found: `Reset browser`

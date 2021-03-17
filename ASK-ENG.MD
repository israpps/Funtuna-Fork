

##  CFG.ELF Freezes when I try to Configure a new app from `mass:/` 

   there are two possible answers for this question based on another question: `did you download Funtuna Fork After 28/1/2021?`
   - if `yes`: __start a [new issue](https://github.com/israpps/Funtuna-Fork/issues)__
   - if `no`: update funtuna from [here](https://github.com/israpps/Funtuna-Fork/releases), this was solved on the commit [71b2dd8](https://github.com/israpps/Funtuna-Fork/commit/b9e3d8f4ff48df03c371a23702f78e8c52444e97)

***
## I had another OPL before installing Funtuna, and now the new OPL Freezes 

probably there's a conflict with the previous config file.

delete the OPL config file and re-launch the new OPL to re-create it

***

## the invisible icon disapeared / isn't the first one anymore

something (probably **you**, if you touched the `BXEXEC-FUNTUNA` folder) affected the Fortuna Folder timestamp
to solve this:
 1. launch uLaunchELF [4.43a_ISR](https://github.com/israpps/wLaunchELF_ISR/releases/tag/rev1) (bundled at the funtuna Package as `ULE_psu.ELF`)
 2. go to filebrowser
 3. go to the memory card `mc0:/` for slot 1 and `mc1:/` for slot 2
 4. select the `BXEXEC-FUNTUNA` folder, press R1 and then select the  `Fortuna Date` item.
 5. check if the date of `BXEXEC-FUNTUNA` changed to 2099





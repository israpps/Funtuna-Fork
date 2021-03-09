

all: BOOT.ELF CFG.ELF
	
	
BOOT.ELF:
	
	$(MAKE) -C ../FreeMcBoot/ 
	@echo "----Compilation Finished"
	@echo "Retrieving $@ "
	cp ../FreeMcBoot/$@ Results/$@
	
CFG.ELF:
	
	$(MAKE) -C ../FreeMcBoot-Configurator/  compressed
	@echo "----Compilation Finished"
	@echo "Retrieving $@ "
	cp ../FreeMcBoot-Configurator/$@ Results/$@

clean:
	@$(MAKE) -C ../FreeMcBoot/ clean
	rm -f Results/BOOT.ELF

	@$(MAKE) -C ../FreeMcBoot-Configurator/ clean 
	rm -f Results/CFG.ELF

Deploy-bin: BOOT.ELF CFG.ELF
        rm -f ../__Binaries/BOOT.ELF
        rm -f ../__Binaries/CFG.ELF
        cp result/BOOT.ELF ../__Binaries/
        cp result/CFG ELF ../__Binaries/

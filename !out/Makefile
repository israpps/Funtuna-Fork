

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


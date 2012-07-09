SCH_NAME := encoder-interface
$(SCH_NAME).pcb $(SCH_NAME).net $(SCH_NAME).cmd: ../$(SCH_NAME).sch
	gsch2pcb --elements-dir ../footprints -o $(SCH_NAME) ../$(SCH_NAME).sch

clean:
	rm -f $(SCH_NAME).pcb $(SCH_NAME).net $(SCH_NAME).cmd


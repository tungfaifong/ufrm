PROCESSES = gateway gateway_mgr # cgisrv dbsrv gamesrv gateway proxy

TARGET = SHELL

all: $(TARGET)
release: $(TARGET)

SHELL:
	cd ./common && make $(MAKECMDGOALS); if (($$? != 0)); then echo "make common error"; exit 1; fi
	@for dir in $(PROCESSES);\
	do\
		$(MAKE) -C $$dir $(MAKECMDGOALS) ;\
		if [ "$$?" != "0" ]; then\
			echo "$$dir make error"; \
			exit 1;\
		fi;\
	done

clean:
	@for dir in $(PROCESSES); do( $(MAKE) -C $$dir clean ) done
PROCESSES = gateway # cgisrv dbsrv gamesrv gateway gateway_mgr proxy

TARGET = SHELL

all: $(TARGET)
release: $(TARGET)

SHELL:
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
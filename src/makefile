PROCESSES = dbsrv gamesrv gateway lbsrv proxy # iosrv 

TARGET = SHELL

all: $(TARGET)
release: $(TARGET)
debug: $(TARGET)

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

install:
	@for dir in $(PROCESSES); do( $(MAKE) -C $$dir $(MAKECMDGOALS) ) done

clean:
	cd ./common && make $(MAKECMDGOALS);
	@for dir in $(PROCESSES); do( $(MAKE) -C $$dir $(MAKECMDGOALS) ) done
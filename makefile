TARGET = SHELL

all: $(TARGET)
release: $(TARGET)
debug: $(TARGET)
clean: $(TARGET)

SHELL:
	cd ./src && make $(MAKECMDGOALS);

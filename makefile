TARGET = SHELL

all: $(TARGET)
release: $(TARGET)
debug: $(TARGET)
install: $(TARGET)
clean: $(TARGET)

SHELL:
	cd ./src && make $(MAKECMDGOALS);

TARGET = SHELL

all: $(TARGET)
release: $(TARGET)

SHELL:
	cd ./src && make $(MAKECMDGOALS);

clean:
	cd ./src && make $(MAKECMDGOALS);
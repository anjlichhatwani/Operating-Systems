TARGET= OS_PRG3
CC= gcc
LDFLAGS= -pthread
SRC = OS_PRG3.c
DEBUG= -ggdb

all: $(TARGET)

OS_PRG3: OS_PRG3.c
	$(CC) $(LDFLAGS) $(DEBUG) $(SRC) -o $(TARGET)
	
# let's clean up the mess
clean:
	rm -rf OS_PRG3



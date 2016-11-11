TARGET= PART1
TARGET1= OS_PRG1
TARGET2= OS_PRG2
CC= gcc
LDFLAGS= -pthread
SRC1 = OS_PRG1.c
SRC2 = OS_PRG2.c
DEBUG= -ggdb

all: $(TARGET)

PART1: OS_PRG1 OS_PRG2

OS_PRG1:
	$(CC) $(LDFLAGS) $(DEBUG) $(SRC1) -o $(TARGET1)

OS_PRG2:
	$(CC) $(LDFLAGS) $(DEBUG) $(SRC2) -o $(TARGET2)
	
# let's clean up the mess
clean:
	rm -rf OS_PRG1
	rm -rf OS_PRG2



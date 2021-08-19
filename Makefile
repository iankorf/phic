# Makefile for phic

LIBS = -lm -lgenomikon
HINC = -I../genomikon
LINC = -L../genomikon

CFLAGS = -O2 -Wall -Werror

APP = phic
SRC = phic.c
OBJ = phic.o

default:
	make $(APP)

$(APP): $(OBJ) $(OBJECTS)
	$(CC) -o $(APP) $(CFLAGS) $(LINC) $(OBJ) $(OBJECTS) $(LIBS)

clean:
	rm -f *.o $(APP)

%.o: %.c
	$(CC) $(CFLAGS) $(HINC) -c -o $@ $<




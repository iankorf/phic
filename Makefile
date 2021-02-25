############################
# Makefile for nclassifier #
############################

LIBS = -lm -lik
HINC = -I../ik
LINC = -L../ik

CFLAGS = -O2 -Wall -Werror

#OBJECTS = \

APP = phic
SRC = phic.c
OBJ = phic.o

###########
# Targets #
###########

default:
	make gcc

$(APP): $(OBJ) $(OBJECTS)
	$(CC) -o $(APP) $(CFLAGS) $(LINC) $(OBJ) $(OBJECTS) $(LIBS)

clean:
	rm -f *.o $(APP)

#################
# Architectures #
#################

gcc:
	make $(APP)


###################
# Inference Rules #
###################

%.o: %.c
	$(CC) $(CFLAGS) $(HINC) -c -o $@ $<




# Makefile for compiling to WiringPi

CC = arm-linux-gnueabihf-gcc  
CFLAGS = -fPIC -Wall -Wextra -O2 -g -DWIRINGPI  # C flags
LDFLAGS =    
RM = rm -f  
TARGET_LIB = libsiodm.a  # target lib

SRCS = $(shell ls ./src/*.c)  # C source files 
OBJS = $(SRCS:.c=.o)

.PHONY: all
all: ${TARGET_LIB}

$(TARGET_LIB): $(OBJS)
	# $(CC) ${LDFLAGS} -o $@ $^
	ar rcs $@ $(shell ls ./src/*.o ./src/*.h)

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@

include $(SRCS:.c=.d)

.PHONY: clean
clean:
	-${RM} ${TARGET_LIB} ${OBJS} $(SRCS:.c=.d)
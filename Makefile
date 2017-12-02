#CROSS_COMPILE=
CROSS_COMPILE=arm-hisiv300-linux-
CC=$(CROSS_COMPILE)gcc
STRIP=$(CROSS_COMPILE)strip

#CROSS_COMPILE=arm-hisiv100nptl-linux-
#CC=$(CROSS_COMPILE)gcc
#STRIP=$(CROSS_COMPILE)strip
#CC=gcc
#STRIP=strip
FLAG=-lm
NAME=loadko

objects= inirw.o strfunc.o main.o cJSON.o

$(NAME):$(objects)
	$(CC) -o $(NAME) $(objects) $(FLAG)
	$(STRIP) -g --strip-unneeded $(NAME)
clean:
	rm *.o* -rf
	rm $(NAME) -rf

TARGET = mutualExclusion
CC = gcc
CFLAGS = -Wall -ansi -pedantic -std=c99
LFLAGS = -o $(TARGET) -lpthread 

$(TARGET): $(TARGET).c bankAccount.h bankAccount.c 
	$(CC) $(CFLAGS) mutualExclusion.c bankAccount.c $(LFLAGS)

clean:
	rm -f $(TARGET) *.o *~ *#
CC = g++
TARGET = start.exe
LIBS = -lboost_system -lcrypto -lssl -lcpprest -lpthread

$(TARGET) : main.o
	$(CC) -g main.o -o $(TARGET) $(LIBS)

main.o : main.cpp
	$(CC) -c main.cpp

clean :
	rm *.o $(TARGET)
CPP = g++
TARGET = start.exe
LIBS = -lmysqlclient -lboost_system -lcrypto -lssl -lcpprest -lpthread -std=c++17

$(TARGET) : main.o rest_handle.o
	$(CPP) -g main.o rest_handle.o -o $(TARGET) $(LIBS)

main.o : main.cpp
	$(CPP) -c main.cpp

rest_handle.o : rest_handle.cpp
	$(CPP) -c rest_handle.cpp

clean :
	rm *.o $(TARGET)
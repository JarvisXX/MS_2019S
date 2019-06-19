CFLAG=-Wfatal-errors -std=c++11

all: c s

c: API.o client.o Connection.o
	g++ ${CFLAG} -o client API.o client.o Connection.o

s: API.o LogicSystemMaster.o server.o Connection.o SystemTree.o ServerJ.o LogicSystemSlave.o
	g++ ${CFLAG} -o server API.o LogicSystemMaster.o server.o Connection.o SystemTree.o ServerJ.o LogicSystemSlave.o

client.o: client.cpp API.hpp Connection.hpp
	g++ ${CFLAG} client.cpp -c -o client.o

server.o: server.cpp API.hpp LogicSystemMaster.hpp Connection.hpp ServerJ.hpp
	g++ ${CFLAG} server.cpp -c -o server.o

API.o: API.cpp API.hpp
	g++ ${CFLAG} API.cpp -c -o API.o

LogicSystemMaster.o: LogicSystemMaster.cpp LogicSystemMaster.hpp SystemTree.hpp ServerJ.hpp System.hpp
	g++ ${CFLAG} LogicSystemMaster.cpp -c -o LogicSystemMaster.o

Connection.o: Connection.cpp Connection.hpp
	g++ ${CFLAG} Connection.cpp -c -o Connection.o

SystemTree.o: SystemTree.cpp SystemTree.hpp LogicSystemMaster.hpp
	g++ ${CFLAG} SystemTree.cpp -c -o SystemTree.o

ServerJ.o: ServerJ.cpp ServerJ.hpp LogicSystemSlave.hpp Connection.hpp API.hpp LogicSystemMaster.hpp LogicSystemSlave.hpp
	g++ ${CFLAG} ServerJ.cpp -c -o ServerJ.o

LogicSystemSlave.o: LogicSystemSlave.cpp LogicSystemSlave.hpp System.hpp Metadata.hpp ServerJ.hpp
	g++ ${CFLAG} LogicSystemSlave.cpp -c -o LogicSystemSlave.o

clean: 
	rm *.o client server

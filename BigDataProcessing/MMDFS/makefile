CFLAG=-Wfatal-errors -std=c++11

all: c s

t: mfilegenerator.o API.o 
	g++ ${CFLAG} -o test_unit mfilegenerator.o API.o

c: API.o client.o mconnection.o
	g++ ${CFLAG} -o client API.o client.o mconnection.o

s: API.o LogicSystemMaster.o server.o mconnection.o SystemTree.o goldenipcode.o Server.o LogicSystemSlave.o
	g++ ${CFLAG} -o server API.o LogicSystemMaster.o server.o mconnection.o SystemTree.o goldenipcode.o Server.o LogicSystemSlave.o

client.o: client.cpp API.hpp mconnection.hpp
	g++ ${CFLAG} client.cpp -c -o client.o

server.o: server.cpp API.hpp LogicSystemMaster.hpp mconnection.hpp goldenipcode.hpp Server.hpp
	g++ ${CFLAG} server.cpp -c -o server.o

API.o: API.cpp API.hpp
	g++ ${CFLAG} API.cpp -c -o API.o

LogicSystemMaster.o: LogicSystemMaster.cpp LogicSystemMaster.hpp SystemTree.hpp Server.hpp System.hpp
	g++ ${CFLAG} LogicSystemMaster.cpp -c -o LogicSystemMaster.o

mconnection.o: mconnection.cpp mconnection.hpp
	g++ ${CFLAG} mconnection.cpp -c -o mconnection.o

SystemTree.o: SystemTree.cpp SystemTree.hpp LogicSystemMaster.hpp
	g++ ${CFLAG} SystemTree.cpp -c -o SystemTree.o

mfilegenerator.o: mfilegenerator.cpp API.hpp
	g++ ${CFLAG} mfilegenerator.cpp -c -o mfilegenerator.o

goldenipcode.o: goldenipcode.cpp goldenipcode.hpp
	g++ ${CFLAG} goldenipcode.cpp -c -o goldenipcode.o

Server.o: Server.cpp Server.hpp LogicSystemSlave.hpp mconnection.hpp API.hpp LogicSystemMaster.hpp LogicSystemSlave.hpp
	g++ ${CFLAG} Server.cpp -c -o Server.o

LogicSystemSlave.o: LogicSystemSlave.cpp LogicSystemSlave.hpp System.hpp Metadata.hpp Server.hpp
	g++ ${CFLAG} LogicSystemSlave.cpp -c -o LogicSystemSlave.o

clean: 
	rm *.o client server
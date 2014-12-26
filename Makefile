
CXX = g++
CXX_FLAGS = -g -std=gnu++11

THRIFT_DIR := /usr/local/include

GEN_DIR = gen-cpp2
GEN_SRC = $(wildcard $(GEN_DIR)/*.cpp)
GEN_OBJ = $(patsubst $(GEN_DIR)/%,$(GEN_DIR)/%,$(GEN_SRC:.cpp=.o))

IFACE_DIR = .
IFACE_DEF = $(IFACE_DIR)/*.thrift
IFACE_SRC = $(patsubst $(IFACE_DIR)/%,$(GEN_DIR)/%,$(IFACE_DEF:.thrift=.cpp))

SRC_DIR = .
SRC = $(SRC_DIR)/*.cpp
OBJ = $(patsubst $(SRC_DIR)/%,$(SRC_DIR)/%,$(SRC:.cpp=.o))

INC = -I gen-cpp2 -I ${THRIFT_DIR}
LIB = -lboost_system -lpthread -lglog -lthrift -lthriftcpp2 -lfolly


.PHONY: clean

all: calculator_server calculator_client calculator_proxy

calculator_client: gen-cpp2 ${GEN_OBJ} CalculatorClient.o
	${CXX} ${CXX_FLAGS} -o calculator_client CalculatorClient.o ${GEN_OBJ} ${INC} ${LIB}

calculator_proxy: gen-cpp2 ${GEN_OBJ} CalculatorProxy.o
	${CXX} ${CXX_FLAGS} -o calculator_proxy CalculatorProxy.o ${GEN_OBJ} ${INC} ${LIB}

calculator_server: gen-cpp2 ${GEN_OBJ} CalculatorServer.o
	${CXX} ${CXX_FLAGS} -o calculator_server CalculatorServer.o ${GEN_OBJ} ${INC} ${LIB}

gen-cpp2:
	python -m thrift_compiler.main --gen cpp2 calculator.thrift

${GEN_DIR}/%.o: ${GEN_DIR}/%.cpp
	${CXX} ${CXX_FLAGS} -o $@ -c $<


%.o: %.cpp
	${CXX} ${CXX_FLAGS} -c $< -I gen-cpp2 -I ${THRIFT_DIR}

clean:
	rm -rf gen-cpp2
	rm *.o

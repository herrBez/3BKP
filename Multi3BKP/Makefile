CC = g++ -std=c++11
CPPFLAGS = -Wall -Werror -O2 



CPX_INCDIR  = /opt/CPLEX_Studio/cplex/cplex/include
CPX_LIBDIR  = /opt/CPLEX_Studio/cplex/cplex/lib/x86-64_linux/static_pic

#CPX_INCDIR = /opt/CPLEX_Studio/cplex/include 
#CPX_LIBDIR = /opt/CPLEX_Studio/cplex/lib/x86-64_linux/static_pic

CPX_LDFLAGS = -lcplex -lm -pthread




SOURCES=main.cpp MasterProblem.cpp SlaveProblem.cpp ../Utils/get_option.cpp
OBJECTS=main.o MasterProblem.o SlaveProblem.o ../Utils/get_option.o
EXECUTABLE=main


all: $(SOURCES) $(EXECUTABLE)

$(OBJECTS): %.o: %.cpp
		$(CC) $(CPPFLAGS) -I$(CPX_INCDIR) -c $^ -o $@

$(EXECUTABLE): $(OBJECTS)
		$(CC) $(CPPFLAGS) $(OBJECTS) -o main -L$(CPX_LIBDIR) $(CPX_LDFLAGS)

clean:
		rm -rf $(OBJECTS) main 

.PHONY: clean

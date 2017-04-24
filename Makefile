CC = g++ -std=c++11
CPPFLAGS = -g -Wall -Werror -O2 
CPPHULLFLAGS =$(CPPFLAGS) 
LDFLAGS = -DVERBOSE

CPX_INCDIR  = /opt/CPLEX_Studio/cplex/cplex/include
CPX_LIBDIR  = /opt/CPLEX_Studio/cplex/cplex/lib/x86-64_linux/static_pic

#CPX_INCDIR = /opt/CPLEX_Studio/cplex/include 
#CPX_LIBDIR = /opt/CPLEX_Studio/cplex/lib/x86-64_linux/static_pic

CPX_LDFLAGS = -lcplex -lm -pthread


OBJ = main.o utils.o MasterProblem.o
OBJ_HULL = main.o utils.o HullProblem.o

%.o: %.cpp
		$(CC) $(CPPFLAGS) -I$(CPX_INCDIR) -c $^ -o $@

main: $(OBJ)	
		$(CC) $(CPPFLAGS) $(OBJ) -o main -L$(CPX_LIBDIR) $(CPX_LDFLAGS)

hull: $(OBJ_HULL)
		$(CC) $(CPPHULLFLAGS) $(OBJ_HULL) -o main -L$(CPX_LIBDIR) $(CPX_LDFLAGS)

clean:
		rm -rf $(OBJ) $(OBJ_HULL) main

.PHONY: clean

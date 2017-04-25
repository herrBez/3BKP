CC = g++ -std=c++11
CPPFLAGS = -g -Wall -Werror -O2 
LDFLAGS = -DVERBOSE


CPX_INCDIR  = /opt/CPLEX_Studio/cplex/cplex/include
CPX_LIBDIR  = /opt/CPLEX_Studio/cplex/cplex/lib/x86-64_linux/static_pic

#CPX_INCDIR = /opt/CPLEX_Studio/cplex/include 
#CPX_LIBDIR = /opt/CPLEX_Studio/cplex/lib/x86-64_linux/static_pic

CPX_LDFLAGS = -lcplex -lm -pthread

OBJ = main.o utils.o MasterProblem.o
SRC = main.cpp utils.cpp MasterProblem.cpp


HULL_OBJ = main.o utils.o HullProblem.o
HULL_SRC = main.cpp utils.cpp HullProblem.cpp

all : compile

normalobj: $(SRC)
		$(CC) $(CPPFLAGS) -I$(CPX_INCDIR) -c main.cpp -o main.o && \
		$(CC) $(CPPFLAGS) -I$(CPX_INCDIR) -c utils.cpp -o utils.o && \
		$(CC) $(CPPFLAGS) -I$(CPX_INCDIR) -c MasterProblem.cpp -o MasterProblem.o
	

hullobj: $(HULL_SRC) 
		$(CC) -DHULL $(CPPFLAGS) -I$(CPX_INCDIR) -c main.cpp -o main.o && \
		$(CC) -DHULL $(CPPFLAGS) -I$(CPX_INCDIR) -c utils.cpp -o utils.o && \
		$(CC) -DHULL $(CPPFLAGS) -I$(CPX_INCDIR) -c HullProblem.cpp -o HullProblem.o

hull: hullobj
		$(CC) $(CPPFLAGS) $(HULL_OBJ) -o main_hull -L$(CPX_LIBDIR) $(CPX_LDFLAGS)
		
compile: normalobj
		$(CC) $(CPPFLAGS) $(OBJ) -o main -L$(CPX_LIBDIR) $(CPX_LDFLAGS)




clean:
		rm -rf $(OBJ) $(OBJ_HULL) main main_hull

.PHONY: clean

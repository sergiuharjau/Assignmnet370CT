# The pre-processor and compiler options.
#OPTIONS
CC 	= mpic++
CFLAGS 	= -Wall -fopenmp -std=c++11 
LDFLAGS = -lgomp -lz

#----------

EXE = bin

OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

#----------

$(EXE): $(OBJS)
	$(CC) $(OBJS) $(LIB_DIRS) $(LLIBS)$(LDFLAGS) -o $(EXE)

$(OBJS): %.o : %.cpp
	$(CC) $(CFLAGS) -c $<

#----------

clean:
	rm -f *.o *.*~ *~  $(EXE) 

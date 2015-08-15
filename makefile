
#------------------------------------------------------------------------------

SOURCE=$(wildcard *.cpp)
MYPROGRAM=parentNChild
MYINCLUDES=.

MYLIBRARIES=pthread -lboost_filesystem-mt
CC=g++

#------------------------------------------------------------------------------



all: $(MYPROGRAM)



$(MYPROGRAM): $(SOURCE)

	$(CC) -I$(MYINCLUDES) $(SOURCE) -o$(MYPROGRAM) -l$(MYLIBRARIES)

clean:

	rm -f $(MYPROGRAM)


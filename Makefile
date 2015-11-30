DEBUGFLAGS = -g

CFLAGS = -Wall -ansi -pedantic-errors $(DEBUGFLAGS)
CXXFLAGS = $(CFLAGS)
CC = $(CXX)			# for linking
LDFLAGS = $(DEBUGFLAGS)

OBJS = TokenStream.o AbstractSyntax.o StaticTypeCheck.o ConcreteSyntax.o Interpreter.o Semantics.o

TARGETS = Interpreter

Interpreter: $(OBJS)
	$(CXX) $(LDFLAGS) $(DEBUGFLAGS) -o $@ $^

cleano:
	rm -f $(OBJS) $(TARGETS)
	@if [ -f core* ] ; then rm core; echo "rm core*" ;fi

clean: cleano
	rm -f *.dep core*

mrproper: clean
	rm -f *~

archive: mrproper
	cd .. && tar cjvf `basename $(PWD)`-`date +%d%B%y-%Hh%Mm%Ss`.tar.bz2 `basename $(PWD)`


# Automatic maintenance of dependencies:

OBJMAKETRANSCMD = sed "s/$*.o:/$*.o $*.dep:/g"

%.dep: %.cc
	$(SHELL) -ec '$(CXX) -c -MM $(CXXFLAGS) $< | $(OBJMAKETRANSCMD) > $@'

include $(addsuffix .dep,$(basename $(OBJS)))

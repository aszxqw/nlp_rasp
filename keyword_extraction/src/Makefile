CC = g++
CCOPT = -Wall -c
LINK = g++
LINKOPT =
LIB = ar
LIBOPT = rc
DOLINK = $(LINK) $(LINKOPT) -o $@ $^
SOURCES := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SOURCES))

CMDIR = ./cppcommon/
CMLIB = $(CMDIR)cmlib.a


.INTERMEDIATE:

# This is a suffix rule 
#.c.o: 
%.o: %.cpp
	$(CC) $(CCOPT) $<

# Main Targets
all: main 

main: $(OBJS) $(CMLIB)
	$(DOLINK) -lIce -lIceUtil

$(CMLIB): $(CMDIR)
	cd $(CMDIR) && $(MAKE)

#main.o: main.cpp TaggerICE.h cppcommon/io_functs.h

TaggerICE.h: TaggerICE.ice
	slice2cpp TaggerICE.ice
TaggerICE.cpp: TaggerICE.ice
	slice2cpp TaggerICE.ice

TaggerICE.o: TaggerICE.cpp TaggerICE.h
	$(CC) $(CCOPT) -I. $<

#TaggerClient.o: TaggerClient.cpp TaggerClient.h TaggerICE.h
#LogicalSegment.o: LogicalSegment.cpp LogicalSegment.h \
	 cppcommon/str_functs.h cppcommon/vec_functs.h cppcommon/vec_functs.tcc \
	  structs.h

test: test.o LogicalSegment.o $(CMLIB)
	$(DOLINK) -lIce -lIceUtil
#test.o: test.cpp cppcommon/io_functs.h LogicalSegment.h \
#	 cppcommon/str_functs.h

#unit_test
LogicalSegment.ut: LogicalSegment.cpp LogicalSegment.h \
	 cppcommon/str_functs.h cppcommon/vec_functs.h cppcommon/vec_functs.tcc \
	  structs.h \
	 $(CMLIB)
	g++ -o $@ $< -DUNIT_TEST $(CMLIB)
TaggerClient.ut: TaggerClient.cpp TaggerClient.h TaggerICE.h \
 cppcommon/io_functs.h cppcommon/vec_functs.h cppcommon/vec_functs.tcc  $(CMLIB)
	g++ -o $@ -I. TaggerClient.cpp TaggerICE.cpp -DUNIT_TEST $(CMLIB)  -lIce -lIceUtil

KeyWordExt.ut: KeyWordExt.cpp KeyWordExt.h structs.h \
	 cppcommon/vec_functs.h cppcommon/vec_functs.tcc typedefs.h \
	 $(CMLIB)
	g++ -o $@ KeyWordExt.cpp -DUNIT_TEST $(CMLIB)


WordCluster.ut: WordCluster.cpp WordCluster.h cppcommon/logger.h \
	 cppcommon/file_functs.h cppcommon/str_functs.h cppcommon/file_functs.h \
	  cppcommon/vec_functs.h cppcommon/vec_functs.tcc structs.h typedefs.h \
	   globals.h
	g++ -o $@ $< -DWORDCLUSTER_UT $(CMLIB)

TemplateExtractor.ut: TemplateExtractor.cpp TemplateExtractor.h
	g++ -o $@ $< -DTEMPLATE_EXTRACTOR_UT $(CMLIB)


clean:
	rm -f test main *.ut *.d *.d.* *.o *.out $(CMLIB)

#generate the dependencde files for *.cpp automatically
sinclude $(SOURCES:.cpp=.d)
%.d:%.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $< > $@.$$$$; \
	sed 's,\($*\).o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

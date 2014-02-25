CC = g++
CCOPT = -Wall -DNo_Phrase -c
LINK = g++
LINKOPT =
LIB = ar
LIBOPT = rc
DOLINK = $(LINK) $(LINKOPT) -o $@ $^

LIBDIR = ./lib/
LIBDB = libdb.a 
LIBDBOPT = -lpthread
LABELLIB = $(LIBDIR)labellib.a
CMDIR = ./cppcommon/
CMLIB = $(CMDIR)cmlib.a


.INTERMEDIATE: main.o Tagger.o TaggerICE.o TaggerICEServer.o testclient.o

# This is a suffix rule 
#.c.o: 
%.o: %.cpp
	$(CC) $(CCOPT) $<

# Main Targets
all: $(LABELLIB) $(CMLIB) main tagger_server tagger_client sentence sentence tok

sentence: sentence.yy.c
	gcc -o $@ $<
tok: token.yy.c
	gcc -o $@ $<

main: main.o Tagger.o $(LABELLIB) $(LIBDB) $(CMLIB)
	$(DOLINK) $(LIBDBOPT)

$(LABELLIB): $(LIBDIR)
	cd $(LIBDIR) && $(MAKE)

$(CMLIB): $(CMDIR)
	cd $(CMDIR) && $(MAKE)

Tagger.o: Tagger.cpp Tagger.h lib/mainl.h lib/common.h lib/diction.h \
	lib/../../database/common/db.h lib/trans.h lib/label.h lib/map.h \
	lib/low.h lib/unkcommon.h lib/unkdiction.h lib/analyse.h lib/options.h \
	lib/mainl.h 

main.o: main.cpp Tagger.h lib/mainl.h lib/common.h lib/diction.h \
	lib/../../database/common/db.h lib/trans.h lib/label.h lib/map.h \
	lib/low.h lib/unkcommon.h lib/unkdiction.h lib/analyse.h lib/options.h \
	lib/mainl.h 


TaggerICE.h: TaggerICE.ice
	slice2cpp TaggerICE.ice
TaggerICE.cpp: TaggerICE.ice
	slice2cpp TaggerICE.ice

TaggerICE.o: TaggerICE.cpp TaggerICE.h
	$(CC) $(CCOPT) -I. TaggerICE.cpp

TaggerICEServer.o: TaggerICEServer.cpp TaggerICEServer.h TaggerICE.h

tagger_server: TaggerICE.o TaggerICEServer.o Tagger.o $(LABELLIB) $(LIBDB) $(CMLIB)
	$(DOLINK)  -lIce -lIceUtil $(LIBDBOPT)

testclient.o: testclient.cpp TaggerICE.h
tagger_client: TaggerICE.o testclient.o $(CMLIB)
	$(DOLINK) -lIce -lIceUtil $(LIBDBOPT)

clean:
	rm -f tagger_client tagger_server *.o *.out $(LABELLIB) $(CMLIB)

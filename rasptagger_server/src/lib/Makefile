CC = g++
CCOPT = -ansi -w -DNo_Phrase -c
LINK = g++
LINKOPT = 
LIB = ar
LIBOPT = rc
DOLINK = $(LINK) $(LINKOPT) -o $@
LABELLIB = labellib.a

# Remove object files after compilation:
.INTERMEDIATE: common.o unkcommon.o diction.o unkdiction.o io.o list.o low.o map.o trans.o \
	analyse.o options.o stack.o label.o \
	mainl.o

# This is a suffix rule 
#.c.o: 
%.o: %.c
	$(CC) $(CCOPT) $<

# Main Targets
all: $(LABELLIB) 

$(LABELLIB): common.o unkcommon.o diction.o unkdiction.o io.o list.o low.o map.o trans.o \
	analyse.o options.o stack.o label.o \
	mainl.o
	$(LIB) $(LIBOPT) $@ $?

# about $(LABELLIB) 
common.o: common.c common.h
unkcommon.o: unkcommon.c common.h diction.h unkcommon.h unkdiction.h map.h
diction.o: diction.c common.h diction.h map.h
unkdiction.o: unkdiction.c common.h diction.h unkcommon.h unkdiction.h map.h
io.o: io.c common.h diction.h low.h map.h label.h stack.h unkcommon.h unkdiction.h analyse.h
list.o: list.c common.h list.h
low.o: low.c common.h low.h map.h
map.o: map.c common.h list.h map.h
trans.o: trans.c common.h trans.h map.h
analyse.o: analyse.c common.h map.h diction.h label.h analyse.h
options.o: options.c options.h mainl.h
stack.o: stack.c common.h map.h diction.h label.h stack.h unkcommon.h unkdiction.h list.h
label.o: label.c common.h diction.h trans.h label.h map.h stack.h \
 analyse.h unkcommon.h


#sentence.yy.o: sentence.yy.c sentence.yy.h
#token.yy.o: token.yy.c token.yy.h
mainl.o: mainl.c mainl.h common.h diction.h trans.h label.h map.h low.h unkcommon.h unkdiction.h analyse.h

clean:
	rm -f *.o *.a

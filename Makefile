GTEST_PREFIX=/usr/local
ICU_PREFIX=/usr/local/opt/icu4c
LIBXML2_PREFIX=/usr/local/opt/libxml2
ICU_INC=$(ICU_PREFIX)/lib/icu/Makefile.inc
TARGET=toy tester
OBJECTS=document.o index.o lexicon.o query.o tokenizer.o util.o
TEST_OBJECTS=tester.o

include $(ICU_INC)

all: $(TARGET)

toy: $(OBJECTS) toy.o
	$(LINK.cc) -o $@ $^ $(ICULIBS) $(ICULIBS_ICUIO)

clean:
	rm -f *.o *~ $(TARGET)

test: tester
	./tester

tester: $(OBJECTS) $(TEST_OBJECTS)
	$(LINK.cc) -o $@ $^ $(ICULIBS) $(ICULIBS_ICUIO) ${GTEST_PREFIX}/lib/libgtest.a ${GTEST_PREFIX}/lib/libgtest_main.a ${LIBXML2_PREFIX}/lib/libxml2.a


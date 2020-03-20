GTEST_PREFIX=/usr/local
CPPREST_PREFIX=/usr/local
ICU_PREFIX=/usr/local/opt/icu4c
ICU_INC=$(ICU_PREFIX)/lib/icu/Makefile.inc
TARGET=indexer cli_server web_server tester toy
OBJECTS=document.o index.o lexicon.o query.o ranker.o tokenizer.o util.o search_engine.o
TEST_OBJECTS=tester.o

include $(ICU_INC)

all: $(TARGET)

toy: $(OBJECTS) toy.o
	$(LINK.cc) -o $@ $^ $(ICULIBS) $(ICULIBS_ICUIO)

cli_server: $(OBJECTS) cli_server.o
	$(LINK.cc) -o $@ $^ $(ICULIBS) $(ICULIBS_ICUIO)

web_server: $(OBJECTS) http_server_async.o
	$(LINK.cc) -o $@ $^ $(ICULIBS) $(ICULIBS_ICUIO)

indexer: $(OBJECTS) indexer.o
	$(LINK.cc) -o $@ $^ $(ICULIBS) $(ICULIBS_ICUIO)

clean:
	rm -f *.o *~ $(TARGET) *.txt

test: tester
	./tester

tester: $(OBJECTS) $(TEST_OBJECTS)
	$(LINK.cc) -o $@ $^ $(ICULIBS) $(ICULIBS_ICUIO) ${GTEST_PREFIX}/lib/libgtest.a ${GTEST_PREFIX}/lib/libgtest_main.a


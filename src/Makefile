
CC=g++
CFLAGS=-c -Wall
LDFLAGS=

INSTALL=/usr/local/bin/install -c

BINDIR=/usr/local/bin
PREFIX=/usr/local

SOURCES=Action.cpp CaicWriter.cpp discretematrix.cpp \
   MesaConsoleApp.cpp nexusblock.cpp QueueElement.cpp TabReader.cpp \
   ActionQueue.cpp callback.cpp distancedatum.cpp \
   MesaConsoleApp_Ui.cpp nexus.cpp Reporter.cpp taxablock.cpp \
   ActionUtils.cpp charactersblock.cpp distancesblock.cpp  MesaGlobals.cpp \
   NexusReader.cpp ReporterPrefix.cpp treesblock.cpp \
   allelesblock.cpp CharComparator.cpp emptyblock.cpp  MesaModel.cpp \
   nexustoken.cpp ResultsDistiller.cpp  TreeWrangler.cpp \
   Analysis.cpp CharEvolRule.cpp Epoch.cpp MesaTree.cpp \
   NexusWriter.cpp RichnessReader.cpp xnexus.cpp \
   assumptionsblock.cpp CharEvolScheme.cpp EvolRule.cpp MesaUtils.cpp \
   Numerics.cpp setreader.cpp XRate.cpp \
   CharStateSet.cpp Macro.cpp NclBlocks.cpp \
   nxsdate.cpp SimGlobals.cpp \
   datablock.cpp main.cpp NclReader.cpp \
   nxsstring.cpp SystemAction.cpp \
   CaicReader.cpp discretedatum.cpp \
   Prune.cpp TabDataReader.cpp RandomService.cpp StringUtils.cpp \
   CaicCode.cpp \
   CommandMgr.cpp ConsoleApp.cpp ConsoleMenuApp.cpp \
	BasicScanner.cpp StreamScanner.cpp StringScanner.cpp 

OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=mesa


all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE) $(PACKAGE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)

#install: all
        #$(INSTALL) tar $(bindir)/$(binprefix)tar
        #-test ! -f rmt || $(INSTALL) rmt /etc/rmt



install: all 
	install -m 0755 $(EXECUTABLE) $(PREFIX)/bin
    
$(phony install)



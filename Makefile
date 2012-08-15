
CC=g++
CFLAGS=-c -Wall
LDFLAGS=

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
   basiccmdline.cpp CharStateSet.cpp Macro.cpp NclBlocks.cpp \
   nxsdate.cpp SimGlobals.cpp \
   Bounds.cpp datablock.cpp main.cpp NclReader.cpp \
   nxsstring.cpp SystemAction.cpp \
   CaicReader.cpp discretedatum.cpp makedoc.cpp ncltest.cpp \
   Prune.cpp TabDataReader.cpp RandomService.cpp StringUtils.cpp \
   CaicCode.cpp CaicReader.cpp CaicWriter.cpp \
   CommandMgr.cpp ConsoleApp.cpp ConsoleMenuApp.cpp 

OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=mesa


all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o

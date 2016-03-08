GPP=g++ 
BIN=./bin/
LDIR=./src/learn/
SDIR=./src/samp/
LIBCINC= -I. -I$(LDIR) -I$(SDIR)
LIBLINC= -L. -L$(LDIR) -L$(SDIR)
FLAGS=-Wall # -pg -g -o5
SOBJS=$(SDIR)cssamp.o $(SDIR)satproblem.o $(SDIR)samplesat.o $(SDIR)slicesampling.o
MOBJS=$(SDIR)marginal.o $(SDIR)satproblem.o $(SDIR)samplesat.o $(SDIR)slicesampling.o
LOBJS=$(LDIR)csslearn.o $(LDIR)satproblem.o $(LDIR)samplesat.o $(LDIR)slicesampling.o
CSL=csslearn.cpp
CSS=cssamp.cpp
MAR=marginal.cpp
SAT=satproblem.cpp
SAM=samplesat.cpp
SLI=slicesampling.cpp

all: csslearn cssamp marginal

############################################################
###                      CSSLEARN                        ###
############################################################
csslearn: $(LOBJS)
	$(GPP) $(FLAGS) -o $@ $(LOBJS) -lm
	mv ./$@ $(BIN)

csslearn.o: $(LDIR)$(CSL)
	$(GPP) $(FLAGS) -c $(LDIR)$(CSL)
	mv ./$@ $(LDIR)

$(LDIR)satproblem.o: $(LDIR)$(SAT)
	$(GPP) $(FLAGS) -c $(LDIR)$(SAT)
	mv satproblem.o $(LDIR)

$(LDIR)samplesat.o: $(LDIR)$(SAM)
	$(GPP) $(FLAGS) -c $(LDIR)$(SAM)
	mv samplesat.o $(LDIR)

$(LDIR)slicesampling.o: $(LDIR)$(SLI)
	$(GPP) $(FLAGS) -c $(LDIR)$(SLI)
	mv slicesampling.o $(LDIR)

$(LDIR)csslearn.o: $(LDIR)csslearn.h $(LDIR)satproblem.h $(LDIR)samplesatparams.h $(LDIR)samplesat.h $(LDIR)slicesampling.h $(LDIR)timer.h $(LDIR)random.h

$(LDIR)satproblem.o: $(LDIR)timer.h $(LDIR)random.h $(LDIR)samplesatparams.h $(LDIR)satproblem.h

$(LDIR)samplesat.o: $(LDIR)samplesat.h $(LDIR)samplesatparams.h $(LDIR)satproblem.h

$(LDIR)slicesampling.o: $(LDIR)samplesat.h $(LDIR)slicesampling.h $(LDIR)samplesatparams.h $(LDIR)satproblem.h


############################################################
###                       CSSAMP                         ###
############################################################
cssamp: $(SOBJS)
	$(GPP) $(FLAGS) -o $@ $(SOBJS) -lm
	mv ./$@ $(BIN)

cssamp.o: $(SDIR)$(CSS)
	$(GPP) $(FLAGS) -c $(SDIR)$(CSS)
	mv ./$@ $(SDIR)

$(SDIR)cssamp.o: $(SDIR)cssamp.h $(SDIR)satproblem.h $(SDIR)samplesatparams.h $(SDIR)samplesat.h $(SDIR)slicesampling.h $(SDIR)timer.h $(SDIR)random.h

$(SDIR)satproblem.o: $(SDIR)timer.h $(SDIR)random.h $(SDIR)samplesatparams.h $(SDIR)satproblem.h

$(SDIR)samplesat.o: $(SDIR)samplesat.h $(SDIR)samplesatparams.h $(SDIR)satproblem.h

$(SDIR)slicesampling.o: $(SDIR)samplesat.h $(SDIR)slicesampling.h $(SDIR)samplesatparams.h $(SDIR)satproblem.h


############################################################
###                     MARGINAL                         ###
############################################################
marginal: $(MOBJS)
	$(GPP) $(FLAGS) -o $@ $(MOBJS) -lm
	mv ./$@ $(BIN)

marginal.o: $(SDIR)$(MAR)
	$(GPP) $(FLAGS) -c $(SDIR)$(MAR)
	mv ./$@ $(SDIR)

$(SDIR)satproblem.o: $(SDIR)$(SAT)
	$(GPP) $(FLAGS) -c $(SDIR)$(SAT)
	mv satproblem.o $(SDIR)

$(SDIR)samplesat.o: $(SDIR)$(SAM)
	$(GPP) $(FLAGS) -c $(SDIR)$(SAM)
	mv samplesat.o $(SDIR)

$(SDIR)slicesampling.o: $(SDIR)$(SLI)
	$(GPP) $(FLAGS) -c $(SDIR)$(SLI)
	mv slicesampling.o $(SDIR)

$(SDIR)cssamp.o: $(SDIR)cssamp.h $(SDIR)satproblem.h $(SDIR)samplesatparams.h $(SDIR)samplesat.h $(SDIR)slicesampling.h $(SDIR)timer.h $(SDIR)random.h

$(SDIR)satproblem.o: $(SDIR)timer.h $(SDIR)random.h $(SDIR)samplesatparams.h $(SDIR)satproblem.h

$(SDIR)samplesat.o: $(SDIR)samplesat.h $(SDIR)samplesatparams.h $(SDIR)satproblem.h

$(SDIR)slicesampling.o: $(SDIR)samplesat.h $(SDIR)slicesampling.h $(SDIR)samplesatparams.h $(SDIR)satproblem.h


############################################################
###                        CLEAN                         ###
############################################################
clean: 
	rm -f *.o
	rm -f $(LDIR)*.o
	rm -f $(SDIR)*.o
	rm -f *.cpp~
	rm -f *.h~

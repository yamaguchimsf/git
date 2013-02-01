GPP=g++ 
FLAGS=-pg -Wall #-g #-o3
OBJS=cssamp.o satproblem.o samplesat.o slicesampling.o
OBJM=marginal.o satproblem.o samplesat.o slicesampling.o
OBJR=mrg.o satproblem.o samplesat.o slicesampling.o
CS=cssamp.cpp
MAR=marginal.cpp
MRG=mrg.cpp
SAT=satproblem.cpp
SAM=samplesat.cpp
SLI=slicesampling.cpp

all: cssamp marginal mrg

cssamp: $(OBJS)
	$(GPP) $(FLAGS) -o $@ $(OBJS) -lm

cssamp.o: $(CS)
	$(GPP) $(FLAGS) -c $(CS)

marginal: $(OBJM)
	$(GPP) $(FLAGS) -o $@ $(OBJM) -lm

marginal.o: $(MAR)
	$(GPP) $(FLAGS) -c $(MAR)

mrg: $(OBJR)
	$(GPP) $(FLAGS) -o $@ $(OBJR) -lm

mrg.o: $(MRG)
	$(GPP) $(FLAGS) -c $(MRG)

satproblem.o:$(SAT)
	$(GPP) $(FLAGS) -c $(SAT)

samplesat.o:$(SAM)
	$(GPP) $(FLAGS) -c $(SAM)

slicesampling.o:$(SLI)
	$(GPP) $(FLAGS) -c $(SLI)

clean:
	rm -f *.o
	rm -f *.cpp~
	rm -f *.h~

cssamp.o: cssamp.h satproblem.h samplesatparams.h samplesat.h slicesampling.h timer.h random.h

marginal.o: marginal.h satproblem.h samplesatparams.h samplesat.h slicesampling.h timer.h random.h

mrg.o: mrg.h satproblem.h samplesatparams.h samplesat.h slicesampling.h timer.h random.h

satproblem.o: timer.h random.h samplesatparams.h satproblem.h

samplesat.o: samplesat.h samplesatparams.h satproblem.h

slicesampling.o:samplesat.h slicesampling.h samplesatparams.h satproblem.h

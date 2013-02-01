GPP=g++ 
FLAGS=-pg -Wall #-g #-o5
#OBJS=cssamp.o satproblem.o samplesat.o slicesampling.o
OBJS=csslearn.o satproblem.o samplesat.o slicesampling.o
#CS=cssamp.cpp
CSL=csslearn.cpp
SAT=satproblem.cpp
SAM=samplesat.cpp
SLI=slicesampling.cpp

all: csslearn

#cssamp: $(OBJS1)
#	$(GPP) $(FLAGS) -o $@ $(OBJS) -lm

#cssamp.o: $(CS)
#	$(GPP) $(FLAGS) -c $(CS)

csslearn: $(OBJS)
	$(GPP) $(FLAGS) -o $@ $(OBJS) -lm

csslearn.o: $(CSL)
	$(GPP) $(FLAGS) -c $(CSL)

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

#cssamp.o: cssamp.h satproblem.h samplesatparams.h samplesat.h slicesampling.h timer.h random.h

csslearn.o: csslearn.h satproblem.h samplesatparams.h samplesat.h slicesampling.h timer.h random.h

satproblem.o: timer.h random.h samplesatparams.h satproblem.h

samplesat.o: samplesat.h samplesatparams.h satproblem.h

slicesampling.o:samplesat.h slicesampling.h samplesatparams.h satproblem.h

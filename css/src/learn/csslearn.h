/*** csslearn.h ***/

#ifndef CSSLEARN_H_
#define CSSLEARN_H_

char*  cnffilename    = NULL;
char*  probfilename   = NULL;
char*  slifilename    = NULL;
char*  outfilename    = NULL;
int    walkmax        = 100000;
int    numSample      = 1000;
double walksatProb    = 0.5;
double randomWalkProb = 0.5;
double temperature    = 0.1;
int    mode           = 3;
int    datasize       = 100;
double ita            = 0.0001;
bool   csmode         = true;
bool   montecarlo     = false;
bool   cdmode         = false;
int    maxiter        = 100;
int    learnsample    = 1000;

int idx;
const bool cdebug = false;

int main(int argc, char** argv);
void printssParameters(samplesatparams* ssparams);
void printlParameters(learnparams* lparams);
void printModel(ostream &out, int* model, SatProblem* sat);

#endif /* _CSSLEARN_H_ */

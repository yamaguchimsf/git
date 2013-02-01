/*** cssamp.h ***/
#ifndef CSSAMP_H_
#define CSSAMP_H_

char*  inCnfFile      = NULL;
char*  inProbFile     = NULL;
int    walkmax        = 1000;
int    numSample      = 1000;
double walksatProb    = 0.5;
double randomWalkProb = 0.5;
double temperature    = 0.1;
int    mode           = 3;
char*  prbfilename    = NULL;
char*  slifilename    = NULL;

int idx;
const bool cdebug = false;

void printParameters(samplesatparams* ssparams);
void printModel(ostream &out, int* model, SatProblem* sat);
int main(int argc, char** argv);

#endif /* _CSSAMP_H_ */

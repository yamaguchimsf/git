/*** marginal.h ***/
#ifndef MARGINAL_H_
#define MARGINAL_H_

char*  inCnfFile      = NULL;
char*  inProbFile     = NULL;
int    walkmax        = 100000;
int    numSample      = 1000;
double walksatProb    = 0.5;
double randomWalkProb = 0.5;
double temperature    = 0.1;
bool   csmode         = true;
char*  prbfilename    = NULL;
char*  outfilename    = NULL;

int**  slisamples;
int**  samsamples;
int idx;
const bool cdebug = false;

int main(int argc, char** argv);
void printParameters(samplesatparams* ssparams);
void printModel(ostream &out, int* model, SatProblem* sat);

#endif /* _MARGINAL_H_ */

/*** mrg.h ***/
#ifndef MRG_H_
#define MRG_H_

char*  inCnfFile      = NULL;
char*  inProbFile     = NULL;
int    walkmax        = 1000000;
int    numSample      = 100;
int    maxiter        = 100;
double walksatProb    = 0.5;
double randomWalkProb = 0.5;
double temperature    = 0.1;
bool   csmode         = true;
char*  prbfilename    = NULL;
char*  slifilename    = NULL;
char*  outfilename    = NULL;
int    datasize       = 100000;
int**  slisamples;
int**  samsamples;
int idx;
const bool cdebug = false;

int main(int argc, char** argv);
void printParameters(samplesatparams* ssparams);
void printModel(ostream &out, int* model, SatProblem* sat);

#endif /* _MRG_H_ */

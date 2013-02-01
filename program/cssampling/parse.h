/*** parse.h ***/

#ifndef PARSE_H_
#define PARSE_H_


char*  cnffile   = NULL;
char*  dbfile    = NULL;
/*
int    walkmax        = 1000;
int    numSample      = 1000;
double walksatProb    = 0.5;
double randomWalkProb = 0.5;
double temperature    = 0.1;
int    mode           = 3;
int    datasize       = 100;
double ita            = 0.0001;
bool   distmode       = false;
bool   cdmode         = true;
int    maxiter        = 100;
int    learnsample    = 1000;

int idx;
*/
const bool cdebug = false;

int main(int argc, char** argv);
/*
void printssParameters(samplesatparams* ssparams);
void printlParameters(learnparams* lparams);
void printModel(ostream &out, int* model, int val);
*/
#endif /* _PARSE_H_ */

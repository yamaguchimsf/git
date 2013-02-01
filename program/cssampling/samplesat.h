/*** samplesat.h ***/

#ifndef SAMPLESAT_H_
#define SAMPLESAT_H_

#include "satproblem.h"
#include "samplesatparams.h"
#include "learnparams.h"
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <math.h>
#include "timer.h"

using namespace std;

const bool ssdebug = false;

class SampleSAT
{
  public:
  SampleSAT(SatProblem* sat, samplesatparams* ssparams, bool csmode, learnparams* lparams);
  ~SampleSAT();
  void solve(SatProblem* sat, int* sample);
  void init_model();
  bool walk();
  int randomWalk();
  int greedyWalk();
  int simAnnealing();
  double getTemp();
  void printModels();
  void calcAve(learnparams* lparams);
  void initAve();
  void setWalkMax(int walkmax);
  void setNumSample(int numSample);
  void setWalkSatProb(double walksatProb);
  void setRandomWalkProb(double randomWalkProb);
  void setTemperature(double temperature);
  void learn(int* sample, char* inputfile, int datasize, char* outfilename);
  void lallocate(int datasize);
  void readdata(char* inputfile, int datasize);
  void islearn(int datasize, char* outfilename);
  double calclik(double *rd);
  double getProb(int* sample);
  bool modelcheck(int i);
  int modelcheck3(int i,int** matrix);
  int modelcount();
  void destlearn(int datasize);
  void normalize(double* vec,int vecsize);

 private:
  bool csmode_;
  SatProblem* sat_;
  samplesatparams* ssparams_;
  learnparams* lparams_;
  Timer timer;
  ifstream ifile;
  char c;
  char buffer[BUFSIZE];
  int* model;
  int step_;
  int nsc_;
  int numAtom;
  int numClause;
  int datasize_t;
  int numRW;
  int numGW;
  int numSA;
  int numSample;
  int hashValue;
  double stepAve;
  double stepVar;
  double* ex;
  double* dd;
  double* sd;
  double cdsdSec;
  double cdddSec;
  double cntSec;
  int* xx;
  int** tdata;
  int** sdata;
  int** sbuf;
  int* key;
  double* ad;
  double* bd;
  double* counts;
  double* realcondprob;
  double* weight;
  int** nmodel;
  double* q_xF;
};

#endif /* SAMPLESAT_H_ */


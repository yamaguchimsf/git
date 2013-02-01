/*** samplesat.h ***/

#ifndef SAMPLESAT_H_
#define SAMPLESAT_H_

#include "satproblem.h"
#include "samplesatparams.h"
#include <iostream>
#include <stdlib.h>
#include <math.h>

using namespace std;

const bool ssdebug = false;

class SampleSAT
{
  public:
  SampleSAT(SatProblem* sat, samplesatparams* ssparams, bool csmode);
  ~SampleSAT();
  void solve(SatProblem* sat, int* sample);
  void init_model();
  bool walk();
  int randomWalk();
  int greedyWalk();
  int simAnnealing();
  double getTemp();
  void printModels();
  void calcAve();
  void calcAve2(int maxiter);
  void initAve();
  void setWalkMax(int walkmax);
  void setNumSample(int numSample);
  void setWalkSatProb(double walksatProb);
  void setRandomWalkProb(double randomWalkProb);
  void setTemperature(double temperature);

 private:
  bool csmode_;
  SatProblem* sat_;
  samplesatparams* ssparams_;
  int* model;
  int step_;
  int nsc_;
  int numAtom;
  int numClause;
  int numRW;
  int numGW;
  int numSA;
  int numSample;
  int hashValue;
  double stepAve;
  double stepVar;
};

#endif /* SAMPLESAT_H_ */


/*** slicesampling.h ***/

#ifndef SLICESAMPLING_H_
#define SLICESAMPLING_H_

#include "samplesatparams.h"
#include "satproblem.h"
#include "samplesat.h"
#include "timer.h"

using namespace std;
const bool sldebug = false;

class SliceSampling
{
 public:
  SliceSampling(SampleSAT* samplesat, SatProblem* sat);
  ~SliceSampling();
  void init();
  void getSample(SatProblem* sat, int* sample);
  void sampleU();
  void sampleX();
  void unitProc(int var, int val);
  void printModel(int* model);
  void printUnitTime();
  void calcFixed(int numSample);
 private:
  SatProblem* sat_;
  SampleSAT* samplesat_;
  double* u;
  int* x;
  Timer timer;
  double aveFixedAtom;
  double aveFixedClause;
  double sumSec1;
  double sumSec2;
  double sumSec3;
  double sumSec4;
  double sumSec5;
  double unitSec1;
  double unitSec2;
  double unitSec3;
  double unitSec4;
  double unitSec5;
};

#endif /* SLICESAMPLING_H_ */

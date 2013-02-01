/*** slicesampling.h ***/

#ifndef SLICESAMPLING_H_
#define SLICESAMPLING_H_

#include "samplesatparams.h"
#include "learnparams.h"
#include "satproblem.h"
#include "samplesat.h"
#include "timer.h"
#include <cstring>
#include <mcheck.h>
#include <math.h>

#define BUFSIZE 51200

using namespace std;
const bool sldebug = false;

class SliceSampling
{
 public:
  SliceSampling(SampleSAT* samplesat, SatProblem* sat, learnparams* lparams);
  ~SliceSampling();
  void init();
  void getSample(SatProblem* sat, int numSample, int* sample);
  void sampleU();
  void sampleX();
  void unitProc(int var, int val);
  void printModel(int* model);
  void printUnitTime();
  void calcFixed(learnparams* lparams);
  void learn(int* sample, char* inputfile, int datasize, bool cdmode, char* outfilename);
  void lallocate(int datasize);
  void readdata(char* inputfile, int datasize);
  double calclik(double *rd);
  void malearn(int datasize, bool cdmode, char* outfilename);
  double getProb(int* sample);
  bool modelcheck(int i);
  int calckey(int i);
  int modelcount();
  void destlearn(int datasize);
  void normalize(double* vec,int vecsize);

 private:
  SatProblem* sat_;
  SampleSAT* samplesat_;
  learnparams* lparams_;
  double* u;
  int* x;
  int* data;
  ifstream ifile;
  char c;
  char buffer[BUFSIZE];
  Timer timer;
  int numAtom;
  int datasize_t;
  double aveFixedAtom;
  double aveFixedClause;
  double sumSec1;
  double sumSec2;
  double sumSec3;
  double sumSec4;
  double unitSec1;
  double unitSec2;
  double unitSec3;
  double unitSec4;
  double cdsdSec;
  double cdddSec;
  double mcsdSec;
  double mcddSec;
  double cntSec;
  double* ex;
  double* dd;
  double* sd;
  int* xx;
  int** tdata;
  int** sdata;
  int** sbuf;
  double* realcondprob;
  double* weight;
  int* key;
  double* ad;
  double* counts;
  int** nmodel;
  double* q_xF;
};

#endif /* SLICESAMPLING_H_ */

/***  csslearn.cpp***/
//for learning
#include "satproblem.h"
#include "samplesatparams.h"
#include "learnparams.h"
#include "samplesat.h"
#include "slicesampling.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <mcheck.h>
#include "timer.h"
#include "random.h"
#include "csslearn.h"

int main(int argc, char** argv)
{
  cout << "//////////////////////////" << endl;
  //mtrace();
  int option;
  int i;
  unsigned long seed  = (unsigned long) time(NULL);
  Timer timer;
  double startSec = timer.time();

  if (cdebug == true){
    cout << "## reading inputs"<< endl;
  }
  while (( option = getopt(argc, argv, "iolsSwprtmcdIuO")) != -1)
    switch(option){
    case 'i' :
      cnffilename = argv[optind];
      break;
    case 'o' :
      probfilename = argv[optind];
      break;
    case 'l' :
      slifilename = argv[optind];
      break;
    case 's' :
      learnsample = atoi(argv[optind]);
      break;
    case 'S' :
      seed = (unsigned long)atol(argv[optind]);
      break;
    case 'w' :
      walkmax = atoi(argv[optind]);
      break;
    case 'p' :
      walksatProb = atof(argv[optind]);
      break;
    case 'r' :
      randomWalkProb = atof(argv[optind]);
      break;
    case 't' :
      temperature = atof(argv[optind]);
      break;
    case 'm' :
      csmode = false;
      break;
    case 'c' :
      montecarlo = true;
      break;
    case 'd' :
      cdmode = true;
      break;
    case 'I' :
      ita = atof(argv[optind]);
      break;
    case 'u' :
      maxiter = atoi(argv[optind]);
      break;
    case 'O' :
      outfilename = argv[optind];
      break;
    default :
      cout << "usage of CSSLEARN ./csslearn [-iolnsSwprtmdIu]" << endl;
      cout << "-i : set input db file   (default = ./exdata/cnf/hoge.cnf)" << endl;
      cout << "-o : set input prob.file (default = ./exdata/cnf/hoge.prb)" << endl;
      cout << "-l : set slifilename     (default = result.sli)" << endl;
      cout << "-n : set walkmax         (default = 1000)" << endl;
      cout << "-s : set num sample      (default = 1000)" << endl;
      cout << "-S : set seed            (default = NULL)" << endl;
      cout << "-p : set walksatProb     (default = 0.5)" << endl;
      cout << "-r : set randomwalkProb  (default = 0.5)" << endl;
      cout << "-t : set temperature     (default = 0.1)" << endl;
      cout << "-m : set SampleSAT mode  (default = cssamp)" << endl;
      cout << "-c : set rejection sampling mode (default = false)" << endl;
      cout << "-d : set CD mode         (default = false)" << endl;
      cout << "-I : set ita             (default = 1.0e-4)" << endl;
      cout << "-u : set maxiteration    (default = 100)" << endl;
      cout << "-O : set output file name(default = result.dat)" << endl;
      return 1;
    }

  // input and reserve settings
  if(cnffilename == NULL){
    cout << "set cnffilename in '-i' option! " << endl; exit(1);
  }
  if(probfilename == NULL){
    cout << "set probfilename in '-f' option! " << endl; exit(1);
  }

  if (cdebug == true){
    cout << "## end reading inputs" << endl;
    cout << "## start setting parameters"<< endl;
  }

  samplesatparams* ssparams  = new samplesatparams;
  if (ssparams == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}
  ssparams->walkmax        = walkmax;
  ssparams->walksatProb    = walksatProb;
  ssparams->randomWalkProb = randomWalkProb;
  ssparams->temperature    = temperature;
  ssparams->seed           = seed;

  learnparams* lparams  = new learnparams;
  if (lparams == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}
  lparams->ita       = ita;
  lparams->maxiter   = maxiter;
  lparams->numsample = learnsample;
  lparams->burnsteps = learnsample/2;

  if (cdebug == true){
    cout << "## end setting parameters " << endl;
  }

  if (cdebug == true){
    cout << "## define satproblem" << endl;
  }

  /// define sat problem ///
  SatProblem* satproblem = new SatProblem(seed, csmode);
  if (satproblem == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}
  satproblem->parseCnfFile(cnffilename);
  satproblem->parseProbFile(probfilename);
  datasize = satproblem->parseSliFile(slifilename);

  /// paramter/vector/matrix settings ///
  numSample = 0;
  idx = 1;
  int* sample = new int[satproblem->getNumVariables()];
  if (sample == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}
  for(i = 0; i < satproblem->getNumVariables(); i++){
    sample[i] = -1;
  }

  /// montecarlo mode ///
  if(montecarlo){
    satproblem->montecarlo(sample);
    delete [] sample;
    delete satproblem;
    delete ssparams;
    delete lparams;
    return 0;
  }

  /// define samplesat ///
  if (cdebug == true){
    cout << "## define samplesat" << endl;
  }
  SampleSAT* samplesat = new SampleSAT(satproblem, ssparams, csmode, lparams);
  if (samplesat == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}

  if (cdebug == true){
    cout << "## print parameters" << endl;
  }
  printssParameters(ssparams);
  printlParameters(lparams);

  /// define slice sampling ///
  if (cdebug == true){
    cout << "## start SLICE SAMPLING";
  }
  SliceSampling* slicesampling = new SliceSampling(samplesat, satproblem, lparams);
  if (slicesampling == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}

  cout << "###       result(time)       ###" << endl;
  cout << "initial settings       : ";
  Timer::printTime(cout, timer.time() - startSec);
  cout << endl;

  /// learn ///
  double lrntime;
  printf("learning..\r");
  fflush(stdout);
  if (csmode){ // learn by slice sampling (proposal method)
    lrntime = timer.time();
    slicesampling->learn(sample, slifilename, datasize, cdmode, outfilename);
    cout << "total time             : ";
    Timer::printTime(cout, timer.time() - lrntime);
    cout << endl;
    slicesampling->printUnitTime();
  }else{ // learn by samplesat (previous method)
    lrntime = timer.time();
    samplesat->learn(sample, slifilename, datasize, outfilename);
    cout << "total time             : ";
    Timer::printTime(cout, timer.time() - lrntime);
    cout << endl;
    cout << endl;
  }

  slicesampling->calcFixed(lparams);
  samplesat->calcAve(lparams);

  // deconstructors
  delete slicesampling;
  delete samplesat;
  delete [] sample;
  delete satproblem;
  delete ssparams;
  delete lparams;
  //muntrace();
  return 0;
}

void printssParameters(samplesatparams* ssparams)
{
  cout << "###    parameter settings    ###" << endl;
  cout << "walkmax         : " << ssparams->walkmax << endl;
  cout << "walksatProb     : " << ssparams->walksatProb << endl;
  cout << "randomWalkProb  : " << ssparams->randomWalkProb << endl;
  cout << "temperature     : " << ssparams->temperature << endl;
  cout << "seed            : " << ssparams->seed  << endl;
  if(csmode){
    printf("approx. method  : CS sampling+MA\n");
  }else{
    printf("approx. method  : SampleSAT+IS\n");
  }
  if(cdmode){
    printf("cdmode          : true\n");
  }else{
    printf("cdmode          : false\n");
  }
}

void printlParameters(learnparams* lparams)
{
  cout << "learning const. : " << lparams->ita << endl;
  cout << "maxiter         : " << lparams->maxiter << endl;
  cout << "num sample      : " << lparams->numsample << endl;
  cout << "burnsteps       : " << lparams->burnsteps  << endl;
  cout << endl;
}

void printModel(ostream &out, int* model, SatProblem* sat)
{
  int i;
  for (i = 0; i < sat->getNumVariables(); i++){
    out << model[i];
    if(i != sat->getNumVariables()-1){ out << ",";}
  }
  out << endl;
}

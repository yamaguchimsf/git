/***  marginal.cpp***/

#include "satproblem.h"
#include "samplesatparams.h"
#include "samplesat.h"
#include "slicesampling.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <mcheck.h>
#include "timer.h"
#include "random.h"
#include "marginal.h"

int main(int argc, char** argv)
{
  //mtrace();
  int option;
  int sa;
  int i;
  unsigned long seed  = (unsigned long) time(NULL);
  ofstream prbFile;
  ofstream outFile;
  Timer timer;
  double startSec = timer.time();
  double begSec;

  if (cdebug == true){
    cout << "## reading inputs"<< endl;
  }
  while (( option = getopt(argc, argv, "ionsprtmSfl")) != -1)
    switch(option){
    case 'i' :
      inCnfFile = argv[optind];
      break;
    case 'o' :
      inProbFile = argv[optind];
      break;
    case 'n' :
      walkmax = atoi(argv[optind]);
      break;
    case 's' :
      numSample = atoi(argv[optind]);
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
    case 'S' :
      seed = (unsigned long)atol(argv[optind]);
      break;
    case 'f' :
      prbfilename = argv[optind];
      break;
    case 'l' :
      outfilename = argv[optind];
      break;
    default :
      cout << "usage of MARGINAL ./marginal [-ionsprtmSflAC]" << endl;
      cout << "-i : set input db file   (default = ./exdata/cnf/hoge.cnf)" << endl;
      cout << "-o : set input prob.file (default = ./exdata/cnf/hoge.prb)" << endl;
      cout << "-n : set walkmax         (default = 1000)" << endl;
      cout << "-s : set numSample       (default = 1000)" << endl;
      cout << "-p : set walksatProb     (default = 0.5)" << endl;
      cout << "-r : set randomwalkProb  (default = 0.5)" << endl;
      cout << "-t : set temperature     (default = 0.1)" << endl;
      cout << "-m : set csmode          (default = true)" << endl;
      cout << "-S : set seed            (default = NULL)" << endl;
      cout << "-f : set prbfilename     (default = result.prb)" << endl;
      cout << "-l : set slifilename     (default = result.sli)" << endl;
      return 1;
    }

  if(inCnfFile == NULL){
    inCnfFile = (char*)"./dice.cnf";
  }
  if(inProbFile == NULL){
    inProbFile = (char*)"./dice.prb";
  }

  if (cdebug == true){
    cout << "## end reading inputs" << endl;
    cout << "## start setting parameters"<< endl;
  }
  samplesatparams* ssparams  = new samplesatparams;
  if (ssparams == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}
  ssparams->walkmax          = walkmax;
  ssparams->numSample        = numSample;
  ssparams->walksatProb      = walksatProb;
  ssparams->randomWalkProb   = randomWalkProb;
  ssparams->temperature      = temperature;
  ssparams->seed             = seed;

  if (cdebug == true){
    cout << "## end setting parameters " << endl;
  }

  if (cdebug == true){
    cout << "## define satproblem" << endl;
  }

  /// define sat problem for CS sampling ///
  SatProblem* satproblem = new SatProblem(seed, csmode);
  if (satproblem == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}
  satproblem->parseCnfFile(inCnfFile);
  if(prbfilename == NULL){
    prbfilename = (char *)"result.prb";
  }

  prbFile.open(prbfilename);
  if (!prbFile){
    cerr << "can't open prbFile."<< endl;
    return 1;
  }
  satproblem->parseProbFile(inProbFile, prbFile);
  prbFile.close();

  // paramter/vector/matrix settings
  numSample = 0;
  idx = 1;
  int* sample = new int[satproblem->getNumVariables()];
  if (sample == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}
  for(i = 0; i < satproblem->getNumVariables(); i++){
    sample[i] = -1;
  }

  /// define SampleSAT ///
  if (cdebug == true){
    cout << "## define samplesat" << endl;
  }
  SampleSAT* samplesat = new SampleSAT(satproblem, ssparams, csmode);
  if (samplesat == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}

  if (cdebug == true){
    cout << "## print parameters" << endl;
  }
  printParameters(ssparams);

  printf("////////////////////////////////\n");
  printf("///    mode : CS sampling    ///\n");
  printf("////////////////////////////////\n");
  /// define slice sampling ///
  if (cdebug == true){
    cout << "## start SLICE SAMPLING";
  }
  SliceSampling* slicesampling = new SliceSampling(samplesat, satproblem);
  if (slicesampling == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}

  cout << endl;
  cout << "###       result(time)       ###" << endl;
  cout << "initial settings     : ";
  Timer::printTime(cout, timer.time() - startSec);
  cout << endl;

  slisamples = new int* [ssparams->numSample];
  for(int j = 0; j < ssparams->numSample; j++){
    slisamples[j] = new int[satproblem->getNumVariables()];
  }

  begSec = timer.time();
  for(sa = 0; sa < ssparams->numSample; sa++){
    slicesampling->getSample(satproblem, sample);
    for(int j = 0; j < satproblem->getNumVariables(); j++){
      slisamples[sa][j] = sample[j];
    }
  }

  cout << "total time           : ";
  Timer::printTime(cout, timer.time() - begSec);
  cout << endl;
  slicesampling->printUnitTime();
  cout << endl;
  cout << "###      result(formula)     ###" << endl;
  slicesampling->calcFixed(ssparams->numSample);
  if (cdebug == true){
    cout << "## end SLICE SAMPLING" << endl << flush;
    cout << "## start output " << endl << flush;
  }
  samplesat->calcAve();

  cout << endl;
  printf("////////////////////////////////\n");
  printf("///     mode : SampleSAT     ///\n");
  printf("////////////////////////////////\n");
  csmode = false;
  startSec = timer.time();

  /// define sat problem for SampleSAT ///
  SatProblem* satproblem2 = new SatProblem(seed, csmode);
  if (satproblem2 == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}
  satproblem2->parseCnfFile(inCnfFile);
  if(prbfilename == NULL){
    prbfilename = (char *)"result.prb";
  }

  prbFile.open(prbfilename);
  if (!prbFile){
    cerr << "can't open prbFile."<< endl;
    return 1;
  }
  satproblem2->parseProbFile(inProbFile, prbFile);
  prbFile.close();

  // paramter/vector/matrix settings
  numSample = 0;
  idx = 1;
  if (cdebug == true){
    cout << "## define samplesat" << endl;
  }

  /// define SampleSAT 2 ///
  SampleSAT* samplesat2 = new SampleSAT(satproblem2, ssparams, csmode);
  if (samplesat2 == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}

  samsamples = new int* [ssparams->numSample];
  for(int j = 0; j < ssparams->numSample; j++){
    samsamples[j] = new int[satproblem2->getNumVariables()];
  }

  cout << "###       result(time)       ###" << endl;
  cout << "initial settings     : ";
  Timer::printTime(cout, timer.time() - startSec);
  cout << endl;

  begSec = timer.time();
  for(sa = 0; sa < ssparams->numSample; sa++){
    samplesat2->solve(satproblem2, sample);
    for(int j = 0; j < satproblem2->getNumVariables(); j++){
      samsamples[sa][j] = sample[j];
    }
  }

  cout << "total time           : ";
  Timer::printTime(cout, timer.time() - begSec);
  cout << endl;
  cout << endl;
  cout << "###      result(formula)     ###" << endl;
  samplesat2->calcAve();
  cout << endl;

  printf("////////////////////////////////\n");
  printf("///       calc marginal      ///\n");
  printf("////////////////////////////////\n");
  satproblem->calcHL(slisamples, samsamples, ssparams->numSample, outfilename);

  // deconstructors
  delete slicesampling;
  delete samplesat2;
  delete samplesat;
  delete[] sample;
  for(int j = 0; j < ssparams->numSample; j++){
    delete[] samsamples[j];
  }
  delete[] samsamples;
  for(int j = 0; j < ssparams->numSample; j++){
    delete[] slisamples[j];
  }
  delete[] slisamples;
  delete satproblem2;
  delete satproblem;
  delete ssparams;
  //muntrace();
  return 0;
}

void printParameters(samplesatparams* ssparams)
{
  cout << "###    parameter settings    ###" << endl;
  cout << "walkmax        : " << ssparams->walkmax << endl;
  cout << "num sample     : " << ssparams->numSample << endl;
  cout << "walksatProb    : " << ssparams->walksatProb << endl;
  cout << "randomWalkProb : " << ssparams->randomWalkProb << endl;
  cout << "temperature    : " << ssparams->temperature << endl;
  cout << "seed           : " << ssparams->seed  << endl;
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

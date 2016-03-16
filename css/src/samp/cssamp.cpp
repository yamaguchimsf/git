/***  cssamp.cpp***/

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
#include "cssamp.h"

int main(int argc, char** argv)
{
  cout << "////////////////////////////////" << endl;
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
  while (( option = getopt(argc, argv, "ionsprtmSflAC")) != -1)
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
      cout << "usage of CSSAMP ./cssamp [-ionsprtmSflAC]" << endl;
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

  // input and reserve settings
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

  /// define sat problem ///
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

  // paramter/vector/matrix settings ///
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

  if(outfilename == NULL){
    if(csmode){
      outfilename = (char *)"result.sli";
    }else{
      outfilename = (char *)"result.sam";
    }
  }
  outFile.open(outfilename);
  if (!outFile){
    cerr << "can't open output file."<< endl;
    return 1;
  }

  /// sampling ///
  begSec = timer.time();
  for(sa = 0; sa < ssparams->numSample; sa++){
    if(csmode){
      slicesampling->getSample(satproblem, sample);
      printModel(outFile, sample, satproblem);
    }else{
      samplesat->solve(satproblem, sample);
      printModel(outFile, sample, satproblem);
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
  cout << "////////////////////////////////" << endl;
  outFile.close();

  // deconstructors
  delete slicesampling;
  delete samplesat;
  delete[] sample;
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
  if(csmode){
    printf("mode           : CS sampling\n");
  }else{
    printf("mode           : sampleSAT\n");
  }
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

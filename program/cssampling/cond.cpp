/***  cond.cpp***/
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
#include "cond.h"
#include "math.h"

#define BUFSIZE 51200

int main(int argc, char** argv)
{
  printf("## COND ##\n");
  int option;
  //int sa;
  int i;

  unsigned long seed  = (unsigned long) time(NULL);

  //Random rand;
  Timer timer;
  double startSec = timer.time();
  //double begSec;

  if (cdebug == true){
    cout << "## reading inputs"<< endl;
  }
  while (( option = getopt(argc, argv, "sp")) != -1)
    switch(option){
    case 's' :
      slifilename = argv[optind];
      break;
    case 'p' :
      probfilename = argv[optind];
      break;
    default :
      cout << "-s : set slifilename     (default = result.sli)" << endl;
      return 1;
    }

  if(probfilename == NULL){
    cout << "set probfilename in '-p' option! " << endl; exit(1);
  }
  if(slifilename == NULL){
    cout << "set slifilename in '-s' option! " << endl; exit(1);
  }

  if (cdebug == true){
    cout << "## end reading inputs" << endl;
    cout << "## start setting parameters"<< endl;
  }

  int numVariables_ = 6;
  int numClauses_ = 16;
  int line = 0;
  ifstream ifile_;
  char buffer_[BUFSIZE];
  char c_;
  double* realProb = new double[numVariables_];
  double* dd = new double[numVariables_];
  double* rd = new double[numVariables_];
  int idx = 0;
  char delims[] = ",";
  char* token_;
  char* buffp_;
  int sampleSize = 0;
  double lik = 0;

  for(int i = 0; i < numVariables_ ; i++){
    realProb[i] = 0.5;
    dd[i] = 0;
    rd[i] = 1;
  }

  ifile_.open(probfilename);
  if (!ifile_){
    cerr << "cannot open input file!"<< endl;
    cerr << probfilename << endl; exit(1);
  }else{
    // get the arguments from the file.
    while (ifile_.good()){
      ifile_.get(buffer_,51200,'\n'); // get a clause
      if (ifile_.get(c_) && c_ != '\n'){
        // input string is longer than expected.
        cout << "too big file1!" << endl;
        return 0;
      }
      realProb[line] = atof(buffer_);
      line++;
    }
  }
  ifile_.close();

  line=0;
  printf("## COND ##\n");
  ifile_.open(slifilename);
  if (!ifile_){
    cerr << "cannot open input file!"<< endl;
    cerr << slifilename << endl; exit(1);
  }else{
    // get the arguments from the file.
    while (ifile_.good()){
      ifile_.get(buffer_,51200,'\n'); // get a clause
      if (ifile_.get(c_) && c_ != '\n'){
        // input string is longer than expected.
        cout << "too big file1!" << endl;
        return 0;
      }

      buffp_ = buffer_;
      token_ = strtok(buffp_, delims);
      while(token_ != NULL){
        dd[idx] += atoi(token_);
        printf("%d,",atoi(token_));
        lik += atoi(token_)*log(realProb[idx])+(1-atoi(token_))*log(1-realProb[idx]);
        printf("token = %f\n",atoi(token_)*log(realProb[idx])+(1-atoi(token_))*log(1-realProb[idx]));
        token_ = strtok(NULL, delims);
        idx++;
      }
      printf("\n");
      idx = 0;
      line++;
    }
    sampleSize = line-1;
  }
  printf("real_lik = %f\n",lik/1000);

  for(int i = 0; i < numVariables_; i++){
    dd[i] /= sampleSize;
    printf("dd[%d]=%f\n",i,dd[i]);
    for(int j = 0; j < numVariables_; j++){
      if(j == i){
        rd[i] *= realProb[j];
      }else{
        rd[i] *= 1-realProb[j];
      }
    }
  }

  printf("\n");
  double sum = 0;
  for(int i = 0; i < numVariables_; i++){
    sum += rd[i];
  }
  printf("sum=%f\n",sum);

  for(int i = 0; i < numVariables_; i++){
    rd[i] /= sum;
    printf("mp[%d]=%f\n",i,rd[i]);
  }

  sum = 0;
  for(int i = 0; i < numVariables_; i++){
    sum += rd[i];
  }
  printf("sum=%f\n",sum);

  for(int i = 0; i < numVariables_; i++){
    printf("realProb[%d]=%f\n",i,realProb[i]);
  }

  ifile_.close();
  /*
  // paramter/vector/matrix settings
  numSample = 0;
  idx = 1;

  int* sample = new int[satproblem->getNumVariables()];
  if (sample == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}

  for(i = 0; i < satproblem->getNumVariables(); i++){
    sample[i] = -1;
    //sample[i] = 0;
  }

  if (cdebug == true){
    cout << "## define samplesat" << endl;
  }

  SampleSAT* samplesat = new SampleSAT(satproblem, ssparams);
  if (samplesat == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}

  if (cdebug == true){
    cout << "## print parameters" << endl;
  }
  printssParameters(ssparams);
  printlParameters(lparams);

  if (cdebug == true){
    cout << "## start SLICE SAMPLING";
  }

  SliceSampling* slicesampling = new SliceSampling(samplesat, satproblem, lparams);
  if (slicesampling == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}

  cout << "CSSAMP:: initial settings took ";
  Timer::printTime(cout, timer.time() - startSec);
  cout << endl;

  double lrntime = timer.time();
  slicesampling->learn(sample, slifilename, datasize);

  cout << "CSSLEARN:: total parameter learning function took ";
  Timer::printTime(cout, timer.time() - lrntime);
  cout << endl;
  */

  //delete dd;
  //delete realProb;

  return 0;
}

void printssParameters(samplesatparams* ssparams)
{
  cout << "walkmax = " << ssparams->walkmax << endl;
  cout << "numSample = " << ssparams->numSample << endl;
  cout << "walksatProb = " << ssparams->walksatProb << endl;
  cout << "randomWalkProb = " << ssparams->randomWalkProb << endl;
  cout << "temperature = " << ssparams->temperature << endl;
  cout << "seed = " << ssparams->seed  << endl;
}

void printlParameters(learnparams* lparams)
{
  cout << "ita = " << lparams->ita << endl;
  cout << "maxiter = " << lparams->maxiter << endl;
  cout << "numsample = " << lparams->numsample << endl;
  cout << "burnsteps = " << lparams->burnsteps  << endl;
  if(distmode){
    cout << "distmode = true" << endl;
  }else{
    cout << "distmode = false" << endl;
  }
  if(cdmode){
    cout << "cdmode = true" << endl;
  }else{
    cout << "cdmode = false" << endl;
  }
}

void printModel(ostream &out, int* model, int numVariables)
{
  int i;
  for (i = 0; i < numVariables; i++){
    out << model[i];
    if(i != numVariables-1){ out << ",";}
  }
  out << endl;
}

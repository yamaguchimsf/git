/*** slicesampling.cpp ***/

#include "slicesampling.h"

SliceSampling::SliceSampling(SampleSAT* samplesat, SatProblem* sat)
{
  if(sldebug == true){
    cout << "# start Slice Constructor " << endl;
  }
  sat_ = sat;
  samplesat_ = samplesat;
  sumSec1 = 0;
  sumSec2 = 0;
  sumSec3 = 0;
  sumSec4 = 0;
  sumSec5 = 0;
  aveFixedAtom = 0;
  aveFixedClause = 0;
  init();
  if(sldebug == true){
    cout << "# end Slice Constructor " << endl;
  }
}

SliceSampling::~SliceSampling()
{
  delete [] u;
}

void SliceSampling::init()
{
  int i;
  int val = sat_->getNumVariables();
  u = new double[val];
  if (u == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}
  for(i = 0; i < val ; i++){
    u[i] = 1.0;
  }
}

void SliceSampling::getSample(SatProblem* sat, int* sample)
{
  if(sldebug == true){
    printf("in Slice\n");
  }
  sat_ = sat;
  unitSec3 = timer.time();
  x = sample;
  if(x[0] == -1){
    sat_->initParams();
    sat_->cpClVector();
    unitSec5 = timer.time();
    samplesat_->solve(sat_, x);
    sumSec5 += timer.time() - unitSec5;
    samplesat_->initAve();
  }
  if(sldebug == true){
    if(sat_->isSat(x) == true)
      cout << "true\t";
    else cout << "false\t";
    cout << " : x = ";
    printModel(x);
  }
  sampleU();
  sampleX();
  sumSec3 += timer.time() - unitSec3;
  //Timer:: printTime(cout, timer.time() - unitSec);
  //cout << endl;
}

void SliceSampling::sampleU()
{
  if(sldebug == true){
    cout << "sampleU" << endl;
  }
  int i;
  double prob;
  for(i = 0; i < sat_->getNumVariables(); i++){
    if(x[i] == 1){
      prob = sat_->getVarProbi(i);
      if(sldebug == true){
        printf("%.2d : %d prob1 = %.8f", i, x[i], prob);
      }
    }else{
      prob = 1 - sat_->getVarProbi(i);
      if(sldebug == true){
        printf("%.2d : %d prob0 = %.8f", i, x[i], prob);
      }
    }
    u[i] = prob * sat_->getGrand();
    if(sldebug == true){
      cout << " u =" << u[i] << endl;
    }
  }
}

void SliceSampling::sampleX()
{
  if(sldebug == true){
    cout << "sampleX" << endl;
  }
  int i, id;
  double prob1, prob0;
  unitSec2 = timer.time();
  sat_->initParams();
  //sat_->showp();
  sumSec2 += timer.time() - unitSec2;
  for(i = 0; i < sat_->getNumVariables(); i++){
    prob1 = sat_->getVarProbi(i);
    prob0 = 1 - sat_->getVarProbi(i);

    id = i + 1;
    if(sldebug == true){
      printf("%.2d : 0 = %.8f, 1 = %.8f", i, prob0, prob1);
      //cout << " : 0 = " << prob0 << ": 1 = " <<prob1;
    }

    if(prob0 < u[i] && u[i] < prob1){
      if(sldebug == true){
        cout << " var[" << i << "] = 1";
      }
      unitProc(i, 1);
    }
    if(prob1 < u[i] && u[i] < prob0){
      if(sldebug == true){
        cout << " var[" << i << "] = 0";
      }
      unitProc(i, 0);
    }
    if(sldebug == true){
      cout << endl;
    }
  }
  if(sldebug == true){
    sat_->show();
  }
  unitSec1 = timer.time();
  aveFixedAtom += (double)sat_->getNumFixedAtom();
  aveFixedClause += (double)sat_->getNumFixedClause();
  //setClauseValue
  samplesat_->solve(sat_, x);
  sumSec1 += timer.time() - unitSec1;
}

void SliceSampling::unitProc(int var, int val)
{
  unitSec4 = timer.time();
  sat_->setAtom(var, val);
  sat_->setClause(var, val);
  sat_->rewriteWL(var, 1 - val);
  //sat_->showp();
  sumSec4 += timer.time() - unitSec4;
}

void SliceSampling::printModel(int* model)
{
  int i;
  cout << "[";
  for(i = 0; i < sat_->getNumVariables(); i++){
    cout << model[i];
    if(i != sat_->getNumVariables()-1){ cout << ",";}
  }
  cout << "]" << endl;
}

void SliceSampling::printUnitTime()
{
  cout << "( SampleSAT time       : ";
  Timer::printTime(cout, sumSec1);
  cout << ")" <<endl;
  cout << "( init parameter time  : ";
  Timer::printTime(cout, sumSec2);
  cout << ")" <<endl;
  cout << "( unit prop. time      : ";
  Timer::printTime(cout, sumSec4);
  cout << ")" <<endl;
  cout << "( 1-st SampleSAT time  : ";
  Timer::printTime(cout, sumSec5);
  cout << ")" <<endl;
  cout << "total sampling time  : ";
  Timer::printTime(cout, sumSec3);
  cout << endl;
}

void SliceSampling::calcFixed(int numSample)
{
  int val = sat_->getNumVariables();
  aveFixedAtom /= (double)numSample;
  cout << "average fixed atom     : " << aveFixedAtom / val * 100 << "% ( ";
  cout << aveFixedAtom << " / " << val << " )" << endl;
  aveFixedClause /= (double)numSample;
  val = sat_->getNumClauses();
  cout << "average fixed clause   : " << aveFixedClause / val * 100 << "% ( ";
  cout << aveFixedClause << " / " << val << " )" << endl;
}

/*** samplesat.cpp ***/

#include "samplesat.h"

//using namespace std;
//const bool ssdebug = true;

SampleSAT::SampleSAT(SatProblem* sat, samplesatparams* ssparams, bool csmode)
{
  sat_ = sat;
  csmode_ = csmode;
  ssparams_ = ssparams;
  setWalkMax(ssparams_->walkmax);
  setNumSample(ssparams_->numSample);
  setWalkSatProb(ssparams_->walksatProb);
  setRandomWalkProb(ssparams_->randomWalkProb);
  setTemperature(ssparams_->temperature);
  numAtom   = sat_->getNumVariables();
  numClause = sat_->getNumClauses();
  numSample = 0;
  stepAve = 0;
}

SampleSAT::~SampleSAT()
{
}

void SampleSAT::solve(SatProblem* sat, int* sample)
{
  sat_ = sat;
  model = sample;

  if(!csmode_){
    //only SampleSAT
    sat_->initParams();
  }

  sat_->cpClVector();
  //sat_->printClVec();
  init_model();
  numRW = 0; numGW = 0; numSA = 0;
  step_ = 0;

  //printModels();
  //if(sat_->isSat(model)){
  if(sat_->initSatCheck(model)){
    stepAve += (double)step_;
    return;
  }
  while(step_ < ssparams_->walkmax && !walk());

  if(ssdebug == true){
    if (step_ < ssparams_->walkmax) cout << "SAT!!"<< endl;
    else cout << "UNKNOWN""!!"<< endl;
    cout << "step  = " << step_ << endl;
    cout << "numRW = " << numRW << endl;
    cout << "numGW = " << numGW << endl;
    cout << "numSA = " << numSA << endl;
  }

  if (step_ >= ssparams_->walkmax){
    printf("NOT satisfiable\n");
    exit(1);
  }

  stepAve += (double)step_;

  return;
}

void SampleSAT::init_model()
{
  int i;
  for(i = 0; i < numAtom; i++){
    if(sat_->getVar(i) == -1){
      //if(sat_->getGrand() <= sat_->getVarProb(i))
      if(sat_->getGrand() <= 0.5){
        model[i] = 0;
      }else{
        model[i] = 1;
      }
    }else{
      model[i] = sat_->getVar(i);
    }
  }

  if(ssdebug == true){
    cout << "initial model = ";
    printModels();
    cout << "sat? = "<< sat_->isSat(model) << endl;
  }
}

bool SampleSAT::walk()
{
  int ca;
  if(ssdebug == true){
    printf("step:%3d\n",step_);
  }
  step_++;

  //printf("nsc=%d\n",sat_->getNumSat());
  if(sat_->getNumSat() > numClause){
    printf("illigal sat clause: %d / %d\n",sat_->getNumSat(),numClause);
  }
  if(sat_->getGrand() < ssparams_->walksatProb){
    if(sat_->getGrand() < ssparams_->randomWalkProb){
      ca = randomWalk();
      if(ca == -1){
        return false;
      }
    }else ca = greedyWalk();
  }else{
    ca = simAnnealing();
    if(ca == -1){
      return false;
    }
  }

  if(ssdebug == true){
    printModels();
  }
  if(sat_->getNumSat() == numClause){
    //printf("### SAT ###\n");
    return true;
  }
  return false;
}

int SampleSAT::randomWalk()
{
  if(ssdebug == true){
    cout << "rw\n";
  }
  numRW++;
  int i,j = 0, idx = 0;
  int* lit = NULL;
  int* unSatCnj = new int[sat_->getClVecSize()];
  int distance = 0;
  int nsc0 = sat_->getNumSat();
  int nsc1;
  for (i = 0; i < sat_->getClVecSize(); i++){
    unSatCnj[i] = 0;
  }

  for (i = 0; i < sat_->getClVecSize(); i++){
    if (sat_->getFixedClausei(sat_->getClVec(i)) == 0){
      unSatCnj[j] = sat_->getClVec(i);
      if(ssdebug == true){
        cout << "unsat clause = " << unSatCnj[j] << endl;
      }
      j++;
    }
  }

  int roopmax = sat_->getClVecSize()*2;
  int roop = 0;
  while(idx == 0){
    idx = 0;
    if(roop > roopmax){
      delete [] unSatCnj;
      return -1;
    }
    roop++;
    i = (int) (rand() * (double) j / (1.0 + RAND_MAX));
    if(ssdebug == true){
      cout << "num. of unsat clauses = " << j << endl;
      cout << "i = " << i << endl;
      cout << "choose " << unSatCnj[i] << endl;
    }
    lit = new int[sat_->getCnjSize(unSatCnj[i])];
    for (int h = 0; h < sat_->getCnjSize(unSatCnj[i]); h++){
      lit[h] = 0;
    }

    for(int k = 0; k < sat_->getCnjSize(unSatCnj[i]); k++){
      if(sat_->getVar(abs(sat_->getCnj(unSatCnj[i],k+1))-1) == -1){
        lit[idx] = abs(sat_->getCnj(unSatCnj[i],k+1))-1;
        idx += 1;
        if(ssdebug == true){
          printf("%d,",lit[idx-1]);
        }
      }
    }
    if(idx == 0){
      delete [] lit;
    }
  }
  if(ssdebug == true){
    printf("\n");
  }
  j = (int) (rand() * (double) idx / (1.0 + RAND_MAX));
  if(ssdebug == true){
    printf("j = %d, lit[j]=%d\n", j, lit[j]);
  }
  i = lit[j];
  nsc1 = sat_->getNsc1(model, i);
  distance = nsc1 - nsc0;
  sat_->addNumSat(distance);
  delete [] lit;
  delete [] unSatCnj;
  return i;
}

int SampleSAT::greedyWalk()
{
  if(ssdebug == true){
    cout << "gw\n";
    printf("now Nsc = %d\n",sat_->getNumSat());
  }
  numGW++;
  int i, maxId = 0, maxNsc = 0;
  int distance;
  int* largeVar = new int[numAtom];
  int idx = 0, rnd = 0;
  //double rndd;

  for (i = 0; i < numAtom; i++){
    largeVar[i] = -1;
  }
  for (i = 0; i < numAtom; i++){
    if(sat_->getVar(i) != -1)
      continue;
    else{
      nsc_ = sat_->getNsc2(model, i);
      if(ssdebug == true){
        cout << i << "-th atom Nsc = " << nsc_ << endl;
      }
      if (nsc_ == maxNsc){
        largeVar[idx] = i;
        idx++;
      }else if (nsc_ > maxNsc){
        for (int j = 0; j < numAtom; j++){
          largeVar[j] = -1;
        }
        largeVar[0] = i;
        idx = 1;
        maxNsc = nsc_;
        maxId = i;
        distance = nsc_ - sat_->getNumSat();
      }
    }
  }
  rnd = (int) (rand() * (double) idx / (1.0 + RAND_MAX));
  if(ssdebug == true){
    for (i = 0; i < idx; i++){
      cout << "largeVar[" << i << "] = " << largeVar[i] << endl;
    }
    cout << "idx = "   << idx << endl;
    cout << "rnd = "   << rnd << endl;
    cout << "maxNsc= "<< maxNsc << endl;
    cout << "maxId = " << largeVar[rnd]  << endl;
    cout << "flipped " << largeVar[rnd]  << endl;
  }
  model[largeVar[rnd]] = 1 - model[largeVar[rnd]];
  sat_->addNumSat(distance);
  sat_->afterFlip(model, largeVar[rnd]);
  delete [] largeVar;
  return maxId;
}

int SampleSAT::simAnnealing()
{
  if(ssdebug == true){
    cout << "sa\n";
  }
  numSA++;
  int i, nsc0, nsc1;
  double r;
  int distance;
  r = exp(-getTemp());
  if(ssdebug == true){
    cout << "acceptance rate : " << r << endl;
  }
  do{
    i = numAtom * sat_->getGrand();
    if(ssdebug == true){
      cout << "choosed "<< i << endl;
    }
  }while(sat_->getVar(i) != -1);
  nsc0 = sat_->getNumSat();
  nsc1 = sat_->getNsc2(model, i);
  distance = nsc1 - nsc0;
  if(ssdebug == true){
    cout << "choose " << i+1 << "-th Atom[1-MAX]" << endl;
    cout << "nsc0 = " << nsc0 << endl;
    cout << "nsc1 = " << nsc1 << endl;
  }
  if (nsc0 < nsc1 || sat_->getGrand() < r){
    if(ssdebug == true){
      cout << "flipped!"<< endl;
    }
    model[i] = 1 - model[i];
    sat_->addNumSat(distance);
    sat_->afterFlip(model, i);
    return i;
  }else if(ssdebug == true){
    cout << "NOT flipped!"<< endl;
  }
  return -1;
}

double SampleSAT::getTemp()
{
  return ssparams_->temperature;
}

void SampleSAT::printModels()
{
  int i;
  cout << "[";
  for(i = 0; i < numAtom; i++){
    cout << model[i];
    if(i != numAtom-1){ cout << ",";}
  }
  cout << "]  ";
}

void SampleSAT::calcAve()
{
  stepAve /= (double)ssparams_->numSample;
  cout << "SampleSAT step average : " << stepAve << endl;
}

void SampleSAT::calcAve2(int maxiter)
{
  stepAve /= (double)ssparams_->numSample*maxiter;
  cout << "SampleSAT step average : " << stepAve << endl;
}

void SampleSAT::initAve()
{
  stepAve = 0;
}

void SampleSAT::setWalkMax(int walkmax)
{
  ssparams_->walkmax = walkmax;
}

void SampleSAT::setNumSample(int numSample)
{
  ssparams_->numSample = numSample;
}

void SampleSAT::setWalkSatProb(double walksatProb)
{
  ssparams_->walksatProb = walksatProb;
}

void SampleSAT::setRandomWalkProb(double randomWalkProb)
{
  ssparams_->randomWalkProb = randomWalkProb;
}

void SampleSAT::setTemperature(double temperature)
{
  ssparams_->temperature = temperature;
}


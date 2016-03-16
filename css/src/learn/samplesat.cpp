/*** samplesat.cpp ***/

#include "samplesat.h"
#include <cstdio>

SampleSAT::SampleSAT(SatProblem* sat, samplesatparams* ssparams, bool csmode, learnparams* lparams)
{
  sat_ = sat;
  ssparams_ = ssparams;
  lparams_ = lparams;
  setWalkMax(ssparams_->walkmax);
  setNumSample(ssparams_->numSample);
  setWalkSatProb(ssparams_->walksatProb);
  setRandomWalkProb(ssparams_->randomWalkProb);
  setTemperature(ssparams_->temperature);
  numAtom   = sat_->getNumVariables();
  numClause = sat_->getNumClauses();
  numSample = 0;
  stepAve = 0;
  csmode_ = csmode;
}

SampleSAT::~SampleSAT()
{
  delete [] ex;
  delete [] dd;
  delete [] sd;
  delete [] weight;
  delete [] realcondprob;
  delete [] key;
  delete [] ad;
  delete [] bd;
  delete [] counts;
  delete [] q_xF;
}

void SampleSAT::solve(SatProblem* sat, int* sample)
{
  sat_ = sat;
  model = sample;

  if(!csmode_){ //only SampleSAT
    sat_->initParams();
  }

  sat_->cpClVector();
  init_model();
  numRW = 0; numGW = 0; numSA = 0;
  step_ = 0;

  if(sat_->initSatCheck(model)){
    stepAve += (double)step_;
    return;
  }
  while(step_ < ssparams_->walkmax && walk() == false);
  if(step_ >= ssparams_->walkmax){
    printf("Walking over\n");
    exit(1);
  }

  if(ssdebug == true){
    if (step_ < ssparams_->walkmax) cout << "SAT!!"<< endl;
    else cout << "UNKNOWN""!!"<< endl;
    cout << "step  = " << step_ << endl;
    cout << "numRW = " << numRW << endl;
    cout << "numGW = " << numGW << endl;
    cout << "numSA = " << numSA << endl;
  }

  stepAve += (double)step_;

  return;
}

void SampleSAT::init_model()
{
  int i;
  for(i = 0; i < numAtom; i++){
    if(sat_->getVar(i) == -1){
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

void SampleSAT::calcAve(learnparams* lparams)
{
  stepAve /= (double)lparams->numsample;
  stepAve /= (double)lparams->maxiter;
  cout << "step average           : " << stepAve << endl;
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

void SampleSAT::learn(int* sample, char* inputfile, int datasize, char* outfilename)
{
  lallocate(datasize);
  readdata(inputfile, datasize);

  double lrntime = timer.time();
  islearn(datasize, outfilename);
  cout << "learning time          : ";
  Timer::printTime(cout, timer.time() - lrntime); cout << endl;

  for(int i = 0; i < datasize; i++){
    for(int j = 0; j < numAtom; j++){
      dd[j] += tdata[i][j];
    }
  }

  for(int j = 0; j < numAtom; j++){
    dd[j] /= datasize;
  }

  /* print learned parameters*/
  /*
  for(int j = 0; j < numAtom; j++){
    printf("theta[%d]=%f\n",j,sat_->getTheta(j));
  } // calcurate sample distribution
  double sumTheta = 0;

  for(int j = 0; j < numAtom; j++){
    sumTheta += sat_->getTheta(j);
  } // calcurate sample distribution

  for(int j = 0; j < numAtom; j++){
    printf("S_theta[%d]=%f\n",j,sat_->getTheta(j)/sumTheta);
  } // calcurate sample distribution
  */

  destlearn(datasize);
}

void SampleSAT::lallocate(int datasize)
{
  int val = numAtom;
  ex = new double[val];
  for(int j = 0; j < val; j++){
    ex[j] = 0;
  }
  dd = new double[val];
  for(int j = 0; j < val; j++){
    dd[j] = 0;
  }

  sd = new double[val];
  for(int j = 0; j < val; j++){
    sd[j] = 0;
  }

  weight = new double[lparams_->numsample];
  for(int j = 0; j < lparams_->numsample; j++){
    weight[j] = 0;
  }

  key = new int[datasize];

  realcondprob = new double[val];
  for(int j = 0; j < val; j++){
    realcondprob[j] = 0;
  }

  tdata = new int*[datasize];
  for(int j = 0; j < datasize; j++){
    tdata[j] = new int[val];
  }
  sbuf = new int*[lparams_->numsample];
  for(int j = 0; j < lparams_->numsample; j++){
    sbuf[j] = new int[val];
  }

  for(int j = 0; j < datasize; j++){
    for(int k = 0; k < val; k++){
      tdata[j][k] = 0;
    }
  }

  xx = new int[val];
  for(int j = 0; j < val; j++){
    xx[j] = 0;
  }
}

void SampleSAT::readdata(char* inputfile, int datasize)
{
  int idx = 0;
  char delims[] = ",";
  char* token;
  int line = 0;
  ifile.open(inputfile);
  if (!ifile){
    cerr << "cannot open input file!"<< endl;
    cerr << inputfile << endl;
    exit(1);
  }else{
    while (ifile.good()){
      ifile.get(buffer, 51200,'\n'); // get a sample
      if (ifile.get(c) && c != '\n'){
        cout << "too big file!" << endl;
        return ;
      }

      token = strtok(buffer, delims);
      while(token != NULL){
        tdata[line][idx] = atoi(token);
        token = strtok(NULL, delims);
        idx++;
      }
      idx=0;
      line++;
    }
  }
  ifile.close();

  bd = new double[lparams_->numsample];
  for(int j = 0; j < lparams_->numsample; j++){
    bd[j] = 0;
  }

  datasize_t = 0;
  for(int i = 0; i < datasize; i++){
    if(modelcheck(i) == true){
      datasize_t++;
    }
  }

  sdata = new int*[datasize_t];
  for(int j = 0; j < datasize_t; j++){
    sdata[j] = new int[numAtom];
  }
  for(int j = 0; j < datasize_t; j++){
    for(int k = 0; k < numAtom; k++){
      sdata[j][k] = 0;
    }
  }

  ad = new double[datasize_t];
  for(int j = 0; j < datasize_t; j++){
    ad[j] = 0;
  }

  counts = new double[datasize_t];
  for(int j = 0; j < datasize_t; j++){
    counts[j] = 0;
  }

  q_xF = new double[datasize_t];
  for(int j = 0; j < datasize_t; j++){
    q_xF[j] = 0;
  }

  idx = 0;
  for(int i = 0; i < datasize; i++){
    if(modelcheck(i)){
      for(int k = 0; k < numAtom; k++){
        sdata[idx][k] = tdata[i][k];
      }
      idx++;
    }
  }

  nmodel = new int*[datasize_t];
  for(int j = 0; j < datasize_t; j++){
    nmodel[j] = new int[numAtom];
  }
  for(int j = 0; j < datasize_t; j++){
    for(int k = 0; k < numAtom; k++){
      nmodel[j][k] = 0;
    }
  }
}

void SampleSAT::islearn(int datasize, char* outfilename)
{
  int iteration = 0;  // iteration counter
  double drand;       // double  type random variable
  int irand;          // integer type random variable
  double lik;         // likelihood
  double stSec1;
  double stSec2;
  double stSec3;
  int step = 1;       // length of Markov chain
  cdsdSec = 0;
  cdddSec = 0;
  cntSec  = 0;
  double pF = 0;
  double adlogmax;
  double logsum;
  int admaxidx;
  double hl_ss;
  double temp;
  bool flag;
  FILE* helFile = fopen(outfilename, "w");

  while(iteration < lparams_->maxiter){
    iteration++;

    if(iteration == 1){
      for(int j = 0; j < numAtom; j++){
        sd[j] = 0;
      }
    }
    stSec1 = timer.time();

    /////  sampling routine ////
    for(int i = 0; i < lparams_->numsample; i++){
      drand = sat_->getGrand()*datasize;
      irand = drand; // get random integers in (0~datasize)
      for(int j = 0; j < numAtom; j++){
        xx[j] = tdata[irand][j];
      } // get random one training data (one model)
      for(int j = 0; j < step; j++){
        solve(sat_, xx);
      }
      for(int j = 0; j < numAtom; j++){
        sbuf[i][j] = xx[j];
      }
      for(int j = 0; j < numAtom; j++){
        if(xx[j] == 1){
          weight[i] += log(sat_->getTheta(j));
        }else{
          weight[i] += log(1-sat_->getTheta(j));
        }
      }
    }
    normalize(weight, lparams_->numsample);

    for(int j = 0; j < numAtom; j++){
      sd[j] += weight[j]*(xx[j])/(sat_->getTheta(j)) - weight[j]*(1 - xx[j])/(1 - sat_->getTheta(j));
    } //sum sample counts
    cdsdSec += timer.time() - stSec1;

    ///  calc. q(\x|F) from local samples ///
    for(int j = 0; j < datasize_t; j++){
      counts[j] = 0;
    }

    stSec3 = timer.time();

    for(int i = 0; i < datasize_t; i++){
      for(int j = 0; j < numAtom; j++){
        if(sdata[i][j] == 1){
          q_xF[i] += log(sat_->getRealProb(j));
        }else{
          q_xF[i] += log(1-sat_->getRealProb(j));
        }
      }
    }
    normalize(q_xF, datasize_t);
    cntSec += timer.time() - stSec3;

    stSec2 = timer.time();
    for(int i = 0; i < datasize; i++){
      for(int j = 0; j < numAtom; j++){
        dd[j] += (tdata[i][j])/(sat_->getTheta(j)) - (1 - tdata[i][j])/(1 - sat_->getTheta(j));
      }
    } //calc dd
    for(int j = 0; j < numAtom; j++){
      dd[j] /= datasize;
    }
    cdddSec += timer.time() - stSec2;

    for(int j = 0; j < numAtom; j++){
      ex[j] = lparams_->ita*(dd[j] - sd[j]);
    } // calc ex

    sat_->updateTheta(ex); //update theta
    pF=0;
    logsum = 0;
    adlogmax = -100000;
    admaxidx = 0;
    lik=0;

    // calc approximated p(x|F) from "samples"
    stSec3 = timer.time();
    for(int j = 0; j < datasize_t; j++){
      ad[j] = 0;
    }

    for(int i = 0; i < datasize_t; i++){
      for(int j = 0; j < numAtom; j++){
        if(sdata[i][j] == 1){
          ad[i] += log(sat_->getTheta(j));
        }else{
          ad[i] += log(1-sat_->getTheta(j));
        }
      }
    }
    normalize(ad,datasize_t);

    /// calc HL[ad,q_xF] ///
    hl_ss = 0;
    for(int i = 0; i < datasize_t; i++){
      flag = false;
      temp = sqrt(ad[i]) - sqrt(q_xF[i]);
      if(temp > 1.0e-6){
        hl_ss += temp*temp;
      }
      temp = sqrt(1-ad[i]) - sqrt(1-q_xF[i]);
      if(temp > 1.0e-6){
        hl_ss += temp*temp;
      }
    }
    hl_ss = sqrt(hl_ss);
    fprintf(helFile,"%11.5e\n",hl_ss);
    cntSec += timer.time() - stSec3;

    //lik = calclik(ad); //calcurate log-likelihood
    //printf("lik=%.15f\n",lik);
    for(int j = 0; j < numAtom; j++){
      sd[j] = 0;
      dd[j] = 0;
    }
  }
  cout << "calc exp. time(sample) : ";
  Timer::printTime(cout, cdsdSec);
  cout << endl;
  cout << "calc exp. time(data)   : ";
  Timer::printTime(cout, cdddSec);
  cout << endl;
  cout << "calc q(x|F) & HD time  : ";
  Timer::printTime(cout, cntSec);
  cout << endl;
}

double SampleSAT::calclik(double *rd)
{
  double lik = 0;
  for(int i = 0; i < datasize_t; i++){
    lik += log(rd[i]);
  }
  lik /= (double)datasize_t;
  return lik;
}

double SampleSAT::getProb(int* sample)
{
  double prob = 1;
  for(int j = 0; j < numAtom; j++){
    if(sample[j] == 1){
      prob *= sat_->getTheta(j);
    }else{
      prob *= 1- sat_->getTheta(j);
    }
  }
  return prob;
}

bool SampleSAT::modelcheck(int i)
{
  if(i == 0){
    return true;
  }else{
    for(int j = 0; j < i; j++){
      for(int k = 0; k < numAtom; k++){
        if(tdata[i][k] == tdata[j][k]){
          if(k == numAtom-1){
            return false;
          }
        }else{
          break;
        }
      }
    }
  }
  return true;
}

int SampleSAT::modelcheck3(int i,int** matrix)
{
  for(int j = 0; j < datasize_t; j++){
    for(int k = 0; k < numAtom; k++){
      if(tdata[i][k] == matrix[j][k]){
        if(k == numAtom-1){
          return j;
        }
      }else{
        break;
      }
    }
  }
  return -1;
}

int SampleSAT::modelcount()
{
  int cnt = 0;
  bool flag = false;
  for(int i = 0; i < lparams_->numsample; i++){
    flag = false;
    for(int j = 0; j < i; j++){
      for(int k = 0; k < numAtom; k++){
        if(sbuf[i][k] == sbuf[j][k]){
          if(k == numAtom-1){
            flag = true;
          }
        }else{
          break;
        }
      }
    }

    if(!flag){
      for(int k = 0; k < numAtom; k++){
        nmodel[cnt][k] = sbuf[i][k];
      }
      cnt++;
    }
  }

  for(int j = 0; j < cnt; j++){
    for(int i = 0; i < lparams_->numsample; i++){
      for(int k = 0; k < numAtom; k++){
        if(sbuf[i][k] == nmodel[j][k]){
          if(k == numAtom-1){
            counts[j]+=1;
          }
        }else{
          break;
        }
      }
    }
  }
  return cnt;
}

void SampleSAT::destlearn(int datasize)
{
  for(int j = 0; j < datasize; j++){
    delete [] tdata[j];
  }
  delete [] tdata;
  for(int j = 0; j < datasize_t; j++){
    delete [] sdata[j];
  }
  delete [] sdata;
  for(int j = 0; j < lparams_->numsample; j++){
    delete [] sbuf[j];
  }
  delete [] sbuf;
  for(int j = 0; j < datasize_t; j++){
    delete [] nmodel[j];
  }
  delete [] nmodel;
}

void SampleSAT::normalize(double* vec, int vecsize)
{
  double adlogmax = -100000;
  int admaxid;
  double logsum = 0;
  double sw = 0;

  for(int i = 0; i < vecsize; i++){
    adlogmax = max(adlogmax, vec[i]);
    if(adlogmax - vec[i] < 10e-5){
      admaxid = i;
    }
  }

  sw = vec[admaxid];
  for(int i = 0; i < vecsize; i++){
    logsum += exp(vec[i] - vec[admaxid]);
  }
  sw += log(logsum);

  for(int j = 0; j < vecsize; j++){
    vec[j] = exp(vec[j]-sw);
  }
  return ;
}

/*** slicesampling.cpp ***/

#include "slicesampling.h"
#include <cstdio>

SliceSampling::SliceSampling(SampleSAT* samplesat, SatProblem* sat, learnparams* lparams)
{
  if(sldebug == true){
    cout << "# start Slice Constructor " << endl;
  }
  sat_ = sat;
  samplesat_ = samplesat;
  lparams_ = lparams;
  sumSec1 = 0;
  sumSec2 = 0;
  sumSec3 = 0;
  sumSec4 = 0;
  aveFixedAtom = 0;
  aveFixedClause = 0;
  numAtom = sat_->getNumVariables();
  init();
  if(sldebug == true){
    cout << "# end Slice Constructor " << endl;
  }
}

SliceSampling::~SliceSampling()
{
  delete [] u;
  delete [] ex;
  delete [] dd;
  delete [] sd;
  delete [] realcondprob;
  delete [] xx;
  delete [] key;
  delete [] ad;
  delete [] counts;
  delete [] q_xF;
}

void SliceSampling::init()
{
  int i;
  int val = numAtom;
  u = new double[val];
  if (u == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}
  for(i = 0; i < val ; i++){
    u[i] = 1.0;
  }
}

void SliceSampling::getSample(SatProblem* sat, int numSample, int* sample)
{
  sat_ = sat;
  unitSec3 = timer.time();
  x = sample;
  if(x[0] == -1){
    sat_->initParams();
    sat_->cpClVector();
    samplesat_->solve(sat_, x);
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
}

void SliceSampling::sampleU()
{
  if(sldebug == true){
    cout << "sampleU" << endl;
  }
  int i;
  double prob;
  for(i = 0; i < numAtom; i++){
    if(xx[i] == 1){
      prob = sat_->getTheta(i);
      if(sldebug == true){
        printf("%.2d : %d prob1 = %.8f", i, xx[i], prob);
      }
    }else{
      prob = 1 - sat_->getTheta(i);
      if(sldebug == true){
        printf("%.2d : %d prob0 = %.8f", i, xx[i], prob);
      }
    }
    u[i] = prob * sat_->getGrand();
    if(sldebug == true){
      cout << " u = " << u[i] << endl;
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
  sumSec2 += timer.time() - unitSec2;

  for(i = 0; i < numAtom; i++){
    prob1 = sat_->getTheta(i);
    prob0 = 1 - prob1;

    id = i + 1;
    if(sldebug == true){
      printf("%.2d : 0 = %.8f, 1 = %.8f", i, prob0, prob1);
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
  samplesat_->solve(sat_, xx);//setClauseValue
  sumSec1 += timer.time() - unitSec1;
}

void SliceSampling::unitProc(int var, int val)
{
  unitSec4 = timer.time();
  sat_->setAtom(var, val);
  sat_->setClause(var, val);
  sat_->rewriteWL(var, 1 - val);
  sumSec4 += timer.time() - unitSec4;
}

void SliceSampling::printModel(int* model)
{
  int i;
  cout << "[";
  for(i = 0; i < numAtom; i++){
    cout << model[i];
    if(i != numAtom-1){ cout << ",";}
  }
  cout << "]" << endl;
}

void SliceSampling::printUnitTime()
{
  cout << "( SampleSAT time         : ";
  Timer::printTime(cout, sumSec1);
  cout << ")" << endl;
  cout << "( init param time        : ";
  Timer::printTime(cout, sumSec2);
  cout << ")" << endl;
  cout << "( unit time              : ";
  Timer::printTime(cout, sumSec4);
  cout << ")" << endl;
  cout << "( slice time             : ";
  Timer::printTime(cout, sumSec3);
  cout << ")" << endl;
  cout << endl;
}

void SliceSampling::calcFixed(learnparams* lparams)
{
  int val = sat_->getNumVariables();
  cout << "###      result(formula)     ###" << endl;
  aveFixedAtom /= (double)lparams->numsample;
  aveFixedAtom /= (double)lparams->maxiter;
  cout << "average fixed atom     : " << aveFixedAtom / val * 100 << "% ( ";
  cout << aveFixedAtom << " / " << val << " )" << endl;
  aveFixedClause /= (double)lparams->numsample;
  aveFixedClause /= (double)lparams->maxiter;
  val = sat_->getNumClauses();
  cout << "average fixed clause   : " << aveFixedClause / val * 100 << "% ( ";
  cout << aveFixedClause << " / " << val << " )" << endl;
}

void SliceSampling::learn(int* sample, char* inputfile, int datasize, bool cdmode, char* outfilename)
{
  lallocate(datasize);
  readdata(inputfile, datasize);
  double lrntime = timer.time();
  malearn(datasize, cdmode, outfilename);
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

  /* print learned paramters */
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

void SliceSampling::lallocate(int datasize)
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

void SliceSampling::readdata(char* inputfile, int datasize)
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
      ifile.get(buffer, 51200,'\n');// get a sample
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

  datasize_t = 0;
  for(int i = 0; i < datasize; i++){
    if(modelcheck(i)){
      datasize_t++;
    }
  }
  //printf("count models:%d\n",datasize_t);

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

double SliceSampling::calclik(double* rd)
{
  double lik = 0;
  for(int i = 0; i < datasize_t; i++){
    lik += log(rd[i]);
  }
  lik /= (double)datasize_t;
  return lik;
}

void SliceSampling::malearn(int datasize, bool cdmode, char* outfilename)
{
  int iteration = 0;  // iteration counter
  double drand;       // double  type random variable
  int irand;          // integer type random variable
  double lik;         // likelihood
  double stSec1;
  double stSec2;
  double stSec3;
  int step = 0;       // length of Markov Chain
  cdsdSec = 0;
  cdddSec = 0;
  cntSec = 0;
  double pF = 0;
  double adlogmax;
  double logsum;
  int admaxidx;
  double hl_cs;
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
    if(cdmode){
      for(int i = 0; i < lparams_->numsample; i++){
        drand = sat_->getGrand()*datasize;
        irand = drand; // get random integers in (0~datasize)
        for(int j = 0; j < numAtom; j++){
          xx[j] = tdata[irand][j];
        } // get random one training data (one model)
        sampleU();
        sampleX();
        for(int j = 0; j < numAtom; j++){
          sbuf[i][j] = xx[j];
        }

        for(int j = 0; j < numAtom; j++){
          sd[j] += (xx[j])/(sat_->getTheta(j)) - (1 - xx[j])/(1 - sat_->getTheta(j));
        } //sum sample counts
      }
    }

    if(!cdmode){
      step = 0;
      drand = sat_->getGrand()*datasize;
      irand = drand; // get random integers in (0~datasize)
      for(int j = 0; j < numAtom; j++){
        xx[j] = tdata[irand][j];
      } // get random one training data (one model)
      for(int i = 0; i < lparams_->numsample; i++){
        sampleU();
        sampleX();
        for(int j = 0; j < numAtom; j++){
          sbuf[step][j] = xx[j];
        }
        step++;

        for(int j = 0; j < numAtom; j++){
          sd[j] += (xx[j])/(sat_->getTheta(j)) - (1 - xx[j])/(1 - sat_->getTheta(j));
        } //sum sample counts
      }
    }

    for(int j = 0; j < numAtom; j++){
      sd[j] /= lparams_->numsample;
    } // calc sample distribution
    cdsdSec += timer.time() - stSec1;

    ///  calc q(\x|F) from local samples ///
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
    adlogmax = -100000;
    admaxidx = 0;
    logsum = 0;
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
    hl_cs = 0;
    for(int i = 0; i < datasize_t; i++){
      flag = false;
      temp = sqrt(ad[i]) - sqrt(q_xF[i]);
      if(temp > 1.0e-6){
        hl_cs += temp*temp;
      }
      temp = sqrt(1-ad[i]) - sqrt(1-q_xF[i]);
      if(temp > 1.0e-6){
        hl_cs += temp*temp;
      }
    }
    hl_cs = sqrt(hl_cs);
    fprintf(helFile,"%11.5e\n",hl_cs);
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

double SliceSampling::getProb(int* sample)
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

bool SliceSampling::modelcheck(int i)
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

int SliceSampling::calckey(int i)
{
  int key = 0;
  int exp=0;
  for(int j = 0; j < numAtom; j++){
    if(tdata[i][j] == 1){
      key += pow(2,exp);
    }
    exp++;
  }

  return key;
}
/*
int SliceSampling::getIdx(int i)
{
  // return ad[j] 's index "j"
  for(int j = 0; j < datasize_t; j++){
    for(int h = 0; h < numAtom; h++){
      if(tdata[i][h] != sdata[j][h]){
        break;
      }else if (h == numAtom-1){
        return j;
      }
    }
  }
  printf("aaa\n");
  return -1;
}
*/
int SliceSampling::modelcount()
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

void SliceSampling::destlearn(int datasize)
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

void SliceSampling::normalize(double* vec, int vecsize)
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

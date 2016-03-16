/*** satproblem.cpp ***/

#include <iostream>
#include <fstream>
#include <cstring>
#include "samplesatparams.h"
#include <math.h>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include "random.h"
#include <mcheck.h>
#include "timer.h"
#include "satproblem.h"

using namespace std;
#define BUFSIZE 512

SatProblem::SatProblem(unsigned long seed, bool csmode)
{
  if(spdebug == true){
    cout << "# start SatProblem Constructor " << endl;
  }
  numVariables_ = 0;
  numClauses_ = 0;
  numAtom_ = 0;
  clVecSize = 0;
  tempNumSat = 0;
  unitMax = 0;
  rtn = false;
  token_ = NULL;
  csmode_ = csmode;
  seed_ = seed;
  rand_.sgenrand(seed_);
  if(spdebug == true){
    cout << "# end SatProblem Constructor"<< endl;
  }
}

SatProblem::~SatProblem()
{
  delete [] fixedAtom;
  delete [] varProb;
  delete [] fixedClause;
  for(int i = 0; i < numClauses_; i++){
    delete [] watchIdx[i];
  }
  delete [] watchIdx;
  for(int i = 0; i < numClauses_; i++){
    delete [] cnf[i];
  }
  delete [] cnf;
  delete [] posVec;
  delete [] negVec;
  delete [] clVec;
}

void SatProblem::parseCnfFile(char* inputFile)
{
  inputFile_ = inputFile;
  if(spdebug == true){
    cout << "# start SatProblem parseCnfFile "<< endl;
  }
  char delims[] = " ";
  bool firstLine = true;
  int line = 0;
  int cnt = 0;
  int temp[50];
  ifile_.open(inputFile_);
  if (!ifile_){
    cerr << "cannot open input file!"<< endl;
    cerr << inputFile << endl;
    exit(1);
  }else{
    // get the arguments from the file.
    while (ifile_.good()){
      ifile_.get(buffer_,512,'\n'); // get a clause
      if (ifile_.get(c_) && c_ != '\n'){
        // input string is longer than expected.
        cout << "too big file!" << endl;
        return ;
      }
      if(c_ == '%' || c_ == '?' ) break;
      ::strcpy(bufferOrig_,buffer_);
      buffp_ = buffer_;
      token_ = strtok(buffp_, delims);

      if(spdebug == true){
        cout << line << "-th line" << endl;
      }

      if(firstLine){
        token_ = strtok(NULL,delims);
        token_ = strtok(NULL,delims);
        numVariables_ = atoi(token_);
        token_ = strtok(NULL,delims);
        numClauses_ = atoi(token_);
        if(spdebug == true){
          cout << numVariables_ << ","
               << numClauses_ << endl;
        }
        firstLine = false;
        allocate();

        for(int i = 0; i < numVariables_ ; i++){
          posVec[i] = 0;
          negVec[i] = 0;
        }
        continue;
      }; // only firstLine

      if(line > numClauses_-1) break;
      numAtom_ = 0;
      while(token_ != NULL){
        temp[numAtom_] = atoi(token_);
        if(spdebug == true){
          printf("line: %.7d, numAtom = %.2d, token = %.3d\n",line,numAtom_,atoi(token_));
        }
        cnt = 0;
        if(numAtom_ < 50){
          if(numAtom_ < 2 && temp[numAtom_] != 0){
            if(temp[numAtom_] > 0){
              posVec[temp[numAtom_]-1] += 1;
            }else if(temp[numAtom_] < 0){
              negVec[abs(temp[numAtom_])-1] += 1;
            }
          }else if(temp[numAtom_] == 0){
            cnf[line] = new int[numAtom_+1];
            cnf[line][0] = numAtom_;
            for(int j = 0; j < numAtom_; j++){
              cnf[line][j+1] = temp[j];
            }
          }
        }else{
          cout << "You should set under 50-SAT problem!" << endl;
          exit(1);
        }
        numAtom_++;
        token_ = strtok(NULL, delims);
      }
      line++;
    }
  }
  ifile_.close();

  int t;
  int h;
  for(int i = 0; i < numClauses_; i++){
    h = 0;
    for(int j = 0; j < cnf[i][0]; j++){
      t = cnf[i][j+1];
      if(h < j){
        for(h = 0; h < j; h++){
          if(t == cnf[i][h+1]){
            break;
          }else if(h == j-1){
            if(t > 0){
              fixpWLl[t-1].push_back(i);
            }else if (t < 0){
              fixnWLl[abs(t)-1].push_back(i);
            }
          }
        }
      }else if(t > 0){
        fixpWLl[t-1].push_back(i);
      }else if (t < 0){
        fixnWLl[abs(t)-1].push_back(i);
      }
    }
  }
  if(spdebug == true){
    show();
  }
}

void SatProblem::allocate()
{
  int i, j;
  //new
  fixedAtom = new int[numVariables_];
  if (fixedAtom == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}
  varProb = new double[numVariables_];
  if (varProb == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}
  posWLl.resize(numVariables_);
  negWLl.resize(numVariables_);
  fixpWLl.resize(numVariables_);
  fixnWLl.resize(numVariables_);

  for(i = 0; i < numVariables_ ; i++){
    fixedAtom[i] = -1;
    varProb[i] = 0.0;
  }
  fixedClause = new int[numClauses_];
  if (fixedClause == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}
  watchIdx = new int*[numClauses_];
  if (watchIdx == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}
  cnf = new int*[numClauses_];
  if (cnf == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}
  for(i = 0; i < numClauses_ ; i++){
    watchIdx[i] = new int[2];
    if (watchIdx[i] == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}
  }
  clVec = new int[numClauses_];
  if (clVec == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}
  //initialize
  for(i = 0; i < numClauses_ ; i++){
    fixedClause[i] = -1;
    clVec[i] = -1;
    for(j = 0; j < 2 ; j++){
      watchIdx[i][j] = j;
    }
  }
  posVec = new int[numVariables_];
  if (posVec == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}
  negVec = new int[numVariables_];
  if (negVec == NULL){ cout << "cannot allocate memory"<< endl; exit(1);}
}

void SatProblem::parseProbFile(char* inputFile, ofstream& prbFile)
{
  int line = 0;
  inputFile_ = inputFile;
  if(spdebug == true){
    cout << "# start SatProblem parseProbFile "<< endl;
  }

  ifile_.open(inputFile_);
  if (!ifile_){
    for(line = 0; line < numVariables_; line++){
      varProb[line] = getGrand();
      prbFile << varProb[line] << endl;
      if(spdebug == true){
        cout << "varProb[" << line << "] = " <<  varProb[line] << endl;
      }
    }
  }else{
    // get the arguments from the file.
    while (ifile_.good()){
      if( line > numVariables_-1) break;
      ifile_.get(buffer_,512,'\n'); // get a clause

      if (ifile_.get(c_) && c_ != '\n'){
        // input string is longer than expected.
        cout << "too big file!" << endl;
        return ;
      }
      varProb[line] = atof(buffer_);
      if(spdebug == true){
        cout << "varProb[" << line << "] = " <<  varProb[line] << endl;
      }
      prbFile << varProb[line] << endl;
      line++;
    }
  }
  ifile_.close();
}

int* SatProblem::getCnf(int i)
{
  return cnf[i];
}

int SatProblem::getCnj(int i,int j)
{
  int* cnj = getCnf(i);
  return cnj[j];
}

void SatProblem::initParams()
{
  int j;
  for(int i = 0; i < numClauses_ ; i++){
    fixedClause[i] = -1;
    j = 0;
    watchIdx[i][j] = j;
    j++;
    if(cnf[i][0] > 1){
      watchIdx[i][j] = j;
    }else if (cnf[i][0] == 1){
      watchIdx[i][j] = -1;
    }
  }

  for(int i = 0; i < numVariables_ ; i++){
    fixedAtom[i] = -1;
    if(!posWLl[i].empty()){
      posWLl[i].clear();
    }
    if(!negWLl[i].empty()){
      negWLl[i].clear();
    }
  }

  for(int i = 0; i < numClauses_; i++){
    for(int j = 0; j < 2; j++){
      int t = cnf[i][watchIdx[i][j]+1];
      if(t > 0){
        posWLl[t-1].push_back(i);
      }else if (t < 0){
        negWLl[abs(t)-1].push_back(i);
      }
    }
  }

  rtn = false;
  int p = 0;

  while(p >= numClauses_ && clVec[p] != -1){
    printf("a %d ,",clVec[p]);
    clVec[p] = -1;
    p++;
  }
}

void SatProblem::cpClVector()
{
  int idx = 0;

  if(!csmode_){
  //only SampleSAT
    for(int i = 0; i < numClauses_ ; i++){
      fixedClause[i] = -1;
    }
  }

  for(int i = 0; i < numClauses_ ; i++){
    if(fixedClause[i] == -1){
      clVec[idx] = i;
      idx++;
    }
  }
  clVecSize = idx;
}

void SatProblem::printClVec()
{
  printf("##clVec##::");
  printf("[");
  for(int i = 0; i < clVecSize; i++){
    printf("%d",clVec[i]);
    if(i != clVecSize-1){
      printf(",");
    }
  }
  printf("]\n");
}

void SatProblem::setAtom(int var, int val)
{
  fixedAtom[var] = val;
}

void SatProblem::setClause(int var, int val)
{
  if(val == 1){
    for(begItl = posWLl[var].begin(); begItl != posWLl[var].end(); begItl++){
      fixedClause[(int)*begItl] = 1;
    }
  }else if(val == 0){
    for(begItl = negWLl[var].begin(); begItl != negWLl[var].end(); begItl++){
      fixedClause[(int)*begItl] = 1;
    }
  }
}

void SatProblem::rewriteWL(int var, int val)
{
  int i, c;
  vector<int> temp;
  int np;
  if(spdebug == true){
    printf("\n var=%d, val=%d, ",var,val);
  }
  if(fixedAtom[var] == -1){
    if(val == 1){// rewrite posWL
      for(begItl = posWLl[var].begin(); begItl != posWLl[var].end(); begItl++){
        temp.push_back((int)*begItl);
      }
      for(i = 0; i < (int)temp.size(); i++){
        c = temp[i];
        if(fixedClause[c] != 1){
          np = max(watchIdx[c][0], watchIdx[c][1]) + 1;
          if (getCnjSize(c) > np){// rewritable
            if(spdebug == true){
              printf("cnf1:%d,cnf2:%d,c:%d,np:%d,cnfSize:%d,",cnf[c][watchIdx[c][0]+1],cnf[c][np+1],c,np,getCnjSize(c));
            }
            setWatchIdx(c, np, var);
          }else{// cannot rewrite -> unit propagation
            unit(c, var); // set var -> set clause -> rewrite (recursive)
          }
        }
      }
    }else{ // val = 0
      for(begItl = negWLl[var].begin(); begItl != negWLl[var].end(); begItl++){
        temp.push_back((int)*begItl);
      }
      for(i = 0; i < (int)temp.size(); i++){
        c = temp[i];
        if(fixedClause[c] != 1){
          np = max(watchIdx[c][0], watchIdx[c][1]) + 1;
          if (getCnjSize(c) > np){// rewritable
            if(spdebug == true){
              printf("cnf1:%d,cnf2:%d,c:%d,np:%d,cnfSize:%d,",cnf[c][watchIdx[c][0]+1],cnf[c][np+1],c,np,getCnjSize(c));
            }
            setWatchIdx(c, np, var);
          }else{// cannot rewrite -> unit propagation
            unit(c, var); // set var -> set clause -> rewrite (recursive)
            if(rtn){
              temp.clear();
              return;
            }
          }
        }
      }
    }
    temp.clear();
  }
}

void SatProblem::setWatchIdx(int c, int np, int var)
{
  int l0 = cnf[c][watchIdx[c][0]+1];
  int l1 = cnf[c][watchIdx[c][1]+1];
  int lnp = cnf[c][np+1];

  if(spdebug == true){
    printf("l0=%d, l1=%d, lnp=%d, var=%d\n",l0,l1,lnp,var);
  }
  if(abs(l0) - 1 == var){// 0-th value is wanted pointer
    if(l0 > 0){
      if(lnp > 0){
        posWLl[lnp-1].push_back(c);
      }else{
        negWLl[abs(lnp)-1].push_back(c);
      }
    }else{
      if(lnp > 0){
        posWLl[lnp-1].push_back(c);
      }else{
        negWLl[abs(lnp)-1].push_back(c);
      }
    }
    watchIdx[c][0] = np;
  }else if(abs(l1) - 1 == var){ // 1-th value is wanted pointer
    if(l1 > 0){
      if(lnp > 0){
        posWLl[lnp-1].push_back(c);
      }else{
        negWLl[abs(lnp)-1].push_back(c);
      }
    }else{
      if(lnp > 0){
        posWLl[lnp-1].push_back(c);
      }else{
        negWLl[abs(lnp)-1].push_back(c);
      }
    }
    watchIdx[c][1] = np;
  }
}

void SatProblem::unit(int c, int var)
{
  int nvar;
  int l0 = cnf[c][watchIdx[c][0]+1];
  int l1 = cnf[c][watchIdx[c][1]+1];
  unitMax++;
  if(unitMax > 5){
    unitMax = 0;
    rtn = true;
  }
  if(rtn){
    printf("returned\n");
    return;
  }
  if(abs(l0) - 1 == var){
    nvar = l1;
    if (nvar > 0){
      setAtom(nvar-1, 1);
      setClause(nvar-1, 1);
      rewriteWL(nvar-1, 0);
    }else if (nvar < 0){
      setAtom(abs(nvar)-1, 0);
      setClause(abs(nvar)-1, 0);
      rewriteWL(abs(nvar)-1, 1);
    }
  }else if(abs(l1) - 1 == var){
    nvar = l0;
    if (nvar > 0){
      setAtom(nvar-1, 1);
      setClause(nvar-1, 1);
      rewriteWL(nvar-1, 0);
    }else if (nvar < 0){
      setAtom(abs(nvar)-1, 0);
      setClause(abs(nvar)-1, 0);
      rewriteWL(abs(nvar)-1, 1);
    }
  }
}

void SatProblem::show()
{
  int i, j;
  printf("\n-----------------------\n");
  printf("##CNF##\n");
  for(i = 0; i < numClauses_; i++){
    printf("%.2d: ",i);
    for(j = 0; j < cnf[i][0]; j++){
      printf("%.2d ", cnf[i][j+1]);
    }
    printf(": size = %d\n",cnf[i][0]);
  }
  printf("##fixedAtom##\n");
  printf("[");
  for(i = 0; i < numVariables_; i++){
    printf("%d",fixedAtom[i]);
    if(i != numVariables_-1){
      printf(",");
    }
  }
  printf("]\n");
  printf("##fixedClause##\n");
  printf("[");
  for(i = 0; i < numClauses_; i++){
    printf("%d",fixedClause[i]);
    if(i != numClauses_-1){
      printf(",");
    }
  }
  printf("]\n");
  printf("##fixpWLl##\n");
  for(i = 0; i < numVariables_; i++){
    printf("%.2d: ",i);
    for(begItl = fixpWLl[i].begin(); begItl != fixpWLl[i].end(); begItl++){
      printf("%.2d ", *begItl);
    }
    printf("\n");
  }
  printf("##fixnWLl##\n");
  for(i = 0; i < numVariables_; i++){
    printf("%.2d: ",i);
    for(begItl = fixnWLl[i].begin(); begItl != fixnWLl[i].end(); begItl++){
      printf("%.2d ", *begItl);
    }
    printf("\n");
  }

  printf("##watchIdx##\n");
  for(i = 0; i < numClauses_; i++){
    printf("%.2d: %d %d\n",i, watchIdx[i][0],watchIdx[i][1]);
  }
  printf("-----------------------\n\n");
}

bool SatProblem::initSatCheck(int* model)
{
  int i;
  bool val = true;
  tempNumSat = numClauses_ - clVecSize;
  for(i = 0; i < clVecSize; i++){
    if(isSatCnj(model, clVec[i]) == false){
      fixedClause[clVec[i]] = 0;
      val = false;
    }else{
      fixedClause[clVec[i]] = 1;
      tempNumSat++;
    }
  }
  return val;
}

void SatProblem::afterFlip(int* model, int atom)
{  // rewrite fixedClause
  for(begItl = fixnWLl[atom].begin(); begItl != fixnWLl[atom].end(); begItl++){
    if(isSatCnj(model, *begItl)){
      fixedClause[*begItl] = 1;
    }else{
      fixedClause[*begItl] = 0;
    }
  }
  for(begItl = fixpWLl[atom].begin(); begItl != fixpWLl[atom].end(); begItl++){
    if(isSatCnj(model, *begItl)){
      fixedClause[*begItl] = 1;
    }else{
      fixedClause[*begItl] = 0;
    }
  }
}

int SatProblem::getNsc1(int* model, int atom)//for rw
{
  int n = tempNumSat;
  for(begItl = fixpWLl[atom].begin(); begItl != fixpWLl[atom].end(); begItl++){
    n -= fixedClause[*begItl];
  }
  for(begItl = fixnWLl[atom].begin(); begItl != fixnWLl[atom].end(); begItl++){
    n -= fixedClause[*begItl];
  }

  model[atom] = 1 - model[atom];
  afterFlip(model, atom);

  for(begItl = fixpWLl[atom].begin(); begItl != fixpWLl[atom].end(); begItl++){
    if(isSatCnj(model, *begItl)){
      n++;
    }
  }
  for(begItl = fixnWLl[atom].begin(); begItl != fixnWLl[atom].end(); begItl++){
    if(isSatCnj(model, *begItl)){
      n++;
    }
  }

  return n;
}

int SatProblem::getNsc2(int* model, int atom)//for gw, sa
{
  int n = tempNumSat;
  for(begItl = fixpWLl[atom].begin(); begItl != fixpWLl[atom].end(); begItl++){
    n -= fixedClause[*begItl];
  }
  for(begItl = fixnWLl[atom].begin(); begItl != fixnWLl[atom].end(); begItl++){
    n -= fixedClause[*begItl];
  }
  model[atom] = 1 - model[atom];

  for(begItl = fixpWLl[atom].begin(); begItl != fixpWLl[atom].end(); begItl++){
    if(isSatCnj(model, *begItl)){
      n++;
    }
  }
  for(begItl = fixnWLl[atom].begin(); begItl != fixnWLl[atom].end(); begItl++){
    if(isSatCnj(model, *begItl)){
      n++;
    }
  }
  model[atom] = 1 - model[atom];
  return n;
}

bool SatProblem::isSat(int* model)
{
  for(int i = 0; i < clVecSize; i++){
    if(isSatCnj(model, clVec[i]) == false){
      return false;
    }
  }
  return true;
}

bool SatProblem::isSatCnj(int* model, int cl)
{
  int i, id;
  for(i = 0; i < cnf[cl][0]; i++){
    if(cnf[cl][i+1] > 0){
      id = cnf[cl][i+1] - 1;
      if(model[id] == 1){
        return true;
      }
    }else{
      id = abs(cnf[cl][i+1]) - 1;
      if(model[id] == 0){
        return true;
      }
    }
  }
  return false;
}

//get
int SatProblem::getNumVariables()
{
  return numVariables_;
}

int SatProblem::getNumClauses()
{
  return numClauses_;
}

int SatProblem::getCnfij(int i, int j)
{
  return cnf[i][j];
}

double SatProblem::getVarProbi(int i)
{
  return varProb[i];
}

int SatProblem::getVar(int id)
{
  return fixedAtom[id];
}

int SatProblem::getClause(int id)
{
  return fixedClause[id];
}

int SatProblem::getCnjSize(int cnj)
{
  return cnf[cnj][0];
}

int SatProblem::getNumFixedAtom()
{
  int i,n = 0;
  for(i = 0; i < numVariables_; i++){
    if(fixedAtom[i] == 1){
      n++;
    }
  }
  return n;
}

int SatProblem::getNumFixedClause()
{
  return numClauses_ - clVecSize;
}

int SatProblem::getFixedClausei(int i)
{
  return fixedClause[i];
}

int SatProblem::getClVecSize()
{
  return clVecSize;
}

int SatProblem::getClVec(int i)
{
  return clVec[i];
}

int SatProblem::getNumSat()
{
  return tempNumSat;
}

//set
void SatProblem::setNumVariables(int numVariables)
{
  numVariables_ = numVariables;
}

void SatProblem::setNumClauses(int numClauses)
{
  numClauses_ = numClauses;
}

void SatProblem::setCnfij(int cnfij, int i, int j)
{
  cnf[i][j] = cnfij;
}

void SatProblem::setVarProbi(double varProbi, int i)
{
  varProb[i] = varProbi;
}

void SatProblem::setFixedClausei(int val, int i)
{
  fixedClause[i] = val;
}

void SatProblem::setFixedAtomi(int val, int i)
{
  fixedAtom[i] = val;
}

void SatProblem::addNumSat(int i)
{
  tempNumSat += i;
}

void SatProblem::setUnitFlag()
{
  rtn = false;
}

double SatProblem::getGrand()
{
  return rand_.genrand();
}

bool SatProblem::modelcheck(int i, int** data)
{
  if(i == 0){
    return true;
  }else{
    for(int j = 0; j < i; j++){
      if(sameModel(data[i], data[j])){
        return false;
      }
    }
  }
  return true;
}

bool SatProblem::modelcheck2(int datasize, int** data, int* model)
{
  for(int j = 0; j < datasize; j++){
    if(sameModel(data[j], model)){
      return false;
    }
  }
  return true;
}

bool SatProblem::sameModel(int* data, int* model)
{
  for(int k = 0; k < numVariables_; k++){
    if(model[k] == data[k]){
      if(k == numVariables_-1){
        return true;
      }
    }else{
      return false;
    }
  }
  return false;
}

void SatProblem::calcHL(int** sli, int** sam, int datasize, char* outfilename)
{
  int datasize_s = 0;
  int** sdata;
  double* ad;
  int idx;
  double* cntsl  = new double[numVariables_];
  double* cntsa  = new double[numVariables_];
  double* weight = new double[datasize];
  double sw = 0;
  bool realmode = false;

  //params for calc. Hellinger distance
  double pF;
  double adlogmax;
  double logsum;
  int admaxidx;
  double adlogmax0;
  double logsum0;
  int admaxidx0;
  double lik;
  double ipF;
  int cnt;
  double* csprob = new double[numVariables_];
  double* ssprob = new double[numVariables_];
  double* reprob = new double[numVariables_];
  double hl_cs;
  double hl_ss;
  double temp;
  double j_temp;
  double buf[8];
  FILE* marFile = fopen(outfilename,"w");

  for(int i = 0; i < numVariables_; i++){
    cntsl[i] = 0;
    cntsa[i] = 0;
    csprob[i] = 0;
    ssprob[i] = 0;
    reprob[i] = 0;
  }

  for(int i = 0; i < datasize; i++){
    weight[i] = 0;
  }
  for(int i = 0; i < datasize; i++){
    for(int j = 0; j < numVariables_; j++){
      cntsl[j] += sli[i][j];
    }
    if(modelcheck(i, sli)){
      datasize_s++;
    }
  }
  //printf("count models:%d\n",datasize_s);
  for(int j = 0; j < numVariables_; j++){
    cntsl[j] /= datasize;
    csprob[j] = cntsl[j];
  }

  adlogmax = -100000;
  admaxidx = 0;
  logsum = 0;
  for(int i = 0; i < datasize; i++){
    for(int j = 0; j < numVariables_; j++){
      if(sam[i][j] == 1){
        weight[i] += log(getVarProbi(j));
      }else{
        weight[i] += log(1-getVarProbi(j));
      }
    }
    adlogmax = max(adlogmax, weight[i]);
    if(adlogmax - weight[i] < 10e-10){
      admaxidx = i;
    }
    if(modelcheck(i, sam)){
      if(modelcheck2(datasize, sli, sam[i])){
        datasize_s++;
      }
    }
  }

  sw = weight[admaxidx];
  for(int i = 0; i < datasize; i++){
    logsum += exp(weight[i] - weight[admaxidx]);
  }
  sw += log(logsum);

  for(int j = 0; j < datasize; j++){
    weight[j] = exp(weight[j]-sw);
  }
  for(int i = 0; i < datasize; i++){
    for(int j = 0; j < numVariables_; j++){
      cntsa[j] += weight[i] * sam[i][j];
    }
  }

  printf("count models:%d\n",datasize_s);
  for(int j = 0; j < numVariables_; j++){
    ssprob[j] = cntsa[j];
  }

  sdata = new int*[datasize_s];
  for(int j = 0; j < datasize_s; j++){
    sdata[j] = new int[numVariables_];
  }
  for(int j = 0; j < datasize_s; j++){
    for(int k = 0; k < numVariables_; k++){
      sdata[j][k] = 0;
    }
  }
  ad = new double[datasize_s];
  for(int j = 0; j < datasize_s; j++){
    ad[j] = 0;
  }
  idx = 0;
  for(int i = 0; i < datasize; i++){
    if(modelcheck(i, sam)){
      for(int k = 0; k < numVariables_; k++){
        sdata[idx][k] = sam[i][k];
      }
      idx++;
    }
  }

  for(int i = 0; i < datasize; i++){
    if(modelcheck(i, sli)){
      if(modelcheck2(datasize, sli, sam[i])){
        for(int k = 0; k < numVariables_; k++){
          sdata[idx][k] = sli[i][k];
        }
      }
      idx++;
    }
  }

  pF=0;
  adlogmax = -100000;
  admaxidx = 0;
  logsum = 0;
  lik=0;

  for(int i = 0; i < datasize_s; i++){
    for(int j = 0; j < numVariables_; j++){
      if(sdata[i][j] == 1){
        ad[i] += log(getVarProbi(j));
      }else{
        ad[i] += log(1-getVarProbi(j));
      }
    }
    adlogmax = max(adlogmax, ad[i]);
    if(adlogmax - ad[i] < 10e-10){
      admaxidx = i;
    }
  }
  pF += ad[admaxidx];

  for(int i = 0; i < datasize_s; i++){
    logsum += exp(ad[i] - ad[admaxidx]);
  }
  pF += log(logsum);

  for(int j = 0; j < datasize_s; j++){
    ad[j] = exp(ad[j]-pF);
  }

  /*** calc. real p(x_i|F,\theta) ***/
  // cntsl : input p(x_i = 0|F,\theta)
  // cntsa : input p(x_i = 1|F,\theta)
  for(int i = 0; i < numVariables_; i++){
    cntsa[i] = 0;
    cntsl[i] = 0;
  }
  for(int i = 0; i < datasize_s; i++){
    ad[i] = 0;
  }

  adlogmax  = -100000;
  admaxidx  = 0;
  logsum    = 0;
  adlogmax0 = -100000;
  admaxidx0 = 0;
  logsum0   = 0;
  cnt       = 0;
  ipF       = 0;
  pF        = 0;
  idx       = 0;

  if(realmode){
    // all models for roop
    int* remodel = new int[numVariables_];
    for(int k = 0; k < numVariables_; k++){
      remodel[k] = 0;
    }
    printf("realmode\n");
    for(int j = 0; j < (int)pow(2, numVariables_); j++){
      j_temp = j+1;
      for(int k = numVariables_-1; k > -1; k--){
        if(j_temp > (int)pow(2,k)){
          remodel[k] = 1;
          j_temp -= (int)pow(2,k);
        }else{
          remodel[k] = 0;
        }
      }
      if(poorIsSat(remodel)){
        ad[idx] = 0;
        for(int k = 0; k < numVariables_; k++){
          if(remodel[k] == 1){
            ad[idx] += log(getVarProbi(k));
          }else{
            ad[idx] += log(1-getVarProbi(k));
          }
        }
        pF += exp(ad[idx]);
        idx++;
      }
    }
    for(int k = 0; k < idx; k++){
      ad[k] = exp(ad[k])/pF;
      reprob[k] = ad[k];
    }
    if(idx != datasize_s){
      delete [] remodel;
    }else{
      delete [] remodel;
    }
  }else{
    for(int i = 0; i < datasize_s; i++){
      for(int j = 0; j < numVariables_; j++){
        if(sdata[i][j] == 1){
          ad[i] += log(getVarProbi(j));
        }else{
          ad[i] += log(1-getVarProbi(j));
        }
      }
      adlogmax = max(adlogmax, ad[i]);
      if(adlogmax - ad[i] < 10e-10){
        admaxidx = i;
      }
    }

    pF += ad[admaxidx];
    for(int i = 0; i < datasize_s; i++){
      logsum += exp(ad[i] - ad[admaxidx]);
    }
    pF += log(logsum);
    adlogmax  = -100000;
    admaxidx  = 0;
    adlogmax0 = -100000;
    admaxidx0 = 0;

    for(int i = 0; i < datasize_s; i++){
      for(int j = 0; j < numVariables_; j++){
        if(sdata[i][j] == 0){
          adlogmax0 = max(adlogmax0, ad[i]);
          if(adlogmax0 - ad[i] < 10e-10){
            admaxidx0 = i;
          }
        }else{
          adlogmax  = max(adlogmax,  ad[i]);
          if(adlogmax - ad[i] < 10e-10){
            admaxidx = i;
          }
        }
      }
    }

    for(int j = 0; j < numVariables_; j++){
      cntsl[j] += ad[admaxidx0];
      cntsa[j] += ad[admaxidx];
    }

    for(int j = 0; j < numVariables_; j++){
      logsum    = 0;
      logsum0   = 0;
      for(int i = 0; i < datasize_s; i++){
        if(sdata[i][j] == 0){
          logsum0 += exp(ad[i] - ad[admaxidx0]);
        }else{
          logsum += exp(ad[i] - ad[admaxidx]);
        }
      }
      cntsl[j] += log(logsum0);
      cntsa[j] += log(logsum);
    }

    for(int j = 0; j < numVariables_; j++){
      cntsa[j] = exp(cntsa[j]-pF);
      cntsl[j] = exp(cntsl[j]-pF);
      reprob[j] = cntsa[j];
    }
  }

  /*** calc. HD ***/
  hl_ss = 0;
  hl_cs = 0;

  for(int i = 0; i < numVariables_; i++){
    temp = sqrt(reprob[i]) - sqrt(csprob[i]);
    if(temp > 1.0e-6){
      hl_cs += temp*temp;
    }
    temp = sqrt(1-reprob[i]) - sqrt(1-csprob[i]);
    if(temp > 1.0e-6){
      hl_cs += temp*temp;
    }
    temp = sqrt(reprob[i]) - sqrt(ssprob[i]);
    if(temp > 1.0e-6){
      hl_ss += temp*temp;
    }
    temp = sqrt(1-reprob[i]) - sqrt(1-ssprob[i]);
    if(temp > 1.0e-6){
      hl_ss += temp*temp;
    }
  }
  hl_cs = sqrt(hl_cs);
  hl_ss = sqrt(hl_ss);
  //printf("%11.5e %11.5e\n",hl_cs, hl_ss);
  //fprintf(marFile,"1 %11.5e %11.5e\n",hl_cs, hl_ss);
  buf[3] = hl_cs;
  buf[7] = hl_ss;

  for(int p = 0; p < 3; p++){
    datasize /= 10;

    adlogmax = -100000;
    admaxidx = 0;
    logsum = 0;

    for(int i = 0; i < numVariables_; i++){
      cntsl[i] = 0;
      cntsa[i] = 0;
      csprob[i] = 0;
      ssprob[i] = 0;
    }

    for(int i = 0; i < datasize; i++){
      weight[i] = 0;
    }
    for(int i = 0; i < datasize; i++){
      for(int j = 0; j < numVariables_; j++){
        cntsl[j] += sli[i][j];
      }
    }
    for(int j = 0; j < numVariables_; j++){
      cntsl[j] /= datasize;
      csprob[j] = cntsl[j];
    }
    for(int i = 0; i < datasize; i++){
      for(int j = 0; j < numVariables_; j++){
        if(sam[i][j] == 1){
          weight[i] += log(getVarProbi(j));
        }else{
          weight[i] += log(1-getVarProbi(j));
        }
      }
      adlogmax = max(adlogmax, weight[i]);
      if(adlogmax - weight[i] < 10e-10){
        admaxidx = i;
      }
    }

    sw = weight[admaxidx];
    for(int i = 0; i < datasize; i++){
      logsum += exp(weight[i] - weight[admaxidx]);
    }
    sw += log(logsum);
    for(int j = 0; j < datasize; j++){
      weight[j] = exp(weight[j]-sw);
    }
    for(int i = 0; i < datasize; i++){
      for(int j = 0; j < numVariables_; j++){
        cntsa[j] += weight[i] * sam[i][j];
      }
    }
    for(int j = 0; j < numVariables_; j++){
      ssprob[j] = cntsa[j];
    }

    /*** calc. HD ***/
    hl_ss = 0;
    hl_cs = 0;

    for(int i = 0; i < numVariables_; i++){
      temp = sqrt(reprob[i]) - sqrt(csprob[i]);
      if(temp > 1.0e-6){
        hl_cs += temp*temp;
      }
      temp = sqrt(1-reprob[i]) - sqrt(1-csprob[i]);
      if(temp > 1.0e-6){
        hl_cs += temp*temp;
      }
      temp = sqrt(reprob[i]) - sqrt(ssprob[i]);
      if(temp > 1.0e-6){
        hl_ss += temp*temp;
      }
      temp = sqrt(1-reprob[i]) - sqrt(1-ssprob[i]);
      if(temp > 1.0e-6){
      hl_ss += temp*temp;
      }
    }
    hl_cs = sqrt(hl_cs);
    hl_ss = sqrt(hl_ss);
    //printf("%11.5e %11.5e\n",hl_cs, hl_ss);
    //fprintf(marFile,"%d %11.5e %11.5e\n",p+2,hl_cs, hl_ss);
    buf[2-p] = hl_cs;
    buf[6-p] = hl_ss;
  }

  for(int j = 0; j < 4; j++){
    printf("%d %11.5e %11.5e\n",j+1,buf[j], buf[j+4]);
    fprintf(marFile,"%d %11.5e %11.5e\n",j+1,buf[j],buf[j+4]);
  }
  for(int j = 0; j < datasize_s; j++){
    delete [] sdata[j];
  }
  delete [] sdata;
  delete [] reprob;
  delete [] ssprob;
  delete [] csprob;
}

bool SatProblem::poorIsSat(int* sample)
{
  for(int i = 0; i < numClauses_; i++){
    if(isSatCnj(sample, i) == false){
      return false;
    }
  }
  return true;
}

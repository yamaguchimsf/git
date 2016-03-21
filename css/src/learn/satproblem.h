/*** satproblem.h ***/

#ifndef SATPROBLEM_H_
#define SATPROBLEM_H_

#include <fstream>
#include "random.h"
#include "timer.h"
#include <vector>
#include <list>

#define BUFSIZE 51200

using namespace std;

const bool spdebug = false;

class SatProblem
{
 public:
  SatProblem(unsigned long seed, bool csmode);
  ~SatProblem();
  void parseCnfFile(char* inputFile);
  void allocate();
  void parseProbFile(char* inputFile);
  int parseSliFile(char* inputFile);
  int* getCnf(int i);
  int getCnj(int i,int j);
  double getTheta(int i);
  void initParams();
  void cpClVector();
  void printClVec();
  void setAtom(int var, int val);
  void setClause(int var, int val);
  void rewriteWL(int var, int val);
  void setWatchIdx(int c, int np, int var);
  void unit(int c, int var);
  void show();
  void showp();
  bool initSatCheck(int* model);
  void afterFlip(int*model, int atom);
  int getNsc1(int* model, int atom);//for rw
  int getNsc2(int* model, int atom);//for gw, sa
  bool isSat(int* model);
  bool isSatCnj(int* model, int cl);
  int getNumVariables();
  int getNumClauses();
  int getCnfij(int i, int j);
  double getRealProb(int i);
  int getVar(int id);
  int getClause(int id);
  int getCnjSize(int cnj);
  int getNumFixedAtom();
  int getNumFixedClause();
  int getFixedClausei(int i);
  int getClVecSize();
  int getClVec(int i);
  int getNumSat();
  int getSampleSize();
  void setNumVariables(int numVariables);
  void setNumClauses(int numClauses);
  void setCnfij(int cnfij, int i, int j);
  void setThetai(double thetai, int i);
  void setFixedClausei(int val, int i);
  void setFixedAtomi(int val, int i);
  void addNumSat(int i);
  void updateTheta(double* dist);
  void setUnitFlag();
  double getGrand();
  void montecarlo(int* sample);
  void printModels(int* sample);
  bool poorIsSat(int* sample);

 private:
  bool csmode_;
  ifstream ifile_;
  char* inputFile_;
  char* token_;
  char* buffp_;
  char c_;
  char buffer_[BUFSIZE];
  char bufferOrig_[BUFSIZE];
  int numFileLines_;
  int numVariables_;
  int numClauses_;
  int numAtom_;
  int tempNumSat;
  int* fixedAtom;
  double* dd;
  double* sd;
  vector<list<int> > posWLl;
  vector<list<int> > negWLl;
  vector<list<int> > fixpWLl;
  vector<list<int> > fixnWLl;
  list<int>::iterator begItl;
  list<int>::iterator endItl;
  int* fixedClause;
  int* clVec;
  int clVecSize;
  int** watchIdx;
  int** cnf;
  double* theta;
  double* realProb;
  int* posVec;
  int* negVec;
  Random rand_;
  Timer timer;
  unsigned long seed_;
  int sampleSize;
  int unitMax;
  bool rtn;
};

#endif /* SATPROBLEM_H_ */
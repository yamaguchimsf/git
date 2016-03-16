/*** learnparams.h ***/

#ifndef LEARNPARAMS_H_
#define LEARNPARAMS_H_

struct learnparams
{
  double ita;    // learning constants
  bool distmode; // true : uniform selected, false : random selected
  int maxiter;   // # of update parameter 
  int numsample; // # of samples in 1 update
  int burnsteps; // # of burning steps
};

#endif /* LEARNPARAMS_H_ */

/*** samplesatparams.h ***/

#ifndef SAMPLESATPARAMS_H_
#define SAMPLESATPARAMS_H_

struct samplesatparams
{
  int walkmax;
  int numSample;
  double walksatProb;
  double randomWalkProb;
  double temperature;
  unsigned long seed;
};

#endif /* SAMPLESATPARAMS_H_ */

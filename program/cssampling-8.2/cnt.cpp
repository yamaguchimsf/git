/***  cnt.cpp***/
//for count

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
//#include <algorithm>
#include <cstring>
#include "cnt.h"

using namespace std;

#define BUFSIZE 51200

int main(int argc, char** argv)
{
  printf("## CNT ##\n");

  int option;
  ifstream ifile;
  char c;
  char buffer[BUFSIZE];
  char* buffp;
  char* token;
  char delim1[]  = "(), ";
  int idx = 0;
  int line = 0;
  int datasize = 100000;
  int data_line[datasize][20];
  int line_size[datasize];
  double *prob;

  if (cdebug == true){
    cout << "## reading inputs"<< endl;
  }
  while (( option = getopt(argc, argv, "s")) != -1)
    switch(option){
    case 's' :
      slifile = argv[optind];
      break;
    default :
      cout << "-s : set slifile      (default = NULL)" << endl;
      return 1;
    }

  printf("-- sli --\n");
  ifile.open(slifile);
  if (!ifile){
    cerr << "cannot open sli file!"<< endl;
    cerr << slifile << endl; exit(1);
  }else{
    // get the arguments from the file.
    while (ifile.good()){
      ifile.get(buffer,5120000,'\n'); // get a clause
      if (ifile.get(c) && c != '\n'){
        // input string is longer than expected.
        cout << "too big file!" << endl;
        return 0;
      }

      idx = 0;
      buffp = buffer;

      token = strtok(buffp, delim1);
      while(token != NULL){
        //strcpy(data_line[line][idx], atoi(token));
        data_line[line][idx] =  atoi(token);
        //printf("%d,",data_line[line][idx]);
        idx++;
        token = strtok(NULL, delim1);
      }
      //printf("\n");
      line_size[line] = idx;
      line++;
    }
  }
  ifile.close();

  prob = new double[line_size[0]];
  for(int k = 0; k < line_size[0]; k++){
    prob[k] = 0;
  }
  for(int j = 0; j < datasize; j++){
    for(int k = 0; k < line_size[j]; k++){
      prob[k] += (double)data_line[j][k];
    }
  }
  for(int k = 0; k < line_size[0]; k++){
    prob[k] /= (double)datasize;
    //printf("prob[%.2d] = %f\n",k,prob[k]);
    printf("%d %f\n",k+1,prob[k]);
  }

  /*
  printf("course  = %d\n",course);
  printf("person  = %d\n",person);
  printf("quarter = %d\n",quarter);
  printf("level   = %d\n",level);
  printf("integer = %d\n",integer);
  printf("position= %d\n",position);
  printf("title   = %d\n",title);
  printf("project = %d\n",project);
  printf("phase   = %d\n",phase);
  sum = 0;
  sum += course;
  sum += person;
  sum += quarter;
  sum += level;
  sum += integer;
  sum += position;
  sum += title;
  sum += project;
  sum += phase;
  printf("sum     = %d\n\n",sum);

  taughtBy *= course;
  taughtBy *= person;
  taughtBy *= quarter;
  courseLevel *= course;
  courseLevel *= level;
  hasPosition *= person;
  hasPosition *= position;
  advisedBy *= person;
  advisedBy *= person;
  inPhase *= person;
  inPhase *= phase;
  tempAdvisedBy *= person;
  tempAdvisedBy *= person;
  yearsInProgram *= person;
  yearsInProgram *= integer;
  ta *= course;
  ta *= person;
  ta *= quarter;
  professor *= person;
  student *= person;
  samePerson *= person;
  samePerson *= person;
  sameCourse *= course;
  sameCourse *= course;
  sameProject *= project;
  sameProject *= project;
  samePhase *= phase;
  samePhase *= phase;
  sameInteger *= integer;
  sameInteger *= integer;
  publication *= title;
  publication *= person;

  printf("taughtBy  = %d\n",taughtBy);
  printf("courseLevel = %d\n",courseLevel);
  printf("hasPosition = %d\n",hasPosition);
  printf("advisedBy = %d\n",advisedBy);
  printf("inPhase = %d\n",inPhase);
  printf("tempAdvisedBy = %d\n",tempAdvisedBy);
  printf("yearsInProgram = %d\n",yearsInProgram);
  printf("ta = %d\n",ta);
  printf("professor = %d\n",professor);
  printf("student = %d\n",student);
  printf("samePerson = %d\n",samePerson);
  printf("sameCourse = %d\n",sameCourse);
  printf("sameProject = %d\n",sameProject);
  printf("samePhase = %d\n",samePhase);
  printf("sameInteger = %d\n",sameInteger);
  printf("publication = %d\n",publication);
  sum = 0;
  sum += taughtBy;
  sum += courseLevel;
  sum += hasPosition;
  sum += advisedBy;
  sum += inPhase;
  sum += tempAdvisedBy;
  sum += yearsInProgram;
  sum += ta;
  sum += professor;
  sum += student;
  sum += samePerson;
  sum += sameCourse;
  sum += sameProject;
  sum += samePhase;
  sum += sameInteger;
  sum += publication;
  printf("sum     = %d\n",sum);

  */
  delete [] prob;
  return 0;
}

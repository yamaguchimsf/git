/***  parse.cpp***/
//for parse
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <mcheck.h>
#include "parse.h"
#include "timer.h"
#define BUFSIZE 51200

int main(int argc, char** argv)
{
  printf("## PARSE ##\n");

  int option;
  int i;
  int line = 0;
  int idx = 0;
  int idx_p = 0;
  int idx_a = 0;
  int datasize = 1000;
  int size = 1000;
  int sum;
  int charsize = 30;
  unsigned long seed  = (unsigned long) time(NULL);
  Timer timer;
  double startSec = timer.time();
  ifstream ifile;
  char c;
  char buffer[BUFSIZE];
  char delim1[]  = "(), ";
  char delimC[]  = "Course";
  char delimP1[] = "Person";
  char delimS1[] = "Spring";
  char delimS2[] = "Summer";
  char delimA[]  = "Autumn";
  char delimW[]  = "Winter";
  char delimL[]  = "Level";
  char delimY[]  = "Year";
  char delimF[]  = "Faculty";
  char delimT[]  = "Title";
  char delimP2[] = "Project";
  char delimP3[] = "Post";
  char delimP4[] = "Pre";
  int person   = 0;
  int title    = 0;
  int course   = 0;
  int quarter  = 0;
  int level    = 0;
  int project  = 0;
  int position = 0;
  int phase    = 0;
  int integer  = 0;
  int taughtBy    = 1;
  int courseLevel = 1;
  int hasPosition  = 1;
  int advisedBy   = 1;
  int inPhase     = 1;
  int tempAdvisedBy = 1;
  int yearsInProgram  = 1;
  int ta = 1;
  int professor = 1;
  int student = 1;
  int samePerson = 1;
  int sameCourse = 1;
  int sameProject = 1;
  int samePhase = 1;
  int sameInteger = 1;
  int publication = 1;
  char* buffp;
  char* token;
  char data_line[datasize][5][charsize];
  int line_size[datasize];
  char atom[size][charsize];
  char predicate[size][charsize];
  bool flg;

  if (cdebug == true){
    cout << "## reading inputs"<< endl;
  }
  while (( option = getopt(argc, argv, "cd")) != -1)
    switch(option){
    case 'c' :
      cnffile = argv[optind];
      break;
    case 'd' :
      dbfile = argv[optind];
      break;
    default :
      cout << "-c : set cnffile     (default = NULL)" << endl;
      cout << "-d : set dbfile      (default = NULL)" << endl;
      return 1;
    }

  printf("-- db  --\n");
  ifile.open(dbfile);
  if (!ifile){
    cerr << "cannot open db file!"<< endl;
    cerr << dbfile << endl; exit(1);
  }else{
    // get the arguments from the file.
    while (ifile.good()){
      ifile.get(buffer,51200,'\n'); // get a clause
      if (ifile.get(c) && c != '\n'){
        // input string is longer than expected.
        cout << "too big file!" << endl;
        return 0;
      }
      idx = 0;
      buffp = buffer;

      token = strtok(buffp, delim1);
      while(token != NULL){
        strcpy(data_line[line][idx], token);
        idx++;
        token = strtok(NULL, delim1);
      }

      line_size[line] = idx;
      for(int j = 0; j < line_size[line]; j++){
        cout << data_line[line][j] << " ";
      }

      if(idx_p > 0){
        flg = true;
        for(int j = 0; j < idx_p; j++){ 
          if(strcmp(predicate[j], data_line[line][0]) == 0){
            flg = false;
          }
        }
        if(flg){
          strcpy(predicate[idx_p], data_line[line][0]);
          idx_p++;
        }
      }else{
        strcpy(predicate[idx_p], data_line[line][0]);
        idx_p++;
      }

      for(int j = 1; j < idx; j++){
        if(idx_a > 0){
          flg = true;
          for(int k = 0; k < idx_a; k++){ 
            if(strcmp(atom[k], data_line[line][j]) == 0){
              flg = false;
            }
          }
          if(flg){
            strcpy(atom[idx_a], data_line[line][j]);
            idx_a++;
          }
        }else{
          strcpy(atom[idx_a], data_line[line][j]);
          idx_a++;
        }
      }

      printf("\n");
      line++;
    }

    cout << idx_p << endl;
    for(int j = 0; j < idx_p; j++){
      cout << "pred[" << j << "]=" <<  predicate[j] << endl;
    }
    cout << idx_a << endl;
    for(int j = 0; j < idx_a; j++){
      cout << "atom[" << j << "]=" <<  atom[j] << endl;
      if(strstr(atom[j], delimC) != NULL){
        course++;
      }else if(strstr(atom[j], delimP1) != NULL){
        person++;
      }else if(strstr(atom[j], delimS1) != NULL){
        quarter++;
      }else if(strstr(atom[j], delimS2) != NULL){
        quarter++;
      }else if(strstr(atom[j], delimA) != NULL){
        quarter++;
      }else if(strstr(atom[j], delimW) != NULL){
        quarter++;
      }else if(strstr(atom[j], delimL) != NULL){
        level++;
      }else if(strstr(atom[j], delimY) != NULL){
        integer++;
      }else if(strstr(atom[j], delimF) != NULL){
        position++;
      }else if(strstr(atom[j], delimT) != NULL){
        title++;
      }else if(strstr(atom[j], delimP2) != NULL){
        project++;
      }else if(strstr(atom[j], delimP3) != NULL){
        phase++;
      }else if(strstr(atom[j], delimP4) != NULL){
        phase++;
      }else{
        printf("not match!!\n");
      }
    }
  }

  ifile.close();
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

  return 0;
}

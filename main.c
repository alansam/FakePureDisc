//
//  fileloader.c
//  CF.FileLoader
//
//  Created by Alan Sampson on 7/13/20.
//  Copyright © 2020 Alan @ FreeShell. All rights reserved.
//

#define CF_DEBUG
//#undef CF_DEBUG

#define CF_SNAP
//#undef CF_SNAP

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef CF_SNAP
#include "snap.h"
#endif

void Load(double **, double **, double **, int *, double *, const double, const char *);

/*
 *  MARK: main()
 *
 *  @arg declare argc as int
 *  @arg declare argv as array of pointer to const char
 */
int main(int argc, const char * argv[]) {
  printf("CF.FileLoader\n");

  //  Take the filename from program argument.
  if (argc < 1) {
    exit(EXIT_FAILURE);
  }
  
  char const * filename;

  filename = argv[1]; // argv[1] is the dat file name.
  printf("Data file name: %s\n", filename);

  //  model's vertices coordinates z1[], y1[], z1[]
  double * x1 = malloc(0);
  double * y1 = malloc(0);
  double * z1 = malloc(0);

  int n1 = 0;
  const double M_VAL = 0.9907;  //  total mass of the model
  double m1 = 0.; //  individual particle's mass

#ifdef CF_DEBUG
  printf("m0: %p %p %p\n", x1, y1, z1);
#endif

  Load(&x1, &y1, &z1, &n1, &m1, M_VAL, filename);

#ifdef CF_DEBUG
  printf("m1: %p %p %p\n", x1, y1, z1);
#endif

#if defined(CF_SNAP) && defined(CF_DEBUG)
  snap(stdout, x1, n1 * sizeof(double));
  snap(stdout, y1, n1 * sizeof(double));
  snap(stdout, z1, n1 * sizeof(double));
#endif

  //  display results
  printf("individual particle's mass: %f\n", m1);

  printf("count of vertices: %d\n", n1);
  for (size_t i_ = 0; i_ < n1; ++i_) {
    printf("%3zu: %lf %lf %lf\n", i_, x1[i_], y1[i_], z1[i_]);
  }

  //  release storage of vertices arrays
  free(x1);
  free(y1);
  free(z1);

  return EXIT_SUCCESS;
}

/*
 *  MARK: Load()
 *
 *  @arg: declare xp as pointer to pointer to double
 *  @arg: declare yp as pointer to pointer to double
 *  @arg: declare zp as pointer to pointer to double
 *  @arg: declare np as pointer to int
 *  @arg: declare mp as pointer to double
 *  @arg: declare M_VAL as const double
 *  @arg: declare filename as pointer to const char
 */
void Load(double ** xp, double ** yp, double ** zp, int * np, double * mp,
          const double M_VAL, const char * filename) {
  printf("Function: \"%s\" entry\n", __func__);

  FILE * fp = fopen(filename, "r"); // open file for reading
  if (fp == NULL) {
    fprintf(stderr, "Error while opening file %s.\nFunction %s abort...\n", filename, __func__);
    exit(EXIT_FAILURE);
  }

#ifdef CF_DEBUG
  printf("l1: %p %p %p\n", *xp, *yp, *zp);
#endif

  double * xt, * yt, * zt;  //  local copies of vertices

  xt = *xp;   // extract address of x vertex array from remote copy
  yt = *yp;   // extract address of y vertex array from remote copy
  zt = *zp;   // extract address of z vertex array from remote copy

  int n_sz = *np;     // extract current row counter from it's remote copy
  double tempx, tempy, tempz;

  //  read data file and stow elements in vertices arraya
  while (fscanf(fp, "%lf %lf %lf", &tempx, &tempy, &tempz) != EOF) {
    *np = n_sz += 1;  //  increment row count and save to remote copy

    //  reallocate arrays of vertices to add this row
    *xp = xt = realloc(xt, n_sz * sizeof(double));
    *yp = yt = realloc(yt, n_sz * sizeof(double));
    *zp = zt = realloc(zt, n_sz * sizeof(double));

#ifdef CF_DEBUG
    printf("l2: %p %p %p\n", xt, yt, zt);
#endif

    //  insert data elements into arrays
    xt[n_sz - 1] = tempx;
    yt[n_sz - 1] = tempy;
    zt[n_sz - 1] = tempz;
  }

  *mp = M_VAL / (double) n_sz;  // calculate particle's mass

  fclose(fp); //  close the data file

#if defined(CF_SNAP) && defined(CF_DEBUG)
  snap(stdout, xt, n_sz * sizeof(double));
  snap(stdout, yt, n_sz * sizeof(double));
  snap(stdout, zt, n_sz * sizeof(double));
#endif

  //  display vertices
  for (size_t i_ = 0; i_ < n_sz; ++i_) {
    printf("%3zu: %lf %lf %lf\n", i_, xt[i_], yt[i_], zt[i_]);
  }

#ifdef CF_DEBUG
  printf("l3: %p %p %p\n", xt, yt, zt);
#endif

#ifdef CF_DEBUG
  printf("l4: %p %p %p\n", *xp, *yp, *zp);
#endif

  printf("Function: \"%s\" exit\n", __func__);
  return;
}

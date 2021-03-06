#define kowalik_N 11
#define kowalik_P 4

static double kowalik_x0[kowalik_P]= {0.25, 0.39, 0.415, 0.39};
static double kowalik_epsrel= 1.0e-6;

static double kowalik_Y[kowalik_N]= {
    0.1957, 0.1947, 0.1735, 0.1600, 0.0844, 0.0627,
    0.0456, 0.0342, 0.0323, 0.0235, 0.0246};

static double kowalik_U[kowalik_N]= {
    4.0000, 2.0000, 1.0000, 0.5000, 0.2500, 0.1670,
    0.1250, 0.1000, 0.0833, 0.0714, 0.0625};

static void
kowalik_checksol( const double x[], const double sumsq,
                  const double epsrel, const char *sname,
                  const char *pname ) {
 size_t i;
 gsl_vector_const_view v= gsl_vector_const_view_array( x, kowalik_P );
 const double norm= gsl_blas_dnrm2( &v.vector );
 const double sumsq_exact1= 3.075056038492370e-04;
 double kowalik_x1[kowalik_P]= {1.928069345723978e-01,
                                1.912823290344599e-01,
                                1.230565070690708e-01,
                                1.360623308065148e-01};
 const double sumsq_exact2= 0.00102734304869549252;
 double kowalik_x2[kowalik_P]= {0.0, /* inf */
                                -14.0758834005984603,
                                0.0,  /* -inf */
                                0.0}; /* -inf */
 const double *kowalik_x;
 double sumsq_exact;

 kowalik_x2[0]= GSL_NAN;
 kowalik_x2[2]= GSL_NAN;
 kowalik_x2[3]= GSL_NAN;

 if ( norm < 10.0 ) {
  kowalik_x= kowalik_x1;
  sumsq_exact= sumsq_exact1;
 } else {
  kowalik_x= kowalik_x2;
  sumsq_exact= sumsq_exact2;
 }

 gsl_test_rel( sumsq, sumsq_exact, epsrel, "%s/%s sumsq",
               sname, pname );

 for ( i= 0; i < kowalik_P; ++i ) {
  if ( !gsl_finite( kowalik_x[i] ) )
   continue;

  gsl_test_rel( x[i], kowalik_x[i], epsrel, "%s/%s i=%zu",
                sname, pname, i );
 }
}

static int
kowalik_f( const gsl_vector *x, void *params, gsl_vector *f ) {
 double x1= gsl_vector_get( x, 0 );
 double x2= gsl_vector_get( x, 1 );
 double x3= gsl_vector_get( x, 2 );
 double x4= gsl_vector_get( x, 3 );
 size_t i;

 for ( i= 0; i < kowalik_N; ++i ) {
  double yi= kowalik_Y[i];
  double ui= kowalik_U[i];
  double fi= yi - ( x1 * ui * ( ui + x2 ) ) / ( x4 + ui * ( ui + x3 ) );
  gsl_vector_set( f, i, fi );
 }

 (void)params; /* avoid unused parameter warning */

 return GSL_SUCCESS;
}

static int
kowalik_df( const gsl_vector *x, void *params, gsl_matrix *J ) {
 double x1= gsl_vector_get( x, 0 );
 double x2= gsl_vector_get( x, 1 );
 double x3= gsl_vector_get( x, 2 );
 double x4= gsl_vector_get( x, 3 );
 size_t i;

 for ( i= 0; i < kowalik_N; ++i ) {
  double ui= kowalik_U[i];
  double term1= ui * ( ui + x2 );
  double term2= ui * ( ui + x3 ) + x4;

  gsl_matrix_set( J, i, 0, -term1 / term2 );
  gsl_matrix_set( J, i, 1, -ui * x1 / term2 );
  gsl_matrix_set( J, i, 2, ui * term1 * x1 / ( term2 * term2 ) );
  gsl_matrix_set( J, i, 3, term1 * x1 / ( term2 * term2 ) );
 }

 (void)params; /* avoid unused parameter warning */

 return GSL_SUCCESS;
}

static int
kowalik_fvv( const gsl_vector *x, const gsl_vector *v,
             void *params, gsl_vector *fvv ) {
 double x1= gsl_vector_get( x, 0 );
 double x2= gsl_vector_get( x, 1 );
 double x3= gsl_vector_get( x, 2 );
 double x4= gsl_vector_get( x, 3 );
 double v1= gsl_vector_get( v, 0 );
 double v2= gsl_vector_get( v, 1 );
 double v3= gsl_vector_get( v, 2 );
 double v4= gsl_vector_get( v, 3 );
 size_t i;

 for ( i= 0; i < kowalik_N; ++i ) {
  double ui= kowalik_U[i];
  double term2= ui * ( ui + x3 ) + x4;
  double term3= ui * ui * v1 - ui * v3 * x1 - v4 * x1 +
                ui * v1 * x3 + v1 * x4;
  double term4= ui * ui * ( v3 - v2 ) + v4 * x2 +
                ui * ( v4 + v3 * x2 - v2 * x3 ) - v2 * x4;

  gsl_vector_set( fvv, i, 2.0 * ui * term3 * term4 / pow( term2, 3.0 ) );
 }

 (void)params; /* avoid unused parameter warning */

 return GSL_SUCCESS;
}

static gsl_multifit_nlinear_fdf kowalik_func=
    {
        kowalik_f,
        kowalik_df,
        kowalik_fvv,
        kowalik_N,
        kowalik_P,
        NULL,
        0,
        0,
        0};

static test_fdf_problem kowalik_problem=
    {
        "kowalik",
        kowalik_x0,
        NULL,
        NULL,
        &kowalik_epsrel,
        &kowalik_checksol,
        &kowalik_func};

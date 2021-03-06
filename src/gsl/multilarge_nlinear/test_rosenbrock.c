#define rosenbrock_N 2
#define rosenbrock_P 2

static double rosenbrock_x0[rosenbrock_P]= {-1.2, 1.0};
static double rosenbrock_epsrel= 1.0e-12;

static double rosenbrock_J[rosenbrock_N * rosenbrock_P];

static void
rosenbrock_checksol( const double x[], const double sumsq,
                     const double epsrel, const char *sname,
                     const char *pname ) {
 size_t i;
 const double sumsq_exact= 0.0;

 gsl_test_rel( sumsq, sumsq_exact, epsrel, "%s/%s sumsq",
               sname, pname );

 for ( i= 0; i < rosenbrock_P; ++i ) {
  gsl_test_rel( x[i], 1.0, epsrel, "%s/%s i=%zu",
                sname, pname, i );
 }
}

static int
rosenbrock_f( const gsl_vector *x, void *params, gsl_vector *f ) {
 double x1= gsl_vector_get( x, 0 );
 double x2= gsl_vector_get( x, 1 );

 gsl_vector_set( f, 0, 10.0 * ( x2 - x1 * x1 ) );
 gsl_vector_set( f, 1, 1.0 - x1 );

 (void)params; /* avoid unused parameter warning */

 return GSL_SUCCESS;
}

static int
rosenbrock_df( CBLAS_TRANSPOSE_t TransJ, const gsl_vector *x,
               const gsl_vector *u, void *params, gsl_vector *v,
               gsl_matrix *JTJ ) {
 gsl_matrix_view J= gsl_matrix_view_array( rosenbrock_J, rosenbrock_N, rosenbrock_P );
 double x1= gsl_vector_get( x, 0 );

 gsl_matrix_set( &J.matrix, 0, 0, -20.0 * x1 );
 gsl_matrix_set( &J.matrix, 0, 1, 10.0 );
 gsl_matrix_set( &J.matrix, 1, 0, -1.0 );
 gsl_matrix_set( &J.matrix, 1, 1, 0.0 );

 if ( v )
  gsl_blas_dgemv( TransJ, 1.0, &J.matrix, u, 0.0, v );

 if ( JTJ )
  gsl_blas_dsyrk( CblasLower, CblasTrans, 1.0, &J.matrix, 0.0, JTJ );

 (void)params; /* avoid unused parameter warning */

 return GSL_SUCCESS;
}

static int
rosenbrock_fvv( const gsl_vector *x, const gsl_vector *v,
                void *params, gsl_vector *fvv ) {
 double v1= gsl_vector_get( v, 0 );

 gsl_vector_set( fvv, 0, -20.0 * v1 * v1 );
 gsl_vector_set( fvv, 1, 0.0 );

 (void)x;      /* avoid unused parameter warning */
 (void)params; /* avoid unused parameter warning */

 return GSL_SUCCESS;
}

static gsl_multilarge_nlinear_fdf rosenbrock_func=
    {
        rosenbrock_f,
        rosenbrock_df,
        rosenbrock_fvv,
        rosenbrock_N,
        rosenbrock_P,
        NULL,
        0,
        0,
        0,
        0};

static test_fdf_problem rosenbrock_problem=
    {
        "rosenbrock",
        rosenbrock_x0,
        NULL,
        &rosenbrock_epsrel,
        &rosenbrock_checksol,
        &rosenbrock_func};

#define vardim_N 7 /* p + 2 */
#define vardim_P 5

static double vardim_x0[vardim_P]= {0.8, 0.6, 0.4, 0.2, 0.0};
static double vardim_epsrel= 1.0e-12;

static double vardim_J[vardim_N * vardim_P];

static void
vardim_checksol( const double x[], const double sumsq,
                 const double epsrel, const char *sname,
                 const char *pname ) {
 size_t i;
 const double sumsq_exact= 0.0;

 gsl_test_rel( sumsq, sumsq_exact, epsrel, "%s/%s sumsq",
               sname, pname );

 for ( i= 0; i < vardim_P; ++i ) {
  gsl_test_rel( x[i], 1.0, epsrel, "%s/%s i=%zu",
                sname, pname, i );
 }
}

static int
vardim_f( const gsl_vector *x, void *params, gsl_vector *f ) {
 size_t i;
 double sum= 0.0;

 for ( i= 0; i < vardim_P; ++i ) {
  double xi= gsl_vector_get( x, i );

  gsl_vector_set( f, i, xi - 1.0 );

  sum+= ( i + 1.0 ) * ( xi - 1.0 );
 }

 gsl_vector_set( f, vardim_P, sum );
 gsl_vector_set( f, vardim_P + 1, sum * sum );

 (void)params; /* avoid unused parameter warning */

 return GSL_SUCCESS;
}

static int
vardim_df( CBLAS_TRANSPOSE_t TransJ, const gsl_vector *x,
           const gsl_vector *u, void *params, gsl_vector *v,
           gsl_matrix *JTJ ) {
 gsl_matrix_view J= gsl_matrix_view_array( vardim_J, vardim_N, vardim_P );
 size_t i;
 double sum= 0.0;
 gsl_matrix_view m= gsl_matrix_submatrix( &J.matrix, 0, 0, vardim_P, vardim_P );

 gsl_matrix_set_identity( &m.matrix );

 for ( i= 0; i < vardim_P; ++i ) {
  double xi= gsl_vector_get( x, i );
  sum+= ( i + 1.0 ) * ( xi - 1.0 );
 }

 for ( i= 0; i < vardim_P; ++i ) {
  gsl_matrix_set( &J.matrix, vardim_P, i, i + 1.0 );
  gsl_matrix_set( &J.matrix, vardim_P + 1, i, 2 * ( i + 1.0 ) * sum );
 }

 if ( v )
  gsl_blas_dgemv( TransJ, 1.0, &J.matrix, u, 0.0, v );

 if ( JTJ )
  gsl_blas_dsyrk( CblasLower, CblasTrans, 1.0, &J.matrix, 0.0, JTJ );

 (void)params; /* avoid unused parameter warning */

 return GSL_SUCCESS;
}

static int
vardim_fvv( const gsl_vector *x, const gsl_vector *v,
            void *params, gsl_vector *fvv ) {
 size_t i;
 double sum= 0.0;

 gsl_vector_set_zero( fvv );

 for ( i= 0; i < vardim_P; ++i ) {
  double vi= gsl_vector_get( v, i );
  sum+= ( i + 1.0 ) * vi;
 }

 gsl_vector_set( fvv, vardim_N - 1, 2.0 * sum * sum );

 (void)x;      /* avoid unused parameter warning */
 (void)params; /* avoid unused parameter warning */

 return GSL_SUCCESS;
}

static gsl_multilarge_nlinear_fdf vardim_func=
    {
        vardim_f,
        vardim_df,
        vardim_fvv,
        vardim_N,
        vardim_P,
        NULL,
        0,
        0,
        0,
        0};

static test_fdf_problem vardim_problem=
    {
        "vardim",
        vardim_x0,
        NULL,
        &vardim_epsrel,
        &vardim_checksol,
        &vardim_func};

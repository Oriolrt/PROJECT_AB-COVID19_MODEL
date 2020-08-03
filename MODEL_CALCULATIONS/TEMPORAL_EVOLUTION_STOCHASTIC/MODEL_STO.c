#include "../../Include/MODEL.h"

extern gsl_rng * r;

int M_O_D_E_L___S_T_O( Parameter_Table * Table )
{
  int i,j,k, n;
  int I_Time, no_Patch;
  int Bad_Times; 
  Time_Control * Time = Table->T;
  
  Parameter_Model * P = (Parameter_Model *)malloc( 1 * sizeof(Parameter_Model) );
  P_A_R_A_M_E_T_E_R___I_N_I_T_I_A_L_I_Z_A_T_I_O_N (Table, P);
  Table->P  = P;
  printf(" Parameter_Model structure has been correctly allocated and initiated\n");
  
  I_Time    = Time->I_Time;
  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */  
  int MODEL_STATE_VARIABLES = Table->MODEL_STATE_VARIABLES;
  Table->Vector_Model_Variables = (double *)calloc( MODEL_STATE_VARIABLES, sizeof(double) );
  Table->Vector_Model_Int_Variables = (int *)calloc( MODEL_STATE_VARIABLES, sizeof(int) );  
  
  /* BEGIN : -------------------------------------------------------------------------
   * Stochastic Community Set Up
   */
  Community ** PATCH = (Community **)malloc( P->No_of_LOCAL_POPULATIONS * sizeof(Community *) );
  C_O_M_M_U_N_I_T_Y___A_L_L_O_C_A_T_I_O_N ( PATCH, P ); 
  C_O_M_M_U_N_I_T_Y___I_N_I_T_I_A_L_I_Z_A_T_I_O_N (PATCH, P);
  /* The Parameter Model structure also keeps the three memmory addresses pointing to 
   *   the Patch System, the Time Control structure, and the CPG structure to plot   
   */
  Table->Patch_System = PATCH;
  P->Time             = Table->T;
  P->CPG              = Table->CPG_STO; 
  /* END ----------------------------------------------------------------------------
   */

#if defined CPGPLOT_REPRESENTATION  /* Initial Plotting Time evolution: just frames!!! */
  int SAME_PLOT = 0;
  // C_P_G___S_U_B___P_L_O_T_T_I_N_G___n___P_L_O_T_S( CPG->DEVICE_NUMBER,
  //                                                  SAME_PLOT, 0, Table );
#endif
  
  /* BEGIN: Main loop: a number of REALIZATIONS (stochastic temporal evolutions) is computed */
  printf("Entering Generation of Stochastic Realizations...\n");   Press_Key();
  for (i=0; i < Time->Realizations; i++){
    /* Input variables: 
       . i, lable of current realization 
       . P, a comprehensive model parameter table (see definition in MODEL.h)
       . Bad_Times is a measure of the performance of the sampling frequency. 
         If Bad_Times is high, interval times should be choosen smaller 
    */
    S_T_O_C_H_A_S_T_I_C___T_I_M_E___D_Y_N_A_M_I_C_S ( i, Table, &Bad_Times );
    
    /* End of the i-th STOCHASTIC REALIZATIONS */
    printf("Realization: %d of a total of %d\n", i+1, Time->Realizations);
    printf("Time failed in %d occasions out of %d time steps\n", Bad_Times, I_Time);
    printf("If the number of failed times is too big, EPSILON might be too small!\n");
    printf("Try to choose a larger EPSILON [Current value: -E %g]\n", P->Time->EPSILON);
  }
  /* END: End of STOCHASTIC REALIZATIONS */

  /* BEGIN : Averaging and saving stochastic realizations */
  /*         One file per each output variable            */  
  printf( " \nAs many as %d stochastic realizations have been successfully completed\n",
          Time->Realizations);
  printf( " Averages and Variances over the ensamble of realizations\n");
  printf( " will be calculated now...\n");
  Press_Key();
  int DATA_POINTS = Time_Control_AVE_VAR_SAVE_VARIABLES( Table );
  printf(" Temporal series of %d (out of %d) data points\n",
	 DATA_POINTS, I_Time);

#if defined CPGPLOT_REPRESENTATION
  SAME_PLOT = 1; 
  C_P_G___S_U_B___P_L_O_T_T_I_N_G___E_R_R_O_R___B_A_R ( Table, SAME_PLOT, 
							DATA_POINTS, Time->time_DEF, 
							Time->AVE, Time->VAR ); 
#endif
  /*   END : Averaging stochastic realizations            */  

  free( Table->Vector_Model_Variables ); 
  free( Table->Vector_Model_Int_Variables );
  
  C_O_M_M_U_N_I_T_Y___F_R_E_E (PATCH, P->No_of_LOCAL_POPULATIONS);
  free ( P );
  
  return(0);
}
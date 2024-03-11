
#include <ilcplex/cplex.h>
//#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

int main(int argc, char *argv[])
{
   //Controlla se il nome del file .mps è stato dato con successo
   if (argc != 2)
   {
      printf("Errore: parametri del problema inesatti");
      goto TERMINATE;
   }
      
   // Inizializza le variabili adibite per lo sviluppo del problema */
   CPXENVptr env = NULL;
   CPXLPptr mip = NULL;
   int status = 0;
   double result = 0;
   int j = 0;
   int * random_variables = NULL; 
   char * set_bounds = NULL;
   char * set_ub = NULL;
   char * set_lb = NULL;
   double * set_values = NULL;
   double * old_ub = NULL;
   double * old_lb = NULL;
   int * total_variables = NULL;
   srand(time(NULL));
   int solution_status = 0;

   //Crea l'ambiente per il problema MIP, in caso di errore va a TERMINATE 
   env = CPXopenCPLEX (&status);
   if (env == NULL)
   {
      printf("Errore nella creazione dell'ambiente\n");
      goto TERMINATE;
   }


   //Crea il problema usando il nome del file, in caso di errore va a TERMINATE
   mip = CPXcreateprob(env, &status, argv[1]);
   
   if (status)
   {
      printf("Errore nella creazione del problema\n");
      goto TERMINATE;
   }

   //Legge i dati da file, in caso di errore va TERMINATE
   status = CPXreadcopyprob(env, mip, argv[1], NULL);

   if (status)
   {
      printf("Errore nella lettura del problema da file\n");
      goto TERMINATE;
   }

   //Ottiene numero di colonne e crea l'array dove verranno salvati i valori delle variabili una volta ottenuta una soluzione
   int num_cols = CPXgetnumcols (env, mip);
   double * x = (double *) malloc (num_cols * sizeof(double));

   //Setta il tipo di algoritmo da usare, in caso di errore va a TERMINATE
   status = CPXsetintparam (env, CPXPARAM_LPMethod, CPX_ALG_AUTOMATIC);

   if (status)
   {
      printf("Errore nel settare l'algoritmo\n");
      goto TERMINATE;
   }

   //Setta la sola soluzione del problema MIP al solo nodo radice, in caso di errore va a TERMINATE
   status = CPXsetintparam (env, CPXPARAM_MIP_Limits_Nodes, 1);

   if (status)
   {
      printf("Errore nel settare il numero di nodi massimo\n");
      goto TERMINATE;
   }

   //Ottiene gli upperbound originali delle variabili del problema, in caso di errore va a TERMINATE
   old_ub = (double *) malloc (num_cols * sizeof(double));
   status = CPXgetub(env, mip, old_ub, 0, num_cols-1);

   if (status)
   {
      printf("Errore nell'ottenere gli upperbound");
      goto TERMINATE;
   }

   //Ottiene i lowerbound originali delle variabili del problema, in caso di errore va a TERMINATE
   old_lb = (double *) malloc (num_cols * sizeof(double));
   status = CPXgetlb(env, mip, old_lb, 0, num_cols-1);

   if (status)
   {
      printf("Errore nell'ottenere i lowerbound");
      goto TERMINATE;
   }

   //Crea un array utilizzato per specificare che i valori vengono utilizzati come lowerbound
   set_lb = (char *) malloc (num_cols * sizeof(char));
   while (j < num_cols)
   {
      set_lb[j] = 'L';
      j++;
   }
   j = 0;


   //Crea un array utilizzato per specificare che i valori vengono utilizzati come upperbound
   set_ub = (char *) malloc (num_cols * sizeof(char));
   while (j < num_cols)
   {
      set_ub[j] = 'U';
      j++;
   }
   j = 0; 

   //Crea un array dove gli indici sono il numero di variabili del problema
   total_variables = (int *) malloc (num_cols * sizeof(int));
   while (j < num_cols)
   {
      total_variables[j] = j;
      j++;
   } 
   j = 0;

   //Array che indica che i bound delle variabili sono settate sia come upper che come lower bound
   set_bounds = (char *) malloc ((num_cols/2) * sizeof(char));

   while (j < num_cols/2)
   {
      set_bounds[j] = 'B';
      j++;
   }
   j = 0;

   //Alloca lo spazio per gli indici randomici e per i valori assunti dalle variabili nelle soluzioni trovate
   random_variables = (int *) malloc ((num_cols/2)* sizeof(int));
   set_values = (double *) malloc ((num_cols/2) * (sizeof(double)));


   //Itera fino ad ottenere la soluzione ottimale
   while (1)
   //while(solution_status != CPXMIP_OPTIMAL)
   {

      //Risolve il problema, in caso di errore va a TERMINATE
      status = CPXmipopt(env, mip);
      if (status)
      {
         printf("Errore nell'ottimizzazione\n");
         goto TERMINATE;
      }

      printf("%d\n",CPXgetstat(env, mip));
      solution_status = CPXgetstat(env, mip);

      //Salva il risultato nella variabile result, in caso di errore va a TERMINATE
      status = CPXgetobjval(env, mip, &result);

      if (status)
      {
         printf("Errore nell'ottenere il risultato ottimo\n");
         goto TERMINATE;
      }

      //Si scrivono in x i valori delle variabili una volta ottenuta la soluzione
      status = CPXgetx(env, mip, x, 0, num_cols-1);

      if (status)
      {
         printf("Errore nell'ottenere il valore delle variabili\n");
         goto TERMINATE;
      }

      //Array che crea gli indici casuali delle variabili
      while (j < num_cols/2)
      {
         //random_variables[j] = rand()%(num_cols);
         int index = rand()%(num_cols);
         int presente = 0;
         int i = 0;
         while (i < j)
         {
            if(random_variables[i] == index)
            {
               presente = 1;
               break;
            }
            i++;
         }
         if(!presente)
         {
            random_variables[j] = index;
            j++;
         }
      
      }
      j = 0;

      
      //Si ottengo i valori delle variabili che verranno fissate
      while (j < num_cols/2)
      {
         set_values[j] = x[random_variables[j]];
         j++;

      }
      j = 0;

      //Si sfissano le variabili precedentemente fissate
      status = CPXchgbds(env, mip, num_cols, total_variables, set_lb, old_lb );
      status = CPXchgbds(env, mip, num_cols, total_variables, set_ub, old_ub );

      //Se ne fissano di nuove
      status = CPXchgbds(env, mip, num_cols/2, random_variables, set_bounds, set_values );

      if (status)
      {
         printf("Errore nel settare le nuove variabili");
         goto TERMINATE;
      }

      CPXsolution(env, mip, &solution_status, NULL, NULL, NULL, NULL, NULL);
      if (solution_status == CPXMIP_OPTIMAL || solution_status == CPXMIP_OPTIMAL_TOL)
      {
         printf("Soluzione ottima trovata: %f\n",result);
         break;
      }


      printf("%f\n",result);
      printf("%d\n", solution_status);
   }

   goto TERMINATE;





   TERMINATE:

   //Libera il puntatore del problema

   if (mip != NULL) 
   {
      status = CPXfreeprob (env, &mip);
      if (status) 
      {
         printf("Errore: impossibile chiudere il problema MIP");
      }
   }

   //Libera il puntatore all'ambiente CPLEX

   if (env != NULL) 
   {
      status = CPXcloseCPLEX (&env);
      if (status)
      { 
         printf("Errore: impossibile chiudere l'ambiente CPLEX");
      }          
   }

   free(x);
   free(random_variables);
   free(set_bounds);
   free(set_values); 
   free(total_variables);
   free(old_lb);
   free(old_ub);
   free(set_lb);
   free(set_ub);
     
   return (status);
}  


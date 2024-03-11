
#include <ilcplex/cplex.h>
//#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

char * set_char_array (char * a, int size, char bound);
int * random_indexes (int * a, int size, int max_index);

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
   double current_time = 0;
   double start_time = 0;
   double elapsed_time = 0;

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
   int rate_fixed_variables = num_cols/2;
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
   set_lb = set_char_array(set_lb, num_cols, 'L');


   //Crea un array utilizzato per specificare che i valori vengono utilizzati come upperbound
   set_ub = set_char_array(set_ub, num_cols, 'U'); 

   //Crea un array dove gli indici sono il numero di variabili del problema
   total_variables = (int *) malloc (num_cols * sizeof(int));
   while (j < num_cols)
   {
      total_variables[j] = j;
      j++;
   } 
   j = 0;

   //Array che indica che i bound delle variabili sono settate sia come upper che come lower bound
   set_bounds = set_char_array(set_bounds, rate_fixed_variables, 'B');

   //Alloca lo spazio per i valori assunti dalle variabili nelle soluzioni trovate, usati poi per fissare variabili
   set_values = (double *) malloc ((rate_fixed_variables) * (sizeof(double)));

   //Ottiene il tempo prima di cominciare ad iterare
   CPXgettime(env, &start_time);

   //Itera fino ad ottenere la soluzione ottimale
   while (elapsed_time < 100)
   {

      //Risolve il problema, in caso di errore va a TERMINATE
      status = CPXmipopt(env, mip);
      if (status)
      {
         printf("Errore nell'ottimizzazione\n");
         goto TERMINATE;
      }

      CPXsolution (env, mip, &solution_status, NULL, NULL, NULL, NULL, NULL);
      //printf("%d\n",solution_status);

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

      random_variables = random_indexes(random_variables, rate_fixed_variables, num_cols);

            
      //Si ottengo i valori delle variabili che verranno fissate
      while (j < rate_fixed_variables)
      {
         set_values[j] = x[random_variables[j]];
         j++;

      }
      j = 0;

      //Si sfissano le variabili precedentemente fissate
      status = CPXchgbds(env, mip, num_cols, total_variables, set_lb, old_lb );
      status = CPXchgbds(env, mip, num_cols, total_variables, set_ub, old_ub );

      //Se ne fissano di nuove
      status = CPXchgbds(env, mip, rate_fixed_variables, random_variables, set_bounds, set_values );

      if (status)
      {
         printf("Errore nel settare le nuove variabili");
         goto TERMINATE;
      }



      //Si ottiene da quanto tempo si sta iterando
      CPXgettime(env, &current_time);
      elapsed_time = current_time - start_time;


      printf("%f\n", result);
      //printf("%f\n", elapsed_time );
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
} //FINE MAIN


//Funzione che alloca un array di char e indica se setterà il lower, l'upper o il valore di 'size' variabili
char * set_char_array (char * a, int size, char bound)
{
   int j = 0;
   a = (char *) malloc (size * sizeof(char));

   while (j < size)
   {
      a[j] = bound;
      j++;
   }

   return a;
} 

//Funzione che alloca un array di int e crea 'size' indici casuali tra 0 e max_index-1
int * random_indexes (int * a, int size, int max_index)
{
   free(a);
   int j = 0;
   a = (int *) malloc (size * sizeof(int));

   while (j < size)
      {
         int index = rand()%(max_index);
         int presente = 0;
         int i = 0;
         while (i < j)
         {
            if(a[i] == index)
            {
               presente = 1;
               break;
            }
            i++;
         }
         if(!presente)
         {
            a[j] = index;
            j++;
         }     
      }

   return a;

}

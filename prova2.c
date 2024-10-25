
#include <ilcplex/cplex.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

char * set_char_array (char * a, int size, char bound);
int * random_indexes (int * a, int size, int max_index);
double count_remaining_time(double start_time, double current_time);
double calc_gap (double primal_bound, double dual_bound);

int main(int argc, char *argv[])
{
   //Controlla se il nome del file .mps è stato dato con successo
   if (argc != 2)
   {
      printf("Errore: parametri del problema inesatti\n");
      goto TERMINATE;
   }
      
   // Inizializza le variabili adibite per lo sviluppo del problema 
   CPXENVptr env = NULL;
   CPXLPptr mip = NULL;
   int status = 0;
   double result = 0;
   double first_result = 0;
   double results[11] = {0};
   double gaps[11] = {0};
   double times[10] = {0};
   double gap = 0;
   double perc_miglioramento = 0;
   double perc_miglioramento_gap = 0;
   double first_node_time = 0;
   int j = 0;
   int i = 1;
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
   double temp_time = 0;
   double init_loop_time = 0;
   double end_loop_time = 0;
   int cont = 0;
   int number_solutions = 0;
   char result_file [20];
   int program_pid = getpid();
   double primal_bound = 0;
   double dual_bound = 0;

   //Creo il file su cui scrivere i risultati
   sprintf(result_file, "/home/farrukujur/tesi2/risultati/output_%s_5.txt", argv[1]);
   printf("%s\n", argv[1]);
   FILE *fp = fopen(result_file, "w");

   if (fp == NULL)
   {
      printf("Errore nel creare il file per raccolta dati\n");
      goto TERMINATE;
   }

   //Crea l'ambiente per il problema MIP, in caso di errore va a TERMINATE 
   env = CPXopenCPLEX (&status);

   if (env == NULL)
   {
      printf("Errore nella creazione dell'ambiente\n");
      goto TERMINATE;
   }

   //Ottiene il tempo all'avvio dell'ambiente
   CPXgettime(env, &start_time);

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

   //Ottiene il numero di colonne, ovvero di variabili, e crea l'array dove verranno salvati i valori delle variabili una volta ottenuta una soluzione
   int num_cols = CPXgetnumcols (env, mip);
   int rate_fixed_variables = num_cols * 0.5;
   double * x = (double *) malloc (num_cols * sizeof(double));

   //Setta il tipo di algoritmo da usare, in caso di errore va a TERMINATE
   status = CPXsetintparam (env, CPXPARAM_LPMethod, CPX_ALG_AUTOMATIC);

   if (status)
   {
      printf("Errore nel settare l'algoritmo\n");
      goto TERMINATE;
   }

   //Setta la sola soluzione del problema MIP al solo nodo radice, in caso di errore va a TERMINATE
   status = CPXsetintparam (env, CPXPARAM_MIP_Limits_Nodes, 0);

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

   CPXgettime(env, &current_time);

   //Setta il tempo per la risoluzione al nodo radice
   status = CPXsetdblparam(env, CPX_PARAM_TILIM, count_remaining_time(start_time, current_time));
   if(status)
   {
      printf("Errore nel settare il tempo\n");
      goto TERMINATE;
   }

   //Risolve il problema al nodo radice
   status = CPXmipopt(env, mip);

   if (status)
   {
      printf("Errore nell'ottimizzare\n");
      goto TERMINATE;
   }

   //Calcola il tempo necessario a risolvere il mip al nodo radice
   CPXgettime(env, &temp_time);
   first_node_time = temp_time - current_time;

   //Ottiene il numero di soluzioni trovate, se non ve ne sono termina il programma
   number_solutions = CPXgetsolnpoolnumsolns(env, mip);

   if (!number_solutions)
   {
      printf("Impossibile risolvere il problema al nodo radice\n");
      fprintf(fp, "Impossibile risolvere il problema al nodo radice\n");
      goto TERMINATE;
   }

   //Salva il risultato del primo risultato ottenuto, in caso di fallimento termina il programma
   status = CPXgetobjval(env, mip, &first_result);

   if (status)
   {
      printf("Errore nell'ottenere la soluzione\n");
      goto TERMINATE;
   }

   primal_bound = first_result;

   //Trovo il dual bound
   status = CPXgetbestobjval(env, mip, &dual_bound);

   if (status)
   {
	  printf("errore nell'ottenere il dual\n");
	  goto TERMINATE;
   }

   gaps[0] = calc_gap(primal_bound, dual_bound);

   //Setta il  numero di nodi da risolvere a 200
   status = CPXsetintparam (env, CPXPARAM_MIP_Limits_Nodes, 200);

   if (status)
   {
      printf("Errore nel settare il numero di nodi da risolvere\n");
      goto TERMINATE;
   }
  
  //Calcola il tempo trascorso sino ad ora 
  CPXgettime(env, &current_time);
  elapsed_time = current_time - start_time;

  //Ottengo il tempo prima di iterare per 10 volte
  CPXgettime(env, &temp_time);

   //Itera per 10 volte e per un massimo di 30 minuti
   while (elapsed_time < 1800 && cont < 10)
   {
      //Ottengo il tempo ad inizio di ogni iterazione
      CPXgettime(env, &init_loop_time);

      //Se è la prima volta che itera fissa il 50% delle variabili
      if (!cont)
      {
         set_bounds = set_char_array(set_bounds, rate_fixed_variables, 'B');
         set_values = (double *) malloc ((rate_fixed_variables) * (sizeof(double)));
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

      //Si sfissano le variabili precedentemente fissate, solo se è una iterazione diversa alla prima
      if (cont)
      {
         status = CPXchgbds(env, mip, num_cols, total_variables, set_lb, old_lb );
         status = CPXchgbds(env, mip, num_cols, total_variables, set_ub, old_ub );
      }

      if (status)
      {
         printf("Errore nello sfissare le vecchie variabili");
         goto TERMINATE;
      }

      //Se ne fissano di nuove
      status = CPXchgbds(env, mip, rate_fixed_variables, random_variables, set_bounds, set_values );

      if (status)
      {
         printf("Errore nel settare le nuove variabili");
         goto TERMINATE;
      }

      //Calcolo il tempo attuale
      CPXgettime(env, &current_time);

      //Setto il tempo ti risoluzione con il tempo rimanente dai 30 minuti, quindi 30 - (current_time - start_time)
      status = CPXsetdblparam(env, CPX_PARAM_TILIM, count_remaining_time(start_time, current_time));

      if (status)
      {
	      printf("Errore nel settare il tempo");
	      goto TERMINATE;
      }

      //Si risolve il subMIP con il limite di 200 nodi ed il tempo limite
      status = CPXmipopt(env, mip);

      if (status)
      {
         printf("Errore nell'ottimizzazione\n");
         goto TERMINATE;
      }

      //Salva il risultato nella variabile result, in caso di errore va a TERMINATE
      status = CPXgetobjval(env, mip, &result);

      if (status)
      {
         printf("Errore nell'ottenere il risultato ottimo\n");
         goto TERMINATE;
      }

      //Calcolo il miglioramento della soluzione in percentuale
      gaps[i] = calc_gap(result, dual_bound);
      fprintf(fp, "%f\n", gaps[i-1] - gaps[i]);


      //Si ottiene da quanto tempo si sta iterando
      CPXgettime(env, &current_time);
      elapsed_time = current_time - start_time;
      
      times[cont] = current_time - init_loop_time;

      cont++;
      i++;

   }


   fprintf(fp, "%f\n", elapsed_time);
   fprintf(fp, "%f\n", first_node_time);

   for (j = 0; j < 10; j++)
   {
	   fprintf(fp, "%f\n", times[j]);
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
   fclose(fp);
     
   return (status);
} //FINE MAIN


//Funzione che alloca un array di char e indica se setterà il lower, l'upper o il valore di 'size' variabili
char * set_char_array (char * a, int size, char bound)
{
   free (a);
   int j = 0;
   a = (char *) malloc (size * sizeof(char));

   while (j < size)
   {
      a[j] = bound;
      j++;
   }

   return a;
} 

//Funzione che alloca un array di int e crea 'size' indici casuali tra 0 e max_index-1 tutti diversi tra loro
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

//Funzione che calcola il tempo rimanente per ottimizzare
double count_remaining_time (double start_time, double current_time)
{	
   double remaining = 1800.0 - (current_time - start_time);
   return remaining; 
}

//Funzione che calcola il gap relativo
double calc_gap(double primal_bound, double dual_bound)
{
	double abs_primal = fabs(primal_bound);
	double abs_dual = fabs(dual_bound);

	if (abs_primal < 1e-6 && abs_dual < 1e-6)
		return 0.0;

	if(primal_bound * dual_bound < 0)
		return 1.0;
	
	if(abs_primal > abs_dual)
		return (fabs(primal_bound - dual_bound)/abs_primal);
	else 
		return (fabs(primal_bound - dual_bound)/abs_dual);
}

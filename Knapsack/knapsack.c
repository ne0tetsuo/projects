#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

// Fonction utilitaire pour obtenir le maximum de deux nombres.
int max(int a, int b) { return (a > b) ? a : b; }
int min(int a, int b)
{ return a < b ? a : b;}
// Définition de la structure pour les objets.
typedef struct
{
    unsigned int weight;
    unsigned int value;
} Item;

// Variables globales pour l'algorithme de backtracking
// (ATTENTION: nous utilisons les variables globales dans cette version pour suivre le schéma vu en cours et en td)
Item *g_itmes;           // tableau d'objets
int g_n;                 // nombre d'objets
int g_W;                 // capacité du sac
int g_bestValue = 0;     // meilleure valeur trouvée jusqu'à présent
int g_currentValue = 0;  // valeur courante du sac
int g_currentWeight = 0; // poids courant du sac

// implémentation récursive de l'algorithme de backtracking
void knapsackBT1(int idx)
{
    // Si l'indice actuel est hors le tableau ou si le sac à dos est plein, mettre à jour bestValue si nécessaire
    if (idx == g_n || g_currentWeight == g_W)
    {
        g_bestValue = max(g_bestValue, g_currentValue);
        return;
    }

    // sauter l'objet courant
    knapsackBT1(idx + 1);

    // Inclure l'objet actuel s'il ne dépasse pas la capacité du sac à dos
    if (g_currentWeight + g_itmes[idx].weight <= g_W)
    {
        // Mettre à jour currentWeight et currentValue avant l'appel récursif
        g_currentWeight += g_itmes[idx].weight;
        g_currentValue += g_itmes[idx].value;

        knapsackBT1(idx + 1);

        // Annuler la décision d'inclure l'objet (backtracking)
        g_currentWeight -= g_itmes[idx].weight;
        g_currentValue -= g_itmes[idx].value;
    }
}

// Algorithme de backtracking sans variables globales
int knapsackBTUtil(Item items[], int n, int W, int idx, int currentValue, int *bestValue)
{
    if (idx == n || W == 0)
    {
        if (currentValue > *bestValue)
            *bestValue = currentValue;
        return *bestValue;
    }

    // Ne pas inclure l'objet idx
    knapsackBTUtil(items, n, W, idx + 1, currentValue, bestValue);

    // Inclure l'objet idx si le poids le permet
    if (items[idx].weight <= W)
    {
        knapsackBTUtil(items, n, W - items[idx].weight, idx + 1, currentValue + items[idx].value, bestValue);
    }

    return *bestValue;
}

int knapsackBT2(Item items[], int n, int W)
{
    int bestValue = 0;
    return knapsackBTUtil(items, n, W, 0, 0, &bestValue);
}
// Fonction de backtracking avec élagage sans utiliser les variables globales
/*int knapsackBTelagageUtil(Item items[], int n, int W, int idx, int currentValue, int *bestValue,int *borne_somme){
    if (idx == n || W == 0) {
        if (currentValue > *bestValue)
            *bestValue = currentValue;
        return *bestValue;
    }
    if(borne_somme[idx]+currentValue<=*bestValue){
        return *bestValue;
    }
    // Ne pas inclure l'objet idx
    knapsackBTelagageUtil(items, n, W, idx + 1, currentValue, bestValue,borne_somme);

    // Inclure l'objet idx si le poids le permet
    if (items[idx].weight <= W) {
        knapsackBTelagageUtil(items, n, W - items[idx].weight, idx + 1, currentValue + items[idx].value, bestValue,borne_somme);
    }

    return *bestValue;
}

int knapsackBTelagae(Item items[], int n, int W) {
    int bestValue = 0;
    int borne_somme[n];
    for(int i=0;i<n;++i){
        borne_somme[i]=0;
        for(int j=0;j<n;++j){
            borne_somme[i]+=items[j].value;
        }
    }
    return knapsackBTelagageUtil(items, n, W, 0, 0, &bestValue,borne_somme);
}

*/
// Fonction Backtracking avec élagage utilisant les variables globales (à la mainére de la fct BT1)
void knapsackBT3(int idx)
{
    if (idx == g_n || g_currentWeight == g_W)
    {
        g_bestValue = max(g_bestValue, g_currentValue);
        return;
    }

    int upper_bound = g_currentValue;
    for (int i = idx; i < g_n; ++i)
    {
        upper_bound += g_itmes[i].value;
    }

    if (upper_bound <= g_bestValue)
    {
        return;
    }
    knapsackBT3(idx + 1);
    if (g_currentWeight + g_itmes[idx].weight <= g_W)
    {
        g_currentWeight += g_itmes[idx].weight;
        g_currentValue += g_itmes[idx].value;

        knapsackBT3(idx + 1);

        g_currentWeight -= g_itmes[idx].weight;
        g_currentValue -= g_itmes[idx].value;
    }
}

// Fonction de programmation dynamique pour le problème du sac à dos 0/1.
int knapsackDP(int W, Item items[], int n)
{
    int i, w;
    int **K = (int **)malloc((n + 1) * sizeof(int *));
    for (i = 0; i <= n; i++)
        K[i] = (int *)malloc((W + 1) * sizeof(int));

    for (i = 0; i <= n; i++)
    {
        for (w = 0; w <= W; w++)
        {
            if (i == 0 || w == 0)
                K[i][w] = 0;
            else if (items[i - 1].weight <= w)
                K[i][w] = max(items[i - 1].value + K[i - 1][w - items[i - 1].weight], K[i - 1][w]);
            else
                K[i][w] = K[i - 1][w];
        }
    }

    int result = K[n][W];
    for (i = 0; i <= n; i++)
        free(K[i]);
    free(K);
    return result;
}

// Fontion qui crée et initialise le tableau dpv (à utiliser dans la fonction knapsackvalue)
int *knapsackdpv(int V, Item items[], int n)
{
    int *dpv = malloc((V + 1) * sizeof(int));
    dpv[0] = 0;
    for (int i = 1; i <= V; ++i)
    {
        dpv[i] = INT_MAX;
    }
    for (int i = 0; i < n; ++i)
    {
        for (int j = V; j >= items[i].value; --j)
        {
            if (dpv[j - items[i].value] != INT_MAX)
                dpv[j] = min(dpv[j - items[i].value] + items[i].weight, dpv[j]);
        }
    }
    return dpv;
}
//Fonction qui retrouve la solution optimale à partir du tableau dpv
int knapsackvalue(int W, Item items[], int n)
{
    int V = 0;
    for (int i = 0; i < n; ++i)
    {
        V += items[i].value;
    }
    int *dpv = knapsackdpv(V, items, n);
    int i = V;
    while (dpv[i] > W && i > 0)
    {
        i--;
    }
    int result = i;
    free(dpv);
    return result;
}
//Fonction DP avec poids minimum C.
int knapsackDPMin(Item items[], int n, int C, int M)
{
    int **dp = (int **)malloc((n + 1) * sizeof(int *));
    for (int i = 0; i <= n; i++)
    {
        dp[i] = (int *)malloc((C + 1) * sizeof(int));
        for (int w = 0; w <= C; w++)
        {
            dp[i][w] = 0;
        }
    }

    for (int i = 1; i <= n; i++)
    {
        for (int w = 0; w <= C; w++)
        {
            if (items[i - 1].weight <= w)
            {
                dp[i][w] = max(dp[i - 1][w], dp[i - 1][w - items[i - 1].weight] + items[i - 1].value);
            }
            else
            {
                dp[i][w] = dp[i - 1][w];
            }
        }
    }

    int maxValue = 0;
    for (int w = M; w <= C; w++)
    {
        if (dp[n][w] > maxValue)
        {
            maxValue = dp[n][w];
        }
    }

    for (int i = 0; i <= n; i++)
    {
        free(dp[i]);
    }
    free(dp);

    return maxValue;
}
// Fonction comparer à fournir à la fct qsort qui compare les ratios valeur/poids
int comparer_ratiobj(const void *a, const void *b)
{
    Item *objet1 = (Item *)a;
    Item *objet2 = (Item *)b;
    if (objet1->value / (float)objet1->weight < objet2->value / (float)objet2->weight)
    {
        return 1;
    }
    if (objet1->value / (float)objet1->weight > objet2->value / (float)objet2->weight)
    {
        return -1;
    }
    return 0;
}
//Fonction gloutonne pour résoudre le probléme du sac à dos.
int knapsackGl(Item items[], int n, int W)
{
    qsort(items, n, sizeof(Item), comparer_ratiobj);

    int poids_total = 0;   // Poids total des objets sélectionnés
    int valeur_totale = 0; // Valeur totale des objets sélectionnés

    for (int i = 0; i < n; i++)
    {
        if (poids_total + items[i].weight <= W)
        {
            poids_total += items[i].weight;
            valeur_totale += items[i].value;
        }
    }

    return valeur_totale;
}

int main()
{
    FILE *fp = fopen("instances.csv", "r");
    FILE *out = fopen("average_times.csv", "w");
    FILE *sol = fopen("solutions.csv", "w");
    if (!fp || !out || !sol)
    {
        printf("Erreur lors de l'ouverture des fichiers.\n");
        return 1;
    }

    // Variables pour stocker les somme des temps et compter les instances par taille de problème
    double sumTimeBT1 = 0, sumTimeBT2 = 0, sumTimeBT3 = 0, sumTimeDP = 0, sumTimeDPV = 0;
    int count = 0, currentN = -1;

    char line[4096];
    while (fgets(line, 4096, fp))
    {
        int W, n;
        int C;
        sscanf(line, "%d,%d,%d", &W, &n,&C);

        //sscanf(line, "%d,%d", &W, &n);

        // Vérifiez si nous passons à une nouvelle taille de problème
        if (n != currentN)
        {
            if (count > 0)
            {
                // Écrivez les moyennes pour la taille de problème précédente
                fprintf(out, "%d, %f, %f, %f\n",
                        currentN, sumTimeBT1 / count, sumTimeBT2 / count, sumTimeDP / count);
            }
            // Réinitialisez les compteurs pour la nouvelle taille
            currentN = n;
            sumTimeBT1 = sumTimeBT2 = sumTimeDP = 0;
            count = 0;
        }

        Item items[n];
        char *token = strtok(line, ",");
        token = strtok(NULL, ","); // Passer les deux premiers tokens (W et n)
        for (int i = 0; i < n && token != NULL; i++)
        {
            sscanf(token, "%d", &items[i].weight);
            token = strtok(NULL, ",");
            sscanf(token, "%d", &items[i].value);
            token = strtok(NULL, ",");
        }

        // Ici, insérez la logique pour configurer et appeler vos fonctions de sac à dos.
        printf("n=%d, W=%d\n", n, W);

        g_itmes = items;
        g_n = n;
        g_W = W;
        g_bestValue = 0;    // meilleure valeur trouvée jusqu'à présent
        g_currentValue = 0; // valeur courante du sac
        g_currentWeight = 0;

        clock_t start = clock();
        knapsackBT1(0);
        int maxValBT1 = g_bestValue;
        clock_t end = clock();
        double timeBT1 = (double)(end - start) / CLOCKS_PER_SEC;
        sumTimeBT1 += timeBT1;
        g_bestValue = 0;
        g_currentValue = 0; // valeur courante du sac
        g_currentWeight = 0;
        start = clock();
        int maxValBT2 = knapsackBT2(items, n, W);
        end = clock();
        double timeBT2 = (double)(end - start) / CLOCKS_PER_SEC;
        sumTimeBT2 += timeBT2;

        start = clock();
        knapsackBT3(0);
        int maxValBT3 = g_bestValue;
        end = clock();
        double timeBT3 = (double)(end - start) / CLOCKS_PER_SEC;
        sumTimeBT3 += sumTimeBT3;

        start = clock();
        int maxValDP = knapsackDP(W, items, n);
        end = clock();
        double timeDP = (double)(end - start) / CLOCKS_PER_SEC;
        sumTimeDP += timeDP;

        start = clock();
        int maxValDPV = knapsackvalue(W, items, n);
        end = clock();
        double timeDPV = (double)(end - start) / CLOCKS_PER_SEC;
        sumTimeDPV += timeDPV;

        int maxDPMin=knapsackDPMin(items,n,W,C);
        int sumtimeGlouton=0;
        start=clock();
        int maxGlouton=knapsackGl(items,n,W);
        end=clock();
        double timeGlouton=(double)(end-start) / CLOCKS_PER_SEC;
        sumtimeGlouton+=timeGlouton;
        printf("Solution Gloutonne:\t%d\nSolution DPMin:\t%d\n",maxGlouton,maxDPMin);
        count++;

        fprintf(sol, "%d, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d\n",
                n, W, timeBT1, maxValBT1, timeBT2, maxValBT2, timeBT3, maxValBT3, timeDP, maxValDP, timeDPV, maxValDPV,timeGlouton,maxGlouton);
    }

    if (count > 0)
    {
        fprintf(out, "%d, %f, %f, %f, %f, %f\n",
                currentN, sumTimeBT1 / count, sumTimeBT2 / count, sumTimeBT3 / count, sumTimeDP / count, sumTimeDPV / count);
    }

    fclose(fp);
    fclose(out);
    return 0;
}

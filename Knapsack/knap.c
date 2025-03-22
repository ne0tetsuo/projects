#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

int getMax(int a, int b) { return (a > b) ? a : b; }
int getMin(int a, int b) { return (a < b) ? a : b; }

typedef struct {
    unsigned int weight;
    unsigned int value;
} Item;

void knapsackBacktracking1(int index) {
    if (index == g_n || g_currentWeight == g_W) {
        g_bestValue = getMax(g_bestValue, g_currentValue);
        return;
    }

    knapsackBacktracking1(index + 1);

    if (g_currentWeight + g_items[index].weight <= g_W) {
        g_currentWeight += g_items[index].weight;
        g_currentValue += g_items[index].value;

        knapsackBacktracking1(index + 1);

        g_currentWeight -= g_items[index].weight;
        g_currentValue -= g_items[index].value;
    }
}

void knapsackBacktracking3(int index) {
    if (index == g_n || g_currentWeight == g_W) {
        g_bestValue = getMax(g_bestValue, g_currentValue);
        return;
    }

    int upperBound = g_currentValue;
    for (int i = index; i < g_n; ++i) {
        upperBound += g_items[i].value;
    }

    if (upperBound <= g_bestValue) {
        return;
    }

    knapsackBacktracking3(index + 1);

    if (g_currentWeight + g_items[index].weight <= g_W) {
        g_currentWeight += g_items[index].weight;
        g_currentValue += g_items[index].value;

        knapsackBacktracking3(index + 1);

        g_currentWeight -= g_items[index].weight;
        g_currentValue -= g_items[index].value;
    }
}

int knapsackBacktrackingUtil(Item items[], int n, int W, int index, int currentValue, int *bestValue) {
    if (index == n || W == 0) {
        if (currentValue > *bestValue)
            *bestValue = currentValue;
        return *bestValue;
    }

    knapsackBacktrackingUtil(items, n, W, index + 1, currentValue, bestValue);

    if (items[index].weight <= W) {
        knapsackBacktrackingUtil(items, n, W - items[index].weight, index + 1, currentValue + items[index].value, bestValue);
    }

    return *bestValue;
}

int knapsackBacktracking2(Item items[], int n, int W) {
    int bestValue = 0;
    return knapsackBacktrackingUtil(items, n, W, 0, 0, &bestValue);
}

int knapsackDynamicProgramming(int W, Item items[], int n) {
    int i, w;
    int **K = (int **)malloc((n + 1) * sizeof(int *));
    for (i = 0; i <= n; i++)
        K[i] = (int *)malloc((W + 1) * sizeof(int));

    for (i = 0; i <= n; i++) {
        for (w = 0; w <= W; w++) {
            if (i == 0 || w == 0)
                K[i][w] = 0;
            else if (items[i - 1].weight <= w)
                K[i][w] = getMax(items[i - 1].value + K[i - 1][w - items[i - 1].weight], K[i - 1][w]);
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

int *knapsackDynamicProgrammingValue(int V, Item items[], int n) {
    int *dpv = (int *)malloc((V + 1) * sizeof(int));
    dpv[0] = 0;
    for (int i = 1; i <= V; ++i) {
        dpv[i] = INT_MAX;
    }
    for (int i = 0; i < n; ++i) {
        for (int j = V; j >= items[i].value; --j) {
            if (dpv[j - items[i].value] != INT_MAX)
                dpv[j] = getMin(dpv[j - items[i].value] + items[i].weight, dpv[j]);
        }
    }
    return dpv;
}

int knapsackValue(int W, Item items[], int n) {
    int V = 0;
    for (int i = 0; i < n; ++i) {
        V += items[i].value;
    }
    int *dpv = knapsackDynamicProgrammingValue(V, items, n);
    int i = V;
    while (dpv[i] > W && i > 0) {
        i--;
    }
    int result = i;
    free(dpv);
    return result;
}

int compareRatioObject(const void *a, const void *b) {
    Item *object1 = (Item *)a;
    Item *object2 = (Item *)b;
    if (object1->value / (float)object1->weight < object2->value / (float)object2->weight) {
        return 1;
    }
    if (object1->value / (float)object1->weight > object2->value / (float)object2->weight) {
        return -1;
    }
    return 0;
}

int knapsackGreedy(Item items[], int n, int W) {
    qsort(items, n, sizeof(Item), compareRatioObject);

    int totalWeight = 0;
    int totalValue = 0;

    for (int i = 0; i < n; i++) {
        if (totalWeight + items[i].weight <= W) {
            totalWeight += items[i].weight;
            totalValue += items[i].value;
        }
    }

    return totalValue;
}

int knapsackDPMin(Item items[], int n, int C, int M) {
    int **dp = (int **)malloc((n + 1) * sizeof(int *));
    for (int i = 0; i <= n; i++) {
        dp[i] = (int *)malloc((C + 1) * sizeof(int));
        for (int w = 0; w <= C; w++) {
            dp[i][w] = 0;
        }
    }

    for (int i = 1; i <= n; i++) {
        for (int w = 0; w <= C; w++) {
            if (items[i - 1].weight <= w) {
                               dp[i][w] = getMax(dp[i - 1][w], dp[i - 1][w - items[i - 1].weight] + items[i - 1].value);
            } else {
                dp[i][w] = dp[i - 1][w];
            }
        }
    }

    int maxValue = 0;
    for (int w = M; w <= C; w++) {
        if (dp[n][w] > maxValue) {
            maxValue = dp[n][w];
        }
    }

    for (int i = 0; i <= n; i++) {
        free(dp[i]);
    }
    free(dp);

    return maxValue;
}

int main() {
    FILE *fp = fopen("instances.csv", "r");
    FILE *out = fopen("average_times.csv", "w");
    FILE *sol = fopen("solutions.csv", "w");
    if (!fp || !out || !sol) {
        printf("Erreur lors de l'ouverture des fichiers.\n");
        return 1;
    }

    double sumTimeBT1 = 0, sumTimeBT2 = 0, sumTimeBT3 = 0, sumTimeDP = 0, sumTimeDPV = 0;
    int count = 0, currentN = -1;

    char line[4096];
    while (fgets(line, 4096, fp)) {
        int W, n;
        sscanf(line, "%d,%d", &W, &n);

        if (n != currentN) {
            if (count > 0) {
                fprintf(out, "%d, %f, %f, %f\n",
                        currentN, sumTimeBT1 / count, sumTimeBT2 / count, sumTimeDP / count);
            }
            currentN = n;
            sumTimeBT1 = sumTimeBT2 = sumTimeDP = 0;
            count = 0;
        }

        Item items[n];
        char *token = strtok(line, ",");
        token = strtok(NULL, ",");
        for (int i = 0; i < n && token != NULL; i++) {
            sscanf(token, "%d", &items[i].weight);
            token = strtok(NULL, ",");
            sscanf(token, "%d", &items[i].value);
            token = strtok(NULL, ",");
        }

        g_items = items;
        g_n = n;
        g_W = W;
        g_bestValue = 0;
        g_currentValue = 0;
        g_currentWeight = 0;

        clock_t start = clock();
        knapsackBacktracking1(0);
        int maxValBT1 = g_bestValue;
        clock_t end = clock();
        double timeBT1 = (double)(end - start) / CLOCKS_PER_SEC;
        sumTimeBT1 += timeBT1;
        g_bestValue = 0;
        g_currentValue = 0;
        g_currentWeight = 0;
        start = clock();
        int maxValBT2 = knapsackBacktracking2(items, n, W);
        end = clock();
        double timeBT2 = (double)(end - start) / CLOCKS_PER_SEC;
        sumTimeBT2 += timeBT2;

        start = clock();
        knapsackBacktracking3(0);
        int maxValBT3 = g_bestValue;
        end = clock();
        double timeBT3 = (double)(end - start) / CLOCKS_PER_SEC;
        sumTimeBT3 += timeBT3;

        start = clock();
        int maxValDP = knapsackDynamicProgramming(W, items, n);
        end = clock();
        double timeDP = (double)(end - start) / CLOCKS_PER_SEC;
        sumTimeDP += timeDP;

        start = clock();
        int maxValDPV = knapsackValue(W, items, n);
        end = clock();
        double timeDPV = (double)(end - start) / CLOCKS_PER_SEC;
        sumTimeDPV += timeDPV;

        count++;

        fprintf(sol, "%d, %d, %f, %d, %f, %d, %f, %d,%f, %d,%f, %d\n",
                n, W, timeBT1, maxValBT1, timeBT2, maxValBT2, timeBT3, maxValBT3, timeDP, maxValDP, timeDPV, maxValDPV);
    }

    if (count > 0) {
        fprintf(out, "%d, %f, %f, %f, %f, %f\n",
                currentN, sumTimeBT1 / count, sumTimeBT2 / count, sumTimeBT3 / count, sumTimeDP / count, sumTimeDPV / count);
    }

    fclose(fp);
    fclose(out);
    return 0;
}

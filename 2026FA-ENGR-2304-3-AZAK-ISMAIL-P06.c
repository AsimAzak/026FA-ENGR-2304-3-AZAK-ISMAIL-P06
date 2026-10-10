/*
2026FA-ENGR-2304-3-AZAK-ISMAIL-P06
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_RECORDS 5000
#define NUM_PLANTS  6
#define MAX_VALUE   1000
#define DEFAULT_FILE "C:\\Users\\BERSERK\\Desktop\\homework\\ENGR\\ENGR 2304 - Information Session P07 - Process Data Analysis.csv"

typedef struct {
    int year, month, day;
    int prod[NUM_PLANTS];
} Record;

typedef struct {
    double mean, variance, stddev;
    int min, max, mode, mode_count;
    long total;
} Stats;

Record data[MAX_RECORDS];
int    count = 0;
Stats  st[NUM_PLANTS];

int load_data(const char *fname)
{
    FILE *fp = fopen(fname, "r");
    char line[256];
    int first = 1;

    if (!fp) return 0;
    count = 0;
    while (fgets(line, sizeof line, fp) && count < MAX_RECORDS) {
        Record r;
        int i;
        char *tok;
        int vals[3 + NUM_PLANTS], n = 0;

        if (first) { first = 0; if (line[0] < '0' || line[0] > '9') continue; }
        tok = strtok(line, ",\t \r\n");
        while (tok && n < 3 + NUM_PLANTS) {
            vals[n++] = atoi(tok);
            tok = strtok(NULL, ",\t \r\n");
        }
        if (n < 3 + NUM_PLANTS) continue;
        r.year = vals[0]; r.month = vals[1]; r.day = vals[2];
        for (i = 0; i < NUM_PLANTS; i++) r.prod[i] = vals[3 + i];
        data[count++] = r;
    }
    fclose(fp);
    return count;
}

void compute_stats(int p)
{
    static int freq[MAX_VALUE + 1];
    int i;
    double sumsq = 0.0;
    Stats *s = &st[p];

    memset(freq, 0, sizeof freq);
    s->total = 0; s->min = data[0].prod[p]; s->max = data[0].prod[p];
    for (i = 0; i < count; i++) {
        int v = data[i].prod[p];
        s->total += v;
        if (v < s->min) s->min = v;
        if (v > s->max) s->max = v;
        if (v >= 0 && v <= MAX_VALUE) freq[v]++;
    }
    s->mean = (double)s->total / count;

    s->mode = s->min; s->mode_count = 0;
    for (i = 0; i <= MAX_VALUE; i++)
        if (freq[i] > s->mode_count) { s->mode_count = freq[i]; s->mode = i; }

    for (i = 0; i < count; i++) {
        double d = data[i].prod[p] - s->mean;
        sumsq += d * d;
    }
    s->variance = (count > 1) ? sumsq / (count - 1) : 0.0;
    s->stddev   = sqrt(s->variance);
}

void print_stats(void)
{
    int p;
    printf("\n=== PLANT DATA ANALYSIS (%d daily records) ===\n", count);
    printf("%-6s %8s %5s %5s %12s %10s %10s %10s\n",
           "Plant", "Mean", "Min", "Max", "Mode(count)", "Variance", "Std Dev", "Total");
    for (p = 0; p < NUM_PLANTS; p++)
        printf("L%-5d %8.2f %5d %5d %7d(%3d) %10.2f %10.2f %10ld\n", p + 1,
               st[p].mean, st[p].min, st[p].max, st[p].mode, st[p].mode_count,
               st[p].variance, st[p].stddev, st[p].total);
}

void print_report(void)
{
    int i, p, best = 0, worst = 0;
    int bestval = -1, bestrec = 0, bestplant = 0;
    int bestsum = -1, bestsumrec = 0;

    for (p = 1; p < NUM_PLANTS; p++) {
        if (st[p].total > st[best].total)  best = p;
        if (st[p].total < st[worst].total) worst = p;
    }
    for (i = 0; i < count; i++) {
        int sum = 0;
        for (p = 0; p < NUM_PLANTS; p++) {
            sum += data[i].prod[p];
            if (data[i].prod[p] > bestval) {
                bestval = data[i].prod[p]; bestrec = i; bestplant = p;
            }
        }
        if (sum > bestsum) { bestsum = sum; bestsumrec = i; }
    }

    printf("\n=== PRODUCTION REPORT ===\n");
    printf("Period: %04d-%02d-%02d to %04d-%02d-%02d  (%d records)\n",
           data[0].year, data[0].month, data[0].day,
           data[count-1].year, data[count-1].month, data[count-1].day, count);
    printf("Best performing plant (overall):  L%d  total = %ld, mean = %.2f units/day\n",
           best + 1, st[best].total, st[best].mean);
    printf("Worst performing plant (overall): L%d  total = %ld, mean = %.2f units/day\n",
           worst + 1, st[worst].total, st[worst].mean);
    printf("Best single production day (single plant): L%d produced %d units on %04d-%02d-%02d\n",
           bestplant + 1, bestval, data[bestrec].year, data[bestrec].month, data[bestrec].day);
    printf("Best single production day (all facilities): %04d-%02d-%02d with %d units total\n",
           data[bestsumrec].year, data[bestsumrec].month, data[bestsumrec].day, bestsum);
    printf("   Breakdown: ");
    for (p = 0; p < NUM_PLANTS; p++) printf("L%d=%d ", p + 1, data[bestsumrec].prod[p]);
    printf("\n");
}

void search_data(void)
{
    int y, m, d, plant, i, p, found = 0;

    printf("\nEnter date - Year Month Day (e.g. 1992 11 14): ");
    if (scanf("%d %d %d", &y, &m, &d) != 3) { while (getchar() != '\n'); printf("Invalid date.\n"); return; }
    printf("Plant number 1-%d, or 0 for all plants: ", NUM_PLANTS);
    if (scanf("%d", &plant) != 1 || plant < 0 || plant > NUM_PLANTS) {
        while (getchar() != '\n');
        printf("Invalid plant.\n");
        return;
    }
    for (i = 0; i < count; i++) {
        if (data[i].year == y && data[i].month == m && data[i].day == d) {
            found = 1;
            printf("\nProduction on %04d-%02d-%02d:\n", y, m, d);
            for (p = 0; p < NUM_PLANTS; p++)
                if (plant == 0 || plant == p + 1)
                    printf("  L%d: %d units\n", p + 1, data[i].prod[p]);
            break;
        }
    }
    if (!found) printf("No data found for %04d-%02d-%02d. Enter YEAR MONTH DAY.\n", y, m, d);
}


int main(int argc, char *argv[])
{
    char fname[256];
    int choice = 0, p;

    strncpy(fname, (argc > 1) ? argv[1] : DEFAULT_FILE, sizeof fname - 1);
    fname[sizeof fname - 1] = '\0';

    while (!load_data(fname)) {
        printf("Could not read '%s'.\nEnter data file name (or q to quit): ", fname);
        if (scanf("%255s", fname) != 1 || strcmp(fname, "q") == 0) return 1;
    }
    printf("Loaded %d records from %s\n", count, fname);
    for (p = 0; p < NUM_PLANTS; p++) compute_stats(p);

    do {
        printf("\n===== MENU =====\n");
        printf("1. Plant Data Analyses\n2. Production Report\n3. Search Production Data\n4. Exit\n");
        printf("Select option: ");
        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); choice = 0; }
        switch (choice) {
            case 1: print_stats();  break;
            case 2: print_report(); break;
            case 3: search_data();  break;
            case 4: printf("Goodbye.\n"); break;
            default: printf("Invalid selection.\n");
        }
    } while (choice != 4);
    return 0;
}
#ifndef MyPreCompiler
#define MyPreCompiler

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fenv.h>
#include <unistd.h>


extern bool star_commenti;


struct file {
    ssize_t size;
    int n_lines;
};

struct Statistic {
    ssize_t file_input_size;
    int counter_input_righe;
    ssize_t file_output_size;
    int counter_output_righe;
    struct file *file_includes; // Cambiato a puntatore
    int counter_includes;
    int counter_variabili;
    int counter_errori;
    int counter_commenti;
};

void replace(char *str, char const *pattern, char const *replace);
char* levaCommenti(char *riga, struct Statistic *statistic);
int setupFile(FILE **input, FILE **output, char *argv[], int argc, bool *verbose, bool *std, char** input_file);
void rimuoviSpazi(char *riga, char *commento);
ssize_t fileLen(FILE *file);
int count_lines_in_file(FILE *file);
void add_include(struct Statistic *statistic, FILE *file);
int includeFileLen(const struct Statistic *statistic);
void initialize(struct Statistic *statistic, FILE *input);
void controlloNomi(char *riga, int n_riga, struct Statistic *statistic, char *input);
char* Include(char *riga, struct Statistic *statistic, const char* base_path);
void checkTipi(char **parole, int n_riga,int size, struct Statistic *statistic,char *input,bool virgola);
void checkParola(char *parola, int n_riga,int counterParole, struct Statistic *statistic,char *input,bool virgola);
void aggiungiParola(char ***array, int *size, const char *parola);
char** splitParole(char *riga, int *dim,char * sep);
bool isReserved(char *token);
char *Resolver(FILE *input, struct Statistic *statistic,char *input_path);
int count_lines_in_script(const char *script);
void trim(char *str);
bool isok(char *parola);
void rigaSbagliata(int n_riga, char *input, struct Statistic *statistic);
char *searchPath(const char *input);

#endif // MyPreCompiler
#include "../src/MyPreCompiler.h"

int main(int argc, char *argv[]) {
    FILE *input = NULL, *output = NULL;
    bool verbose = false, std = false;
    char *input_path=NULL;
    struct Statistic statistic;

    if (setupFile(&input, &output, argv, argc, &verbose, &std,&input_path) == EXIT_FAILURE) return EXIT_FAILURE;
    initialize(&statistic, input);

    char *file_script = Resolver(input, &statistic, input_path);
     if (!file_script) {
        fclose(input);
        if (!std) fclose(output);
        return EXIT_FAILURE;
    }

    char *res = calloc(strlen(file_script) + 1, sizeof(char));
    if (!res) {
        perror("Errore di allocazione memoria");
        free(file_script);
        fclose(input);
        if (!std) fclose(output);
        return EXIT_FAILURE;
    }
    strcpy(res,file_script);

    statistic.counter_output_righe = count_lines_in_script(res);

    std ? printf("%s", res) : fprintf(output, "%s", res);
    if (!std) statistic.file_output_size=fileLen(output);

    if (verbose) {
        fprintf(stderr,"Statistiche:\n");
        fprintf(stderr, "File di input: %ld byte, %d righe\n", statistic.file_input_size, statistic.counter_input_righe);
        if (!std) fprintf(stderr,"File di output: %ld byte, %d righe\n", statistic.file_output_size, statistic.counter_output_righe);
        fprintf(stderr,"Include: %d file inclusi\n", includeFileLen(&statistic));
        for (int i = 0; i < includeFileLen(&statistic); i++)
            fprintf(stderr,"Include %d: %ld byte, %d righe\n", i+1, statistic.file_includes[i].size, statistic.file_includes[i].n_lines);
        fprintf(stderr,"Variabili: %d\n", statistic.counter_variabili);
        fprintf(stderr,"Errori: %d\n", statistic.counter_errori);
        fprintf(stderr,"Commenti: %d\n", statistic.counter_commenti);
    }

    free(file_script);
    fclose(input);
    if (!std) fclose(output);

    return EXIT_SUCCESS;
}

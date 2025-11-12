#include "../src/MyPreCompiler.h"

bool star_commenti = false;
int counter_commenti = 0;
bool isenum = false;

/*
funzione per inizialire la struct
che tiene conto delle statistiche
*/
void initialize(struct Statistic *statistic, FILE *input) {
    statistic->file_input_size = fileLen(input);
    statistic->file_output_size = 0;
    statistic->counter_output_righe = 0;
    statistic->counter_input_righe = count_lines_in_file(input);
    statistic->counter_variabili = 0;
    statistic->counter_errori = 0;
    statistic->counter_commenti = 0;
    statistic->counter_includes = 0;
    statistic->file_includes = NULL;
}

ssize_t fileLen(FILE *file) {
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        ssize_t file_size = ftell(file);
        rewind(file);
        return file_size;
    }
    return 0;
}

/*
funzione per accorciare le righe da analizzare
*/
void trim(char *str) {
    char *start = str;
    char *end;

    // Salta gli spazi iniziali
    while (*start && isspace((unsigned char) *start)) {
        start++;
    }

    // Se la stringa è tutta spazi
    if (*start == '\0') {
        *str = '\0';
        return;
    }

    // Trova l’ultimo carattere non spazio
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char) *end)) {
        end--;
    }

    // Termina la stringa dopo l’ultimo carattere valido
    *(end + 1) = '\0';

    // Sposta la stringa al buffer originale
    if (start != str) {
        memmove(str, start, end - start + 2); // +1 per '\0'
    }
}
/*
funzione che tiene conto della lunghezza delle righe analizzate
*/
int count_lines_in_file(FILE *file) {
    if (file == NULL) {
        printf("Errore: il puntatore al file è NULL.\n");
        return -1; // Indica errore
    }
    int lines = 1;
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            lines++;
        }
    }
    rewind(file);
    return lines;
}
/*
funzione che tiene conto della lunghezza delle righe analizzate
*/
int count_lines_in_script(const char *script) {
    if (script == NULL) {
        perror("Errore: script vuoto.\n");
        return -1; // Indica errore
    }
    int lines = 1;
    for (int i = 0; script[i] != '\0'; i++) {
        if (script[i] == '\n') {
            lines++;
        }
    }
    return lines;
}

/*
funzione che salva i file negli include
*/
void add_include(struct Statistic *statistic, FILE *file) {
    // Verifica che il file sia valido
    if (file == NULL) {
        fprintf(stderr, "Errore: il file passato a add_include è NULL.\n");
        exit(EXIT_FAILURE);
    }

    // Rialloca la memoria per un nuovo elemento
    struct file *temp = realloc(statistic->file_includes,
                                (statistic->counter_includes + 1) * sizeof(struct file));
    if (temp == NULL) {
        perror("Errore di allocazione memoria");
        exit(EXIT_FAILURE);
    }

    // Assegna il puntatore riallocato
    statistic->file_includes = temp;

    // Inizializza il nuovo elemento
    ssize_t file_size = fileLen(file);
    struct file include;
    include.size = file_size;
    include.n_lines = count_lines_in_file(file);

    // Aggiungi il nuovo elemento all'array
    statistic->file_includes[statistic->counter_includes] = include;
    statistic->counter_includes++; // Incrementa il contatore
}

int includeFileLen(const struct Statistic *statistic) {
    return statistic->counter_includes;
}

/*

*/
void replace(char *str, char const *pattern, char const *replace) {
    if (str == NULL || pattern == NULL) { return; }
    int j;
    int lenstr = (int) strlen(str);
    int lenpattern = (int) strlen(pattern);
    int lenreplace = (int) strlen(replace);
    int diff;

    for (int i = 0; i <= (lenstr - lenpattern); i++) {
        if (strncmp(&str[i], pattern, lenpattern) == 0) // se trovo la sotto-stringa
        {
            if ((lenstr - lenpattern + lenreplace) >= strlen(str)) break;
            // se la dimensione della sostituzione diventa maggiore della lunghezza della stringa iniziale mi fermo per non avere problemi di spazio.
            if (lenpattern == lenreplace) { strncpy(&str[i], replace, lenreplace); } else if (lenpattern > lenreplace) {
                diff = lenpattern - lenreplace;
                strncpy(&str[i], replace, lenreplace);
                // compact
                j = i + lenreplace;
                for (; j < lenstr - diff; j++)
                    str[j] = str[j + diff];
                str[j] = '\0';
            } else // lensearch < lensubst
            {
                diff = lenreplace - lenpattern;
                // expand
                j = lenstr - lenpattern + lenreplace;
                str[j] = '\0';
                for (; j > i + lenpattern; j--)
                    str[j] = str[j - diff];
                strncpy(&str[i], replace, lenreplace);
                // update string length and position
                lenstr += lenreplace - lenpattern;
                i += (lenreplace - 1);
            }
        }
    }
}
/*
funzione per rimuovere gli spazi superflui sille righe
*/
void rimuoviSpazi(char *riga, char *commento) {
    char *start = riga;
    //vado avanti fino a quando non trovo un carattere diverso dallo spazio
    while (*start && isspace(*start)) { start++; }
    start == commento ? replace(riga, riga, "") : replace(riga, commento, "\n");
}

/*
funzione che leva i commenti all'interno del file
*/

char *levaCommenti(char *riga, struct Statistic *statistic) {
    if (strstr(riga, "*/") != NULL) {
        // è un fine commento, dopo questa linea non elimino piu le linee a prescindere in quanto non sono più commenti.
        replace(riga, riga, "");
        star_commenti = false;
        statistic->counter_commenti += 1;
    } else if (star_commenti) {
        replace(riga, riga, "");
        statistic->counter_commenti += 1;
    } else if (strstr(riga, "//") != NULL) {
        char *commento = strstr(riga, "//");
        rimuoviSpazi(riga, commento);
        statistic->counter_commenti += 1;
    } else if (strstr(riga, "/*") != NULL) {
        char *commento = strstr(riga, "/*");
        strstr(commento, "*/") == NULL ? star_commenti = true : false;
        // se il '*/' sta sulla stessa riga, allora la elimino come fosse un //. Altrimenti devo eliminare
        //ogni riga successiva fino al raggiungimento di '*/' in quanto sono tutti commenti.
        rimuoviSpazi(riga, commento);
        statistic->counter_commenti += 1;
    }
    return riga;
}


//funzione per controllare se all'interno della definizione di una variabile sono presente delle parole riservate
bool isReserved(char *token) {
    const char *parole_riservate[] = {
        "auto", "break", "case", "char", "const", "continue", "default", "do",
        "double", "else", "enum", "extern", "float", "for", "goto", "if",
        "inline", "int", "long", "register", "restrict", "return", "short", "signed",
        "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void",
        "volatile", "while", "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex",
        "_Generic", "_Imaginary", "_Noreturn", "_Static_assert", "_Thread_local", "char", "short", "int", "long",
        "long long",
        "signed", "signed char", "signed short", "signed int", "signed long", "signed long long",
        "unsigned", "unsigned char", "unsigned short", "unsigned int", "unsigned long", "unsigned long long",
        "float", "double", "long double",
        "_Bool", "void",
        "_Complex", "_Complex float", "_Complex double", "_Complex long double",
        "_Imaginary", "_Imaginary float", "_Imaginary double", "_Imaginary long double",
        "NULL", "switch", "error", "const", "volatile", "static", "return"
    };
    int num_types = sizeof(parole_riservate) / sizeof(parole_riservate[0]); // Numero di elementi
    replace(token, "(", "");
    replace(token, ")", "");
    replace(token, "{", "");
    replace(token, "}", "");
    replace(token, "[", "");
    replace(token, "]", "");
    for (int i = 0; i < num_types; i++) {
        if (token && strcmp(parole_riservate[i], token) == 0) {
            return true;
        }
    }
    return false;
}

//funzione per aggiungere elemnti ad un arry dinamico che viene passato per riferimento,in modo tale che nella funzione chiamante
//venga modificato l'array inizializzato a NULL
void aggiungiParola(char ***array, int *size, const char *parola) {
    char **temp = realloc(*array, (*size + 1) * sizeof(char *));
    if (temp == NULL) {
        perror("Errore realloc array");
        exit(EXIT_FAILURE);
    }
    *array = temp;

    (*array)[*size] = malloc(strlen(parola) + 1);
    if ((*array)[*size] == NULL) {
        perror("Errore malloc parola");
        exit(1);
    }
    strcpy((*array)[*size], parola);
    (*size)++;
}

//funzione che passata una riga del file di input ne divide le parole
char **splitParole(char *riga, int *dim, char *sep) {
    char **parole = NULL;
    int size = 0;

    char *riga_dup = strdup(riga);
    char *token = strtok(riga_dup, sep);

    while (token != NULL) {
        trim(token);
        aggiungiParola(&parole, &size, token);
        token = strtok(NULL, sep);
    }
    dim[0] = size;

    return parole;
}

// Funzione di supporto: segnala errore e aggiorna i contatori
void rigaSbagliata(int n_riga, char *input, struct Statistic *statistic) {
    printf("sbagliata la riga %d in file %s \n", n_riga, input);
    statistic->counter_errori++;
    statistic->counter_variabili++;
}

/*
controlla se il nome di una variabile è scritto correttamente
*/
bool isok(char *parola) {
    if (parola == NULL) return false;
    if (isReserved(parola)) return false;
    if (isdigit(parola[0])) return false;
    for (int i = 0; i < strlen(parola); i++) {
        if (!isalnum(parola[i]) && parola[i] != '_' && parola[i] != '$') {
            // In enum, '*' non è ammesso
            if (isenum && parola[i] == '*') return false;
            if (!isenum && parola[i] != '*') return false;
            if (!isenum && parola[i] == '*') continue;
            return false;
        }
    }
    return true;
}

/*
presa la parte di dichiarazione del nome vede se è corretto e lo divide per diversi casi
*/
void checkParola(char *parola, int n_riga, int counterParole, struct Statistic *statistic, char *input, bool virgola) {
    if (parola == NULL) return;
    replace(parola, ",", "");

    const char delimitatori[] = {'=', ';'};
    for (int d = 0; d < 2; d++) {
        char *delim_pos = strchr(parola, delimitatori[d]);
        if (delim_pos) {
            if (counterParole >= 2 && !virgola) {
                rigaSbagliata(n_riga, input, statistic);
                return;
            }
            char *nuovaParola = strtok(parola, (delimitatori[d] == '=') ? "=" : ";");
            if (nuovaParola == NULL) return;
            if (!isok(nuovaParola)) {
                rigaSbagliata(n_riga, input, statistic);
                return;
            }
            statistic->counter_variabili++;
            return;
        }
    }

    // Caso enum
    if (isenum) {
        if (!isok(parola)) {
            rigaSbagliata(n_riga, input, statistic);
            return;
        }
        statistic->counter_variabili++;
        return;
    }

    // Caso generale
    if (!isok(parola)) {
        rigaSbagliata(n_riga, input, statistic);
        return;
    }
}

//funzione che dato un'array di parole che rappresentano una riga di codice ne verifca se è una dichiarazione di variabile o meno.
void checkTipi(char **parole, int n_riga, int size, struct Statistic *statistic, char *input, bool virgola) {
    if (parole == NULL) { return; }

    int counterParole = 0;
    if (strcmp(parole[0], "return") == 0 || strcmp(parole[0], "for") == 0 || strcmp(parole[0], "while") == 0 ||
        strcmp(parole[0], "switch") == 0) return;
    if (strcmp(parole[0], "enum") == 0) {
        if (strchr(parole[1], '{') == NULL) {
            isenum = true;
            char *new_p = strchr(parole[2], '{');
            int dim = 0;
            replace(new_p, "{", "");
            replace(new_p, "}", "");
            replace(new_p, ";", "");
            char **arr_p = splitParole(new_p, &dim, ",");
            for (int i = 0; i < dim; i++) {
                counterParole++;
                checkParola(arr_p[i], n_riga, counterParole, statistic, input, virgola);
            }
        }
    } else {
        if (parole[0] && parole[1] && !isReserved(parole[0]) && !isReserved(parole[1])) {
            return;
        }
        if (size == 1 && !virgola) {
            if (isenum && strchr(parole[0], '}') != NULL) {
                isenum = false;
            }
            return;
        }

        for (int i = 0; i < size; i++) {
            if (!isReserved(parole[i])) {
                counterParole++;

                checkParola(parole[i], n_riga, counterParole, statistic, input, virgola);
            }
        }
    }
}

//funzione principale per il controllo corretto di una dichiarazione di varibile, che al suo interno ne chiama altre necessarie
//per svolgere l'operazione.
void controlloNomi(char *riga, int n_riga, struct Statistic *statistic, char *input) {
    int dim = 0;
    char **parole = splitParole(riga, &dim, " ");
    checkTipi(parole, n_riga, dim, statistic, input, strchr(riga, ',') != NULL);
    for (int i = 0; i < dim; i++) {
        free(parole[i]);
    }
    free(parole);
}
/*
funzione per aprire il file da analizzare
*/

int setupFile(FILE **input, FILE **output, char *argv[], int argc, bool *verbose, bool *std, char **input_path) {
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--in=", 5) == 0) {
            *input = fopen(argv[i] + 5, "r");
            *input_path = calloc(strlen(argv[i] + 5)+1, sizeof(char));
            strcpy(*input_path, argv[i] + 5);
            if (*input == NULL) {
                printf("Impossibile aprire il file %s \n", argv[i] + 5);
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            *input = fopen(argv[++i], "r");
            *input_path = calloc(strlen(argv[i--])+1, sizeof(char));
            strcpy(*input_path, argv[++i]);
            if (*input == NULL) {
                printf("Impossibile aprire il file %s \n", argv[i]);
                return EXIT_FAILURE;
            }
        } else if (strncmp(argv[i], "--out=", 6) == 0) {
            *output = fopen(argv[i] + 6, "r+");

            if (*output == NULL) {
                printf("Impossibile aprire il file %s \n", argv[i] + 6);
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            *output = fopen(argv[++i], "w+");
            if (*output == NULL) {
                printf("Impossibile aprire il file %s \n", argv[i]);
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            *verbose = true;
        } else if (*input == NULL) {
            *input = fopen(argv[i], "r");
            *input_path = calloc(strlen(argv[i]), sizeof(char));
            strcpy(*input_path, argv[i]);
            if (*input == NULL) {
                printf("Impossibile aprire il file %s \n", argv[i]);
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[i], ">") == 0) {
            *output = fopen(argv[i + 1], "r+");
            return 0;
        } else {
            printf("Impossibile eseguire. Riprovare. \n");
            return EXIT_FAILURE;
        }
    }
    if (*output == NULL) {
        *std = true;
    }
    if (*input == NULL) {
        printf("Manca il file di input.");
        return EXIT_FAILURE;
    }
    return 0;
}
/*
funzione per analizzare le direttive include
*/
char *Include(char *riga, struct Statistic *statistic, const char *base_path) {
    char *import = strstr(riga, "#include");
    if (import != NULL) {
        char *include = NULL;
        if (strstr(import, "\"") != NULL) {
            include = strstr(import, "\"");
            replace(include, "\"", ""); // Rimuovi le virgolette dalla stringa
            replace(include, "\"", "");
            replace(include, "\n", "");
            include = strtok(include, " ");
        } else if (strstr(import, "<") != NULL) {
            include = strstr(import, "<");
            replace(include, "<", ""); // Rimuovi le virgolette dalla stringa
            replace(include, ">", "");
            replace(include, "\n", "");
        }
        if (include == NULL) {
            perror("Nessun Inlcude");
            free(import);
            return NULL;
        }

        // Determina la lunghezza del percorso e alloca la memoria necessaria

        size_t path_length = strlen(base_path) + strlen(include) + 1;
        char *path = malloc(path_length);

        if (path == NULL) {
            perror("Errore di allocazione memoria per il path");
            return "";
        }

        // Crea il percorso completo
        strcpy(path, base_path);
        strcat(path, include);

        FILE *input = fopen(path, "r");
        if (input == NULL) {
            perror("Errore nell'apertura del file incluso");
            return "";
        }
        add_include(statistic, input);
        char buffer[256] = "";
        int n_riga = 1;
        char *includes = Resolver(input, statistic, path);
        while (fgets(buffer, sizeof(buffer), input)) {
            controlloNomi(buffer, n_riga, statistic, path);
            n_riga++;
        }
        ssize_t file_size = fileLen(input) + (ssize_t) strlen(includes); // Ottieni la dimensione del file
        char *contenuto = malloc((file_size + 1) * sizeof(char));
        if (contenuto == NULL) {
            perror("Errore di allocazione memoria");
            fclose(input);
            return "";
        }
        strcpy(contenuto, includes);

        contenuto[strlen(contenuto)] = '\0'; // Termina la stringa
        fclose(input);
        free(path);

        return contenuto;
    }
    return "";
}

/*
funzione per trovare i path dei file
*/
char *searchPath(const char *input) {
    if (input == NULL) return NULL;

    // Trova l'ultimo '/'
    const char *last_slash = strrchr(input, '/');
    if (!last_slash) {
        return strdup(""); // nessuna cartella, ritorno stringa vuota
    }

    size_t len = last_slash - input + 1; // includo lo slash
    char *base = malloc(len + 1);
    if (!base) return NULL;

    strncpy(base, input, len);
    base[len] = '\0';
    return base;
}

/*
funzione per scrivere sul file
*/
char *Resolver(FILE *input, struct Statistic *statistic, char *input_path) {
    int n_riga = 1;
    char buffer[256] = "\0";

    size_t file_size = fileLen(input);
    if (file_size <= 0) {
        perror("Errore: lunghezza del file non valida.\n");
        return NULL;
    }

    char *file_script = calloc(file_size + 1, sizeof(char)); // Usa calloc per inizializzare a zero
    if (!file_script) {
        perror("Errore di allocazione memoria");
        return NULL;
    }
    while (fgets(buffer, sizeof(buffer), input)) {
        const char *content_to_add;
        size_t content_length;

        if (!strstr(levaCommenti(buffer, statistic), "include")) {
            controlloNomi(buffer, n_riga, statistic, input_path);
            content_to_add = buffer;
            content_length = strlen(buffer);
        } else {
            char *base_path = searchPath(input_path);
            char *processed_line = Include(buffer, statistic, base_path);
            content_to_add = processed_line;
            content_length = strlen(processed_line);
        }

        file_size = strlen(file_script) + content_length + 1;

        char *temp = realloc(file_script, file_size);
        if (temp == NULL) {
            perror("Errore di riallocazione memoria");
            free(file_script);
            return NULL;
        }
        file_script = temp;
        strncat(file_script, content_to_add,file_size - strlen(file_script) - 1);
        n_riga++;
    }
    strcat(file_script, "\n");
    return file_script;
}

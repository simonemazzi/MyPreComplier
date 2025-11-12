/****
* Questo commento contiene informazioni sull'autore e sulla licenza di utilizzo
 *
 */

//meglio eliminare questo include
//#include "hello_world_1.c"

#include "test1.h"

/*le variabili global sono pericolose*/

int global_counter=0;
const int 4backup=25;

int main(int argc, char *args[])
{
    /*variabili locali*/
    float average=0; //media
    double square error;
    char switch; //per parsing input
    int i;

    /*
     * Qui' il codice per gestire i parametri di input
     */
    for (i=1;i<=10;i++){
        average=average+i;
        global_counter++;
    }
    average=average/(float) global_counter;
    square_error = sqrt(average);

    return 0;
}

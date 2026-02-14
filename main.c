#include <stdio.h>
#include "gamelib.h"
#include <stdlib.h>
#include <time.h>

int main(){

    int scelta = 0;

    time_t t;

    srand((unsigned)time(&t));

    do {

        printf("====================================================================\n");
        printf("|                                                                  |\n");
        printf("|                            COSESTRANE                            |\n");
        printf("|                      AVVENTURA AD OCCHINZ                        |\n");
        printf("|                                                                  |\n");
        printf("|__________________________________________________________________|\n");
        printf("|              |                                                   |\n");
        printf("|   PREMI (1)  |                 IMPOSTAZIONE GIOCO                |\n");
        printf("|______________|___________________________________________________|\n");
        printf("|              |                                                   |\n");
        printf("|   PREMI (2)  |                        GIOCA                      |\n");
        printf("|______________|___________________________________________________|\n");
        printf("|              |                                                   |\n");
        printf("|   PREMI (3)  |                   TERMINA  GIOCO                  |\n");
        printf("|______________|___________________________________________________|\n");
        printf("|              |                                                   |\n");
        printf("|   PREMI (4)  |                       CREDITI                     |\n");
        printf("|              |                                                   |\n");
        printf("====================================================================\n");

        if (scanf("%d", &scelta) != 1){

            printf("\n --- ATTENZIONE: DEVI SCEGLIERE UNA DELLE OPZIONI. ---\n");

            while(getchar() != '\n');
            
            scelta = 0;

            continue;
        }

        while(getchar() != '\n');

        switch (scelta){

            case 1:
            printf("--- AVETE SCELTO *IMPOSTAZIONE GIOCO*. OTTIMO! ---\n");
            imposta_gioco();
            printf("--- TUTTO È STATO IMPOSTATO CORRETTAMENTE. ---\n");
            break;

            case 2:
            printf("--- AVETE SCELTO *GIOCA*. ---\n");
            gioca();
            break;

            case 3:
            printf("--- AVETE SCELTO *TERMINA GIOCO*. ATTENDERE... ---\n");
            termina_gioco();
            break;

            case 4:
            printf("--- AVETE SCELTO *CREDITI*. VISUALIZZAZIONE IN CORSO... ---\n");
            crediti();
            break;

            default:
            printf("--- ATTENZIONE: DEVI SCEGLIERE UNA DELLE OPZIONI. ---\n");
            break;
        }

    }while (scelta != 3);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gamelib.h"
#include <string.h>

static Giocatore* giocatori[4] = {NULL};
static Zona_mondoreale* zona1_mondoreale = NULL;
static Zona_soprasotto* zona1_soprasotto = NULL;
static void genera_mappa();
static void inserisci_zona();
static void cancella_zona();
static void stampa_mappa();
static void stampa_zona();
static void chiudi_mappa();
static int UndiciVirgolaCinque = 0;
static int n;
static int modifica_statistiche;
static int mappa_generata = 0;
static Tipo_nemico gnr_enemy_real();
static Tipo_oggetto gnr_object();
static Tipo_nemico gnr_enemy_updown();
static void deallocate_map();
static int win;
static int n_players_alive;
static void gestisci_turno_giocatore(int udtq);
static int movimento;
static int bici_usa;
static char winner[20] = "";
static char hof[3][20] = {"-----", "-----", "-----"};

static const char* prendi_nome_nemico(Tipo_nemico n) {
    switch(n) {
        case nessun_nemico: return "NESSUNO";
        case billi: return "BILLI";
        case democane: return "DEMOCANE";
        case demotorzone: return "DEMOTORZONE";
        default: return "???";
    }
}


static const char* prendi_nome_zona(Tipo_zona t) {
    switch(t) {
        case bosco: return "BOSCO";
        case scuola: return "SCUOLA";
        case laboratorio: return "LABORATORIO";
        case caverna: return "CAVERNA";
        case strada: return "STRADA";
        case giardino: return "GIARDINO";
        case supermercato: return "SUPERMERCATO";
        case centrale_elettrica: return "CENTRALE ELETTRICA";
        case deposito_abbandonato: return "DEPOSITO ABBANDONATO";
        case stazione_polizia: return "STAZIONE DI POLIZIA";
        default: return "???";
    }
}

static const char* prendi_nome_zaino(Tipo_oggetto o) {
    switch(o) {
        case nessun_oggetto: return "NIENTE";
        case bicicletta: return "BICICLETTA";
        case maglietta_fuocoinferno: return "MAGLIETTA FUOCOINFERNO";
        case bussola: return "BUSSOLA";
        case schitarrata_metallica: return "SCHITARRATA METALLICA";
        default: return "???";
    }
}

 static void deallocate_map() {
    struct Zona_mondoreale* curr = zona1_mondoreale;
    while (curr != NULL) {
        struct Zona_mondoreale* now_real = curr;
        struct Zona_soprasotto* now_updown = curr->link_soprasotto;
        curr = curr->avanti;
        free(now_real);
        free(now_updown);
    }
    zona1_mondoreale = NULL;
    zona1_soprasotto = NULL;
    mappa_generata = 0;
}

 static Tipo_nemico gnr_enemy_updown() {
    int q = rand() % 100;
    if (q < 55) {
        return nessun_nemico;
    } else {
        return democane;
    }
}

static Tipo_oggetto gnr_object() {
    int r = rand() % 100;

    if (r < 40) return nessun_oggetto;
    if (r < 55) return bicicletta;
    if (r < 70) return maglietta_fuocoinferno;
    if (r < 85) return bussola;
    return schitarrata_metallica;
}
    
static Tipo_nemico gnr_enemy_real() {
    int r = rand() % 100;
    if (r < 60) return nessun_nemico;
    if (r < 85) return democane;       
    return billi;                      
}


static void renew_hof(){
    strcpy(hof[2], hof[1]);
    strcpy(hof[1], hof[0]);

if (strlen(winner) > 0) {
        strcpy(hof[0], winner);
    } else {
        strcpy(hof[0], "-----");
    }

    strcpy(winner, "");
}

void imposta_gioco(){
    deallocate_map();
    UndiciVirgolaCinque = 0;
    for (int i = 0; i < 4; i++) {
        if (giocatori[i] != NULL) {
            free(giocatori[i]);
            giocatori[i] = NULL;
        }
    }

    static int decisione_dono_valida = 0;

    do{
    printf("--- DEFINIRE IL NUMERO DI GIOCATORI ---\n");
    printf("---       SINGOLO (PREMI 1)         ---\n");
    printf("---       COPPIA  (PREMI 2)         ---\n");
    printf("---       TRIO    (PREMI 3)         ---\n");
    printf("---       SQUADRA (PREMI 4)         ---\n");
    scanf("%d", &n);
    while(getchar() != '\n');
    n_players_alive = n;
} while (n < 1 || n > 4);

for (int i = 0; i < n; i++) {
        giocatori[i] = (struct Giocatore*)malloc(sizeof(struct Giocatore));
        
        printf("---  IL NOME DEL GIOCATORE %d È...  ---\n", i + 1);
        scanf("%s", giocatori[i]->nome);

        giocatori[i]->mondo = 0;
        giocatori[i]->attacco_psichico = (rand() % 20) + 1;
        giocatori[i]->difesa_psichica = (rand() % 20) + 1;
        giocatori[i]->fortuna = (rand() % 20) + 1;
        if (n == 1){
            giocatori[i]->pf = 25;
        } else if (n == 2) {
            giocatori[i]->pf = 20;
        } else {
            giocatori[i]->pf = 15;
        }
        giocatori[i]->maglia_usa = 0;
        for(int j=0; j<3; j++) giocatori[i]->zaino[j] = nessun_oggetto;


        printf("--- STATISTICHE ATTUALI: ATTACCO PSICHICO %d, DIFESA PSICHICA %d, FORTUNA %d ---\n", giocatori[i]->attacco_psichico, giocatori[i]->difesa_psichica, giocatori[i]->fortuna);
        printf("\n");
        printf("---  SELEZIONARE IL DONO DA RICEVERE  ---\n");
        printf("\n");
        printf("--- DONO DELLA MENTE SOVRASTANTE  (PREMI 1) ---\n");
        printf("--- DONO DELLA MENTE BARRICATA    (PREMI 2) ---\n");
        if (UndiciVirgolaCinque == 0){
            printf("--- DONO DI UNDICI-VIRGOLA-CINQUE (PREMI 3) ---\n");
        } else {
            printf("---  UNDICI-VIRGOLA-CINQUE NON DISPONIBILE  ---\n");
        }
        printf("---    NON HO BISOGNO DI DONI     (PREMI 4) ---\n");

        scanf("%d", &modifica_statistiche);
    while(getchar() != '\n');

        switch(modifica_statistiche) {
            case 1:
                giocatori[i]->attacco_psichico += 3;
                giocatori[i]->difesa_psichica -= 3;
                decisione_dono_valida = 1;
                break;
            case 2:
                giocatori[i]->attacco_psichico -= 3;
                giocatori[i]->difesa_psichica += 3;
                decisione_dono_valida = 1;
                break;
            case 3:
                if (UndiciVirgolaCinque == 0) {
                    giocatori[i]->attacco_psichico += 4;
                    giocatori[i]->difesa_psichica += 4;
                    giocatori[i]->fortuna -= 7;
                    strcpy(giocatori[i]->nome, "UndiciVirgolaCinque");
                    UndiciVirgolaCinque = 1;
                    decisione_dono_valida = 1;
                } else {
                    printf("--- UN UNICO UNDICI-VIRGOLA-CINQUE PUÒ ESISTERE NELLO STESSO GIOCO! ---\n");
                }
                break;

            case 4:
            decisione_dono_valida = 1;
            break;
            
            default:
            printf("--- ATTENZIONE: DEVI SCEGLIERE UNA DELLE OPZIONI. ---\n");
            break;
        } while(decisione_dono_valida == 0);
    }

static int pam  = 0;
    do {
        printf("====================================================\n");
        printf("|                                                  |\n");
        printf("|                IMPOSTAZIONI MAPPA                |\n");
        printf("|__________________________________________________|\n");
        printf("|                                                  |\n");
        printf("|    (PREMI 1)          GENERA MAPPA               |\n");
        printf("|    (PREMI 2)          INSERISCI ZONA             |\n");
        printf("|    (PREMI 3)          CANCELLA ZONA              |\n");
        printf("|    (PREMI 4)          STAMPA MAPPA               |\n");
        printf("|    (PREMI 5)          STAMPA ZONA                |\n");
        printf("|    (PREMI 6)          CHIUDI MAPPA               |\n");
        printf("|                                                  |\n");
        printf("====================================================\n");
        
        if (scanf("%d", &pam) != 1) {
            while(getchar() != '\n');
            continue;
        }

        switch(pam) {
            case 1: genera_mappa(); break;
            case 2: inserisci_zona(); break;
            case 3: cancella_zona(); break;
            case 4: stampa_mappa(); break;
            case 5: stampa_zona(); break;
            case 6: 
                chiudi_mappa(); 
                if (mappa_generata == 0) {
                    printf("--- ATTENZIONE: MAPPA NON GENERATA CORRETTAMENTE! ---\n");
                    pam = 0;
                }
                break;
            default: printf("--- ATTENZIONE: DEVI SCEGLIERE UNA DELLE OPZIONI. ---\n");
        }
    } while (pam != 6);

    printf("\n--- IMPOSTAZIONE COMPLETATA! ---\n");
}
void genera_mappa() {
    deallocate_map();
    struct Zona_mondoreale* last_real = NULL;
    struct Zona_soprasotto* last_updown = NULL;
    int boss = 0;

    for (int i = 0; i < 15; i++) {
        struct Zona_mondoreale* nr = malloc(sizeof(struct Zona_mondoreale));
        struct Zona_soprasotto* ns = malloc(sizeof(struct Zona_soprasotto));

        nr->tipo = (enum Tipo_zona)(rand() % 10);
        ns->tipo = nr->tipo;
        nr->link_soprasotto = ns;
        ns->link_mondoreale = nr;
        
        nr->nemico = gnr_enemy_real();
        nr->oggetto = gnr_object();
        ns->nemico = gnr_enemy_updown(&boss);

     
        if (i == 14 && boss == 0) {
            ns->nemico = demotorzone;
        }

        nr->avanti = NULL; ns->avanti = NULL;
        if (i == 0) {
            zona1_mondoreale = nr; zona1_soprasotto = ns;
            nr->indietro = NULL; ns->indietro = NULL;
        } else {
            last_real->avanti = nr; nr->indietro = last_real;
            last_updown->avanti = ns; ns->indietro = last_updown;
        }
        last_real = nr; last_updown = ns;
    }
    printf("--- MAPPA GENERATA CON SUCCESSO! ---\n");
}

static void inserisci_zona() {
    int pos_ins;
    int n_zone = 0;
    Zona_mondoreale* temp = zona1_mondoreale;
    while (temp != NULL) {
        n_zone++;
        temp = temp->avanti;
    }

    printf("DEFINIRE LA POSIZIONE IN CUI INSERIRE LA ZONA (Da 0 a 14) ---\n");
    scanf("%d", &pos_ins);
    while(getchar() != '\n');

    if (pos_ins < 0 || pos_ins > n_zone) {
        printf("--- POSIZIONE NON VALIDA! ---\n");
        return;
    }

    Zona_mondoreale* new_real = (Zona_mondoreale*)malloc(sizeof(Zona_mondoreale));
    Zona_soprasotto* new_updown = (Zona_soprasotto*)malloc(sizeof(Zona_soprasotto));

    new_real->tipo = (Tipo_zona)(rand() % 10);
    new_updown->tipo = new_real->tipo;
    new_real->link_soprasotto = new_updown;
    new_updown->link_mondoreale = new_real;

    printf("\n--- SELEZIONARE IL NEMICO NEL MONDO REALE---\n");
    printf("---    (PREMI 0)        NESSUN NEMICO    ---\n");
    printf("---    (PREMI 1)            BILLI        ---\n");
    printf("---    (PREMI 2)           DEMOCANE      ---\n");
    static int n_scelta;
    scanf("%d", &n_scelta);
    while(getchar() != '\n');
    new_real->nemico = (Tipo_nemico)n_scelta;

    printf("\n---     SELEZIONARE IL TIPO DI OGGETTO     ---\n");
    printf("---    (PREMI 0)        NESSUN OGGETTO     ---\n");
    printf("---    (PREMI 1)           BICICLETTA      ---\n");
    printf("---    (PREMI 2)     MAGLIA FUOCOINFERNO   ---\n"); 
    printf("---    (PREMI 3)             BUSSOLA       ---\n");
    printf("---    (PREMI 4)    SCHITARRATA ELETTRICA  ---\n");
    static int o_scelta;
    scanf("%d", &o_scelta);
    while(getchar() != '\n');
    new_real->oggetto = (Tipo_oggetto)o_scelta;

    printf("\n---     SELEZIONARE IL TIPO DI NEMICO NEL SOPRASOTTO   ---\n");
    printf("---    (PREMI 0)             NESSUN NEMICO             ---\n");
    printf("---    (PREMI 2)                DEMOCANE               ---\n"); 
    printf("---    (PREMI 3)              DEMOTORZONE              ---\n");
    scanf("%d", &n_scelta);
    while(getchar() != '\n');
    new_updown->nemico = (Tipo_nemico)n_scelta;

    if (pos_ins == 0) {
        new_real->avanti = zona1_mondoreale;
        new_real->indietro = NULL;
        new_updown->avanti = zona1_soprasotto;
        new_updown->indietro = NULL;

        if (zona1_mondoreale != NULL) {
            zona1_mondoreale->indietro = new_real;
            zona1_soprasotto->indietro = new_updown;
        }
        zona1_mondoreale = new_real;
        zona1_soprasotto = new_updown;

    } else {
        Zona_mondoreale* prev_real = zona1_mondoreale;
        Zona_soprasotto* prev_updown = zona1_soprasotto;
        
        for (int i = 0; i < pos_ins - 1; i++) {
            prev_real = prev_real->avanti;
            prev_updown = prev_updown->avanti;
        }

        new_real->avanti = prev_real->avanti;
        new_real->indietro = prev_real;
        new_updown->avanti = prev_updown->avanti;
        new_updown->indietro = prev_updown;

        if (prev_real->avanti != NULL) {
            prev_real->avanti->indietro = new_real;
            prev_updown->avanti->indietro = new_updown;
        }
        prev_real->avanti = new_real;
        prev_updown->avanti = new_updown;
    }

    printf("--- ZONA INSERITA CON SUCCESSO! ---\n");
}


static void cancella_zona() {
    if (zona1_mondoreale == NULL) {
        printf("--- ERRORE: PRIMA DI CANCELLARE BISOGNA CREARE! ---\n");
        return;
    }

    int pos_canc, conta = 0;
    Zona_mondoreale* temp = zona1_mondoreale;
    while (temp != NULL) { conta++; temp = temp->avanti; }

    printf("--- DICHIARARE LA ZONA DA ELIMINARE (Da 0 a 14) ---");
    scanf("%d", &pos_canc);
    while(getchar() != '\n');

    if (pos_canc < 0 || pos_canc >= conta) {
        printf("--- ATTENZIONE! POSIZIONE NON VALIDA! ---\n");
        return;
    }

    Zona_mondoreale* elim_real = zona1_mondoreale;
    for (int i = 0; i < pos_canc; i++) elim_real = elim_real->avanti;

    Zona_soprasotto* elim_updown = elim_real->link_soprasotto;

    if (elim_real->indietro != NULL) {
        elim_real->indietro->avanti = elim_real->avanti;
        elim_updown->indietro->avanti = elim_updown->avanti;
    } else {
        zona1_mondoreale = elim_real->avanti;
        zona1_soprasotto = elim_updown->avanti;
    }

    if (elim_real->avanti != NULL) {
        elim_real->avanti->indietro = elim_real->indietro;
        elim_updown->avanti->indietro = elim_updown->indietro;
    }

    free(elim_real);
    free(elim_updown);
    printf("--- ZONA %d È STATA CANCELLATA ---\n", pos_canc);
}

static void stampa_mappa() {
    static int map_print;
    printf("--- SCEGLIERE LA MAPPA DA STAMPARE (0: MONDO REALE, 1: SOPRASOTTO) ---");
    scanf("%d", &map_print);
    while(getchar() != '\n');

    if (map_print == 0) {
        Zona_mondoreale* current = zona1_mondoreale;
        while (current != NULL) {
            printf("[TIPO: %s | NEMICO: %s | OGGETTO: %s] <-> ", prendi_nome_zona(current->tipo), prendi_nome_nemico(current->nemico), prendi_nome_zaino(current->oggetto));
            current = current->avanti;
        }
    } else if (map_print == 1){
        Zona_soprasotto* current = zona1_soprasotto;
        while (current != NULL) {
            printf("[TIPO: %s | NEMICO: %s] <-> ", prendi_nome_zona(current->tipo), prendi_nome_nemico(current->nemico));
            current = current->avanti;
        } 
    }else {
    printf("--- ATTENZIONE: DEVI SCEGLIERE UNA DELLE OPZIONI ---\n");
        }
}

static void stampa_zona() {
    if (zona1_mondoreale == NULL) {
        printf("--- ERRORE: BISOGNA PRIMA CREARE LA MAPPA! ---\n");
        return;
    }

    int zona_stampa;
    int conta = 0;
    Zona_mondoreale* scan = zona1_mondoreale;

 
    while (scan != NULL) {
        conta++;
        scan = scan->avanti;
    }

    printf("--- INSERIRE IL NUMERO DELLA ZONA DA STAMPARE (DA 0 A %d) ---\n", conta - 1);
    if (scanf("%d", &zona_stampa) != 1) {
        printf("ATTENZIONE: DEVE INSERIRE IL NUMERO DI UNA ZONA ESISTENTE ---\n");
        while(getchar() != '\n');
        return;
    }

    if (zona_stampa < 0 || zona_stampa >= conta) {
        printf("--- ERRORE! POSIZIONE NON VALIDA! ---\n");
        return;
    }

    Zona_mondoreale* curr_real = zona1_mondoreale;
    for (int i = 0; i < zona_stampa; i++) {
        curr_real = curr_real->avanti;
    }

    Zona_soprasotto* curr_updown = curr_real->link_soprasotto;

    printf("\n======================================\n");
    printf("   DETTAGLI ZONA (Indice %d)\n", zona_stampa);
    printf("======================================\n");
    printf("[MONDO REALE]\n");
    printf("TIPO: %s | NEMICO: %s | OGGETTO: %s\n", 
            prendi_nome_zona(curr_real->tipo), prendi_nome_nemico(curr_real->nemico), prendi_nome_zaino(curr_real->oggetto));

    if (curr_updown != NULL) {
        printf("--------------------------------------\n");
        printf("[SOPRASOTTO]\n");
        printf("TIPO: %s | NEMICO: %s\n", prendi_nome_zona(curr_updown->tipo), prendi_nome_nemico(curr_updown->nemico));
    } else {
        printf("--- ERRORE! SOPRASOTTO NON ESISTENTE ---\n");
    }
    printf("======================================\n\n");
}

static void chiudi_mappa() {
    int conta_zone = 0;
    int conta_boss = 0;
    
    Zona_soprasotto* current = zona1_soprasotto;
    while (current != NULL) {
        conta_zone++;
        if (current->nemico == demotorzone) {
            conta_boss++;
        }
        current = current->avanti;
    }

    if (conta_zone < 15) {
        printf("--- ERRORE: LA MAPPA DEVE CONTENERE 15 ZONE (attuali: %d) ---\n", conta_zone);
        mappa_generata = 0;
    } else if (conta_boss != 1) {
        printf("--- ERRORE: DEVE ESSERE PRESENTE ESATTAMENETE 1 DEMOTORZONE (attuali: %d) ---\n", conta_boss);
        mappa_generata = 0;
    } else {
        printf("--- LA MAPPA È STATA STAMPATA CON SUCCESSO. EVVIVA! ---.\n");
        mappa_generata = 1;
    }
}

void gioca(){

    if (mappa_generata == 0) {
        printf("ERRORE! BISOGNA PRIMA GENERARE LA MAPPA NEL MENÙ IMPOSTA GIOCO ---\n");
        return;
    }
    win = 0;
    for (int i = 0; i < 4; i++) {
        if (giocatori[i] != NULL) {
            giocatori[i]->pos_mondoreale = zona1_mondoreale;
            giocatori[i]->mondo = 0;
        }
    }

    int alive = n_players_alive;
    
    while (alive > 0 && !win) {
        int ordine[4] = {-1, -1, -1, -1};
        int n_alive = 0;

        for (int i = 0; i < 4; i++) {
            if (giocatori[i] != NULL) {
                ordine[n_alive++] = i;
            }
        }

        for (int i = n_alive - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            int temp = ordine[i];
            ordine[i] = ordine[j];
            ordine[j] = temp;
        }

        for (int i = 0; i < n_alive; i++) {
            int udtq = ordine[i];
            if (giocatori[udtq] != NULL) {
                gestisci_turno_giocatore(udtq);
                
                if (win || n_players_alive == 0){
                    renew_hof();
                } 
            }
        }
        
        alive = n_players_alive;
    }
}
    static void utilizza_oggetto(int udtq, int* movimento, int* pf_nemico, int fight) {
    printf("--- COSA VUOI USARE? ---\n");
    for (int j = 0; j < 3; j++) {
        printf("%d) %s\n", j + 1, prendi_nome_zaino(giocatori[udtq]->zaino[j]));
    }
    
    int scelta;
    scanf("%d", &scelta);
    while(getchar() != '\n');
    scelta--;

    if (scelta < 0 || scelta > 2 || giocatori[udtq]->zaino[scelta] == nessun_oggetto) {
        printf("--- ATTENZIONE! ASSICURATI DI UTILIZZARE UN OGGETTO IN TUO POSSEDIMENTO ---\n");
    } else {
        Tipo_oggetto o = giocatori[udtq]->zaino[scelta];

    switch(o) {
        case bicicletta:
        if (fight == 1){
            printf("--- ATTENZIONE! NON È POSSIBILE PEDALARE VIA DA UN COMBATTIMENTO! ---\n");
        } else if (*movimento == 1){
            *movimento = 0;
        } else {bici_usa = 1;
        }

            break;
        case maglietta_fuocoinferno:
        giocatori[udtq]->maglia_usa = 1;

            break;
        case bussola:
            int distanza = 0;
            int trovato = 0;
            Zona_soprasotto* scan = zona1_soprasotto;
            while (scan != NULL) {
                if (scan->nemico == demotorzone) {
                    trovato = 1;
                    break;
                }
                distanza++;
                scan = scan->avanti;
            }
            if (trovato) {
                printf("--- L'AGO GIRA IMPAZZITO... IL DEMOTORZONE SI TROVA ALLA ZONA %d DEL SOPRASOTTO! ---\n", distanza);
            }
            break;
        
        case schitarrata_metallica:
        if (fight != 1){
            printf("--- ATTENZIONE! DEVI ESSERE IN COMBATTIMENTO PER POTER USARE QUESTO OGGETTO! ---\n");
        } else {
            printf("--- LANCI UN ASSOLO DI CHITARRA CONTRO IL NEMICO! ---\n");
            *pf_nemico = *pf_nemico - 3;
        }
            break;

        default:
        
            break;
        }
        giocatori[udtq]->zaino[scelta] = nessun_oggetto;
    }
}

static void combatti(int udtq) {

    int fight = 1;
    Tipo_nemico nemico_attivo;
    
    if (giocatori[udtq]->mondo == 0){ nemico_attivo = giocatori[udtq]->pos_mondoreale->nemico;
    } else {nemico_attivo = giocatori[udtq]->pos_soprasotto->nemico;
    }
    int pf_nemico, def_nemico, atk_nemico;
    switch(nemico_attivo) {
        case billi:       pf_nemico = 3; def_nemico = 8; atk_nemico = 5; break;
        case democane:    pf_nemico = 5; def_nemico = 13; atk_nemico = 9; break;
        case demotorzone: pf_nemico = 12; def_nemico = 18; atk_nemico = 13; break;
        default: 
    
    return;
    }

    printf("\n--- INIZIA IL COMBATTIMENTO CONTRO %s! ---\n", prendi_nome_nemico(nemico_attivo));

    while (pf_nemico > 0 && giocatori[udtq] != NULL) {
        
        printf("\n PF %s: %d | PF %s: %d\n", giocatori[udtq]->nome, giocatori[udtq]->pf, prendi_nome_nemico(nemico_attivo), pf_nemico);
        printf("---   OPZIONI  ---\n");
        printf("\n");
        printf("1)   ATTACCA   ---\n");
        printf("2) USA OGGETTO ---\n");

        int scelta;
        scanf("%d", &scelta);
        while(getchar() != '\n');

        if (scelta == 1) {
            int d20 = (rand() % 20) + 1;
            int totale_atk = d20 + giocatori[udtq]->attacco_psichico;

            if (totale_atk > def_nemico) {
                int danno = 1;
                int d_fortuna = (rand() % 20) + 1;
                if (d_fortuna < giocatori[udtq]->fortuna) {
                    printf("--- COLPO CRITICO! ---\n");
                    danno = 2;
                }
                pf_nemico -= danno;
                printf("--- NEMICO COLPITO CON SUCCESSO! ---\n");
            } else {
                printf("--- L'ATTACCO VIENE SCHIVATO... ---\n");
            }
        } else {
            utilizza_oggetto(udtq, &movimento, &pf_nemico, fight); 
        }

        if (pf_nemico > 0) {
            if (giocatori[udtq]->maglia_usa == 1) {
                printf("--- IL NEMICO PROVA AD ATTACCARE, MA LA MAGLIETTA FUOCOINFERNO TI PROTEGGE DAL COLPO! ---\n");
                giocatori[udtq]->maglia_usa = 0;
            } else{
            int d6 = (rand() % 6) + 1;
            int totale_atk_n = d6 + atk_nemico;
            printf("\n--- %s ATTACCA! ---\n", prendi_nome_nemico(nemico_attivo));

            if (totale_atk_n > giocatori[udtq]->difesa_psichica) {
                giocatori[udtq]->pf--;
                printf("--- SEI STATO COLPITO! PUNTI FERITA RIMANENTI: %d ---\n", giocatori[udtq]->pf);
            } else {
                printf("--- L'ATTACCO NEMICO VA A VUOTO. PER UN PELO! ---\n");
            }
        }
        }

        if (giocatori[udtq]->pf <= 0) {
            printf("\n--- %s È CADUTO IN BATTAGLIA... ---\n", giocatori[udtq]->nome);
            free(giocatori[udtq]);
            giocatori[udtq] = NULL;
            n_players_alive--;
            return; 
        }
    }

    if (pf_nemico <= 0) {
        printf("\n--- HAI SCONFITTO %s! ---\n", prendi_nome_nemico(nemico_attivo));
        
        if (nemico_attivo == demotorzone) {
            strcpy(winner, giocatori[udtq]->nome);
            win = 1; 
            return;
        }

        if (rand() % 100 < 50) {
            if (giocatori[udtq]->mondo == 0) giocatori[udtq]->pos_mondoreale->nemico = nessun_nemico;
            else giocatori[udtq]->pos_soprasotto->nemico = nessun_nemico;
            printf("--- IL NEMICO SI RITIRA DAL CAMPO DI BATTAGLIA ---\n");
        }
    }
}

    static void gestisci_turno_giocatore(int udtq) {
    int scelta;
    int fine_turno = 0;
    movimento = 0;
    bici_usa = 0;

    printf("\n--- TURNO DI %s ---\n", giocatori[udtq]->nome);

if (giocatori[udtq]->mondo == 0) {
    if (giocatori[udtq]->pos_mondoreale->nemico != nessun_nemico) {
        printf("--- NEMICO PRESENTE. COMBATTI! ---\n");
    combatti(udtq);
    }
} else {
    if (giocatori[udtq]->pos_soprasotto->nemico != nessun_nemico) {
        printf("--- NEMICO PRESENTE. COMBATTI! ---\n");
    combatti(udtq);
    }
}
if (giocatori[udtq] == NULL) return;
    do {
        printf("\n=========================================\n");
        printf("|                                       |\n");
        printf("|      (1)   AVANZA                     |\n");
        printf("|                                       |\n");
        printf("|      (2)   INDIETREGGIA               |\n");
        printf("|                                       |\n");
        printf("|      (3)   CAMBIA MONDO               |\n");
        printf("|                                       |\n");
        printf("|      (4)   STAMPA GIOCATORE           |\n");
        printf("|                                       |\n");
        printf("|      (5)   STAMPA ZONA                |\n");
        printf("|                                       |\n");
        printf("|      (6)   RACCOGLI OGGETTO           |\n");
        printf("|                                       |\n");
        printf("|      (7)   UTILIZZA OGGETTO           |\n");
        printf("|                                       |\n");
        printf("|      (8)   PASSA                      |\n");
        printf("|                                       |\n");
        printf("=========================================\n");
        scanf("%d", &scelta);
        while(getchar() != '\n');
        
        switch(scelta){

            case 1:
            if (movimento == 0){
            if (giocatori[udtq]->mondo == 0) {
        if (giocatori[udtq]->pos_mondoreale->avanti != NULL) {
            giocatori[udtq]->pos_mondoreale = giocatori[udtq]->pos_mondoreale->avanti;
            giocatori[udtq]->pos_soprasotto = giocatori[udtq]->pos_mondoreale->link_soprasotto;
            printf("SEI ARRIVATO NELLA DESTINAZIONE: %s\n", prendi_nome_zona(giocatori[udtq]->pos_mondoreale->tipo));
            if (bici_usa == 1) {bici_usa = 0;
            } else {movimento = 1;
            }

        } else {
            printf("--- NON È POSSIBILE MUOVERSI PIÙ AVANTI. ---\n");
        }
    } else {
        if (giocatori[udtq]->pos_soprasotto->avanti != NULL) {
            giocatori[udtq]->pos_soprasotto = giocatori[udtq]->pos_soprasotto->avanti;
            giocatori[udtq]->pos_mondoreale = giocatori[udtq]->pos_soprasotto->link_mondoreale;
            printf("--- AVANZI NELL'OSCURITÀ... ---\n");
            if (bici_usa == 1) {bici_usa = 0;
            } else {movimento = 1;
            }

        } else {
            printf("--- NON RIESCI A VEDERE OLTRE UNA COLTRE DI NEBBIA. NON PUOI PROSEGUIRE ---\n");
        }
    }
 } else {
    printf("--- TI SEI GIÀ MOSSO, NON PUOI PROSEGUIRE DI PIÙ ---");
  }

            break;

            case 2:
            if (movimento == 0){
            if (giocatori[udtq]->mondo == 0) {
        if (giocatori[udtq]->pos_mondoreale->indietro != NULL) {
            giocatori[udtq]->pos_mondoreale = giocatori[udtq]->pos_mondoreale->indietro;
            giocatori[udtq]->pos_soprasotto = giocatori[udtq]->pos_mondoreale->link_soprasotto;
            printf("SEI ARRIVATO NELLA DESTINAZIONE: %s\n", prendi_nome_zona(giocatori[udtq]->pos_mondoreale->tipo));
            if (bici_usa == 1) {bici_usa = 0;
            } else {movimento = 1;
            }

        } else {
            printf("--- NON È POSSIBILE ANDARE PIÙ INDIETRO. ---\n");
        }
    } 
    else {
        if (giocatori[udtq]->pos_soprasotto->indietro != NULL) {
            giocatori[udtq]->pos_soprasotto = giocatori[udtq]->pos_soprasotto->indietro;
            giocatori[udtq]->pos_mondoreale = giocatori[udtq]->pos_soprasotto->link_mondoreale;
            printf("--- INDIETREGGI... ---\n");
            if (bici_usa == 1) {bici_usa = 0;
            } else {movimento = 1;
            }

        } else {
            printf("--- PROVI A TORNARE INDIETRO, MA UN CLAMORE IMPROVVISO TI SPRONA AD AVANZARE ---\n");
        }
    }
} else {
    printf("--- TI SEI GIÀ MOSSO, NON PUOI MUOVERTI NUOVAMENTE ---\n");
}
            break;

            case 3:
    if (movimento == 0){
    if (giocatori[udtq]->mondo == 0) { 
        giocatori[udtq]->mondo = 1;
        printf("--- CON LE TUE FORZE PSICHICHE GENERI UN PORTALE. IL SOPRASOTTO TI ASPETTA... ---\n");
        movimento = 1;
        
    } else { 
        int d20 = (rand() % 20) + 1;

        if (d20 < giocatori[udtq]->fortuna) {
            giocatori[udtq]->mondo = 0;
            printf("--- RIESCI AD APRIRTI UN VARCO PER IL MONDO REALE. SEI SALVO, PER ORA... ---\n");
            movimento = 1;
        } else {
            printf("--- CERCHI DI APRIRE UN PORTALE DI RITORNO PER CASA, MA LE TEMPESTE PSICHICHE DEL SOPRASOTTO TI IMPEDISCONO DI CONCENTRARTI... ---\n");
            movimento = 1;
        }
      }
    } else {
        printf("--- TI SEI GIÀ MOSSO, NON PUOI MUOVERTI NUOVAMENTE ---\n");
    }
            break;

            case 4:
            printf("\n--- INFO GIOCATORE ---\n");
    printf("NOME: %s\n", giocatori[udtq]->nome);
    printf("DIMENSIONE: %s\n", (giocatori[udtq]->mondo == 0) ? "MONDO REALE" : "SOPRASOTTO");
    printf("STATISTICHE: [ATK: %d] [DEF: %d] [FORT: %d]\n", giocatori[udtq]->attacco_psichico, giocatori[udtq]->difesa_psichica, giocatori[udtq]->fortuna);
    printf("\n");
    printf("--- ZAINO ---");
    for(int j = 0; j < 3; j++) {
        printf("[%s] ", prendi_nome_zaino(giocatori[udtq]->zaino[j])); 
    }
    printf("\n----------------------\n");
    break;

            case 5:
    printf("\n--- INFO ZONA ATTUALE ---\n");
    if (giocatori[udtq]->mondo == 0) {
        Zona_mondoreale* zr = giocatori[udtq]->pos_mondoreale;
        printf("DIMENSIONE: MONDO REALE\n");
        printf("TIPO: %s\n", prendi_nome_zona(zr->tipo));
        printf("NEMICO: %s\n", prendi_nome_nemico(zr->nemico));
        printf("OGGETTO PRESENTE: %s\n", prendi_nome_zaino(zr->oggetto));
    } else {
        Zona_soprasotto* zs = giocatori[udtq]->pos_soprasotto;
        printf("DIMENSIONE: SOPRASOTTO\n");
        printf("TIPO: %s\n", prendi_nome_zona(zs->tipo));
        printf("NEMICO: %s\n", prendi_nome_nemico(zs->nemico));
    }
    printf("------------------------\n");
    break;

            case 6:
            if (giocatori[udtq]->mondo != 0) {
        printf("--- IN QUESTO MONDO NON VI È NIENTE DI UTILE, SOLO CAOS... ---\n");
    }

    Zona_mondoreale* curr_real = giocatori[udtq]->pos_mondoreale;

    if (curr_real->oggetto == nessun_oggetto) {
        printf("--- NON C'È NULLA DA RACCOGLIERE QUI. ---\n");
    }

    int raccolto = 0;
    for (int j = 0; j < 3; j++) {
        if (giocatori[udtq]->zaino[j] == nessun_oggetto) {
            giocatori[udtq]->zaino[j] = curr_real->oggetto;
            printf("--- HAI RACCOLTO: %s! ---\n", prendi_nome_zaino(curr_real->oggetto));
            
            curr_real->oggetto = nessun_oggetto;
            raccolto = 1;
            break;
        }
    }

    if (!raccolto) {
        printf("--- IL TUO ZAINO È PIENO! DEVI USARE QUALCOSA PRIMA ---\n");
    }

        break;

            case 7:
            int pfn_finti = 0;
            utilizza_oggetto(udtq, &movimento, &pfn_finti, 0);
        break;

            case 8:
            printf("--- IL TUO TURNO È FINITO ---\n");
            fine_turno = 1;
            break;

            default:
            printf("ATTENZIONE: DEVI SCEGLIERE UNA DELLE OPZIONI\n");
        } 
        if (win) fine_turno = 1;
  } while (!fine_turno && giocatori[udtq] != NULL);
}

void termina_gioco(){
    deallocate_map();
    for (int i = 0; i < 4; i++) {
        if (giocatori[i] != NULL) {
            free(giocatori[i]);
            giocatori[i] = NULL;
        }
    }
    printf("\n--- GRAZIE PER AVER PARTECIPATO A QUESTO GIOCO. A PRESTO! ---");
}

void crediti(){

    printf("\n=======================================\n");
    printf("                                       \n");
    printf("                CREDITI                \n");
    printf("_______________________________________\n");
    printf("                                       \n");
    printf("     SVILUPPATO DA MARCO BIAGIOLI      \n");
    printf("_______________________________________\n");
    printf("                                       \n");
    printf("---    ELENCO MIGLIORI GIOCATORI    ---\n");
    printf("                                       \n");
    printf("  1)   %s\n", hof[0]);
    printf("                                       \n");
    printf("  2)   %s\n", hof[1]);
    printf("                                       \n");
    printf("  3)   %s\n", hof[2]);
    printf("_______________________________________\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("--- PREMERE INVIO PER TORNARE AL MENÙ PRINCIPALE ---");
    while(getchar() != '\n');
    getchar();
}

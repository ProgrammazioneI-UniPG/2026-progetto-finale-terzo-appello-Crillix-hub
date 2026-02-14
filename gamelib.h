#ifndef GAMELIB_H 
#define GAMELIB_H 
#include <stdio.h>
#include <stdlib.h>

typedef enum Tipo_zona{
    bosco, scuola, laboratorio, caverna, strada, giardino, supermercato, stazione_polizia, centrale_elettrica, deposito_abbandonato,
}Tipo_zona;

typedef enum Tipo_nemico{
    nessun_nemico, billi, democane, demotorzone
}Tipo_nemico;

typedef enum Tipo_oggetto{
nessun_oggetto, bicicletta, maglietta_fuocoinferno, bussola, schitarrata_metallica, 
}Tipo_oggetto;

struct Zona_mondoreale;
struct Zona_soprasotto;

typedef struct Zona_mondoreale{
    Tipo_zona tipo;
    Tipo_nemico nemico;
    Tipo_oggetto oggetto;

    struct Zona_mondoreale *avanti;
    struct Zona_mondoreale *indietro;
    struct Zona_soprasotto *link_soprasotto;
}Zona_mondoreale;

typedef struct Zona_soprasotto{
    Tipo_zona tipo;
    Tipo_nemico nemico;

    struct Zona_soprasotto *avanti;
    struct Zona_soprasotto *indietro;
    struct Zona_mondoreale *link_mondoreale;
}Zona_soprasotto;

typedef struct Giocatore{
char nome[20];
int mondo;
Zona_mondoreale *pos_mondoreale;
Zona_soprasotto *pos_soprasotto;
int attacco_psichico;
int difesa_psichica;
int fortuna;
int pf;
int maglia_usa;
Tipo_oggetto zaino[3];
}Giocatore;

void imposta_gioco();
void gioca();
void termina_gioco();
void crediti();

#endif

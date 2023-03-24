/* OPREA Radu-Andrei - 335CB  */
#include <stdio.h>
#include "tlg.h"
#include "thash.h"
#include <string.h>

// Structura unui cuvant
typedef struct cuvant {
	char * text;
	int aparitii;
} Cuvant;



// Functie pentru memorarea cheilor pentru tabela
int addKey(int * v, int e) {
	int i;
	for (i = 0; i < ('Z' - 'A') ; i++)
	{	
		if (v[i] == e)
			return 0;
		else if (v[i] == -1){
			v[i] = e;
			return 1;
		}
	}
}

// Functie pentru verificarea existentei unei chei in tabela
int inKeys(int * v, int e) {
	int i;
	for (i = 0; i < ('Z' - 'A') ; i++)
	{	
		if (v[i] == e){
			return 1;
		}
	}
	return 0;
}

// Functia hash
int codHash(void * element)
{
	Cuvant * cuv = (Cuvant *) element;
	char * text = cuv -> text;

	if(* text - 'A' >= 32) {
		return * text - 'a';
	}

	return * text - 'A';
}

// Functie pentru inserare de structuri Cuvant intr-o lista
int Ins_Cuvinte(TLG* aL, void * e) {

	Cuvant * c = (Cuvant *) e;
	Cuvant * aux;				// variabile pentru usurinta 
	TLG ind = *aL;				// lucrului cu structuri
	char * aux_t;	
	while(ind != NULL) {
		aux = (Cuvant*) (ind -> info);
		aux_t = aux -> text;


		// Verificarea daca nu exista deja cuvantul in lista
		// In caz afirmativ, creste numarul de aparitii
		if( strcmp(aux_t, c -> text) == 0){

			aux -> aparitii += 1;
			return 1;
		}
		ind = ind -> urm;
	}

	return Ins_IncLG(aL, c);
}

// Functie pentru inserarea cuvintelor in tabela Hash
int InsTHCuvinte(TH*a, Cuvant * ae, TFCmp fcmp)
{
    int cod = a->fh(ae), rez;
    TLG el;


    for(el = a->v[cod]; el != NULL; el = el->urm) {   	
        if (fcmp(el->info, ae) == 1)
            return 1;
    }
    
    rez = Ins_Cuvinte(a->v+cod, ae); 
    return rez;
}
// Comparator pentru tabela Hash
int cmp(void * e1, void * e2) {

	Cuvant * c1 = (Cuvant *) e1;
	Cuvant * c2 = (Cuvant *) e2;

	if(strcmp(c1 -> text, c2 -> text) != 0)
		return 0;
	if(c1 -> aparitii != c2 -> aparitii){
		return 0;
	}

	return 1;
}


// Comparator pentru sortarea in ordinea dorita: lungime, numar 
// de aparitii, lexicografic.
int ordonator(void * e1, void * e2) {

	// Variabile pentru usurinta lucrului cu structuri.
	Cuvant * c1 = (Cuvant *) e1;
	Cuvant * c2 = (Cuvant *) e2;

	char * t1 = c1 -> text;
	char * t2 = c2 -> text;

	int val1, val2;



	if (strlen(t1) < strlen(t2))
		return 1;

	else if (strlen(t1) == strlen(t2)) {
		if (c1 -> aparitii > c2 -> aparitii)
			return 1;
		else if (c1 -> aparitii == c2 -> aparitii)
			return ((*t1 - 'A') < (*t2 - 'A'));
	}

	return 0;

}



// Functie de sortare a unei liste: folosim comparatorul definit
// anterior, urmand un bubblesort.
void sortList(TLG p) {

	TLG el;
	TLG aux = NULL;
	char * temp;

	if (p == NULL)
		return;

	for (el = p ; el != NULL; el = el -> urm)
	{	
		for ( aux = el -> urm; aux != NULL ; aux = aux -> urm ) {
			if ( ordonator ( el -> info, aux -> info) > 0){
				temp = (char*) el->info;
				el->info = aux->info;
				aux->info = (char*) temp;
			}
		}

	}
}

// Functie pentru afisarea unui cuvant, in format: "text/aparitii"
void afisareCuvant(void * elem) {
	Cuvant * cuv = (Cuvant *) elem;
	printf("%s/%d", cuv -> text, cuv -> aparitii);
}

// Functie pentru generarea tabelei hash.
TH * GenerareHash(TLG lista)
{
 	TH *h = NULL;
	TLG p;
    Cuvant * cuv;
	int rez;

	// calcul dimensiuni maxime pt tabela hash 
	size_t M = ('Z'-'A') + 1;

	// initializare tabela hash 
	h = (TH *) InitTH(M, codHash);
	if(h == NULL)
		return NULL;

	// sortare
	sortList(lista);

	// se ia fiecare cuvant din lista, se memoreaza intr-o strucura
	// Cuvant, si se introduce in Hash.
	for(p = lista; p != NULL; p = p->urm) {

      cuv = (Cuvant *) malloc(sizeof(Cuvant));
      if(cuv == NULL){
			return h;
      }
		cuv -> text = malloc(sizeof(char *) 
			* strlen(((Cuvant *)p -> info) -> text ) + 1);


      memcpy(cuv, p->info, sizeof(Cuvant));

	rez = InsTHCuvinte(h, cuv, cmp);
      if(!rez) {
			free(cuv);
			return h;
		}
	}


	return h;
}

// Functia de afisare. Parametrii sunt tabela Hash, un int de maxim
// si o litera.
// In functie de comanda apelata, parametrii pot avea ori valori "nule"
// ori pot fi folositi pentru cele trei print-uri.
void PrintAll(TH* ah, int maxim, char litera) {

	TLG p, el;
	int i, j, len = 0, unregistered_key, litera_val;
	Cuvant * c;

	// pentru a nu afisa valori de pe pozitiile nule, se cauta i 
	// in vectorul de chei. Daca nu se gaseste, inseamna ca urmeaza o
	// pozitie cu valori nule, si o evitam
	for (i = 0; i < ah -> M; i++) {
		unregistered_key = 0;

		for (j = 0; j < ah -> M; j++) {
			if ( inKeys(ah -> keys, i) == 0) {
				unregistered_key = 1;
				break;
			}
		}
		if (unregistered_key == 1)
			continue;

		// in functie de valoarea parametrilor, se afiseaza diferit pozita,
		// dupa ref-urile din checker.
		if (maxim == -1)
			printf("pos %d: ", i);
		else if (litera == '0')
			printf("pos%d: ", i);
		// in cazul cuvintelor de o anumita lungime sau litera, se va compara
		// litera_val. Se schimba valoarea daca este majuscula sau nu.
		else {
			if( litera - 'a' < 0) {
				litera_val = litera - 'A';
			}
			else litera_val = litera - 'a'; 
		}
		
		p = ah -> v[i];

		//Print obijnuit
		if(p && maxim == -1) {
			for (el = p; el != NULL; el = el -> urm) {
				c = (Cuvant *) el -> info;
				if(strlen(c -> text) != len) {
					len = strlen(c -> text);
					printf("(%d:", len);
				}
				afisareCuvant(c);
				if (el -> urm == NULL)
					printf(")\n");
				else if ( strlen(((Cuvant *) el->urm->info) -> text) != len)
					printf(")");
				else
					printf(", ");
			}
		}
		//Print dupa numarul de aparitii
		else if(p && litera == '0') {
			for (el = p; el != NULL; el = el -> urm) {
				c = (Cuvant *) el -> info;

				if (c -> aparitii != maxim){
					if (el -> urm == NULL)
						printf("\n");
					continue;
				}

				if(strlen(c -> text) != len) {
					len = strlen(c -> text);
					printf("(%d: ", len);
				}
				afisareCuvant(c);
				if (el -> urm == NULL)
					printf(")\n");
				else if ( strlen(((Cuvant *) el->urm->info) -> text) != len)
					printf(")");
				else
					printf(", ");
			}
		}
		//Print dupa litera si lungime
		else if(p && i == litera_val){

			printf("(%d:",maxim);
			for (el = p; el != NULL; el = el -> urm) {
				c = (Cuvant *) el -> info;

				if(strlen(c -> text) != maxim) {
					if (el -> urm == NULL)
						printf("\n");
					continue;
				}
				afisareCuvant(c);
				if ( strlen(((Cuvant *) el->urm->info) -> text) != maxim)
					printf(")");
				else
					printf(", ");
			}
		}
	}
}
// Functia realizeaza atat citirea cat si apelarea functiei de Print
void citesteLista(char * file) {

	char * line = NULL;
	size_t len = 0;
	char * p;
	TH * h = NULL;
	TLG L = NULL;

	FILE * f = fopen(file, "rt");

	if (!f){
		return;
	}
	int key = -1;
	int * keys = malloc((('Z' - 'A') + 1) * sizeof(int));

	int i;
	// popularea vectorului
	for (i = 0; i < ('Z' - 'A') ; i++)
	{
		keys[i] = -1;
	}

	//parcurgerea fisierului si a cuvintelor
	while(getline(&line, &len, f) != -1) {
		
		p = strtok(line, " ,\n.");

		if (strcmp(p, "insert") == 0) {

			p = strtok(NULL, " ,\n.");

			while(p != NULL) {

				// lucram doar pe cele de len > 3;
				if (strlen(p) >= 3) {

					//ignoram cuvintele cu caractere non-litere
					if (p[0] < 'A'){
						p = strtok(NULL, " ,\n.");
						continue;
					}

					
					// primul caracter va fi cheia
					if(p[0] - 'A' >= 32)
						key = p[0] - 'a';
					else key = p[0] - 'A';

				
					// adaugam cheia in vectorul de chei
					addKey(keys,key);

					
					// corectarea unui neajuns al strtok-ului.
					if(p[strlen(p)-1] == '.')
						p[strlen(p)-1] = '\0';

						
					Cuvant * cuv = malloc(sizeof(Cuvant));
					if (cuv == NULL){
						break;
					}

					cuv -> text = malloc(sizeof(char*) * strlen(p) + 1);
					strcpy(cuv -> text, p);

					cuv -> aparitii = 1;
					Ins_Cuvinte(&L, (void *) cuv);
					

				}
				p = strtok(NULL, " ,\n.");
			}

			// Generare
			h = GenerareHash(L);
			if (h == NULL) {
				printf("Tabela hash nu a putut fi generata\n");
		        return;
		    }

		    // In plus fata de un hash table obijnuit, avem si o lista de chei
		    // pentru usurinta in printare.
		    h -> keys = keys;
		    if (h -> keys == NULL){
		    	printf("Chei negenerate\n");
		  		return;
			}


		}
		else if (strcmp(p, "print") == 0) {
			
			// maxim poate servi atat ca si numar de aparitii, cat si
			// ca si lungime a cuvintelor, pentru cele doua cazuri 
			// speciale
			int maxim = -1;
			char litera = '0';

			p = strtok(NULL, " ,\n.");

			// la un print obijnuit, acestea vor fi -1, respectiv '0'
			if (p == NULL){
			}
			else if (p[0] < 'A') {
				maxim = p[0] - '0';
			}
			else {
				litera = p[0];
				p = strtok(NULL, " ,\n.");
				maxim = p[0] - '0';
				
			}
			
			PrintAll(h, maxim, litera);
		}
		
	}
	// eliberari si inchideri de fisier
	free(keys);
	DistrugeLG(&L, free);
	DistrTH(&h, free);
	fclose(f);	
}



// main
int main(int argc, char *argv[])
{

 	citesteLista(argv[1]);

  	return 0;
}

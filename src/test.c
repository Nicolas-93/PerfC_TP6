#include "test.h"
#include "tableau.h"
#include "ABR.h"
#include <string.h>
#include <stdio.h>

// Ces fonctions sont privées
static int test_MOT_est_correct();
static int test_MOT_normaliser();
static int test_TAB_tri();
static int test_ALG_mots_avant_x();
static int test_ALG_compter_mots();
static int test_ALG_mots_apres_x();
static int test_ALG_expressions();
static int test_ABR_initialiser();
static int test_ABR_alloue_noeud();
static int test_ABR_cherche_mot();
static int test_ABR_appartient();
static int test_ABR_ajouter_mot();


static char* t1[] = {
    "zzz", "zzz", "zzz", "zzz", "zzz",
    "abc", "abc", "abc", "abc",
    "aaa", "aaa", "aaa",
    "caa", "caa",
    "pfff"
};

TestFunc tests[] = {
    test_MOT_normaliser,
    test_MOT_est_correct,
    test_ABR_initialiser,
    test_ABR_alloue_noeud,
    test_ABR_ajouter_mot,
    test_ABR_cherche_mot,
    test_ABR_appartient,
    test_ALG_compter_mots,
    test_TAB_tri,
    test_ALG_mots_avant_x,
    test_ALG_mots_apres_x,
    test_ALG_expressions,
};

int test() {
    for (int i = 0; i < SIZEOF_ARRAY(tests); i++) {
        if (!tests[i]())
            return 0;
    }
    return 1;
}

void test_assert_print(const char* file, int line, const char* func, const char* expr, int res) {
    FILE* stream;
    char* msg;
    if (res) {
        stream = stdout;
        msg = COLOR_STR(COLOR_GREEN, "Assertion correcte : ");
    } else { 
        stream = stderr;
        msg = COLOR_STR(COLOR_RED, "Erreur d'assertion : ");
    } 
    fprintf(stream, 
        "%s"
        COLOR_STR(COLOR_BLUE, "%s:%d : ")
        COLOR_STR(COLOR_YELLOW, "%s : ")
        COLOR_STR(COLOR_R, "'%s'\n"),
        msg, file, line, func, expr);
}

static int test_MOT_normaliser() {
    char t1[] = "HelLo WORLd";
    char t2[] = "LicEnCE 1";
    char t3[] = "ù$*ùù Corr$^*ùm*pu";
    
    MOT_normaliser(t1);
    MOT_normaliser(t2);
    MOT_normaliser(t3);

    test_assert(STR_EQUALS(t1, "hello world"));
    test_assert(STR_EQUALS(t2, "licence 1"));
    test_assert(STR_EQUALS(t3, "ù$*ùù corr$^*ùm*pu"));

    return 1;
}

static int test_MOT_est_correct() {
    test_assert(MOT_est_correct("Hello"));
    test_assert(MOT_est_correct(""));
    test_assert(!MOT_est_correct("$ù*é-'"));
    test_assert(!MOT_est_correct("sa1ut"));
    test_assert(!MOT_est_correct("sa1ut3"));
    test_assert(!MOT_est_correct("5a1ut"));

    return 1;
}

static int test_TAB_tri() {

    Mots* ens1 = ABR_initialiser(), *ens2 = ABR_initialiser();
    
    // Test 1 : Avec le tableau t1 //

    STRING_ARRAY_TO_ENS(t1, ens1);
    TabMots* tab1 = TAB_arbre_en_tab(ens1);

    // l'arbre est sensé être déja trié lexicographiquement
    test_assert(STR_EQUALS(tab1->tab[0]->mot, "aaa"));
    test_assert(STR_EQUALS(tab1->tab[4]->mot, "zzz"));
    
    TAB_tri(tab1, TRI_LEXICO, false);
    test_assert(STR_EQUALS(tab1->tab[0]->mot, "zzz"));
    test_assert(STR_EQUALS(tab1->tab[4]->mot, "aaa"));
    
    TAB_tri(tab1, TRI_APPARITION, true);
    test_assert(STR_EQUALS(tab1->tab[0]->mot, "zzz"));
    test_assert(STR_EQUALS(tab1->tab[4]->mot, "pfff"));
    
    TAB_tri(tab1, TRI_NB_OCCURENCES, false);
    test_assert(STR_EQUALS(tab1->tab[0]->mot, "zzz"));
    test_assert(STR_EQUALS(tab1->tab[4]->mot, "pfff"));

    // Test 2 : Avec le texte sujet_test_ordre.txt //

    FILE* f1 = fopen("textes/sujet_test_ordre.txt", "r");
    test_assert(f1);

    ALG_compter_mots(ens2, f1);
    TabMots* tab2 = TAB_arbre_en_tab(ens2);
    fclose(f1);

    // l'arbre est sensé être déja trié lexicographiquement
    test_assert(STR_EQUALS(tab2->tab[0]->mot, "deux"));
    test_assert(STR_EQUALS(tab2->tab[1]->mot, "deuxdeux"));
    test_assert(STR_EQUALS(tab2->tab[3]->mot, "un"));
    
    TAB_tri(tab2, TRI_APPARITION, true);
    test_assert(STR_EQUALS(tab2->tab[0]->mot, "trois"));
    test_assert(STR_EQUALS(tab2->tab[3]->mot, "deuxdeux"));
    
    TAB_tri(tab2, TRI_NB_OCCURENCES, false);
    test_assert(STR_EQUALS(tab2->tab[0]->mot, "deux"));
    test_assert(STR_EQUALS(tab2->tab[3]->mot, "trois"));

    // Retrions lexicographiquement
    TAB_tri(tab2, TRI_LEXICO, true);
    test_assert(STR_EQUALS(tab2->tab[0]->mot, "deux"));
    test_assert(STR_EQUALS(tab2->tab[1]->mot, "deuxdeux"));
    test_assert(STR_EQUALS(tab2->tab[3]->mot, "un"));

    TAB_libere(tab1);
    TAB_libere(tab2);
    ABR_libere(ens1);
    ABR_libere(ens2);

    return 1;
}

static int test_ALG_compter_mots() {
    Mots* ens1 = ABR_initialiser(), *ens2 = ABR_initialiser();
    test_assert(ens1);
    
    FILE* f1 = fopen("textes/sujet_test_mot_suivant_x.txt", "r");
    FILE* f2 = fopen("textes/casse_doublons.txt", "r");
    test_assert(f1 && f2);

    ALG_compter_mots(ens1, f1);
    ALG_compter_mots(ens2, f2);
    fclose(f1);
    fclose(f2);

    test_assert(ABR_ASSERT_OCCURENCES(ens1, "ab", 8));
    test_assert(ABR_ASSERT_OCCURENCES(ens1, "aaa", 3));
    test_assert(ABR_ASSERT_OCCURENCES(ens1, "abc", 1));
    test_assert(ABR_ASSERT_OCCURENCES(ens1, "a", 3));
    test_assert(ABR_ASSERT_OCCURENCES(ens1, "b", 3));
    
    test_assert(ABR_ASSERT_OCCURENCES(ens2, "bonjour", 2));
    test_assert(ABR_ASSERT_OCCURENCES(ens2, "salut", 3));
    test_assert(ABR_ASSERT_OCCURENCES(ens2, "voila", 1));

    ABR_libere(ens1);
    ABR_libere(ens2);
    return 1;
}


static int test_ALG_mots_apres_x() {
    Mots* ens1 = ABR_initialiser();
    test_assert(ens1);
    
    FILE* f = fopen("textes/sujet_test_mot_suivant_x.txt", "r");
    test_assert(f);

    ALG_mots_apres_x(ens1, f, "ab");
    fclose(f);

    test_assert(ABR_ASSERT_OCCURENCES(ens1, "ab", 1));
    test_assert(ABR_ASSERT_OCCURENCES(ens1, "aaa", 3));
    test_assert(ABR_ASSERT_OCCURENCES(ens1, "a", 2));
    test_assert(ABR_ASSERT_OCCURENCES(ens1, "b", 2));
    
    test_assert(ABR_cherche_mot(ens1, "abc") == NULL);

    ABR_libere(ens1);
    return 1;
}


static int test_ALG_mots_avant_x() {
    Mots* ens1 = ABR_initialiser();
    test_assert(ens1);
    
    FILE* f = fopen("textes/sujet_test_mot_suivant_x.txt", "r");
    test_assert(f);

    ALG_mots_avant_x(ens1, f, "ab");
    fclose(f);

    test_assert(ABR_ASSERT_OCCURENCES(ens1, "a", 3));
    test_assert(ABR_ASSERT_OCCURENCES(ens1, "ab", 1));
    test_assert(ABR_ASSERT_OCCURENCES(ens1, "aaa", 2));
    test_assert(ABR_ASSERT_OCCURENCES(ens1, "b", 1));
    
    test_assert(ABR_cherche_mot(ens1, "abc") == NULL);

    ABR_libere(ens1);
    return 1;
}

static int test_ALG_expressions() {
    Mots* ens1 = ABR_initialiser();
    test_assert(ens1);
    
    FILE* f1 = fopen("textes/sujet_exprs.txt", "r");
    test_assert(f1);

    ALG_expressions(ens1, f1, 2);
    fclose(f1);

    // Vérification des occurences
    test_assert(ABR_ASSERT_OCCURENCES(ens1, "un un", 3));
    test_assert(ABR_ASSERT_OCCURENCES(ens1, "un deux", 1));
    test_assert(ABR_ASSERT_OCCURENCES(ens1, "deux trois", 2));
    test_assert(ABR_ASSERT_OCCURENCES(ens1, "trois trois", 1));
    test_assert(ABR_ASSERT_OCCURENCES(ens1, "trois un", 1));
    test_assert(ABR_ASSERT_OCCURENCES(ens1, "un trois", 1));
    test_assert(ABR_ASSERT_OCCURENCES(ens1, "trois deux", 1));
    
    TabMots* tab1 = TAB_arbre_en_tab(ens1);
    
    // Vérification de l'ordre d'apparition
    TAB_tri(tab1, TRI_APPARITION, true);
    test_assert(STR_EQUALS(tab1->tab[0]->mot, "un un"));
    test_assert(STR_EQUALS(tab1->tab[1]->mot, "un deux"));
    test_assert(STR_EQUALS(tab1->tab[2]->mot, "deux trois"));
    test_assert(STR_EQUALS(tab1->tab[3]->mot, "trois trois"));
    test_assert(STR_EQUALS(tab1->tab[4]->mot, "trois un"));
    test_assert(STR_EQUALS(tab1->tab[5]->mot, "un trois"));
    test_assert(STR_EQUALS(tab1->tab[6]->mot, "trois deux"));

    
    Mots* ens2 = ABR_initialiser();
    FILE* f2 = fopen("textes/brassens.txt", "r");
    test_assert(f2);

    ALG_expressions(ens2, f2, 3);
    fclose(f2);

    // Vérification des occurences
    test_assert(ABR_ASSERT_OCCURENCES(ens2, "de mon arbre", 2));
    test_assert(ABR_ASSERT_OCCURENCES(ens2, "heureux j aurais", 1));
    test_assert(ABR_ASSERT_OCCURENCES(ens2, "eloigner de mon", 1));

    TabMots* tab2 = TAB_arbre_en_tab(ens2);

    // Vérification de l'ordre d'apparition
    TAB_tri(tab2, TRI_APPARITION, true);
    test_assert(STR_EQUALS(tab2->tab[0]->mot, "aupres de mon"));
    test_assert(STR_EQUALS(tab2->tab[1]->mot, "de mon arbre"));
    test_assert(STR_EQUALS(tab2->tab[12]->mot, "eloigner de mon"));

    ABR_libere(ens1);
    ABR_libere(ens2);
    TAB_libere(tab1);
    TAB_libere(tab2);

    return 1;
}

static int test_ABR_initialiser() {
    Mots* ens = ABR_initialiser();
    test_assert(ens);
    
    test_assert(ens->racine == NULL);
    test_assert(ens->len == 0);
    test_assert(ens->max_str_len == 0);

    ABR_libere(ens);

    return 1;
}

static int test_ABR_alloue_noeud() {
    char* mot = "there is no node";
    MotEntry* entry = ABR_alloue_noeud(mot, 89);
    test_assert(entry);
    
    test_assert(STR_EQUALS(entry->mot, mot));
    test_assert(entry->mot != mot); // Le mot est copié
    
    test_assert(entry->fg == NULL);
    test_assert(entry->fd == NULL);
    test_assert(entry->apparition == 89);
    test_assert(entry->nb_occ == 1);

    ABR_libere_helper(&entry);

    return 1;
}

static int test_ABR_cherche_mot() {
    Mots* ens = ABR_initialiser();
    MotEntry* entry;
    test_assert(ens);
    
    // Recherche dans un arbre vide
    entry = ABR_cherche_mot(ens, t1[0]);
    test_assert(entry == NULL);

    // Recherche dans un arbre non vide
    ABR_ajouter_mot(ens, t1[0]);
    ABR_ajouter_mot(ens, t1[0]);
    ABR_ajouter_mot(ens, t1[1]);

    entry = ABR_cherche_mot(ens, t1[0]);
    test_assert(entry != NULL);
    test_assert(STR_EQUALS(entry->mot, t1[0]));

    // Recherche d'un mot NULL
    entry = ABR_cherche_mot(ens, NULL);
    test_assert(entry == NULL);

    ABR_libere(ens);

    return 1;
}

static int test_ABR_appartient() {
    Mots* ens = ABR_initialiser();
    test_assert(ens);

    // Recherche dans un arbre vide
    test_assert(ABR_appartient(ens, t1[0]) == false);

    // Recherche dans un arbre non vide
    ABR_ajouter_mot(ens, t1[0]);
    ABR_ajouter_mot(ens, t1[0]);
    ABR_ajouter_mot(ens, t1[1]);

    test_assert(ABR_appartient(ens, t1[0]) == true);

    // Recherche d'un mot inexistant
    test_assert(ABR_appartient(ens, "Bye!") == false);

    // Recherche d'un mot NULL
    test_assert(ABR_appartient(ens, NULL) == false);

    ABR_libere(ens);
    return 1;
}

static int test_ABR_ajouter_mot() {
    Mots* ens = ABR_initialiser();
    test_assert(ens);

    STRING_ARRAY_TO_ENS(t1, ens);
    test_assert(ABR_ASSERT_OCCURENCES(ens, "zzz", 5));
    test_assert(ABR_ASSERT_OCCURENCES(ens, "abc", 4));
    test_assert(ABR_ASSERT_OCCURENCES(ens, "aaa", 3));
    test_assert(ABR_ASSERT_OCCURENCES(ens, "caa", 2));
    test_assert(ABR_ASSERT_OCCURENCES(ens, "pfff", 1));

    test_assert(ens->max_str_len == 4);
    test_assert(ens->len == 5);

    // Ajout d'un mot NULL
    test_assert(ABR_ajouter_mot(ens, NULL) == -1);

    ABR_libere(ens);
    return 1;
}

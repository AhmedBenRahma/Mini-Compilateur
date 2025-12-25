/*
    Mini-projet de compilation

    Phases gérées :
      - Analyse lexicale   (scanner / nextToken)
      - Analyse syntaxique (descente récursive)
      - Génération de code pour une machine à pile

    Fichiers utilisés :
      - source.txt : programme source
      - code.txt   : code intermédiaire généré

    Auteur : [abidi eùna , ben rahma ahmed]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

/* Codes des tokens reconnus par l'analyseur lexical */

#define PROGRAM   1
#define VAR       2
#define BEGIN_    3
#define END_      4
#define IF        5
#define THEN      6
#define ELSE      7
#define WHILE     8
#define DO        9
#define READ      10
#define READLN    11
#define WRITE     12
#define WRITELN   13
#define INTEGER   14
#define CHAR      15
#define ID        16
#define NB        17
#define SEMI      18
#define COLON     19
#define COMMA     20
#define DOT       21
#define ASSIGN    22
#define LPAREN    23
#define RPAREN    24
#define OPREL     25    // opérateurs relationnels (=, <, >, <=, >=, <>)
#define OPADD     26    // +, -, ||
#define OPMUL     27    // *, /, %, &&
#define END_FILE  28    // fin de fichier

/* Variables globales utilisées par le compilateur */

int token;           // token courant
char lexeme[50];     // lexème courant
FILE *f, *fcode;     // f : source.txt, fcode : code.txt

// Table simple pour mémoriser les identificateurs déclarés
char table_id[200][50];
int nb_id = 0;

/* Gestion des identificateurs */

// Retourne 1 si l'identificateur existe déjà, sinon 0
int id_existe(char *nom) {
    for (int i = 0; i < nb_id; i++)
        if (strcmp(table_id[i], nom) == 0)
            return 1;
    return 0;
}

// Ajoute un identificateur dans la table s'il n'y est pas déjà
void ajouter_identificateur(char *nom) {
    if (!id_existe(nom)) {
        strcpy(table_id[nb_id], nom);
        nb_id++;
    }
}

/* Génération d'une ligne de code dans code.txt */

void gen(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(fcode, fmt, args);
    fprintf(fcode, "\n");
    va_end(args);
}

/* Lecture des commentaires de la forme (* ... *) */

void lire_commentaire() {
    int c, d;
    c = fgetc(f);
    d = fgetc(f);
    while (!(c == '*' && d == ')')) {
        c = d;
        d = fgetc(f);
        if (d == EOF) {
            printf("Erreur : commentaire non terminé\n");
            exit(1);
        }
    }
}

/* Analyse lexicale : nextToken lit le prochain token dans 'f' */

void nextToken() {
    int c, i = 0;

    // Ignorer espaces, tabulations et retours à la ligne
    do {
        c = fgetc(f);
    } while (c != EOF && (c==' ' || c=='\t' || c=='\n' || c=='\r'));

    // Gestion des commentaires (* ... *)
    if (c == '(') {
        int d = fgetc(f);
        if (d == '*') {
            lire_commentaire();   // consomme tout le commentaire
            nextToken();          // lit le token suivant
            return;
        }
        // pas un commentaire, on remet le 2e caractère
        ungetc(d, f);
    }

    if (c == EOF) { token = END_FILE; return; }

    // Mots-clés et identificateurs
    if (isalpha(c)) {
        lexeme[i++] = c;
        while ((c = fgetc(f)) != EOF && isalnum(c))
            lexeme[i++] = c;
        lexeme[i] = '\0';
        ungetc(c, f);

        // Mots-clés
        if      (!strcmp(lexeme,"program")) token=PROGRAM;
        else if (!strcmp(lexeme,"var"))     token=VAR;
        else if (!strcmp(lexeme,"begin"))   token=BEGIN_;
        else if (!strcmp(lexeme,"end"))     token=END_;
        else if (!strcmp(lexeme,"if"))      token=IF;
        else if (!strcmp(lexeme,"then"))    token=THEN;
        else if (!strcmp(lexeme,"else"))    token=ELSE;
        else if (!strcmp(lexeme,"while"))   token=WHILE;
        else if (!strcmp(lexeme,"do"))      token=DO;
        else if (!strcmp(lexeme,"read"))    token=READ;
        else if (!strcmp(lexeme,"readln"))  token=READLN;
        else if (!strcmp(lexeme,"write"))   token=WRITE;
        else if (!strcmp(lexeme,"writeln")) token=WRITELN;
        else if (!strcmp(lexeme,"integer")) token=INTEGER;
        else if (!strcmp(lexeme,"char"))    token=CHAR;
        else {
            // sinon c'est un identificateur (utilisation)
            token = ID;
            // On n'ajoute plus ici dans la table :
            // la déclaration se fait dans Dcl() / Liste_id()
        }
        return;
    }

    // Nombres entiers
    if (isdigit(c)) {
        lexeme[i++] = c;
        while ((c = fgetc(f)) != EOF && isdigit(c))
            lexeme[i++] = c;
        lexeme[i] = '\0';
        ungetc(c, f);
        token = NB;
        return;
    }

    // Opérateurs relationnels (<, <=, <>, >, >=, =)
    if (c=='<') {
        int d=fgetc(f);
        if      (d=='=') { strcpy(lexeme,"<="); token=OPREL; }
        else if (d=='>') { strcpy(lexeme,"<>"); token=OPREL; }
        else {
            strcpy(lexeme,"<"); token=OPREL;
            ungetc(d,f);
        }
        return;
    }

    if (c=='>') {
        int d=fgetc(f);
        if (d=='=') { strcpy(lexeme,">="); token=OPREL; }
        else {
            strcpy(lexeme,">"); token=OPREL;
            ungetc(d,f);
        }
        return;
    }

    if (c=='=') {
        strcpy(lexeme,"=");
        token=OPREL;
        return;
    }

    // Opérateurs logiques || et &&
    if (c=='|') {
        int d=fgetc(f);
        if (d=='|') { strcpy(lexeme,"||"); token=OPADD; }
        else {
            printf("Erreur lexicale : '|' seul interdit\n");
            exit(1);
        }
        return;
    }

    if (c=='&') {
        int d=fgetc(f);
        if (d=='&') { strcpy(lexeme,"&&"); token=OPMUL; }
        else {
            printf("Erreur lexicale : '&' seul interdit\n");
            exit(1);
        }
        return;
    }

    // Symboles simples et opérateurs arithmétiques
    switch(c) {
        case ';': token=SEMI;  strcpy(lexeme,";");  break;
        case ',': token=COMMA; strcpy(lexeme,",");  break;
        case '.': token=DOT;   strcpy(lexeme,".");  break;
        case '(': token=LPAREN;strcpy(lexeme,"(");  break;
        case ')': token=RPAREN;strcpy(lexeme,")");  break;
        case '+': token=OPADD; strcpy(lexeme,"+");  break;
        case '-': token=OPADD; strcpy(lexeme,"-");  break;
        case '*': token=OPMUL; strcpy(lexeme,"*");  break;
        case '/': token=OPMUL; strcpy(lexeme,"/");  break;
        case '%': token=OPMUL; strcpy(lexeme,"%");  break;

        case ':':
            c=fgetc(f);
            if (c=='=') { token=ASSIGN; strcpy(lexeme,":="); }
            else {
                token=COLON;
                strcpy(lexeme,":");
                ungetc(c,f);
            }
            break;

        default:
            printf("Erreur lexicale : caractère invalide '%c'\n", c);
            exit(1);
    }
}

/* Vérifie que le token courant est bien t, sinon erreur */

void match(int t) {
    if (token != t) {
        printf("Erreur syntaxique: attendu %d mais trouvé '%s'\n", t, lexeme);
        exit(1);
    }
    nextToken();
}

/* Gestion des labels pour IF / WHILE */

int label_id = 0;

// Génère un label du type L0, L1, L2, ...
char* newLabel() {
    static char buf[20];
    sprintf(buf,"L%d", label_id++);
    return buf;
}

/* Prototypes de fonctions de parsing */

void P();
void Dcl();
void Liste_id();
void Type();
void Inst_compo();
void Inst();
void I();
void Exp();
void Exp_simple();
void Terme();
void Facteur();

/*
    P() : analyse du programme complet

    Grammaire :
      P → program id ; Dcl Inst_compo .
*/

void P() {
    match(PROGRAM);   // 'program'
    match(ID);        // nom du programme
    match(SEMI);      // ';'
    Dcl();            // déclarations
    Inst_compo();     // bloc principal begin ... end
    match(DOT);       // '.'
}

/*
    Dcl() : déclarations de variables

    Grammaire :
      Dcl → var Liste_id : Type ; Dcl | ε
*/

void Dcl() {
    if (token == VAR) {
        match(VAR);
        Liste_id();
        match(COLON);
        Type();
        match(SEMI);
        Dcl();     // permet plusieurs blocs var
    }
}

/*
    Liste_id() : liste d'identificateurs

    Grammaire :
      Liste_id → id { , id }

    Ici, on considère que les identificateurs lus
    sont des variables déclarées : on les ajoute dans table_id.
*/

void Liste_id() {
    if (token != ID) {
        printf("Erreur syntaxique : identificateur attendu dans la declaration\n");
        exit(1);
    }

    // Premier identificateur
    ajouter_identificateur(lexeme);
    match(ID);

    // Identificateurs suivants séparés par des virgules
    while (token == COMMA) {
        match(COMMA);

        if (token != ID) {
            printf("Erreur syntaxique : identificateur attendu après ','\n");
            exit(1);
        }

        ajouter_identificateur(lexeme);
        match(ID);
    }
}

/*
    Type() : type de variable

    Grammaire :
      Type → integer | char
*/

void Type() {
    if (token == INTEGER) match(INTEGER);
    else if (token == CHAR) match(CHAR);
    else {
        printf("Type inconnu\n");
        exit(1);
    }
}

/*
    Inst_compo() : bloc d'instructions

    Grammaire :
      Inst_compo → begin Inst end
*/

void Inst_compo() {
    match(BEGIN_);
    Inst();
    match(END_);
}

/*
    Inst() : suite d'instructions

    Grammaire :
      Inst → I { ; I } | ε
*/

void Inst() {
    if (token == END_) return;  // pas d'instruction

    I();                        // première instruction

    while (token == SEMI) {
        match(SEMI);
        if (token == END_) return; // s'arrête si 'end' suit
        I();                       // instruction suivante
    }
}

/*
    Exp() :

    Grammaire :
      Exp → Exp_simple [ OPREL Exp_simple ]

    Gère les expressions relationnelles (comparaisons)
*/

void Exp() {
    Exp_simple();

    if (token == OPREL) {
        char op[3];
        strcpy(op, lexeme);    // sauvegarde l'opérateur
        match(OPREL);
        Exp_simple();          // partie droite

        // Génère le code de comparaison
        if      (!strcmp(op,"="))  gen("EQ");
        else if (!strcmp(op,"<>")) gen("NE");
        else if (!strcmp(op,"<"))  gen("LT");
        else if (!strcmp(op,"<=")) gen("LE");
        else if (!strcmp(op,">"))  gen("GT");
        else if (!strcmp(op,">=")) gen("GE");
    }
}

/*
    Exp_simple() :

    Grammaire :
      Exp_simple → Terme { OPADD Terme }

    OPADD = + | - | ||
*/

void Exp_simple() {
    Terme();
    while (token == OPADD) {
        char op[3];
        strcpy(op, lexeme);   // '+', '-', ou '||'
        match(OPADD);
        Terme();

        if      (!strcmp(op,"+"))  gen("ADD");
        else if (!strcmp(op,"-"))  gen("SUB");
        else if (!strcmp(op,"||")) gen("OR");
    }
}

/*
    Terme() :

    Grammaire :
      Terme → Facteur { OPMUL Facteur }

    OPMUL = *, /, %, &&
*/

void Terme() {
    Facteur();
    while (token == OPMUL) {
        char op[3];
        strcpy(op, lexeme);   // '*', '/', '%', ou '&&'
        match(OPMUL);
        Facteur();

        if      (!strcmp(op,"*"))  gen("MUL");
        else if (!strcmp(op,"/"))  gen("DIV");
        else if (!strcmp(op,"%"))  gen("MOD");
        else if (!strcmp(op,"&&")) gen("AND");
    }
}

/*
    Facteur() :

    Grammaire :
      Facteur → id | nb | ( Exp )

    Ici, on vérifie qu'un identificateur utilisé est bien déclaré.
*/

void Facteur() {
    if (token == ID) {
        char nom[50];
        strcpy(nom, lexeme);

        // Vérifier que la variable a été déclarée
        if (!id_existe(nom)) {
            printf("Erreur semantique : identificateur '%s' non declare\n", nom);
            exit(1);
        }

        match(ID);
        gen("LOAD %s", nom);  // charge la variable
    }
    else if (token == NB) {
        char val[50];
        strcpy(val, lexeme);
        match(NB);
        gen("PUSH %s", val);  // empile la constante
    }
    else if (token == LPAREN) {
        match(LPAREN);
        Exp();                // expression entre parenthèses
        match(RPAREN);
    }
    else {
        printf("Facteur invalide : %s\n", lexeme);
        exit(1);
    }
}

/*
    I() : une instruction

    Grammaire simplifiée :
      I → id := Exp
        | if Exp then I else I
        | while Exp do I
        | read(id) | readln(id)
        | write(id) | writeln(id)
*/

void I() {
    // Affectation : id := Exp
    if (token == ID) {
        char nom[50];
        strcpy(nom, lexeme);   // nom de la variable

        // Vérifier que la variable a été déclarée
        if (!id_existe(nom)) {
            printf("Erreur semantique : identificateur '%s' non declare (affectation)\n", nom);
            exit(1);
        }

        match(ID);
        match(ASSIGN);         // ':='
        Exp();                 // calcule l'expression
        gen("STORE %s", nom);  // stocke le résultat
    }

    // IF ... THEN ... ELSE ...
    else if (token == IF) {
        match(IF);
        Exp();                 // condition, résultat au sommet de pile
        char *Lfalse = newLabel();
        gen("JZ %s", Lfalse);  // si 0 → saute vers Lfalse

        match(THEN);
        I();                   // bloc THEN

        char *Lend = newLabel();
        gen("JMP %s", Lend);   // saute le ELSE

        gen("%s:", Lfalse);    // début du ELSE
        match(ELSE);
        I();                   // bloc ELSE

        gen("%s:", Lend);      // fin du IF
    }

    // WHILE Exp DO I
    else if (token == WHILE) {
        match(WHILE);
        char *Lstart = newLabel();
        char *Lend   = newLabel();

        gen("%s:", Lstart);    // début de boucle
        Exp();                 // condition
        gen("JZ %s", Lend);    // si 0 → sortie

        match(DO);
        I();                   // corps de la boucle

        gen("JMP %s", Lstart); // retour au début
        gen("%s:", Lend);      // sortie de boucle
    }

    // READ / READLN
    else if (token == READ || token == READLN) {
        int isLn = (token == READLN); // pour readln
        match(token);
        match(LPAREN);
        char var[50];
        strcpy(var, lexeme);

        // Vérifier que la variable a été déclarée
        if (!id_existe(var)) {
            printf("Erreur semantique : identificateur '%s' non declare (read/readln)\n", var);
            exit(1);
        }

        match(ID);
        match(RPAREN);
        gen("READ %s", var);
        if (isLn) gen("READLN");
    }

    // WRITE / WRITELN
    else if (token == WRITE || token == WRITELN) {
        int isLn = (token == WRITELN); // pour writeln
        match(token);
        match(LPAREN);
        char var[50];
        strcpy(var, lexeme);

        // Vérifier que la variable a été déclarée
        if (!id_existe(var)) {
            printf("Erreur semantique : identificateur '%s' non declare (write/writeln)\n", var);
            exit(1);
        }

        match(ID);
        match(RPAREN);
        gen("WRITE %s", var);
        if (isLn) gen("WRITELN");
    }

    // Instruction invalide
    else {
        printf("Instruction invalide : %s\n", lexeme);
        exit(1);
    }
}

/*
    main() :

    - ouvre source.txt en lecture
    - ouvre code.txt en écriture
    - lance l'analyse (lexicale + syntaxique)
    - affiche la table des identificateurs déclarés
*/

int main() {
    f = fopen("source.txt", "r");
    if (!f) {
        printf("Erreur ouverture source.txt\n");
        return 1;
    }

    fcode = fopen("code.txt", "w");
    if (!fcode) {
        printf("Erreur creation code.txt\n");
        fclose(f);
        return 1;
    }

    nextToken();   // premier token
    P();           // analyse du programme

    printf("\nAnalyse syntaxique terminée.\n");

    // Affiche les identificateurs déclarés
    printf("\n=== Identificateurs declares ===\n");
    for (int i = 0; i < nb_id; i++)
        printf("%d : %s\n", i+1, table_id[i]);

    fclose(f);
    fclose(fcode);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>


typedef struct  
{
    char nom[30];
    int valeur;
    char type_attaque[30];
    char description[200];
    int nb_tour_actif;
    int nb_tour_recharge;
    int recharge_act;
} Technique;

/*
Nom: 
Valeur: 
Type:
Description: 
Duree: 
Recharge:
*/

typedef struct
{
    Technique technique;
    int nb_tour_restant;
} Effet;


typedef struct
{
    char nom[30];
    int max_pv;
    int act_pv;
    int attaque;
    int defense;
    int agilite;
    int vitesse;
    int jauge;
    Technique* technique;
    int nb_technique;
    Effet* effet;
    int nb_effet;
} Fighter;

/*
Nom: 
PV: 
Attaque: 
Défense: 
Agilité: 
Vitesse: 
Techniques: 
*/

typedef struct
{
    char nom_equipe[30];
    Fighter* fighter;
    int nb_fighter;
} Equipe;


int compter_virgules(const char* ligne) {
    int count = 0;
    for (int i = 0; ligne[i] != '\0'; i++) {
        if (ligne[i] == ','){
            count++;
        };
    }
    return count;
}


Fighter lire_fighters(FILE* f){
    Fighter f1;
    char ligne[100];
    char ligne_tech[100];
    
    while (fgets(ligne, sizeof(ligne), f)) {
        if (ligne[0] != '\n' && ligne[0] != '\r') break;
    }
    if (feof(f)) return f1;

    sscanf(ligne, "Nom: %[^\n]", f1.nom);

    fgets(ligne, sizeof(ligne), f); 
    sscanf(ligne, "PV: %d", &f1.max_pv);
    f1.act_pv = f1.max_pv;
    fgets(ligne, sizeof(ligne), f); 
    sscanf(ligne, "Attaque: %d", &f1.attaque);
    
    fgets(ligne, sizeof(ligne), f); 
    sscanf(ligne, "Défense: %d", &f1.defense);

    fgets(ligne, sizeof(ligne), f); 
    sscanf(ligne, "Agilité: %d", &f1.agilite);

    fgets(ligne, sizeof(ligne), f); 
    sscanf(ligne, "Vitesse: %d", &f1.vitesse);

    f1.jauge = 0;

    f1.nb_effet = 0;
    f1.effet = NULL;

    fgets(ligne_tech, sizeof(ligne_tech), f); 

    char* techs_fighter = ligne_tech + 11;
    f1.nb_technique = compter_virgules(techs_fighter) + 1;

    f1.technique = malloc(f1.nb_technique * sizeof(Technique));
    if(NULL == f1.technique){
        printf("Malloc failed");
    }

    char* token = strtok(techs_fighter, ",\n");
    int i = 0;
    while (token && i < f1.nb_technique) {
        while (*token == ' '){
            token++;
        }
        strncpy(f1.technique[i].nom, token, sizeof(f1.technique[i].nom) - 1);
        f1.technique[i].nom[sizeof(f1.technique[i].nom) - 1] = '\0'; 
        i++;
        token = strtok(NULL, ",\n");
    }

    return f1;
}

void afficheFighter(Fighter f1){
    printf("Nom : %s | PV : %d | Attaque : %d | Defense : %d | Agilite : %d | Vitesse : %d | Techniques : %d\n", f1.nom, f1.max_pv, f1.attaque, f1.defense, f1.agilite, f1.vitesse, f1.nb_technique);
    for (int i = 0; i < f1.nb_technique; i++) {
        printf("  - %s\n", f1.technique[i].nom);
    }
    printf("\n");
}

Technique lire_techniques(FILE* f){
    Technique t1;
    char ligne[200];

    while (fgets(ligne, sizeof(ligne), f)) {
        if (ligne[0] != '\n' && ligne[0] != '\r') break;
    }
    if (feof(f)) return t1;

    sscanf(ligne, "Nom: %[^\n]", t1.nom);

    fgets(ligne, sizeof(ligne), f); 
    sscanf(ligne, "Valeur: %d", &t1.valeur);

    fgets(ligne, sizeof(ligne), f); 
    sscanf(ligne, "Type: %[^\n]", t1.type_attaque);

    fgets(ligne, sizeof(ligne), f); 
    sscanf(ligne, "Description: %[^\n]", t1.description);

    fgets(ligne, sizeof(ligne), f); 
    sscanf(ligne, "Duree: %d", &t1.nb_tour_actif);

    fgets(ligne, sizeof(ligne), f); 
    sscanf(ligne, "Recharge: %d", &t1.nb_tour_recharge);

    t1.recharge_act = 0;

    return t1;
}

void afficheTechnique(Technique t1){
    printf("Nom : %s\n", t1.nom);
    printf(" -Valeur : %d\n", t1.valeur);
    printf(" -Type : %s\n", t1.type_attaque);
    printf(" -Description : %s\n", t1.description);
    printf(" -Duree : %d\n", t1.nb_tour_actif);
    printf(" -Temps Recharge : %d\n", t1.nb_tour_recharge);
    printf("\n");
}

void afficheNomFighters(Fighter* list, int nb_fighter){
    printf("\n");
    for(int i = 0; i < nb_fighter; i++){
        printf("-Nom perso Numero %d : %s\n", i+1, list[i].nom);
    }
    printf("\n");
}

void appliquerEffet(Fighter* f1){
    for(int i = 0; i < f1->nb_effet; i++){
        if(f1->effet[i].technique.valeur > 0){
            printf("%s beneficie des effet de %s : +%d\n",f1->nom, f1->effet[i].technique.nom, f1->effet[i].technique.valeur);
        }
        else{
            printf("%s subit des effet de %s : %d\n",f1->nom, f1->effet[i].technique.nom, f1->effet[i].technique.valeur);
        }
        f1->act_pv += f1->effet[i].technique.valeur;
        if (f1->act_pv > f1->max_pv){
            f1->act_pv = f1->max_pv;
        }
        f1->effet[i].nb_tour_restant -= 1;

        if(f1->effet[i].nb_tour_restant == 0){
            printf("L'effet %s est fini", f1->effet[i].technique.nom);
            for(int j = i; j < f1->nb_effet; j++){
                f1->effet[j] = f1->effet[j + 1];
            }
            f1->nb_effet -= 1;
            
            Effet* tmp = realloc(f1->effet, f1->nb_effet * sizeof(Effet));
            if(!tmp && f1->nb_effet > 0){
                perror("Erreur realloc");
                exit(1);
                }
            f1->effet = tmp;
            i -= 1;
        }
    }
}

void ajouterEffet(Fighter* f1, Technique t1){
    f1->nb_effet += 1;
    Effet* tmp = realloc(f1->effet, f1->nb_effet * sizeof(Effet));
    if (!tmp) {
        perror("Erreur realloc");
        exit(1);
    }
    f1->effet = tmp;
    f1->effet[f1->nb_effet - 1 ].technique = t1;
    f1->effet[f1->nb_effet - 1 ].nb_tour_restant = t1.nb_tour_actif;
}


void combat_fighter(Fighter *joueur, Equipe *e_aliee, Equipe *adverse, int nb_adversaire) {
    if (joueur->act_pv <= 0) return;

    bool au_moins_une_dispo = false;
    for (int i = 0; i < joueur->nb_technique; i++) {
        if (joueur->technique[i].recharge_act == 0) {
            au_moins_une_dispo = true;
            break;
        }
    }

    if (!au_moins_une_dispo) {
        printf("%s n'a aucune technique disponible ce tour. Il passe son tour.\n", joueur->nom);
        return;
    }

    // Réduire la recharge des techniques
    for (int i = 0; i < joueur->nb_technique; i++) {
        if (joueur->technique[i].recharge_act > 0){
            joueur->technique[i].recharge_act--;
        }
    }

    // Choix de la technique
    int choix = -1;
    do {
        printf("Quelle technique utilisez-vous (1 à %d) ? ", joueur->nb_technique);
        scanf("%d", &choix);
        while (getchar() != '\n');

        if (choix < 1 || choix > joueur->nb_technique) {
            printf("Numéro invalide. Veuillez réessayer.\n");
            continue;
        }

        Technique *tech = &joueur->technique[choix - 1];

        if (tech->recharge_act > 0) {
            printf("Cette technique est encore en recharge (%d/%d tours). Choisissez-en une autre.\n",
                   tech->recharge_act, tech->nb_tour_recharge);
            choix = -1; // recommencer la boucle
        } 
        else {
            break; // sortie : technique valide
        }

    } while (1);

    Technique *tech = &joueur->technique[choix - 1];


    // Déterminer la cible selon si soin ou attaque
    Fighter *cible = NULL;
    int index_cible;

    if (tech->valeur > 0) {
        // Cible alliée
        printf("Cible à soigner :\n");
        for (int i = 0; i < e_aliee->nb_fighter; i++) {
            if (e_aliee->fighter[i].act_pv > 0)
                printf("[%d] %s (%d PV)\n", i + 1, e_aliee->fighter[i].nom, e_aliee->fighter[i].act_pv);
        }

        scanf("%d", &index_cible);
        while (getchar() != '\n');
        if (index_cible < 1 || index_cible > e_aliee->nb_fighter){
            return;
        }
        cible = &e_aliee->fighter[index_cible - 1];

        if (tech->nb_tour_actif > 0) {
            ajouterEffet(cible, *tech);
            printf(" L'effet %s est appliqué à %s.\n", tech->nom, cible->nom);
        } 
        else {
            cible->act_pv += tech->valeur;
            if (cible->act_pv > cible->max_pv)
                cible->act_pv = cible->max_pv;
            printf(" %s récupère %d PV.\n", cible->nom, tech->valeur);
        }
    } 
    else {
        // Cible ennemie
        printf("Cible à attaquer :\n");
        for (int i = 0; i < nb_adversaire; i++) {
            if (adverse->fighter[i].act_pv > 0)
                printf("[%d] %s (%d PV)\n", i + 1, adverse->fighter[i].nom, adverse->fighter[i].act_pv);
        }

        scanf("%d", &index_cible);
        while (getchar() != '\n');
        if (index_cible < 1 || index_cible > nb_adversaire){
            return;
        }
        cible = &adverse->fighter[index_cible - 1];

        if (tech->nb_tour_actif > 0) {
            ajouterEffet(cible, *tech);
            printf(" L'effet %s est appliqué à %s.\n", tech->nom, cible->nom);
        }

        int base_degats = joueur->attaque - cible->defense + (-tech->valeur);
        if (base_degats < 0){
            base_degats = 0;
        }

        bool cible_a_inflammable = false;
        for (int e = 0; e < cible->nb_effet; e++) {
            if (strcmp(cible->effet[e].technique.type_attaque, "Inflammable") == 0) {
            cible_a_inflammable = true;
            break;
        } 
    }

    // Application du bonus feu + inflammable
    int degats = base_degats;
    if (strcmp(tech->type_attaque, "Feu") == 0 && cible_a_inflammable) {
        degats *= 2;
        printf("SURPRISE : Feu + Inflammable ! Dégâts de feu doublés !\n");
    }

        cible->act_pv -= degats;
        if (cible->act_pv < 0){
            cible->act_pv = 0;
        }

        printf(" %s inflige %d dégâts à %s.\n", joueur->nom, degats, cible->nom);
    }

    // Réinitialisation de la recharge
    tech->recharge_act = tech->nb_tour_recharge;
}

void afficher_barre_vie(int actuel, int max) {
    int total = 20;
    int rempli = (actuel * total) / max;
    for (int i = 0; i < rempli; i++) printf(">");
    for (int i = rempli; i < total; i++) printf(" ");
}

void afficher_barre_jauge(int jauge) {
    int total = 20; 
    int rempli = (jauge * total) / 100;
    if (rempli > total){
        rempli = total;
    }
    for (int i = 0; i < rempli; i++) printf("#");
    for (int i = rempli; i < total; i++) printf(" ");
}


void afficher_techniques_speciales(Fighter f) {
    printf("\n%s - TECHNIQUES SPECIALES\n", f.nom);
    for (int i = 0; i < f.nb_technique; i++) {
        Technique* t = &f.technique[i];  // pointeur direct

        if (t->recharge_act > 0) {
            printf("[X] %s (Recharge: %d/%d tours) Non disponible\n", t->nom, t->recharge_act, t->nb_tour_recharge);
        } else {
            printf("[%d] %s", i + 1, t->nom);
            if (t->nb_tour_actif > 0) {
                printf(" (durée: %d tours)", t->nb_tour_actif);
            }
            printf("\n");
        }

        printf("  ⦿ %s\n", t->description);
    }
    printf("\n");
}

void afficher_interface_combat(Equipe e1, Equipe e2, int index_actif_e1, int index_actif_e2) {
    printf("\n╔═════════════════════ EQUIPE 1 : %-15s ═════════════════════╗\n", e1.nom_equipe);
    for (int i = 0; i < e1.nb_fighter; i++) {
        Fighter f = e1.fighter[i];
        printf("│ %-15s |%d| [", f.nom, i + 1);
        afficher_barre_vie(f.act_pv, f.max_pv);
        printf("] (%d/%d PV)\n", f.act_pv, f.max_pv);

        printf("│   Jauge :          [");
        afficher_barre_jauge(f.jauge);
        printf("] (%d%%)\n", f.jauge);

        if (f.nb_effet > 0) {
            printf("│   Type effet : ");
            for (int j = 0; j < f.nb_effet; j++) {
                printf("%s", f.effet[j].technique.type_attaque);
                if (j < f.nb_effet - 1) printf(", ");
            }
            printf("\n");
        }

        for (int j = 0; j < f.nb_effet; j++) {
            printf("│   Effet: %s (%d tours restants)\n", f.effet[j].technique.nom, f.effet[j].nb_tour_restant);
        }

        if (i == index_actif_e1) {
            printf("│ >>> C'est à %s de jouer !\n", f.nom);
        }
    }

    printf("╠═════════════════════ EQUIPE 2 : %-15s ═════════════════════╣\n", e2.nom_equipe);
    for (int i = 0; i < e2.nb_fighter; i++) {
        Fighter f = e2.fighter[i];
        printf("│ %-15s |%d| [", f.nom, i + 1);
        afficher_barre_vie(f.act_pv, f.max_pv);
        printf("] (%d/%d PV)\n", f.act_pv, f.max_pv);

        printf("│   Jauge :          [");
        afficher_barre_jauge(f.jauge);
        printf("] (%d%%)\n", f.jauge);

        if (f.nb_effet > 0) {
            printf("│   Type effet : ");
            for (int j = 0; j < f.nb_effet; j++) {
                printf("%s", f.effet[j].technique.type_attaque);
                if (j < f.nb_effet - 1) printf(", ");
            }
            printf("\n");
        }

        for (int j = 0; j < f.nb_effet; j++) {
            printf("│   Effet: %s (%d tours restants)\n", f.effet[j].technique.nom, f.effet[j].nb_tour_restant);
        }

        if (i == index_actif_e2) {
            printf("│ >>> C’est à %s de jouer !\n", f.nom);
        }
    }

    printf("╚════════════════════════════════════════════════════════════╝\n");

    // Affichage des techniques spéciales du joueur actif
    if (index_actif_e1 != -1) {
        afficher_techniques_speciales(e1.fighter[index_actif_e1]);
    } else if (index_actif_e2 != -1) {
        afficher_techniques_speciales(e2.fighter[index_actif_e2]);
    }
}


void clear_terminal(){
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}




int main(){

    SetConsoleOutputCP(CP_UTF8);
    
    FILE* f_fighter = fopen("fighters.txt", "r");
    if (!f_fighter) {
        perror("Erreur ouverture fichier");
        return 1;
    }

    //Le tableau dynamique 
    Fighter* list_fighter = NULL;
    int nb_fighter = 0;

    while (1) {
        Fighter f1 = lire_fighters(f_fighter);
        if (strlen(f1.nom) == 0 || feof(f_fighter)){
            break;
        }
    
        list_fighter = realloc(list_fighter, (nb_fighter + 1) * sizeof(Fighter));
        if (NULL == list_fighter){
            perror("Erreur realloc");
            exit(1);
        }
    
        list_fighter[nb_fighter] = f1;
        nb_fighter++;
    }
    
    fclose(f_fighter);

    FILE* f_tech = fopen("techniques.txt", "r");
    if (!f_tech) {
        perror("Erreur ouverture fichier");
        return 1;
    }

    Technique* list_technique = NULL;
    int nb_tech = 0;

    while (1) {
        Technique t1 = lire_techniques(f_tech);
        if (strlen(t1.nom) == 0 || feof(f_tech)){
            break;
        }
    
        list_technique = realloc(list_technique, (nb_tech + 1) * sizeof(Technique));
        if (NULL == list_technique){
            perror("Erreur realloc");
            exit(1);
        }
    
        list_technique[nb_tech] = t1;
        nb_tech++;
    }

    
    
    fclose(f_tech);

    //on va mettre les elements de technique dans fighter.technique
    for (int i = 0; i < nb_fighter; i++) {
        Fighter* f = &list_fighter[i];
        for (int j = 0; j < f->nb_technique; j++) {
            for (int k = 0; k < nb_tech; k++) {
                if (strcmp(f->technique[j].nom, list_technique[k].nom) == 0) {
                    f->technique[j] = list_technique[k]; 
                    f->technique[j].recharge_act = 0;  
                    break;
                }
            }
        }
    }


    
    char reponse;
    int reponse_fighter;
    int valeur_scan;
    char reponse_second;
    int valeur_scan_tech;
    int reponse_tech;

    afficheNomFighters(list_fighter, nb_fighter);
    printf("Voulez-vous les details d'un perso (Y/n) :");
    scanf("%c", &reponse);
    while(reponse == 'Y' || reponse == 'y'){
        printf("Lorque vous voudrez sortir du menu des details repondez avec un lettre \n");
        printf("Quel perso (entre 1 et %d) : ",nb_fighter);
        valeur_scan = scanf("%d",&reponse_fighter);
        if(valeur_scan != 1){
            break;
        }
        
        if(reponse_fighter >= 1 && reponse_fighter <= nb_fighter){
            afficheFighter(list_fighter[reponse_fighter-1]);
            printf("Voulez-vous les details d'une tech (Y/n) :");
            while (getchar() != '\n');
            scanf("%c", &reponse_second);
            while(reponse_second == 'Y' || reponse_second == 'y'){
                printf("\nLorque vous voudrez sortir du menu des details repondez avec un lettre \n");
                printf("Quelle technique (entre 1 et %d) : ",list_fighter[reponse_fighter-1].nb_technique);
                valeur_scan_tech = scanf("%d",&reponse_tech);
                if(valeur_scan_tech != 1){
                    reponse_second = 'n';
                }
                if(reponse_tech >= 1 && reponse_tech <= list_fighter[reponse_fighter-1].nb_technique){
                    afficheTechnique(list_fighter[reponse_fighter-1].technique[reponse_tech-1]);
                    printf("Voir une autre technique ? (Y/n) : ");
                    while (getchar() != '\n');
                    scanf("%c", &reponse_second);
                }
                else{
                    break;
                }
            }
        }
        else{
            break;
        }
    }
    

    Equipe e1;
    printf("\nJoueur 1 : Quelle est le nom de votre equipe : ");
    while (getchar() != '\n');
    scanf("%[^\n]",e1.nom_equipe);

    if(strlen(e1.nom_equipe) == 0){
        strcpy(e1.nom_equipe, "Equipe 1");
    }
    printf("Voici le nom de l'equipe du J1 : %s\n",e1.nom_equipe);

    Equipe e2;
    printf("Joueur 2 : Quelle est le nom de votre equipe : ");
    while (getchar() != '\n');
    scanf("%[^\n]",e2.nom_equipe);

    if(strlen(e2.nom_equipe) == 0){
        strcpy(e2.nom_equipe, "Equipe 2");
    }
    printf("Voici le nom de l'equipe du J2 : %s\n",e2.nom_equipe);

    // On réinitialise la recharge de toutes les techniques des équipes
    for (int i = 0; i < e1.nb_fighter; i++) {
        for (int j = 0; j < e1.fighter[i].nb_technique; j++) {
            e1.fighter[i].technique[j].recharge_act = 0;
        }
    }
    for (int i = 0; i < e2.nb_fighter; i++) {
        for (int j = 0; j < e2.fighter[i].nb_technique; j++) {
            e2.fighter[i].technique[j].recharge_act = 0;
        }
    }

    e1.nb_fighter = 0;
    e1.fighter = malloc(3 * sizeof(Fighter));
    if (!e1.fighter) {
        perror("Erreur malloc e1");
        exit(1);
    }

    e2.nb_fighter = 0;
    e2.fighter = malloc(3 * sizeof(Fighter));
    if (!e2.fighter) {
        perror("Erreur malloc e2");
        exit(1);
    }

    bool stop1, stop2;
    stop1 = false;
    stop2 = false;
    int choix_perso1, choix_perso2;
    char choix_arret1, choix_arret2;
    while ((e1.nb_fighter < 3 && !stop1) || (e2.nb_fighter < 3 && !stop2)){
        if (e1.nb_fighter < 3 && !stop1) {
            int deja_choisi1;
            do{
                deja_choisi1 = 0;
                printf("J1 quel fighter veut tu ajouter a %s : ", e1.nom_equipe);
                scanf("%d", &choix_perso1);
                while (getchar() != '\n');

                for(int i = 0; i < e1.nb_fighter; i++){
                    if(strcmp(e1.fighter[i].nom, list_fighter[choix_perso1 - 1].nom) == 0){
                        deja_choisi1 = 1;
                        printf("Ce perso est deja choisi choisissez un autre\n");
                        break;
                    }
                }
            } while (deja_choisi1);
            
            e1.fighter[e1.nb_fighter] = list_fighter[choix_perso1 - 1];
            e1.nb_fighter += 1;
            printf("Tu viens d'ajouter : %s\n",list_fighter[choix_perso1 - 1].nom);
            printf("J1 voulez vous vous arretez ici (Y/n) : ");
            scanf("%c", &choix_arret1);
            while (getchar() != '\n');
            if (choix_arret1 == 'Y' || choix_arret1 == 'y') {
                stop1 = true;
            }
        }

        if (e2.nb_fighter < 3 && !stop2) {
            int deja_choisi2;
            do
            {
                deja_choisi2 = 0;
                printf("J2 quel fighter veut tu ajouter a %s : ", e2.nom_equipe);
                scanf("%d", &choix_perso2);
                while (getchar() != '\n');

                for(int i = 0; i < e2.nb_fighter; i++){
                    if(strcmp(e2.fighter[i].nom, list_fighter[choix_perso2 - 1].nom) == 0){
                        deja_choisi2 = 1;
                        printf("Ce perso est deja choisi choisissez un autre\n");
                        break;
                    }
                }

            } while (deja_choisi2);
            
            e2.fighter[e2.nb_fighter] = list_fighter[choix_perso2 - 1];
            e2.nb_fighter += 1;
            printf("Tu viens d'ajouter : %s\n",list_fighter[choix_perso2 - 1].nom);
            printf("J2 voulez vous vous arretez ici (Y/n) : ");
            scanf("%c", &choix_arret2);
            while (getchar() != '\n');
            if (choix_arret2 == 'Y' || choix_arret2 == 'y') {
                stop2 = true;
            }
        }
    }
    printf("Voici les perso de l'equipe %s :\n", e1.nom_equipe);
    for(int l = 0; l < e1.nb_fighter; l++){
        printf("-%s\n",e1.fighter[l].nom);
    }

    printf("Voici les perso de l'equipe %s :\n", e2.nom_equipe);
    for(int k = 0; k < e2.nb_fighter; k++){
        printf("-%s\n",e2.fighter[k].nom);
    }



    
    printf("Que le combat commence !");
    int combat_termine = 0;
    while(!combat_termine){

        int vivants1 = 0, vivants2 = 0;
        for (int i = 0; i < e1.nb_fighter; i++) {
            if (e1.fighter[i].act_pv > 0) vivants1++;
        }
        for (int i = 0; i < e2.nb_fighter; i++) {
            if (e2.fighter[i].act_pv > 0) vivants2++;
        }
        if (vivants1 == 0 || vivants2 == 0) {
            combat_termine = 1;
            if (vivants1 == 0 && vivants2 == 0){
                printf("\nMatch nul !\n");
            }                
            else if (vivants1 == 0){
                printf("\n%s a gagné !\n", e2.nom_equipe);
            }   
            else{
                printf("\n%s a gagné !\n", e1.nom_equipe);
            }
            break;
            return 0;
        }

        for(int i = 0; i < e1.nb_fighter; i++){
            int vivants1 = 0;
            for (int i = 0; i < e1.nb_fighter; i++) {
                if (e1.fighter[i].act_pv > 0)
                    vivants1++;
            }
            if (vivants1 == 0) {
                printf("\n%s a gagné !\n", e2.nom_equipe);
                combat_termine = 1;
                break;
            }
            if(e1.fighter[i].act_pv <= 0)
            continue;

            e1.fighter[i].jauge += e1.fighter[i].vitesse;

            if(e1.fighter[i].jauge > 100){
                clear_terminal();
                afficher_interface_combat(e1, e2, i, -1);
                combat_fighter(&e1.fighter[i], &e1, &e2, e2.nb_fighter);
                appliquerEffet(&e1.fighter[i]);
                e1.fighter[i].jauge -= 100;
            }
        }

        for(int j = 0; j < e2.nb_fighter; j++){
            int vivants2 = 0;
            for (int i = 0; i < e2.nb_fighter; i++) {
                if (e2.fighter[i].act_pv > 0)
                    vivants2++;
            }
            if (vivants2 == 0) {
                printf("\n%s a gagné !\n", e1.nom_equipe);
                combat_termine = 1;
                break;
            }
            if(e2.fighter[j].act_pv <= 0)
            continue;

            e2.fighter[j].jauge += e2.fighter[j].vitesse;

            if(e2.fighter[j].jauge > 100){
                clear_terminal();
                afficher_interface_combat(e1, e2, -1, j);
                combat_fighter(&e2.fighter[j], &e2, &e1, e1.nb_fighter);
                appliquerEffet(&e2.fighter[j]);
                e2.fighter[j].jauge -= 100;
            }
        }


    }
    

    
    for (int i = 0; i < nb_fighter; i++) {
        free(list_fighter[i].technique);
        free(list_fighter[i].effet);
    }
    free(list_fighter);

    free(e1.fighter);
    free(e2.fighter);

    printf("\nAppuyez sur Entree pour quitter...");
    getchar();
    

    return 0;
}

//Compil
//gcc -o exe_shadow shadow_fighter.c
//gcc -Wall -Wextra -o exe_shadow shadow_fighter.c
//.\exe_shadow

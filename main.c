#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <conio2.h>
#include <time.h>
#include<unistd.h>
#include<windows.h>

typedef struct Date
{
    int jour;
    int mois;
    int annee;
} Date;

typedef struct etablissement
{
  int code;
  char noun[100]; //le nom de l'établissement
  char region[10]; // sa région (Est-Centre-Ouest)
} etablissement;

typedef struct Tenreg
{
    int matricule;
    char Nom[30];
    char Prenom[30];
    Date date_naissance;
    char wilaya_naissance[30];
    int sexe;
    char groupe_sanguin[3];
    Date date_recrutement;
    char Grade[30];
    char specialite[30];
    char dernier_diplome[30];
    etablissement etablissement_univ;
}Tenreg ;

typedef struct Tbloc
{
    Tenreg T[1000];
    int nb_enreg_inseres;
} Tbloc ;

typedef struct Entete
 {
     int nb_blocs;/* nombre de blocs dans le fichier = numero du dernier bloc */
     int nb_enreg_inseres;
     int position_libre;
 } Entete;

typedef struct Fichier /* Comme l'entête n'a pas la même structure que Tenreg, donc elle doit être seule au début du fichier */
{  FILE *f;
   Entete entete;
} Fichier;

/* ----------------------------------------- MACHINES ABSTRAITES --------------------------------------------- */

/*-------------- Ouverture d'un fichier --------------*/

int ouvrir(Fichier* fichier,char nomfichier[30],char c,FILE* *f) // c : le mode d'ouverture du fichier, soit 'A' (ancien) soit 'N' (nouveau)
{
      if((c == 'A') || (c == 'a'))
      { *f = fopen(nomfichier, "rb+");
       fread(&((*fichier).entete), sizeof(Entete), 1, *f);
      }//ouverture en lecture/écriture
      else if ((c == 'N') || (c == 'n'))
      {
          *f = fopen(nomfichier, "wb+");
          (*fichier).entete.nb_blocs= 0;
          (*fichier).entete.position_libre=0;
          fwrite(&((*fichier).entete),sizeof(Entete),1,*f);
      }//création d'un nouveau fichier en lecture/écriture
      else return 0;
      if(*f == NULL) return 0;
      return 1;
}

/*-------------- Fermeture d'un fichier --------------*/

void fermer (FILE* f,Fichier *fichier)
{
    fseek(f,0,SEEK_SET);
    fwrite(&((*fichier).entete),sizeof(Entete),1,f);
    fclose(f);
    free(f);
}

/*------------ Modification de l'entête-------------*/

void aff_entete(Fichier *f, int i, int val)
{
    switch (i)
    {
    case 1:
        (*f).entete.nb_blocs=val;
        break;
    case 2 :
        (*f).entete.nb_enreg_inseres=val;
        break;
    case 3:
        (*f).entete.position_libre=val;
        break;
    }
}

/*---------------- Lecture de l'entête---------------*/

int entete (Fichier *f, int i)
{
    switch (i)
    {
    case 1 :
        return (*f).entete.nb_blocs;
        break;
    case 2 :
        return (*f).entete.nb_enreg_inseres;
        break;
    case 3 :
        return (*f).entete.position_libre;
        break;
    }
}

/*----------------- Lecture d'un bloc----------------*/

void lireDir(FILE* f,int N_Bloc,Tbloc *buffer)
{
         fseek(f, sizeof(Entete) + (N_Bloc-1) * sizeof(Tbloc),SEEK_SET);
         fread(&(*buffer),sizeof(Tbloc),1, f);
}


/*----------------- Ecriture d'un bloc----------------*/

void ecrireDir(FILE* f,int N_Bloc,Tbloc *buffer)
{
     fseek(f,sizeof(Entete)+(N_Bloc-1)*sizeof(Tbloc),SEEK_SET);
     fwrite(&(*buffer),sizeof(Tbloc),1,f);

}

/*------------- Allocation d'un nouveau bloc ---------*/
int alloc_bloc(Fichier fichier) // au faite on n'aura pas besoin de ce module car je fais aff_entete et j'incrémente le i dans l'algorithme
 {
      aff_entete(&fichier,1,entete(&fichier,1)+1);
      return entete(&fichier,1);
}


void chargement_initial (int N, int b) // N: le nombre d'enregistrements qu'on veut charger ; b: la taille maximale du bloc
{
    Tbloc Buf; char tab[26];

    int a =0;
    while (a<26) //tableau de l'alphabet pour génerer les noms/prénoms
    {
        for (char c='a';c<='z';c++)
           {
            tab[a]=c;
            a++;
           };
    };

    typedef struct wilaya
    {
        char name[40];
    }wilaya;

    wilaya w[48];   // Tableau des 48 Wilayas ordonnées
    FILE* g=fopen("wilaya.txt","r");
    if (g !=NULL)
    {
        int l=0;
        while (!feof(g))
        {
            fscanf(g,"%s",&w[l].name);
            l++;
        }
        fclose(g);
    }

    typedef struct gr_sang  //le groupe sanguin
    { char sang[10];
    }gr_sang;

    gr_sang sanguin[8];  // Tableau des groupes sanguins
    strcpy(sanguin[0].sang,"O+");
    strcpy(sanguin[1].sang,"A+");
    strcpy(sanguin[2].sang,"B+");
    strcpy(sanguin[3].sang,"O-");
    strcpy(sanguin[4].sang,"A-");
    strcpy(sanguin[5].sang,"AB+");
    strcpy(sanguin[6].sang,"B-");
    strcpy(sanguin[7].sang,"AB-");

    typedef struct gr_ade   // le grade
    { char gr[40];
    }gr_ade;

    gr_ade grade[6];  // Tableau de grade des enseignants
    strcpy(grade[0].gr,"Professeur(e)");
    strcpy(grade[1].gr,"Maitre de conferences Rang A");
    strcpy(grade[2].gr,"Maitre de conferences Rang B");
    strcpy(grade[3].gr,"Maitre assistant(e) Rang A");
    strcpy(grade[4].gr,"Maitre assistant(e) Rang B");
    strcpy(grade[5].gr,"Assistant(e)");

    typedef struct speciality // la spécialité
    {
      char specialism[30];
    }speciality;

    speciality s[20]; // Tableau des différentes spécialités
    FILE* h=fopen("specialite.txt","r");
    if (h!=NULL)
    {
        int l=0;
        while(!feof(h))
        {
            fscanf(h,"%s",&s[l].specialism);
            l++;
        }
        fclose(h);
    };

    typedef struct der_dip // dernier diplôme
    {
        char t[40];
    }der_dip;

    der_dip tableau[5]; // Tableau des derniers diplômes
    strcpy(tableau[0].t,"Doctorat d'etat");
    strcpy(tableau[1].t,"Doctorat en sciences");
    strcpy(tableau[2].t,"PhD");
    strcpy(tableau[3].t,"HDR");
    strcpy(tableau[4].t,"Magistère");

    etablissement etab[108];// Tableau des établissements existant en Algérie, on a trouvé 107 établissements et pas 106
    FILE* p=fopen("etablissement1.txt","r");
    if (p!=NULL)
    {
        int a=0;
        while((a<108))
            {
               fscanf(p,"%d %s %s",&etab[a].code,&etab[a].noun,&etab[a].region);
               a++;
               if(a==100)
               {
                   p=fopen("etablissement2.txt","r");
                   if (p!=NULL)
                   {
                       while (a<108)
                       {
                           fscanf(p,"%d %s %s",&etab[a].code,&etab[a].noun,&etab[a].region);
                           a++;
                       }
                       fclose(p);
                   }
               };
            };
            fclose(p);
    }
    FILE* f;
    Fichier fi;
    ouvrir(&fi,"ENSEIGNANT-MESRS.dat",'n',&f);
    int j=0; int i=1;
    for (int k=1;k<=N;k++)
    {
        if (j<b) // les indices du tableau commencent àa partir de 0 jusqu'à b-1, ie, il y a b éléments dans le tableau
        {
            /* ---------- Génération du matricule -------- */
            Buf.T[j].matricule= (rand()%888889)+111111; // random génère un nombre entre [0,888889[ donc on rajoute 1 à (999999-111111=888888) pourque 999999 soit inclus

            /* ---------- Génération du Nom/Prénom -------- */
            for (int m=0;m<=rand()%27 +4;m++) //on genère un nombre entre 4 et 30 (ce nombre étant le nb de char pour le nom/prenom) et pour chaque
            {                                 //caractère on genère un nombre entre 1 et 26 et on met dans ce char la lettre qui correspond*/
               Buf.T[j].Nom[m]=tab[rand()%26];
            };
            for (int n=0;n<=rand()%27 +4;n++)
               {
                   Buf.T[j].Prenom[n]=tab[rand()%26];
               };

            /* ---------- Génération de la date de naissance-------- */
            Buf.T[j].date_naissance.annee= (rand()%40)+1953 ;
            Buf.T[j].date_naissance.mois= (rand()%12)+1 ;

            int Mois=Buf.T[j].date_naissance.mois;
            if ((Mois==1) ||(Mois==3) || (Mois==5)|| (Mois==7) || (Mois==8) || (Mois==10) || (Mois==12))
            {
                Buf.T[j].date_naissance.jour=(rand()%31)+1;
            };
            if ((Mois==4)|| (Mois==6)|| (Mois==9)||(Mois==11))
            {
                Buf.T[j].date_naissance.jour=(rand()%30)+1;
            };
            if (Mois==2) // cas particulier pour le mois de Février (selon les années :bissextiles ou non-bissextiles
            {
                int annee=Buf.T[j].date_naissance.annee;
                if ((annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0)) // Si l'année est bissextile
                    {
                        Buf.T[j].date_naissance.jour=(rand()%29)+1; // Dans ce cas on aura un jour en plus qui le 29 février
                    }
                    else // l'année n'est pas bissextile
                        Buf.T[j].date_naissance.jour=(rand()%28)+1; // Dans ce cas on aura 28 jours pour le mois de Févrirer
            };

            /* ---------- Génération de la Wilaya de naissance-------- */
            strcpy(Buf.T[j].wilaya_naissance,w[rand()%48].name);

            /* ---------- Génération du sexe-------- */
            Buf.T[j].sexe=rand()%2 +1;

            /* ---------- Génération du groupe sanguin ------- */
            strcpy(Buf.T[j].groupe_sanguin,sanguin[rand()%7+1].sang);

            /* ---------- Génération de la date de recrutement ------- */
            Buf.T[j].date_recrutement.annee= (rand()%40)+1979 ;
            Buf.T[j].date_recrutement.mois= (rand()%12)+1 ;
            int mois=Buf.T[j].date_recrutement.mois;
            if ((mois==1) ||(mois==3) || (mois==5)|| (mois==7) || (mois==8) || (mois==10) || (mois==12))
            {
                Buf.T[j].date_recrutement.jour=(rand()%31)+1; // les mois ayant 31 comme dernier jour
            };
            if ((mois==4)|| (mois==6)|| (mois==9)||(mois==11)) // les mois ayant 30 comme dernier jour
            {
                Buf.T[j].date_recrutement.jour=(rand()%30)+1;
            };
            if (mois==2) // cas particulier pour le mois de Février (selon les années :bissextiles ou non-bissextiles)
            {
                int annee=Buf.T[j].date_recrutement.annee;
                if ((annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0)) // Si l'année est bissextile
                    {
                        Buf.T[j].date_recrutement.jour=(rand()%29)+1;
                    }
                    else // l'année n'est pas bissextile
                        Buf.T[j].date_recrutement.jour=(rand()%28)+1;
            };

            /* ---------- Génération du grade -------- */
            strcpy(Buf.T[j].Grade,grade[rand()%6+1].gr);

            /* ---------- Génération de spécialité -------- */
            strcpy(Buf.T[j].specialite,s[rand()%5+1].specialism);

            /* ---------- Génération du dernier diplôme -------- */
            strcpy(Buf.T[j].dernier_diplome,tableau[rand()%5].t);

            /* ---------- Génération de l'établissement univ -------- */
            int c=rand()%108;
            strcpy(Buf.T[j].etablissement_univ.noun,etab[c].noun); // on doit garder le même indice de l'établissement choisi pour récuperer sa région correspondnate
            strcpy(Buf.T[j].etablissement_univ.region,etab[c].region);
            Buf.T[j].etablissement_univ.code=c;


            j++;
            }
            else // j>=b, si j=b (j=1000 pour notre cas), 'élément T[1000] n'existe pas dans le tableau car les indices varient de 0 jusqu'à 999
            {

              Buf.nb_enreg_inseres=j ;//On rentre dans ce sinon avec j=b (j=1000) dans le nb_enreg_inseres dans le bloc est 1000
              ecrireDir(f,i,&Buf);

              /* ---------- Génération du matricule -------- */
            Buf.T[0].matricule= (rand()% 888889)+111111;

            /* ---------- Génération du Nom/Prénom -------- */
            for (int m=0;m<=rand()%27 +4;m++) //on genère un nombre entre 4 et 30 (ce nombre étant le nb de char pour le nom/prenom) et pour chaque
            {                                 //caractère on genère un nombre entre 1 et 26 et on met dans ce char la lettre qui correspond*/
               Buf.T[0].Nom[m]=tab[rand()%26];
            };
            for (int n=0;n<=rand()%27 +4;n++)
               {
                   Buf.T[0].Prenom[n]=tab[rand()%26];
               };

            /* ---------- Génération de la date de naissance-------- */
            Buf.T[0].date_naissance.annee= (rand()%40)+1953 ;
            Buf.T[0].date_naissance.mois= (rand()%12)+1 ;

            int Mois=Buf.T[0].date_naissance.mois;
            if ((Mois==1) ||(Mois==3) || (Mois==5)|| (Mois==7) || (Mois==8) || (Mois==10) || (Mois==12))
            {
                Buf.T[0].date_naissance.jour=(rand()%31)+1;
            };
            if ((Mois==4)|| (Mois==6)|| (Mois==9)||(Mois==11))
            {
                Buf.T[0].date_naissance.jour=(rand()%30)+1;
            };
            if (Mois==2) // cas particulier pour le mois de Février (selon les années :bissextiles ou non-bissextiles
            {
                int annee=Buf.T[0].date_naissance.annee;
                if ((annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0)) // Si l'année est bissextile
                    {
                        Buf.T[0].date_naissance.jour=(rand()%29)+1; // Dans ce cas on aura un jour en plus qui le 29 février
                    }
                    else // l'année n'est pas bissextile
                        Buf.T[0].date_naissance.jour=(rand()%28)+1; // Dans ce cas on aura 28 jours pour le mois de Févrirer
            };

            /* ---------- Génération de la Wilaya de naissance-------- */
            strcpy(Buf.T[0].wilaya_naissance,w[rand()%48].name);

            /* ---------- Génération du sexe-------- */
            Buf.T[0].sexe=rand()%2+1;

            /* ---------- Génération du groupe sanguin ------- */
            strcpy(Buf.T[0].groupe_sanguin,sanguin[rand()%8].sang);

            /* ---------- Génération de la date de recrutement ------- */
            Buf.T[0].date_recrutement.annee= (rand()%40)+1979 ;
            Buf.T[0].date_recrutement.mois= (rand()%12)+1 ;
            int mois=Buf.T[0].date_recrutement.mois;
            if ((mois==1) ||(mois==3) || (mois==5)|| (mois==7) || (mois==8) || (mois==10) || (mois==12))
            {
                Buf.T[0].date_recrutement.jour=(rand()%31)+1; // les mois ayant 31 comme dernier jour
            };
            if ((mois==4)|| (mois==6)|| (mois==9)||(mois==11)) // les mois ayant 30 comme dernier jour
            {
                Buf.T[0].date_recrutement.jour=(rand()%30)+1;
            };
            if (mois==2) // cas particulier pour le mois de Février (selon les années :bissextiles ou non-bissextiles)
            {
                int annee=Buf.T[0].date_recrutement.annee;
                if ((annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0)) // Si l'année est bissextile
                    {
                        Buf.T[0].date_recrutement.jour=(rand()%29)+1;
                    }
                    else // l'année n'est pas bissextile
                        Buf.T[0].date_recrutement.jour=(rand()%28)+1;
            };

            /* ---------- Génération du grade -------- */
            strcpy(Buf.T[0].Grade,grade[rand()%6].gr);

            /* ---------- Génération de spécialité -------- */
            strcpy(Buf.T[0].specialite,s[rand()%5].specialism);

            /* ---------- Génération du dernier diplôme -------- */
            strcpy(Buf.T[0].dernier_diplome,tableau[rand()%5].t);

            /* ---------- Génération de l'établissement univ -------- */
            int c=rand()%108;
            strcpy(Buf.T[0].etablissement_univ.noun,etab[c].noun); // on doit garder le même indice de l'établissement choisi pour récuperer sa bonne région
            strcpy(Buf.T[0].etablissement_univ.region,etab[c].region);
            Buf.T[j].etablissement_univ.code=c;


            j=1;
            i++;
            };
    };
    Buf.nb_enreg_inseres=j;
    ecrireDir(f,i,&Buf);
    aff_entete(&fi,1,i);
    aff_entete(&fi,2,N);
    aff_entete(&fi,3,Buf.nb_enreg_inseres);//nb_enreg_inseres du dernier bloc est l'indice de la 1ere position libre
    fermer(f,&fi);
}

            /* ---------- Epuration ---------- */
/*Pour l'épuration, on a choisit une méthode qui est la plus optimale (en nombre d'accés à la mémoire secondaire,le temps d'acces..etc) et qui est
divisée en 4 étapes:
    1- Remplissage de la table d'index : on remplit la table d'index avec tous les matricules (index dense) en parcourant tout le fichier bloc par bloc.
    2- Tri par bulles: on trie la table d'index (en ordre croissant).
    3- Suppression des doublons du fichier.
    4- Suppression des doublons de la table d'index (par décalage).*/
typedef struct tableau // tableau ; va contenir tous les matricules avec les numeros de blocs (i) pour chaque matricule et sa position (j)
{
    int matricule;
    int i;
    int j;
}tableau;

typedef struct maillon *ptr; /*cette structure et ces machines abstraites seront utilisées pour la prochaine question et celàa afin d'éviter de redéclarer
                              des structures presque identiques sauf qu'on va pas utiliser le champs "code" pour cette question, on a juste besoin du i et du j.*/
struct maillon{
int code;
int i;
int j;
ptr suiv;
};

ptr Allouer(ptr *p)
{
    *p = (struct maillon *) malloc( sizeof( struct maillon))   ;
    (*p)->suiv = NULL;
}

void aff_val(ptr p,int i,int val)
{
    switch (i)
    {
    case 1:
        p->code=val;
        break;
    case 2:
        p->i=val;
        break;
    case 3:
        p->j=val;
        break;
    }
}

void aff_adr(ptr p,ptr q)
{
    p->suiv=q;
}

ptr suivant( ptr p)
{ return( p->suiv); }

int valeur(ptr p, int i)
{
    switch (i)
    {
    case 1:
        return (p->code);
        break;
    case 2:
        return (p->i);
        break;
    case 3:
        return (p->j);
        break;
    }
}

void liberer(ptr p)
{
    free(p);
}

typedef struct branche
{
    char speciality[30];
    ptr tete;
}branche;

void swap (tableau T[],int a, int b) // swap two elements of the array
{
    tableau temp;
    temp.i=T[a].i;
    temp.j=T[a].j;
    temp.matricule=T[a].matricule;
    T[a].i=T[b].i;
    T[a].j=T[b].j;
    T[a].matricule=T[b].matricule;
    T[b].i=temp.i;
    T[b].j=temp.j;
    T[b].matricule=temp.matricule;

}
void tri_bulles(tableau T[],int N) // Pour trier le tableau des matricules en ordre croisssant
{
    int tab_en_ordre = 0;
    while(tab_en_ordre!=1)
    {
        tab_en_ordre = 1;
        for(int k=0 ; k < N-1 ; k++)
        {
            if(T[k].matricule > T[k+1].matricule)
            {
                swap(T,k,k+1);
                tab_en_ordre = 0;
            }
        }
        N--;
    }
}

void index_specialite()
{

    Fichier fi;FILE *f;Tbloc buff;
    ptr tete,p,q;
    FILE *g=fopen("specialite.txt","r");
    branche brch[20];
    int k=0;
    if (g!=NULL)
    {
        while (!feof(g)) // initialisation de la table d'index
        {
           fscanf(g,"%s",&brch[k].speciality);
           brch[k].tete=NULL;
           k++;
        }
        fclose(g);
    };
    ouvrir(&fi,"ENSEIGNANT-MESRS.dat",'a',&f);
    int i=1;int j;
    while (i<=entete(&fi,1)) // construction de la table d'index
    {
        lireDir(f,i,&buff);
        j=0;
        while (j<buff.nb_enreg_inseres)
        {
            k=0;
            while (strcmp(buff.T[j].specialite,brch[k].speciality)!=0)
        {
            k++;
        };
        if (brch[k].tete==NULL) //liste vide
        {
            Allouer(&tete);
            aff_val(tete,2,i);
            aff_val(tete,3,j);
            aff_adr(tete,NULL);
            brch[k].tete=tete;
        }
        else // tete!=NULL
        {
            q=brch[k].tete;
            while (suivant(q)!=NULL)
            {
                q=suivant(q);
            };
            Allouer(&p);
            aff_val(p,2,i);
            aff_val(p,3,j);
            aff_adr(q,p);
            aff_adr(p,NULL);
        };
        j++;
        };
        i++;
    };
    fermer(f,&fi);


    /* ------ sauvegarde de la table d'index ------ */
    g=fopen("fichier_index2.txt","w");
    k=0;
    if (g!=NULL)
    {
        while (k<20)
        {
            fprintf(g,"%s %p\n",brch[k].speciality,brch[k].tete);
            k++;
        }
    }
    fclose(g);
}

typedef struct vecteur{
int anciennete;
ptr tete;
}vecteur;

void index_anciennete(int current_year)
{
    int max=current_year-1979;
    vecteur tab[max+1]; // on met "max+1" éléments pour avoir ancienneté entre 0 et max, car si on met max on aura ancienneté entre 0 et max-1
    int k=0;          // on considère ancienneté=0 pour les enseignants recrutés au cours de l'année courrante
    while (k<max+1)
    {
        tab[k].anciennete=k; // on mesure l'ancienneté par an
        tab[k].tete=NULL; // on initialise les têtes à NULL
        k++;
    }
    Fichier fi; FILE* f; Tbloc buff; int i,j,anciennete;
   ouvrir(&fi,"ENSEIGNANT-MESRS.dat",'a',&f);
   ptr tete,p,q;
   i=1;
   while (i<=entete(&fi,1)) // on parcourt tout le fichier et pour chaque enregistrement on calcule son ancienneté et on l'affecte à la liste dans la table d'index
   {
       lireDir(f,i,&buff);
       j=0;
       while (j<buff.nb_enreg_inseres)
       {
           anciennete= current_year - buff.T[j].date_recrutement.annee;
           if (tab[anciennete].tete==NULL) // liste vide // l'indice ancienneté correspond à la case qui contient la valeur "ancienneté"
           {
               Allouer(&tete);
               aff_val(tete,1,buff.T[j].etablissement_univ.code);
               aff_val(tete,2,i);
               aff_val(tete,3,j);
               tab[anciennete].tete=tete;
               aff_adr(tete,NULL);
           }
           else // tête != NULL
           {
               Allouer(&p);
               aff_val(p,1,buff.T[j].etablissement_univ.code);
               aff_val(p,2,i);
               aff_val(p,3,j);
               q=tab[anciennete].tete;
               while (suivant(q)!=NULL)
               {
                   q=suivant(q);
               }; // on sort de la boucle avec un pointeur vers le dernier maillon (suivant(q)==NULL)
               aff_adr(q,p);
               aff_adr(p,NULL);
           };
           j++;
       }
       i++;
   };
   fermer(f,&fi);


   /* ----- sauvgarde de l'index ----- */
   k=0;
   FILE* h=fopen("fichier_index3.txt","w");
   if (h!=NULL)
   {
            while (k<max+1)
            {
                fprintf(h,"%d %p\n",tab[k].anciennete,tab[k].tete);
                k++;
            };
            fclose(h);
    }
}

typedef struct table  // strucuture de la table d'index
{
    char region[10];
    ptr tete;
}table;

void index_region()
{
    table index[3]; //initialisation de la table d'index
    strcpy(index[0].region,"est");
    strcpy(index[1].region,"centre");
    strcpy(index[2].region,"ouest");
    int k=0;
    while (k<3)
    {
        index[k].tete=NULL;
        k++;
    };

    Fichier fi; FILE *f; Tbloc buff;
    ptr tete,p,q;
    ouvrir(&fi,"ENSEIGNANT-MESRS.dat",'a',&f);
    int i=1; int j;
    while (i<=entete(&fi,1)) // initilisation des listes et affectation des têtes à la table d'index
    {
        lireDir(f,i,&buff);
        j=0;
        while (j<buff.nb_enreg_inseres)
        {
            if (strcmp(buff.T[j].etablissement_univ.region,"est")==0) // la région de l'établissement d'exercice de cet enseignant est "est"
            {
                if (index[0].tete==NULL)//liste vide
                    {
                        Allouer(&tete);
                        aff_val(tete,2,i); //on affecte le numero de bloc
                        aff_val(tete,3,j); // ona ffecte la position
                        index[0].tete=tete;
                        aff_adr(tete,NULL);
                    }
                    else // tête!=NULL
                    {
                        Allouer(&p);
                        aff_val(p,2,i);
                        aff_val(p,3,j);
                        q=index[0].tete; // q pointe la tête de la liste
                        while (suivant(q)!=NULL) //parcours de la liste pour extraire le dernier maillon
                        {
                            q=suivant(q);
                        }
                        aff_adr(q,p);
                        aff_adr(p,NULL);
                    }
            };
            if (strcmp(buff.T[j].etablissement_univ.region,"centre")==0) // la région de l'établissement d'exercice de cet enseignant est "est"
            {
                if (index[1].tete==NULL)//liste vide
                    {
                        Allouer(&tete);
                        aff_val(tete,2,i); //on affecte le numero de bloc
                        aff_val(tete,3,j); // ona ffecte la position
                        index[1].tete=tete;
                        aff_adr(tete,NULL);
                    }
                    else // tête!=NULL
                    {
                        Allouer(&p);
                        aff_val(p,2,i);
                        aff_val(p,3,j);
                        q=index[1].tete; // q pointe la tête de la liste
                        while (suivant(q)!=NULL) //parcours de la liste pour extraire le dernier maillon
                        {
                            q=suivant(q);
                        }
                        aff_adr(q,p);
                        aff_adr(p,NULL);
                    }
            };
            if (strcmp(buff.T[j].etablissement_univ.region,"ouest")==0) // la région de l'établissement d'exercice de cet enseignant est "est"
            {
                if (index[2].tete==NULL)//liste vide
                    {
                        Allouer(&tete);
                        aff_val(tete,2,i); //on affecte le numero de bloc
                        aff_val(tete,3,j); // ona ffecte la position
                        index[2].tete=tete;
                        aff_adr(tete,NULL);
                    }
                    else // tête!=NULL
                    {
                        Allouer(&p);
                        aff_val(p,2,i);
                        aff_val(p,3,j);
                        q=index[2].tete; // q pointe la tête de la liste
                        while (suivant(q)!=NULL) //parcours de la liste pour extraire le dernier maillon
                        {
                            q=suivant(q);
                        }
                        aff_adr(q,p);
                        aff_adr(p,NULL);
                    }
            };
            j++;
        }
        i++;// on incrémente le num de bloc
    };
    fermer(f,&fi);

    /* -------- sauvgarde de la table d'index --------- */
    FILE *g=fopen("fichier_index4.txt","w");
    if (g!=NULL)
    {
        k=0;
        while (k<3)
        {
            fprintf(g,"%s %p\n",index[k].region,index[k].tete);
            k++;
        };
        fclose(g);
    };
}


void Epuration(int current_year)
{
    Fichier fi; FILE* f; int i=1;int j; // i est le numero de bloc, j est la position
    Tbloc buff1;Tbloc buff2;//buff1 est utilisé dans la suite pour le dernier bloc, buff2 pour le bloc qui contient un doublon à supprimer
    int k=0; //k est l'indice de la table d'index
    ouvrir(&fi,"ENSEIGNANT-MESRS.dat",'a',&f);
    int taille=fi.entete.nb_enreg_inseres;
    tableau t[taille];
    /* ----------- initialisation de la table d'index des matricules ------------ */
    while (i<=entete(&fi,1)) // on parcourt tous les blocs pour récupérer tous les matricules
    {
        lireDir(f,i,&buff1);
        j=0;
        while (j<buff1.nb_enreg_inseres) // On récupère les matricules du bloc i
        {
            t[k].matricule=buff1.T[j].matricule;
            t[k].j=j;
            t[k].i=i;
            j++;
            k++;
        }
        i++;
    };

    /* ----------- Tri par bulles ------------ */
    tri_bulles(t,taille);

    int a=0;
    while (a<taille-1) // on parcourt le tableau qui est ordonné en ordre croissant et on compare chaque élément avec son suivant s'ils ont le mm matricule ou pas
    {
        if (t[a].matricule==t[a+1].matricule)
        {
            /* ----------- suppression des doublons du fichier ----------- */
            j=t[a].j;
            lireDir(f,t[a].i,&buff2);
            lireDir(f,entete(&fi,1),&buff1); //On doit toujours relire le dernier bloc car il est mis à jour aprés chaque suppression

            buff2.T[j].matricule=buff1.T[buff1.nb_enreg_inseres-1].matricule; // on remplace le doublon par le dernier element du dernier bloc (suppression physique)
            strcpy(buff2.T[j].Nom,buff1.T[buff1.nb_enreg_inseres-1].Nom);
            strcpy(buff2.T[j].Prenom,buff1.T[buff1.nb_enreg_inseres-1].Prenom);
            buff2.T[j].sexe=buff1.T[buff1.nb_enreg_inseres-1].sexe;
            buff2.T[j].date_naissance.annee=buff1.T[buff1.nb_enreg_inseres-1].date_naissance.annee;
            buff2.T[j].date_naissance.mois=buff1.T[buff1.nb_enreg_inseres-1].date_naissance.mois;
            buff2.T[j].date_naissance.annee=buff1.T[buff1.nb_enreg_inseres-1].date_naissance.jour;
            strcpy(buff2.T[j].wilaya_naissance,buff1.T[buff1.nb_enreg_inseres-1].wilaya_naissance);
            strcpy(buff2.T[j].specialite,buff1.T[buff1.nb_enreg_inseres-1].specialite);
            strcpy(buff2.T[j].groupe_sanguin,buff1.T[buff1.nb_enreg_inseres-1].groupe_sanguin);
            strcpy(buff2.T[j].Grade,buff1.T[buff1.nb_enreg_inseres-1].Grade);
            strcpy(buff2.T[j].dernier_diplome,buff1.T[buff1.nb_enreg_inseres-1].dernier_diplome);
            strcpy(buff2.T[j].etablissement_univ.noun,buff1.T[buff1.nb_enreg_inseres-1].etablissement_univ.noun);
            strcpy(buff2.T[j].etablissement_univ.region,buff1.T[buff1.nb_enreg_inseres-1].etablissement_univ.region);
            buff2.T[j].etablissement_univ.code=buff1.T[buff1.nb_enreg_inseres-1].etablissement_univ.code; // on remplace le doublon par le dernier element du dernier bloc (suppression physique)
            buff2.T[j].date_recrutement.annee=buff1.T[buff1.nb_enreg_inseres-1].date_recrutement.annee;
            buff2.T[j].date_recrutement.mois=buff1.T[buff1.nb_enreg_inseres-1].date_recrutement.mois;
            buff2.T[j].date_recrutement.annee=buff1.T[buff1.nb_enreg_inseres-1].date_recrutement.jour;

            ecrireDir(f,t[a].i,&buff2); //on écrit le bloc modifié
            aff_entete(&fi,2,entete(&fi,2)-1); // on décrémente le nb_enregistrments_insérés
            if (buff1.nb_enreg_inseres==1)
            {
                aff_entete(&fi,1,entete(&fi,1)-1); //on supprime le dernier bloc car il contient un seul enregistrement
                aff_entete(&fi,3,buff2.nb_enreg_inseres); //on détaillera pourquoi on a mis taille dans le module insertion.
            }
            else {
                buff1.nb_enreg_inseres--;
                aff_entete(&fi,3,buff1.nb_enreg_inseres);
                ecrireDir(f,entete(&fi,1),&buff1);
            };
            fermer(f,&fi);
            /* -------- suppression des doublons de la table d'index -------- */

            k=a;
            while (k<taille-1)
            {
                t[k].matricule=t[k+1].matricule;
                t[k].i=t[k+1].i;
                t[k].j=t[k+1].j;
                k++;
            }
            taille--;
        }
        else { // T[a]!= T[a+1]
            a++;
        }

    };

    fermer(f,&fi);

    /* ------ sauvegarde de l'index des matricules ------ */
    FILE* g=fopen("fichier_index1.txt","w");
    a=0;
    if (g!=NULL){
    while (a<taille)
    {
        fprintf(g,"%d %d %d\n",t[a].matricule,t[a].i,t[a].j);
        a++;
    };
    };
    fclose(g);

    /* -------- initialisation des index ---------- */
    index_specialite();
    index_anciennete(current_year);
    index_region();


}

/* ---------------- Ajout d'un enseignant ---------------- */
/*On récupère la 1ere position libre depuis l'entête, ensuite on vérifie: si j<buff.nb_enreg_inseres on insère dans ce bloc, sinon on insère
dans un nouveau bloc avec j=0 */

void Rech_Dicho_1(int matricule, int *trouv,int *i,int *j,int *pos) // Recherche dichotomique du matricule dans la table d'index
{
    Fichier fi; FILE* f;
    ouvrir(&fi,"ENSEIGNANT-MESRS.dat",'a',&f);
    int taille=fi.entete.nb_enreg_inseres;
    fermer(f,&fi);
    tableau T[taille]; // on alloue une table d'index de taille nb_enreg_insérés

     FILE* m=fopen("fichier_index1.txt","r"); //chargement de la table d'index
    if (m!=NULL)
    { int k=0;
    while (!feof(m)) // chargement de la table d'index
    {
        fscanf(m,"%d %d %d",&T[k].matricule,&T[k].i,&T[k].j);
        k++;
    };
    fclose(m);
 }
    int inf=0; int sup=taille-1;
    int milieu;
    (*trouv)=0;
    while ((inf<=sup) && ((*trouv)==0))
    {
        milieu=(inf+sup)/2;
    if (matricule==T[milieu].matricule)
    {
        (*trouv)=1;
        (*j)=T[milieu].j;
        (*i)=T[milieu].i;
        (*pos)=milieu;
    }
    else {
     if (matricule<T[milieu].matricule)
     {
         sup=milieu-1;
     };
     if (matricule>T[milieu].matricule)
     {
         inf=milieu+1;
     };
    };
    };

}

void Ajout_index_specialite(int i,int j,char speciality[30])
{
    branche brch[20];
    FILE *g=fopen("fichier_index2.txt","r");
    int k=0;
    if (g!=NULL) // chargement de la table d'index
    {
        while (!feof(g))
        {
            fscanf(g,"%s %p",&brch[k].speciality,&brch[k].tete);
            k++;
        }
        fclose(g);
    };
    int taille_table=k;
    k=0;
    while(strcmp(speciality,brch[k].speciality)!=0)
    {
        k++;
    } // on sort de la boucle avec l'indice k où se trouve la spécialité ajoutée suite à l'ajout de l'enseignant
    ptr p;
    Allouer(&p);
    aff_val(p,2,i);
    aff_val(p,3,j);
    aff_adr(p,brch[k].tete); // on insère en début de iste afin d'éviter le parcours de toute la liste
    brch[k].tete=p; // mise à jour de la tête
    // sauvegarde de la tabe d'index
    k=0;
    g=fopen("fichier_index2.txt","w");
    if (g!=NULL)
    {
        while (k<taille_table)
        {
            fprintf(g,"%s %p\n",brch[k].speciality,brch[k].tete);
            k++;
        }
        fclose(g);
    }
}

void Ajout_index_anciennete(int current_year,int i,int j,int anciennete)
{
    int max=current_year-1979;
    FILE* g=fopen("fichier_index3.txt","r");
    int k=0; vecteur tab[max+1];// on met "max+1" éléments pour avoir ancienneté entre 0 et max, car si on met max on aura ancienneté entre 0 et max-1
    if(g!=NULL)
    {
        while (!feof(g)) //chargement de la table d'index
        {
            fscanf(g,"%d %p",&tab[k].anciennete,&tab[k].tete);
            k++;
        }
        fclose(g);
    }
    ptr p;
    Allouer(&p);
    aff_val(p,2,i);
    aff_val(p,3,j);
    aff_adr(p,tab[anciennete].tete);// insertion en début de liste pour éviter le parcours
    tab[anciennete].tete=p; // mise à jour de la tête
    // sauvegarde de la tabe d'index

    k=0;
    g=fopen("fichier_index3.txt","w");
    if (g!=NULL)
    {
        while (k<max+1)
        {
            fprintf(g,"%d %p\n",tab[k].anciennete,tab[k].tete);
            k++;
        }
        fclose(g);
    };
}

void Ajout_index_region(int i,int j,char region[10])
{
    table index[3]; int k;
    /* -------- chargement de la table d'index --------- */
    FILE *g=fopen("fichier_index4.txt","r");
    if (g!=NULL)
    {
        k=0;
        while (k<3)
        {
            fscanf(g,"%s %p\n",&index[k].region,&index[k].tete);
            k++;
        };
    };
    fclose(g);

    ptr p;
    if (strcmp(region,"est")==0)
    {
       Allouer(&p);
       aff_val(p,2,i);
       aff_val(p,3,j);
       aff_adr(p,index[0].tete);//insertion en début de liste
       index[0].tete=p;// mise à jour de la tête
    };
    if (strcmp(region,"centre")==0)
    {
       Allouer(&p);
       aff_val(p,2,i);
       aff_val(p,3,j);
       aff_adr(p,index[1].tete);//insertion en début de liste
       index[1].tete=p;// mise à jour de la tête
    };
    if (strcmp(region,"ouest")==0)
    {
       Allouer(&p);
       aff_val(p,2,i);
       aff_val(p,3,j);
       aff_adr(p,index[2].tete);//insertion en début de liste
       index[2].tete=p;// mise à jour de la tête
    };
    /* ------ sauvegarde de l'index ------ */
    g=fopen("fchier_index4.txt","w");
    if (g!=NULL)
    {
        k=0;
        while (k<3)
        {
            fprintf(g,"%s %p\n",index[k].region,index[k].tete);
            k++;
        }
        fclose(g);
    };
}

void Ajout_enseignant(int current_year)
{
    FILE* f;Fichier fi;Tbloc buff; int matricule,taille,i,position_j,anciennete;char specialite[30];char region[10];
    ouvrir(&fi,"ENSEIGNANT-MESRS.dat",'a',&f);

    lireDir(f,entete(&fi,1),&buff);// on lit le dernier bloc puisque c'est un fichier non ordonné donc on écrit en dérnier
    int j=entete(&fi,3); // on récupère la position libre pour l'insertion
    if (j <buff.nb_enreg_inseres)
    {
        printf("- Matricule :");
        scanf("%d",&buff.T[j].matricule);
        matricule=buff.T[j].matricule; // on sauvegarde le matricule car on en aura besoin dans la suite
        printf("\n- Nom et Prenom :");
        scanf("%s %s",&buff.T[j].Nom,&buff.T[j].Prenom);
        printf("\n- Sexe :( 1-Homme  2-Femme )");
        scanf("%d",&buff.T[j].sexe);
        printf("\n- Date de naissance : ");
        scanf("%d %d %d",&buff.T[j].date_naissance.jour,&buff.T[j].date_naissance.mois,&buff.T[j].date_naissance.annee);
        int Mois=buff.T[j].date_naissance.mois;
        if (Mois>12)
        {printf("\nErreur,veuillez reintroduire le mois :");
        scanf("%d",&buff.T[j].date_naissance.mois);}
        if ((Mois==1) ||(Mois==3) || (Mois==5)|| (Mois==7) || (Mois==8) || (Mois==10) || (Mois==12)) // contrôl de saisie
        {
            if (buff.T[j].date_naissance.jour>31)
            {printf("\nErreur,veuillez reintroduire le jour :");
            scanf("%d",&buff.T[j].date_naissance.jour);
            }
        };
        if ((Mois==4)|| (Mois==6)|| (Mois==9)||(Mois==11))
        {
            if(buff.T[j].date_naissance.jour>30) {
                    printf("\nErreur,veuillez reintroduire le jour :");
                    scanf("%d",&buff.T[j].date_naissance.jour);
            }
        };
        if (Mois==2) // cas spécial mois de février
        {
            int annee=buff.T[j].date_naissance.annee;
        if ((annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0)) // si l'année est bissextile
        {
            if(buff.T[j].date_naissance.jour>29) {
                    printf("\nErreur,veuillez reintroduire le jour :");
                    scanf("%d",&buff.T[j].date_naissance.jour);}

        } //l'année n'est pas bissextile
        else
        {
            if (buff.T[j].date_naissance.jour>28) {printf("\nErreur,veuillez reintroduire le jour :");
                        scanf("%d",&buff.T[j].date_naissance.jour);}
        };
        };
        printf("\n- Wilaya de naissance :");
        scanf("%s",&buff.T[j].wilaya_naissance);
        printf("\n- Groupe sanguin: ");
        scanf("%s",&buff.T[j].groupe_sanguin);
        printf("\n- Specialite :");
        scanf("%s",&buff.T[j].specialite);
        strcpy(specialite,buff.T[j].specialite); // on sauvegarde la specialité car on en aura besoin dan sla suite
        printf("\n- Dernier diplome: ");
        scanf("%s",&buff.T[j].dernier_diplome);
        printf("\n- Date de recrutement :");
        scanf("%d %d %d",&buff.T[j].date_recrutement.jour,&buff.T[j].date_recrutement.mois,&buff.T[j].date_recrutement.annee);
        anciennete=current_year-buff.T[j].date_recrutement.annee;
        Mois=buff.T[j].date_recrutement.mois;
        if (Mois>12) {printf("\nErreur,veuillez reintroduire le mois :");
        scanf("%d",&buff.T[j].date_naissance.mois);}
        if ((Mois==1) ||(Mois==3) || (Mois==5)|| (Mois==7) || (Mois==8) || (Mois==10) || (Mois==12)) // contrôl de saisie
        {
            if (buff.T[j].date_naissance.jour>31){printf("\nErreur,veuillez reintroduire le jour :");
            scanf("%d",&buff.T[j].date_naissance.jour);}
        };
        if ((Mois==4)|| (Mois==6)|| (Mois==9)||(Mois==11))
        {
            if(buff.T[j].date_naissance.jour>30) {printf("\nErreur,veuillez reintroduire le jour :");
            scanf("%d",&buff.T[j].date_naissance.jour);}
        };
        if (Mois==2) // cas spécial mois de février
        {
            int annee=buff.T[j].date_naissance.annee;
        if ((annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0)) // si l'année est bissextile
        {
            if(buff.T[j].date_naissance.jour>29) {printf("\nErreur,veuillez reintroduire le jour :");
            scanf("%d",&buff.T[j].date_naissance.jour);}
        } //l'année n'est pas bissextile
        else
        {
            if (buff.T[j].date_naissance.jour>28) {printf("\nErreur,veuillez reintroduire le jour :");
            scanf("%d",&buff.T[j].date_naissance.jour);}
        };
        };
        printf("\n- Grade :");
        scanf("%s",&buff.T[j].Grade);

        int code;
        etablissement etab[108];
    FILE* p=fopen("etablissement1.txt","r");
    if (p!=NULL)
    {
        int a=0;
        while((a<108))
            {
               fscanf(p,"%d %s %s",&etab[a].code,&etab[a].noun,&etab[a].region);
               a++;
               if(a==100)
               {
                   p=fopen("etablissement2.txt","r");
                   if (p!=NULL)
                   {
                       while (a<108)
                       {
                           fscanf(p,"%d %s %s",&etab[a].code,&etab[a].noun,&etab[a].region);
                           a++;
                       }
                       fclose(p);
                   }
               };
            };
            fclose(p);
    }
       int k=0;
        while (k<108) //affichage des etablissements avec leurs codes
        {
            printf("%d %s %s\n",etab[k].code,etab[k].noun,etab[k].region);
            k++;
        };
        printf("\nVeuillez introduire le code de l'etablissement :");/*on utilise des codes pour chaque établissement afin d'éviter les fautes d'othographes éventuelles
                                                qui vont générer des problèmes dans la recherche oubien la requête à intervalle*/
        scanf("%d",&code);
        buff.T[j].etablissement_univ.code=code;
        strcpy(buff.T[j].etablissement_univ.noun,etab[code].noun);
        strcpy(buff.T[j].etablissement_univ.region,etab[code].region);
        position_j=j; // on sauvegarde la position j car on en aura besoin dans la suite
        i=entete(&fi,1); // num du dernier bloc

        j++;
        buff.nb_enreg_inseres=j;
        ecrireDir(f,entete(&fi,1),&buff); // on écrit le dernier bloc
        aff_entete(&fi,2,entete(&fi,2)+1); // on incrémente le nb_enreg_inseres
        aff_entete(&fi,3,buff.nb_enreg_inseres); // on met à jour la position libre
        taille=entete(&fi,2);
        fermer(f,&fi);
    }
    else // j>= buff.nb_enreg_inseres
    {
        // on insère à la 1ere position du bloc (j=0)
        printf("- Matricule :");
        scanf("%d",&buff.T[0].matricule);
        matricule=buff.T[0].matricule;
        printf("\n- Nom et Prenom :");
        scanf("%s %s",&buff.T[0].Nom,&buff.T[0].Prenom);
        printf("\n- Sexe :( 1-Homme  2-Femme )");
        scanf("%d",&buff.T[0].sexe);
        printf("\n- Date de naissance : ");
        scanf("%d %d %d",&buff.T[0].date_naissance.jour,&buff.T[0].date_naissance.mois,&buff.T[0].date_naissance.annee);
        int Mois=buff.T[0].date_naissance.mois;
        if (Mois>12) {printf("\nErreur,veuillez reintroduire le mois :");
        scanf("%d",&buff.T[j].date_naissance.mois);}
        if ((Mois==1) ||(Mois==3) || (Mois==5)|| (Mois==7) || (Mois==8) || (Mois==10) || (Mois==12)) // contrôl de saisie
        {
            if (buff.T[0].date_naissance.jour>31) {printf("\nErreur,veuillez reintroduire le jour :");
            scanf("%d",&buff.T[j].date_naissance.jour);}
        };
        if ((Mois==4)|| (Mois==6)|| (Mois==9)||(Mois==11))
        {
            if(buff.T[0].date_naissance.jour>30) {printf("\nErreur,veuillez reintroduire le jour :");
            scanf("%d",&buff.T[j].date_naissance.jour);}
        };
        if (Mois==2) // cas spécial mois de février
        {
            int annee=buff.T[0].date_naissance.annee;
        if ((annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0)) // si l'année est bissextile
        {
            if(buff.T[0].date_naissance.jour>29){printf("\nErreur,veuillez reintroduire le jour :");
            scanf("%d",&buff.T[j].date_naissance.jour);}
        } //l'année n'est pas bissextile
        else
        {
            if (buff.T[0].date_naissance.jour>28) {printf("\nErreur,veuillez reintroduire le jour :");
            scanf("%d",&buff.T[j].date_naissance.jour);}
        };
        };
        printf("\n- Wilaya de naissance :");
        scanf("%s",&buff.T[0].wilaya_naissance);
        printf("\n- Groupe sanguin: ");
        scanf("%s",&buff.T[0].groupe_sanguin);
        printf("\n- Specialite :");
        scanf("%s",&buff.T[0].specialite);
        strcpy(specialite,buff.T[0].specialite); // on sauvegarde la specialité car on en aura besoin dan sla suite
        printf("\n- Dernier diplome: ");
        scanf("%s",&buff.T[0].dernier_diplome);
        printf("\n- Date de recrutement :");
        scanf("%d %d %d",&buff.T[0].date_recrutement.jour,&buff.T[0].date_recrutement.mois,&buff.T[0].date_recrutement.annee);
        anciennete=current_year-buff.T[0].date_recrutement.annee;
        Mois=buff.T[0].date_recrutement.mois;
        if (Mois>12) {printf("\nErreur,veuillez reintroduire le mois :");
        scanf("%d",&buff.T[j].date_naissance.mois);}
        if ((Mois==1) ||(Mois==3) || (Mois==5)|| (Mois==7) || (Mois==8) || (Mois==10) || (Mois==12)) // contrôl de saisie
        {
            if (buff.T[0].date_naissance.jour>31){printf("\nErreur,veuillez reintroduire le jour :");
            scanf("%d",&buff.T[j].date_naissance.jour);}
        };
        if ((Mois==4)|| (Mois==6)|| (Mois==9)||(Mois==11))
        {
            if(buff.T[0].date_naissance.jour>30) {printf("\nErreur,veuillez reintroduire le jour :");
            scanf("%d",&buff.T[j].date_naissance.jour);}
        };
        if (Mois==2) // cas spécial mois de février
        {
            int annee=buff.T[0].date_naissance.annee;
        if ((annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0)) // si l'année est bissextile
        {
            if(buff.T[0].date_naissance.jour>29){printf("\nErreur,veuillez reintroduire le jour :");
            scanf("%d",&buff.T[j].date_naissance.jour);}
        } //l'année n'est pas bissextile
        else
        {
            if (buff.T[0].date_naissance.jour>28){printf("\nErreur,veuillez reintroduire le jour :");
            scanf("%d",&buff.T[j].date_naissance.jour);}
        };
        };
        printf("\n- Grade :");
        scanf("%s",&buff.T[0].Grade);

        int code;
        etablissement etab[108];
      FILE* p=fopen("etablissement1.txt","r");
    if (p!=NULL)
    {
        int a=0;
        while((a<108))
            {
               fscanf(p,"%d %s %s",&etab[a].code,&etab[a].noun,&etab[a].region);
               a++;
               if(a==100)
               {
                   p=fopen("etablissement2.txt","r");
                   if (p!=NULL)
                   {
                       while (a<108)
                       {
                           fscanf(p,"%d %s %s",&etab[a].code,&etab[a].noun,&etab[a].region);
                           a++;
                       }
                       fclose(p);
                   }
               };
            };
            fclose(p);
    }
       int k=0;
        while (k<108) //affichage des etablissements avec leurs codes
        {
            printf("%d %s %s\n",etab[k].code,etab[k].noun,etab[k].region);
            k++;
        };
        printf("\nVeuillez introduire le code de l'etablissement :");
        scanf("%d",&code);
        buff.T[0].etablissement_univ.code=code;
        strcpy(buff.T[0].etablissement_univ.noun,etab[code].noun);
        strcpy(buff.T[0].etablissement_univ.region,etab[code].region);
        position_j=0; // on sauvegarde la position j car on en aura besoin dans la suite
        i=entete(&fi,1)+1; // num du bloc contenant cet enregistrement


        buff.nb_enreg_inseres=1;// on incrémente le nb_enreg_inseres dans le bloc
        int i=entete(&fi,1)+1; // on incrémente le nb_blocs
        ecrireDir(f,i,&buff);
        aff_entete(&fi,1,i);
        aff_entete(&fi,2,entete(&fi,2)+1); // on incrémente le nb_enreg_inseres dans le fichier
        aff_entete(&fi,3,buff.nb_enreg_inseres); //1ere posiiton libre :j=1
        taille=entete(&fi,2); // taille sera utilisée pour la nouvelle table d'index (apres mise à jour)
        fermer(f,&fi);
    };

    /* --------- Ajout du nouveau matricule dans les tables d'index---------- */
    //mise à jour de la tablde d'index1 (matricules)
        tableau Tab[taille];
        int k=0;
        FILE *g=fopen("fichier_index1.txt","r"); // chargement de la table d'index
        if (g!=NULL)
        {
            while (!feof(g))
            {
                fscanf(g,"%d %d %d",&Tab[k].matricule,&Tab[k].i,&Tab[k].j);
                k++;
            }
            fclose(g);
        } // la table d'index contient (taille-1) éléments, il faut rajouter maintenant le nouveau matricule
        int pos,trouv;
        Rech_Dicho_1(matricule,&trouv,&i,&j,&pos);
        if (trouv==1)
        {
            k=taille-1; // k=dernier indice de la table d'index qui ne contient aucun élément car la table est de taille "taille" et elle contient "nb_enreg_inseres-1" éléments
            while (k>pos)
            {
                Tab[k].matricule=Tab[k-1].matricule;
                Tab[k].i=Tab[k-1].i;
                Tab[k].j=Tab[k-1].j;
                k--;
            }
            Tab[pos].matricule=matricule; // on insère le nouveau matricule dans sa bonne position
            Tab[pos].j=position_j;
            Tab[pos].i=i;
            k=0;
            g=fopen("fichier_index1.txt","w");
            if (g!=NULL)
            {
                while (k<taille)
                {
                    fprintf(g,"%d %d %d\n",Tab[k].matricule,Tab[k].i,Tab[k].j);
                    k++;
                }
                fclose(g);
            };

        };

        Ajout_index_specialite(i,position_j,specialite);
        Ajout_index_anciennete(current_year,i,position_j,anciennete);
        Ajout_index_region(i,position_j,region);
}

/* -------- Modification de l'etablissement univ d'un enseignant en raison de sa mutation -------- */
/*On utilise le matricule de l'enseignant pour récuperer sa position j dans le bloc i (d'apres la table d'index) ensuite on modifie l'etablissement en utilisant
le code associé afin d'eviter les fautes d'orthographe éventuelles*/

void Modification_etablissement(int matricule)
{
    int i,j,pos,trouv;
    Rech_Dicho_1(matricule,&trouv,&i,&j,&pos);
    if (trouv==1)
    {
        Tbloc buff;
        FILE* f;Fichier fi;
        ouvrir(&fi,"ENSEIGNANT-MESRS.dat",'a',&f);
        lireDir(f,i,&buff);
        printf("\n\n/* ------ Liste des etablissements ------ */:\n");
        etablissement tab[108];
        FILE* p=fopen("etablissement1.txt","r");
        int k=0;
    if (p!=NULL)
    {
        while((k<108))
            {
               fscanf(p,"%d %s %s",&tab[k].code,&tab[k].noun,&tab[k].region);
               k++;
               if(k==100)
               {
                   p=fopen("etablissement2.txt","r");
                   if (p!=NULL)
                   {
                       while (k<108)
                       {
                           fscanf(p,"%d %s %s",&tab[k].code,&tab[k].noun,&tab[k].region);
                           k++;
                       }
                       fclose(p);
                   }
               };
            };
            fclose(p);
    }
        k=0;
        while (k<108)
        {
            printf("%d %s %s\n",tab[k].code,tab[k].noun,tab[k].region);
            k++;
        }
        printf("\n- Ancien etablissement : ");
        textcolor(12);
        printf("%s %s\n",buff.T[j].etablissement_univ.noun,buff.T[j].etablissement_univ.region);
        int code;
        textcolor(0);
        printf("\n Veuillez choisir le code du nouveau etablissement :");
        scanf("%d",&code);
        buff.T[j].etablissement_univ.code=code;
        strcpy(buff.T[j].etablissement_univ.noun,tab[code].noun);
        strcpy(buff.T[j].etablissement_univ.region,tab[code].region);
        printf("\n- Nouveau etablissement : ");
        textcolor(2);
        printf("%s %s\n",buff.T[j].etablissement_univ.noun,buff.T[j].etablissement_univ.region);
        ecrireDir(f,i,&buff);
        fermer(f,&fi);
    }
}


/* ------- Suppression d'un enseignant donné par un matricule ------- */
/* La  suppression d'un enseignant donné par un matricule va se faire de la manière suivante :
    -On recherche dichotomiquement le matricule dans la table d'index.
    -On récupère le bloc i et la position j où se trouve l'enregistrement, ensuite on charge le dernier bloc du fichier et on remplace
    l'élément qu'on veut supprimer par le dernier élément du dernier bloc (suppression physique)
    -On met à jour la table d'index (par décalage).*/

void update_index_specialite(char speciality[30],int i,int j)
{
    branche brch[20];
    FILE *g=fopen("fichier_index2.txt","r");
    int k=0;
    if (g!=NULL) // chargement de la table d'index
    {
        while (!feof(g))
        {
            fscanf(g,"%s %p",&brch[k].speciality,&brch[k].tete);
            k++;
        }
        fclose(g);
    };
    int taille_table=k;
    k=0;
    while (strcmp(speciality,brch[k].speciality)!=0)
    {
        k++;
    }
    ptr p,q; p=brch[k].tete;
    q=p;
    int trouv=0;
    while ((p!=NULL)&&(trouv!=1))
    {
        if ((valeur(p,2)==i)&&(valeur(p,3)==j))
        {
            if (p==brch[k].tete) //suppression de la tete
            {
                brch[k].tete=suivant(p); // mise à jour de la tête
                liberer(p);
                trouv=1;
            }
            else //p!=tete
            {
                while (suivant(q)!=p) // on extrait l'éléménet précédent de p
                {
                    q=suivant(q);
                };
                aff_adr(q,suivant(p));
                liberer(p);
                trouv=1;
            }
        }
        else // ((valeur(p,2)!=i)||(valeur(p,3)!=j))
        {
            p=suivant(p);
        }
    }
    /* ----- sauvegarde de l'index ----- */
    g=fopen("fichier_index2.txt","w");
    if (g!=NULL)
    {
       k=0;
       while (k<taille_table)
       {
           fprintf(g,"%s %p\n",brch[k].speciality,brch[k].tete);
           k++;
       }
       fclose(g);
    };
}

void update_index_anciennete(int current_year,int anciennete,int i,int j)
{
    int max=current_year-1979;
    FILE* g=fopen("fichier_index3.txt","r");
    int k=0; vecteur tab[max+1];// on met "max+1" éléments pour avoir ancienneté entre 0 et max, car si on met max on aura ancienneté entre 0 et max-1
    if(g!=NULL)
    {
        while (!feof(g)) //chargement de la table d'index
        {
            fscanf(g,"%d %p",&tab[k].anciennete,&tab[k].tete);
            k++;
        }
        fclose(g);
    }
    ptr p,q;
    p=tab[anciennete].tete; q=p;
    int trouv=0;
    while ((p!=NULL)&&(trouv!=1))
    {
        if ((valeur(p,2)==i)&&(valeur(p,3)==j))
        {
            if (p==tab[anciennete].tete) //suppression de la tete
            {
                tab[anciennete].tete=suivant(p); // mise à jour de la tête
                liberer(p);
                trouv=1;
            }
            else //p!=tete
            {
                while (suivant(q)!=p) // on extrait l'éléménet précédent de p
                {
                    q=suivant(q);
                };
                aff_adr(q,suivant(p));
                liberer(p);
                trouv=1;
            }
        }
        else // ((valeur(p,2)!=i)||(valeur(p,3)!=j))
        {
            p=suivant(p);
        }
    }
    /* ----- sauvegarde de l'index ----- */
   k=0;
   g=fopen("fichier_index3.txt","w");
   if (g!=NULL)
   {
            while (k<max+1)
            {
                fprintf(g,"%d %p\n",tab[k].anciennete,tab[k].tete);
                k++;
            };
            fclose(g);
    };
}

void update_index_region(char region[10],int i,int j)
{
    table index[3]; int k;
    /* -------- chargement de la table d'index --------- */
    FILE *g=fopen("fichier_index4.txt","r");
    if (g!=NULL)
    {
        k=0;
        while (k<3)
        {
            fscanf(g,"%s %p\n",&index[k].region,&index[k].tete);
            k++;
        };
    };
    fclose(g);
    ptr p,q;
    if (strcmp(region,"est")==0)
    {
       p=index[0].tete; q=p;
    int trouv=0;
    while ((p!=NULL)&&(trouv!=1))
    {
        if ((valeur(p,2)==i)&&(valeur(p,3)==j))
        {
            if (p==index[0].tete) //suppression de la tete
            {
                index[0].tete=suivant(p); // mise à jour de la tête
                liberer(p);
                trouv=1;
            }
            else //p!=tete
            {
                while (suivant(q)!=p) // on extrait l'éléménet précédent de p
                {
                    q=suivant(q);
                };
                aff_adr(q,suivant(p));
                liberer(p);
                trouv=1;
            }
        }
        else // ((valeur(p,2)!=i)||(valeur(p,3)!=j))
        {
            p=suivant(p);
        }
    }
    };
    if (strcmp(region,"centre")==0)
    {
       p=index[1].tete; q=p;
    int trouv=0;
    while ((p!=NULL)&&(trouv!=1))
    {
        if ((valeur(p,2)==i)&&(valeur(p,3)==j))
        {
            if (p==index[1].tete) //suppression de la tete
            {
                index[1].tete=suivant(p); // mise à jour de la tête
                liberer(p);
                trouv=1;
            }
            else //p!=tete
            {
                while (suivant(q)!=p) // on extrait l'éléménet précédent de p
                {
                    q=suivant(q);
                };
                aff_adr(q,suivant(p));
                liberer(p);
                trouv=1;
            }
        }
        else // ((valeur(p,2)!=i)||(valeur(p,3)!=j))
        {
            p=suivant(p);
        }
    }
    };
        if (strcmp(region,"ouest")==0)
    {
       p=index[2].tete; q=p;
    int trouv=0;
    while ((p!=NULL)&&(trouv!=1))
    {
        if ((valeur(p,2)==i)&&(valeur(p,3)==j))
        {
            if (p==index[2].tete) //suppression de la tete
            {
                index[2].tete=suivant(p); // mise à jour de la tête
                liberer(p);
                trouv=1;
            }
            else //p!=tete
            {
                while (suivant(q)!=p) // on extrait l'éléménet précédent de p
                {
                    q=suivant(q);
                };
                aff_adr(q,suivant(p));
                liberer(p);
                trouv=1;
            }
        }
        else // ((valeur(p,2)!=i)||(valeur(p,3)!=j))
        {
            p=suivant(p);
        }
    }
    };
    /* -------- sauvgarde de la table d'index --------- */
    g=fopen("fichier_index4.txt","w");
    if (g!=NULL)
    {
        k=0;
        while (k<3)
        {
            fprintf(g,"%s %p\n",index[k].region,index[k].tete);
            k++;
        };
        fclose(g);
    };

}
void Suppression_enseignant (int matricule,int current_year)
{
    int i,j,position,trouv,anciennete; Tbloc buff1,buff2; FILE *f; Fichier fi; char speciality[30];char region[10];
    Rech_Dicho_1(matricule,&trouv,&i,&j,&position);
    if (trouv==1)
    {
        ouvrir(&fi,"ENSEIGNANT-MESRS.dat",'a',&f);
        lireDir(f,i,&buff2); // buff2 contient le bloc où se trouve l'enseignant qu'on veut supprimer
        lireDir(f,entete(&fi,1),&buff1); // buff1 contient le dernier bloc

        buff2.T[j].matricule=buff1.T[buff1.nb_enreg_inseres-1].matricule;// on remplace le doublon par le dernier element du dernier bloc (suppression physique)
        strcpy(buff2.T[j].Nom,buff1.T[buff1.nb_enreg_inseres-1].Nom);
        strcpy(buff2.T[j].Prenom,buff1.T[buff1.nb_enreg_inseres-1].Prenom);
        buff2.T[j].sexe=buff1.T[buff1.nb_enreg_inseres-1].sexe;
        buff2.T[j].date_naissance.annee=buff1.T[buff1.nb_enreg_inseres-1].date_naissance.annee;
        buff2.T[j].date_naissance.mois=buff1.T[buff1.nb_enreg_inseres-1].date_naissance.mois;
        buff2.T[j].date_naissance.annee=buff1.T[buff1.nb_enreg_inseres-1].date_naissance.jour;
        strcpy(buff2.T[j].wilaya_naissance,buff1.T[buff1.nb_enreg_inseres-1].wilaya_naissance);
        strcpy(speciality,buff2.T[j].specialite); // on sauvegarde la specialité car on en aura besoin plus tard
        strcpy(buff2.T[j].specialite,buff1.T[buff1.nb_enreg_inseres-1].specialite);
        strcpy(buff2.T[j].groupe_sanguin,buff1.T[buff1.nb_enreg_inseres-1].groupe_sanguin);
        strcpy(buff2.T[j].Grade,buff1.T[buff1.nb_enreg_inseres-1].Grade);
        strcpy(buff2.T[j].dernier_diplome,buff1.T[buff1.nb_enreg_inseres-1].dernier_diplome);
        strcpy(region,buff2.T[j].etablissement_univ.region); // on sauvegarde la region car on en aura besoin
        strcpy(buff2.T[j].etablissement_univ.noun,buff1.T[buff1.nb_enreg_inseres-1].etablissement_univ.noun);
        strcpy(buff2.T[j].etablissement_univ.region,buff1.T[buff1.nb_enreg_inseres-1].etablissement_univ.region);
        buff2.T[j].etablissement_univ.code=buff1.T[buff1.nb_enreg_inseres-1].etablissement_univ.code; // on remplace le doublon par le dernier element du dernier bloc (suppression physique)
        anciennete=current_year-buff2.T[j].date_recrutement.annee; // on sauvegarde l'ancienneté car on en aura besoin plus tard
        buff2.T[j].date_recrutement.annee=buff1.T[buff1.nb_enreg_inseres-1].date_recrutement.annee;
        buff2.T[j].date_recrutement.mois=buff1.T[buff1.nb_enreg_inseres-1].date_recrutement.mois;
        buff2.T[j].date_recrutement.annee=buff1.T[buff1.nb_enreg_inseres-1].date_recrutement.jour;

        ecrireDir(f,i,&buff2); //on écrit le bloc modifié
        aff_entete(&fi,2,entete(&fi,2)-1); // on décrémente le nb_enregistrments_insérés
        if (buff1.nb_enreg_inseres==1)
            {
               aff_entete(&fi,1,entete(&fi,1)-1); //on supprime le dernier bloc car il contient un seul enregistrement qui est déplacé vers le bloc i
               aff_entete(&fi,3,buff2.nb_enreg_inseres); //on détaillera pourquoi on a mis la taille maximale du bloc dans le module insertion.
            }
        else{
               buff1.nb_enreg_inseres--;
               aff_entete(&fi,3,buff1.nb_enreg_inseres);
               ecrireDir(f,entete(&fi,1),&buff1);
            };
           fermer(f,&fi);

           /* ------ mise à jour de la table d'index ------- */
           FILE* g=fopen("fichier_index1.txt","r");
           int taille =entete(&fi,2)+1; // car le nb_enreg_inseres est décrémenté de 1 donc on doit récupérer l'ancien nb_enreg_inseres car la table d'index est trié
                                         //donc on risque de perdre le dernier élément
           int k=0; tableau Tab[taille];
           if (g!=NULL) // chargement de la table d'index
           {
               while (!feof(g))
               {
                   fscanf(g,"%d %d %d",&Tab[k].matricule,&Tab[k].i,&Tab[k].j);
                   k++;
               }
               fclose(g);
           }
           k=position; //suppression de l'élément qui se trouve dans la case "position"(ie milieu) par décalage
           while (k<taille-1)
            {
                Tab[k].matricule=Tab[k+1].matricule;
                Tab[k].i=Tab[k+1].i;
                Tab[k].j=Tab[k+1].j;
                k++;
            };
            taille--; // on décremente la taille de la table d'index car on a supprimé un élément

            /* ------- Mise à jour des index -------- */
            //index 1: matricules
           k=0;
           g=fopen("fichier_index1.txt","w");
           while (k<taille)
           {
               fprintf(g,"%d %d %d\n",Tab[k].matricule,Tab[k].i,Tab[k].j);
               k++;
           }
           fclose(g);

           update_index_specialite(speciality,i,j);
           update_index_anciennete(current_year,anciennete,i,j);
           update_index_region(region,i,j);
    }
    else // !trouv
    {
        printf("Le matricule que vous avez introduit n'existe pas.");
    };
}


/* ---------- Suprression d'une spécialité donnée ---------- */
/*La suppression d'une spécialité donnée consiste à regrouper les spécialités dans des listes (chaque spécialité a une liste qui contient le num de bloc
et la posiiton de l'enregistrement qui contient cette spécialité).Donc, on construit une table d'index qui contient 2 champs : spécialité et la tête de la
liste.Pour supprimer une spécialité, il faut parcourir sa liste et pour chaque (i,j) on lit le bloc i et on supprime l'enregistrement j,ensuite on la
table d'index à jour*/

void update_index_matricule(int taille,int matricule)
{
    int trouv,position,i,j;
    Rech_Dicho_1(matricule,&trouv,&i,&j,&position);
    if (trouv==1)
    {
           FILE* g=fopen("fichier_index1.txt","r");
           int k=0; tableau Tab[taille];
           if (g!=NULL) // chargement de la table d'index
           {
               while (!feof(g))
               {
                   fscanf(g,"%d %d %d",&Tab[k].matricule,&Tab[k].i,&Tab[k].j);
                   k++;
               }
               fclose(g);
           }
           k=position; //suppression de l'élément qui se trouve dans la case "position"(ie milieu) par décalage
           while (k<taille-1)
            {
                Tab[k].matricule=Tab[k+1].matricule;
                Tab[k].i=Tab[k+1].i;
                Tab[k].j=Tab[k+1].j;
                k++;
            };
            taille--; // on décremente la taille de la table d'index car on a supprimé un élément

            /* ------- Mise à jour de la table d'index -------- */
           k=0;
           g=fopen("fichier_index1.txt","w");
           while (k<taille)
           {
               fprintf(g,"%d %d %d\n",Tab[k].matricule,Tab[k].i,Tab[k].j);
               k++;
           }
           fclose(g);
    };
}
void Suppression_specialite(char specialite[30],int current_year)
{
    branche brch[20]; int matricule,anciennete;char region[10];
    FILE *g=fopen("fichier_index2.txt","r");
    int k=0;
    if (g!=NULL) // chargement de la table d'index
    {
        while (!feof(g))
        {
            fscanf(g,"%s %p",&brch[k].speciality,&brch[k].tete);
            k++;
        }
        fclose(g);
    };
    int taille_table=k-1;
    k=0;
    while(strcmp(specialite,brch[k].speciality)!=0)
    {
        k++;
    } // on sort de la boucle avec k l'indice où se trouve la spécialité qu'on veut supprimier
    ptr p,q; int i,j; Tbloc buff1,buff2; //buff1 pour le dernier bloc,buff2 pour le bloc qu'on va modifier
    Fichier fi;FILE *f;
    ouvrir(&fi,"ENSEIGNANT-MESRS.dat",'a',&f); int taille_table2=entete(&fi,2); // on récupère le nb_enreg_inseres pour déclarer la taille de la table d'index des matrciules
    p=brch[k].tete; // on récupère la tête de la liste
    while (p!=NULL)
    {
        i=valeur(p,2);
        j=valeur(p,3);
        lireDir(f,i,&buff2);
        lireDir(f,entete(&fi,1),&buff1); //dernjier bloc
        // suppression physique

        matricule=buff2.T[j].matricule;//on sauvegarde le matricule pour le supprimer de la table d'index des matricules
        update_index_matricule(taille_table2,matricule);
        buff2.T[j].matricule=buff1.T[buff1.nb_enreg_inseres-1].matricule;// on remplace le doublon par le dernier element du dernier bloc (suppression physique)
        strcpy(buff2.T[j].Nom,buff1.T[buff1.nb_enreg_inseres-1].Nom);
        strcpy(buff2.T[j].Prenom,buff1.T[buff1.nb_enreg_inseres-1].Prenom);
        buff2.T[j].sexe=buff1.T[buff1.nb_enreg_inseres-1].sexe;
        buff2.T[j].date_naissance.annee=buff1.T[buff1.nb_enreg_inseres-1].date_naissance.annee;
        buff2.T[j].date_naissance.mois=buff1.T[buff1.nb_enreg_inseres-1].date_naissance.mois;
        buff2.T[j].date_naissance.annee=buff1.T[buff1.nb_enreg_inseres-1].date_naissance.jour;
        strcpy(buff2.T[j].wilaya_naissance,buff1.T[buff1.nb_enreg_inseres-1].wilaya_naissance);
        strcpy(buff2.T[j].specialite,buff1.T[buff1.nb_enreg_inseres-1].specialite);
        strcpy(buff2.T[j].groupe_sanguin,buff1.T[buff1.nb_enreg_inseres-1].groupe_sanguin);
        strcpy(buff2.T[j].Grade,buff1.T[buff1.nb_enreg_inseres-1].Grade);
        strcpy(buff2.T[j].dernier_diplome,buff1.T[buff1.nb_enreg_inseres-1].dernier_diplome);
        strcpy(region,buff2.T[j].etablissement_univ.region);//on sauvegarde la région de létab pour la supprimer de la table d'index des régions
        update_index_region(region,i,j);
        strcpy(buff2.T[j].etablissement_univ.noun,buff1.T[buff1.nb_enreg_inseres-1].etablissement_univ.noun);
        strcpy(buff2.T[j].etablissement_univ.region,buff1.T[buff1.nb_enreg_inseres-1].etablissement_univ.region);
        buff2.T[j].etablissement_univ.code=buff1.T[buff1.nb_enreg_inseres-1].etablissement_univ.code; // on remplace le doublon par le dernier element du dernier bloc (suppression physique)
        anciennete=current_year-buff2.T[j].date_recrutement.annee; //on sauvegarde l'ancienneté pour la supprimer de la table d'index des anciennetés
        update_index_anciennete(current_year,anciennete,i,j);
        buff2.T[j].date_recrutement.annee=buff1.T[buff1.nb_enreg_inseres-1].date_recrutement.annee;
        buff2.T[j].date_recrutement.mois=buff1.T[buff1.nb_enreg_inseres-1].date_recrutement.mois;
        buff2.T[j].date_recrutement.annee=buff1.T[buff1.nb_enreg_inseres-1].date_recrutement.jour;

        ecrireDir(f,i,&buff2); //on écrit le bloc modifié
        aff_entete(&fi,2,entete(&fi,2)-1); // on décrémente le nb_enregistrments_insérés
        if (buff1.nb_enreg_inseres==1)
            {
               aff_entete(&fi,1,entete(&fi,1)-1); //on supprime le dernier bloc car il contient un seul enregistrement qui est déplacé vers le bloc i
               aff_entete(&fi,3,buff2.nb_enreg_inseres); //on détaillera pourquoi on a mis la taille maximale du bloc dans le module insertion.
            }
        else{
               buff1.nb_enreg_inseres--;
               aff_entete(&fi,3,buff1.nb_enreg_inseres);
               ecrireDir(f,entete(&fi,1),&buff1);
            };

            p=suivant(p); // on avance dans la liste
    }
    fermer(f,&fi);

    /* ------- mise à jour de la table d'index des spécialités -------- */
    p=brch[k].tete;
    q=brch[k].tete;
    while (q!=NULL) //suppression de la liste et mise à jour des index matricule,anciennete et specialité
    {
        q=suivant(q);
        liberer(p);
        p=q;
    };
    while (k<taille_table-1) // (taille_table-1) car on va faire un décalage
    {
        strcpy(brch[k].speciality,brch[k+1].speciality); // mise à jour de la table d'index
        brch[k].tete=brch[k+1].tete;
        k++;
    };

    /* --------- sauvegarde de la table d'index --------- */
    k=0;
    g=fopen("fichier_index2.txt","w");
    if (g!=NULL)
    {
        while (k<taille_table-1)//on décremente la taille de la table d'index car on a supprimé une spécialité
        {
            fprintf(g,"%s %p\n",brch[k].speciality,brch[k].tete);
            k++;
        }
        fclose(g);
    };


}


/* -- Affichage des enseignants exerçant dans un établissement donné ayant une ancienneté entre deux valeurs données (Requête à intervalle) -- */

/*On utilise une table d'index ici qui contient 2 champs : ancienneté et la tête de la liste des enseignants ayant comme ancienneté la valeur "ancienneté"
chaque maillon de la liste contient le code_univ (ie l'établissement de l'enseignant), le bloc i et la position j de chaque enrengistrement*/
// On utilise la structure (du maillon) et les machines abstraites de la question précédente

void Requete_a_intervalle(int code_univ,int current_year,int a, int b) //code_univ est le code de l'établissement, a:borne inf de l'intervalle, b: borne sup de l'intervalle
{
    int max=current_year-1979;
    FILE* g=fopen("fichier_index3.txt","r");
    int k=0; vecteur tab[max+1];// on met "max+1" éléments pour avoir ancienneté entre 0 et max, car si on met max on aura ancienneté entre 0 et max-1
    if(g!=NULL)
    {
        while (!feof(g)) //chargement de la table d'index
        {
            fscanf(g,"%d %p",&tab[k].anciennete,&tab[k].tete);
            k++;
        }
        fclose(g);
    }

    ptr p;int i,j;
    Fichier fi;FILE *f;Tbloc buff;
    ouvrir(&fi,"ENSEIGNANT-MESRS.dat",'a',&f);
    if (a>max)//l'ancienneté donnée est > l'ancienneté maximale (max)
    {
        printf("Il n'y a pas d'enseignants qui ont une ancienneté entre %d et %d.",a,b);
    }
    else //(0<= a <= max)
    {
        while (a<=b)
        {
            p=tab[a].tete; // on récupère la tête de la liste
            if (p==NULL) // liste vide
            {
                printf("Il n'y a pas d'enseignants qui ont une anciennete de %d ans\n",a);
            }
            else //tete!=NULL
            {
                while (p!=NULL) // parcours de la liste
                {
                    if (code_univ==valeur(p,1)) // si le code donné est identique à celui qui se trouve dans le maillon alors on l'affiche
                    {
                        printf("\nLes enseignants ayant une anciennete de %d ans: \n",a);
                        i=valeur(p,2);
                        j=valeur(p,3);
                        lireDir(f,i,&buff);
                        printf("-Matricule: %d\n",buff.T[j].matricule);
                        printf("-Nom et prenom: %s %s\n",buff.T[j].Nom,buff.T[j].Prenom);
                        printf("-Date de naissance: %d/%d/%d\n",buff.T[j].date_naissance.jour,buff.T[j].date_naissance.mois,buff.T[j].date_naissance.annee);
                        printf("-Wilaya de naissance: %s\n",buff.T[j].wilaya_naissance);
                        printf("-Sexe: %d\n",buff.T[j].sexe);
                        printf("-Group sanguin: %s\n",buff.T[j].groupe_sanguin);
                        printf("-Date de recrutement: %d/%d/%d\n",buff.T[j].date_recrutement.jour,buff.T[j].date_recrutement.mois,buff.T[j].date_recrutement.annee);
                        printf("-Grade: %s\n",buff.T[j].Grade);
                        printf("-Specialite: %s\n",buff.T[j].specialite);
                        printf("-Dernier diplome: %s\n",buff.T[j].dernier_diplome);
                        printf("-Etablissement universitaire: %s %s",buff.T[j].etablissement_univ.noun,buff.T[j].etablissement_univ.region);
                        printf("\n");

                        p=suivant(p);
                    }
                    else // code_univ != code qui se trouve dans le maillon donc on avance dans la liste
                    {
                    p=suivant(p);
                    }
                }
            }
            a++; // on avance dans la table d'index
        };
    };
   fermer(f,&fi);
}

/* ----- Consulter tous les enseignants exerçant dans une région donnée (Ouest, Centre et Est) ----- */

/*On utilise une table d'index qui contient 2 champs : region(est,ouest,centre) et la tête de la liste qui contient tous les enseignants exerçant dans cette région.
On commence par remplir la table d'index et bien évidemment la liste des enseignants de chaque région,pour celà on va récuperer la région de l'établissement
d'exercice de chaque enseignant, ensuite on affecte le num de bloc 'i' et la position 'j' dans le maillon de la liste qui est pointée par
l'élément de la table d'index contenant la même région */

/*Pour les maillons de la liste, on utilisera la même structure que celle de la requête à intervalle (question précédente) afin d'utiliser les mêmes
machines abstraites sauf que c'est n'est pas la peine de remplir le champs 'code_université' car on n'en aura pas besoin, on aura besoin juste du i et du j pour l'affichage*/

void Enseignant_par_region(char region[10])
{
    table index[3]; int k;
    /* -------- chargement de la table d'index --------- */
    FILE *g=fopen("fichier_index4.txt","r");
    if (g!=NULL)
    {
        k=0;
        while (k<3)
        {
            fscanf(g,"%s %p\n",&index[k].region,&index[k].tete);
            k++;
        };
    };
    fclose(g);

    Fichier fi; FILE *f; Tbloc buff;
    ptr p;
    ouvrir(&fi,"ENSEIGNANT-MESRS.dat",'a',&f);
    int i=1; int j;
    /* ----------------- Affichage --------------------*/
    if (strcmp(region,"est")==0)
    {
       p=index[0].tete; //car index[0].region contient "est"
       while (p!=NULL)
       {
          i=valeur(p,2);
          j=valeur(p,3);
          lireDir(f,i,&buff);
                        printf("\n-Matricule: %d\n",buff.T[j].matricule);
                        printf("-Nom et prenom: %s %s\n",buff.T[j].Nom,buff.T[j].Prenom);
                        printf("-Date de naissance: %d/%d/%d\n",buff.T[j].date_naissance.jour,buff.T[j].date_naissance.mois,buff.T[j].date_naissance.annee);
                        printf("-Wilaya de naissance: %s\n",buff.T[j].wilaya_naissance);
                        printf("-Sexe: %d\n",buff.T[j].sexe);
                        printf("-Group sanguin: %s\n",buff.T[j].groupe_sanguin);
                        printf("-Date de recrutement: %d/%d/%d\n",buff.T[j].date_recrutement.jour,buff.T[j].date_recrutement.mois,buff.T[j].date_recrutement.annee);
                        printf("-Grade: %s\n",buff.T[j].Grade);
                        printf("-Specialite: %s\n",buff.T[j].specialite);
                        printf("-Dernier diplome: %s\n",buff.T[j].dernier_diplome);
                        printf("-Etablissement universitaire: %s %s\n",buff.T[j].etablissement_univ.noun,buff.T[j].etablissement_univ.region);

           p=suivant(p);
    }
 };
  if (strcmp(region,"centre")==0)
 {
    p=index[1].tete;
    while (p!=NULL)
    {
        i=valeur(p,2);
        j=valeur(p,3);
        lireDir(f,i,&buff);
        printf("-Matricule: %d\n",buff.T[j].matricule);
                        printf("-Nom et prenom: %s %s\n",buff.T[j].Nom,buff.T[j].Prenom);
                        printf("-Date de naissance: %d/%d/%d\n",buff.T[j].date_naissance.jour,buff.T[j].date_naissance.mois,buff.T[j].date_naissance.annee);
                        printf("-Wilaya de naissance: %s\n",buff.T[j].wilaya_naissance);
                        printf("-Sexe: %d\n",buff.T[j].sexe);
                        printf("-Group sanguin: %s\n",buff.T[j].groupe_sanguin);
                        printf("-Date de recrutement: %d/%d/%d\n",buff.T[j].date_recrutement.jour,buff.T[j].date_recrutement.mois,buff.T[j].date_recrutement.annee);
                        printf("-Grade: %s\n",buff.T[j].Grade);
                        printf("-Specialite: %s\n",buff.T[j].specialite);
                        printf("-Dernier diplome: %s\n",buff.T[j].dernier_diplome);
                        printf("-Etablissement universitaire: %s %s\n",buff.T[j].etablissement_univ.noun,buff.T[j].etablissement_univ.region);
                        printf("\n");

                        p=suivant(p);
    }
 };
  if (strcmp(region,"ouest")==0)
 {
    p=index[2].tete;
    while (p!=NULL)
    {
        i=valeur(p,2);
        j=valeur(p,3);
        lireDir(f,i,&buff);
        printf("-Matricule: %d\n",buff.T[j].matricule);
                        printf("-Nom et prenom: %s %s\n",buff.T[j].Nom,buff.T[j].Prenom);
                        printf("-Date de naissance: %d/%d/%d\n",buff.T[j].date_naissance.jour,buff.T[j].date_naissance.mois,buff.T[j].date_naissance.annee);
                        printf("-Wilaya de naissance: %s\n",buff.T[j].wilaya_naissance);
                        printf("-Sexe: %d\n",buff.T[j].sexe);
                        printf("-Group sanguin: %s\n",buff.T[j].groupe_sanguin);
                        printf("-Date de recrutement: %d/%d/%d\n",buff.T[j].date_recrutement.jour,buff.T[j].date_recrutement.mois,buff.T[j].date_recrutement.annee);
                        printf("-Grade: %s\n",buff.T[j].Grade);
                        printf("-Specialite: %s\n",buff.T[j].specialite);
                        printf("-Dernier diplome: %s\n",buff.T[j].dernier_diplome);
                        printf("-Etablissement universitaire: %s %s\n",buff.T[j].etablissement_univ.noun,buff.T[j].etablissement_univ.region);
                        printf("\n");

                        p=suivant(p);
    }

};
fermer(f,&fi);
}


/* ------- Affichage de l'entête du fichier -------- */
void Affichage_entete()
{
    Fichier fi;FILE *f;
    ouvrir(&fi,"ENSEIGNANT-MESRS.dat",'a',&f);
    printf("- Nombre de blocs: %d\n\n",fi.entete.nb_blocs);
    printf("- Nombre d'enregistrements inseres: %d\n\n",fi.entete.nb_enreg_inseres);
    printf("- Premiere position libre (pour l'insertion): %d\n\n",fi.entete.position_libre);
    fermer(f,&fi);
}

int main()
{
//1
 system("COLOR FC");
 textbackground(15);
 printf("  ");
textbackground(0);
textcolor(0);
printf( " ______  ");

textbackground(15);
 printf("   ");
textbackground(0);
textcolor(0);
printf( "_______");
textbackground(15);
 printf("   ");
textbackground(1);
textcolor(1);
printf("| |\n");

//2
textbackground(15);
 printf("  ");
textbackground(0);
textcolor(0);
printf("|   __  |");
textbackground(15);
 printf("   ");
textbackground(0);
textcolor(0);
printf( "_______");
textbackground(15);
 printf("   ");
textbackground(1);
textcolor(1);
printf("| |\n");
//3
textbackground(15);
 printf("  ");
textbackground(0);
textcolor(0);
printf("|  |");textbackground(15);
 printf("   "); textbackground(0);
textbackground(0);
 printf("||");
 textbackground(15);
 printf("   ");
textbackground(0);
textbackground(0);
printf("| |\n");textbackground(15);

//4
textbackground(15);
 printf("  ");
textbackground(0);
textbackground(0);
printf("|  _____|");
textbackground(15);
 printf("   ");
textbackground(0);
textbackground(0);
printf("|  ___|");
textbackground(15);
 printf("   ");
textbackground(0);
textcolor(0);
printf("| |\n");


//5
textbackground(15);
 printf("  ");
textbackground(0);
textbackground(0);
printf("|  _____|");
textbackground(15);
 printf("   ");
textbackground(0);
textbackground(0);
printf("|  ___|");
textbackground(15);
 printf("   ");
textbackground(0);
textcolor(0);
printf("| |\n");

//6
textbackground(15);
 printf("  ");
 textbackground(0);
textbackground(0);
printf(" | |");
textbackground(15);
 printf("            ");
 textbackground(0);
textbackground(0);
printf(" ||");
textbackground(15);
 printf("   ");
textbackground(0);
textcolor(0);
printf("| |\n");

//7
textbackground(15);
 printf("  ");
 textbackground(0);
textbackground(0);
printf("|_______|");
textbackground(15);
 printf("   ");
 textbackground(0);
textbackground(0);
printf("|_____|");
textbackground(15);
 printf("   ");
textbackground(0);
textcolor(0);
printf("| |\n");
//8
textbackground(15);
 printf("  ");
 textbackground(0);
textbackground(0);
printf("|_______|");
textbackground(15);
 printf("   ");
 textbackground(0);
textbackground(0);
printf("|_____|");
textbackground(15);
 printf("   ");
textbackground(0);
textcolor(0);
printf("| |\n");
textcolor(0);
 textbackground(15);
 gotoxy(34,2);
char ab[]="ECOLE NATIONALE SUPERIEURE D'INFORMATIQUE ";
for (int i=0;i<strlen(ab);i++)
{
   putchar(ab[i]);
     Sleep(20);
}
gotoxy(49,5);
textcolor(8);
printf("TP1-SFSD ");
gotoxy(36,12);
 textcolor(12);
 printf("MESRS\n");
 textcolor(0);
char cd[]="      MINISTERE DE L'ENSEIGNEMENT SUPERIREUR ET DE LA RECHERCHE SCIENTIFIQUE\n\n\n";
 for (int i=0;i<strlen(cd);i++)
{
   putchar(cd[i]);
     Sleep(20);
}
 textbackground(1);
 textcolor(1);
printf("      Ministre de l'Enseignement Supérieur et de la Recherche Scientifique      \n\n");
textbackground(15);
textcolor(2);
printf("\t\t   Chargement initial du fichier MESRS.bin\n\n");

   int nb_enreg,taille_bloc;int repeat=1;int choix;
   int matricule;char specialite[20];int code_univ,code,current_year,a,b;char region[6];char name[50]; etablissement etab[108];

   textcolor(0);
   printf("- Veuillez introduire le nombre d'enseignants : ");
   textcolor(2);
   scanf("%d",&nb_enreg);
   textcolor(0);
   printf("\n- Veuillez introduire la taille maximale du bloc : ");
   textcolor(2);
   scanf("%d",&taille_bloc);
   chargement_initial(nb_enreg,taille_bloc);
   textcolor(0);
   printf("\n- Veuillez introduire l'annee courante : ");
   textcolor(2);
   scanf("%d",&current_year);
   Epuration(current_year);
   textbackground(1);
 textcolor(1);
printf("\n\n      Ministre de l'Enseignement Supérieur et de la Recherche Scientifique      \n\n");
repeat=1; int cpt=1;
  while (repeat!=0)
  {
      if (cpt==1)
      {
          gotoxy(30,27);
textbackground(1);
 textcolor(14);
 printf("~~~~~~~~~~~~~~~~~~");
gotoxy(36,27);
textbackground(1);
 textcolor(14);
        printf(" MENU \n\n\n\n");

//1
textbackground(15);
 textcolor(15);
 printf("\t   ");
textbackground(1);
 textcolor(14);
printf(" 1 ");
textbackground(15);
 textcolor(0);
 char ab[]=" Ajouter un nouvel enseignant au fichier\n\n";
  int i;
  for(i=0;i<strlen(ab);i++){
     putchar(ab[i]);
     Sleep(10);
                            }

  //2
  textbackground(15);
 textcolor(15);
 printf("\t   ");
textbackground(14);
 textcolor(15);
printf(" 2 ");
textbackground(15);
 textcolor(0);
 char ba[]=" Modifier l'etablissement d'un enseignant en raison de sa mutation\n\n";
   i;
  for(i=0;i<strlen(ba);i++){
     putchar(ba[i]);
     Sleep(10);
                            }

   //3
   textbackground(15);
 textcolor(15);
 printf("\t   ");
textbackground(1);
 textcolor(14);
printf(" 3 ");
textbackground(15);
 textcolor(0);
 char c[]=" Supprimer un enseingnant donne par le matricule\n\n";
   i;
  for(i=0;i<strlen(c);i++){
     putchar(c[i]);
     Sleep(10);
                            }

//4
   textbackground(15);
 textcolor(15);
 printf("\t   ");
textbackground(14);
 textcolor(15);
printf(" 4 ");
textbackground(15);
 textcolor(0);
 char d[]=" Supprimer une specialite\n\n";
   i;
  for(i=0;i<strlen(d);i++){
     putchar(d[i]);
     Sleep(10);
                            }

//5
   textbackground(15);
 textcolor(15);
 printf("\t   ");
textbackground(1);
 textcolor(14);
printf(" 5 ");
textbackground(15);
 textcolor(0);
 char e[]=" Afficher les enseignants par etablissement et anciennete\n\n";
   i;
  for(i=0;i<strlen(e);i++){
     putchar(e[i]);
     Sleep(10);
                            }


  //6
   textbackground(15);
 textcolor(15);
 printf("\t   ");
textbackground(14);
 textcolor(15);
printf(" 6 ");
textbackground(15);
 textcolor(0);
 char o[]=" Consulter les enseignants exercant dans une region donnee\n\n";
   i;
  for(i=0;i<strlen(o);i++){
     putchar(o[i]);
     Sleep(10);
                            }

  //7
   textbackground(15);
 textcolor(15);
 printf("\t   ");
textbackground(1);
 textcolor(14);
printf(" 7 ");
textbackground(15);
 textcolor(0);
 char g[]=" Afficher l'entete du fichier \n\n";
   i;
  for(i=0;i<strlen(g);i++){
     putchar(g[i]);
     Sleep(10);
                            }

  //0
   textbackground(15);
 textcolor(15);
 printf("\t  \t \t      ");
textbackground(14);
 textcolor(15);
printf(" 0 ");
textbackground(15);
 textcolor(0);
 char h[]=" Quitter \n\n";
   i;
  for(i=0;i<strlen(h);i++){
     putchar(h[i]);
     Sleep(10);
                            }
   textcolor(12);
        cprintf("\t \t   Veuillez introduire votre choix : ");
        textbackground(15);
        textcolor(0);
        scanf("\%d",&choix);
        if (choix==0) repeat=0;
      }
      else {
gotoxy(30,0);
textbackground(1);
 textcolor(14);printf("\n                              ~~~~~~ MENU ~~~~~~                                \n\n\n\n");

//1
textbackground(15);
 textcolor(15);
 printf("\t   ");
textbackground(1);
 textcolor(14);
printf(" 1 ");
textbackground(15);
 textcolor(0);
 char ab[]=" Ajouter un nouvel enseignant au fichier\n\n";
  int i;
  for(i=0;i<strlen(ab);i++){
     putchar(ab[i]);
     Sleep(10);
                            }

  //2
  textbackground(15);
 textcolor(15);
 printf("\t   ");
textbackground(14);
 textcolor(15);
printf(" 2 ");
textbackground(15);
 textcolor(0);
 char ba[]=" Modifier l'etablissement d'un enseignant en raison de sa mutation\n\n";
   i;
  for(i=0;i<strlen(ba);i++){
     putchar(ba[i]);
     Sleep(10);
                            }

   //3
   textbackground(15);
 textcolor(15);
 printf("\t   ");
textbackground(1);
 textcolor(14);
printf(" 3 ");
textbackground(15);
 textcolor(0);
 char c[]=" Supprimer un enseingnant donne par le matricule\n\n";
   i;
  for(i=0;i<strlen(c);i++){
     putchar(c[i]);
     Sleep(10);
                            }

//4
   textbackground(15);
 textcolor(15);
 printf("\t   ");
textbackground(14);
 textcolor(15);
printf(" 4 ");
textbackground(15);
 textcolor(0);
 char d[]=" Supprimer une specialite\n\n";
   i;
  for(i=0;i<strlen(d);i++){
     putchar(d[i]);
     Sleep(10);
                            }

//5
   textbackground(15);
 textcolor(15);
 printf("\t   ");
textbackground(1);
 textcolor(14);
printf(" 5 ");
textbackground(15);
 textcolor(0);
 char e[]=" Afficher les enseignants par etablissement et anciennete\n\n";
   i;
  for(i=0;i<strlen(e);i++){
     putchar(e[i]);
     Sleep(10);
                            }


  //6
   textbackground(15);
 textcolor(15);
 printf("\t   ");
textbackground(14);
 textcolor(15);
printf(" 6 ");
textbackground(15);
 textcolor(0);
 char o[]=" Consulter les enseignants exercant dans une region donnee\n\n";
   i;
  for(i=0;i<strlen(o);i++){
     putchar(o[i]);
     Sleep(10);
                            }

  //7
   textbackground(15);
 textcolor(15);
 printf("\t   ");
textbackground(1);
 textcolor(14);
printf(" 7 ");
textbackground(15);
 textcolor(0);
 char g[]=" Afficher l'entete du fichier \n\n";
   i;
  for(i=0;i<strlen(g);i++){
     putchar(g[i]);
     Sleep(10);
                            }

  //0
   textbackground(15);
 textcolor(15);
 printf("\t  \t \t      ");
textbackground(14);
 textcolor(15);
printf(" 0 ");
textbackground(15);
 textcolor(0);
 char h[]=" Quitter \n\n";
   i;
  for(i=0;i<strlen(h);i++){
     putchar(h[i]);
     Sleep(10);
                            }
   textcolor(12);
        cprintf("\t \t   Veuillez introduire votre choix : ");
        textbackground(15);
        textcolor(0);
        scanf("\%d",&choix);
        if (choix==0) repeat=0;}

   switch(choix)
   {
   case 1:
       textbackground(2);
       textcolor(15);
       printf("\n************************ Ajout d'un nouvel enseignant **************************\n\n");
       textbackground(15);
       textcolor(0);
       Ajout_enseignant(current_year);
       break;

   case 2:
       textbackground(13);
       textcolor(15);
       printf("\n*** Modification de l'etablissement d'un enseignant en raison de sa mutation  ***\n\n");
       textbackground(15);
       textcolor(5);
       printf("* ~~~~~~~~~~~~~~~~ Index Matricule ~~~~~~~~~~~~~~~~~ *\n");
       printf("                   ~~~~~~~~~~~~~~~~                    \n");
       FILE* p=fopen("fichier_index1.txt","r");
       if(p!=NULL)
       {
           while(!feof(p))
           {
               fscanf(p,"%d %d %d",&matricule,&a,&b);
               printf("                  | %d | %d | %d |\n",matricule,a,b);
           }
           fclose(p);
       }
       textbackground(13);
       textcolor(15);
       printf("\n -> Veuillez introduire le matricule de l'enseignant :");
       textbackground(15);
       textcolor(0);
       scanf("%d",&matricule);
       Modification_etablissement(matricule);
       break;

   case 3:
       textbackground(14);
       textcolor(15);
       printf("/* ------------ Suppression d'un enseignant ------------- */\n\n");
       textcolor(6);
       textbackground(15);
       printf("* ~~~~~~~~~~~~~~~~ Index Matricule ~~~~~~~~~~~~~~~~~ *\n");
       printf("                   ~~~~~~~~~~~~~~~~                    \n");
       FILE* q=fopen("fichier_index1.txt","r");
       if(q!=NULL)
       {
           while(!feof(q))
           {
               fscanf(q,"%d %d %d",&matricule,&a,&b);
               printf("                  | %d | %d | %d |\n",matricule,a,b);
           }
           fclose(q);
       }
       textbackground(14);
       textcolor(15);
       printf("\nVeuillez introduire le matricule de l'enseignant que vous voulez supprimer :");
       textcolor(0);
       textbackground(15);
       scanf("%d",&matricule);
       Suppression_enseignant(matricule,current_year);
       textcolor(6);
       textbackground(15);
       printf("\n* ~~~~~~~~~~~~~~~~ Index Matricule ~~~~~~~~~~~~~~~~~ *\n");
       printf("                   ~~~~~~~~~~~~~~~~                    \n");
        q=fopen("fichier_index1.txt","r");
       if(q!=NULL)
       {
           while(!feof(q))
           {
               fscanf(q,"%d %d %d",&matricule,&a,&b);
               printf("                  | %d | %d | %d |\n",matricule,a,b);
           }
           fclose(q);
       }
       break;

   case 4:
       textbackground(7);
       textcolor(13);
       printf("/*************** Suppression d'une specialite ***************/\n\n");
       textbackground(15);
       textcolor(8);
       printf("* ~~~~~~~~~~~~~~~~~ Index Specialite ~~~~~~~~~~~~~~~~ *\n");
       printf("                   ~~~~~~~~~~~~~~~~                    \n");
       FILE* s=fopen("fichier_index2.txt","r");
       ptr r;
       int w=0;
       if(s!=NULL)
       {
           while(w<20) //20: le nb de spécialités contenues dans le fichier
           {
               fscanf(s,"%s %p",&specialite,&r);
               printf(" %s \n",specialite);
               w++;
           }
           fclose(s);
       }
       textbackground(7);
       textcolor(13);
       printf("\nVeuillez introduire la specialite que vous voulez supprimer :");
       textbackground(15);
       textcolor(0);
       scanf("%s",&specialite);
       Suppression_specialite(specialite,current_year);
       textbackground(15);
       textcolor(8);
       printf("* ~~~~~~~~~~~~~~~ Index Specialite ~~~~~~~~~~~~~~~~ *\n");
       printf("                   ~~~~~~~~~~~~~~~~                    \n");
       s=fopen("fichier_index2.txt","r");
       w=0;
       if(s!=NULL)
       {
           while(w<19) //on décremente le nb de spécilités car on a déjà supprimer une parmis eux
           {
               fscanf(s,"%s %p",&specialite,&r);
               printf(" %s \n",specialite);
               w++;
           }
           fclose(s);
       }
       break;

   case 5:
       etab[108];
       FILE* m=fopen("etablissement1.txt","r");
    int a=0;
    if (m!=NULL)
    {
        while((a<108))
            {
               fscanf(m,"%d %s %s",&etab[a].code,&etab[a].noun,&etab[a].region);
               a++;
               if(a==100)
               {
                   m=fopen("etablissement2.txt","r");
                   if (m!=NULL)
                   {
                       while (a<108)
                       {
                           fscanf(m,"%d %s %s",&etab[a].code,&etab[a].noun,&etab[a].region);
                           a++;
                       }
                       fclose(m);
                   }
               };
            };
            fclose(m);
    }

    a=0;
    while (a<108)
    {
        printf("%d %s %s\n",etab[a].code,etab[a].noun,etab[a].region);
        a++;
    }
    textbackground(3);
       textcolor(15);
    printf("\n*** Affichage des enseingants exercant dans un etablissement donne et ayant une anciennete entre 2 valeurs ***\n\n");
        textbackground(15);
       textcolor(3);
       printf("\n- Choisissez un code de l'etablissement que vous voulez :\n");
        textbackground(15);
       textcolor(0);
       scanf("%d",&code_univ);
       textbackground(15);
       textcolor(3);
       printf("- Vous voulez une anciennete entre :");
       textbackground(15);
       textcolor(0);
       scanf("%d %d",&a,&b);
       Requete_a_intervalle(code_univ,current_year,a,b);
       break;

   case 6:
       textbackground(5);
       textcolor(15);
            printf("******** Consultation des enseignants exercant dans une region donnee ********\n\n");
            textbackground(15);
            textcolor(5);
            printf("Veuillez introduire la region (est/centre/ouest) :");
            textbackground(15);
            textcolor(0);
            scanf("%s",&region);
            Enseignant_par_region(region);
            break;

   case 7:
           textbackground(0);
           textcolor(15);
           printf("******** Affichage de l'entete du fichier ENSEIGNANT-MESRS.dat ********\n\n");
           textbackground(15);
           textcolor(0);
           Affichage_entete(); // le nombre d'enregistrements insérés peut se diminuer suite à l'épuration
           break;}
           cpt++;
           Sleep(50);
  }
      textbackground(15);
           textcolor(1);
           printf("\n\n      Ministre de l'Enseignement Supérieur et de la Recherche Scientifique      \n\n");
textbackground(15);
textcolor(15);
 if (choix==0) return 0;
}


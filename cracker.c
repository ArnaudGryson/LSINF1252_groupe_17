#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <semaphore.h>

int nthreads=1; //nombre maximal de thread par defaut 
int voyelle=1;  //par defaut on compte les voyelles
int fini1=0;    // variable indiquant si le thread de lecture a fini son execution
int fini2=0;    // variable indiquant si les threads de calcul ont fini leurs executions

pthread_mutex_t mutex1; //mutex et sémaphores du premier producteur-consommateur
sem_t empty1;
sem_t full1;

pthread_mutex_t mutex2; //mutex et sémaphores du premier producteur-consommateur
sem_t empty2;
sem_t full2;

typedef struct node {  //structure representant un noeud
   struct node *next;
   char value[];
} node_t;

typedef struct list {  //structure representant une liste de noeud
   struct node *first;
   int size;
} list_t;

typedef struct mesParams1{  // structure permettant de stocker les arguments du premier type de thread
   list_t fich;
   uint8_t* buffer1[];
} params1_t;

typedef struct mesParams2{ //structure permettant de stocker les arguments du second type de thread
   uint8_t* buffer1[];
   char* buffer2[];
} params2_t;

typedef struct mesParams3{ //structure permettant de stocker les arguments du troisième type thread
   char* buffer2[];
   list_t candi;
} params3_t;

int main(int argc, char* argv[]) {  //fonction main

   int fileo=0; //par defaut il n'y a pas de fichier de sortie
   char* fileout;
   list_t* filein = (list_t*)malloc(sizeof(list_t)); //liste qui contiendra les fichiers entrants
   if(filein==NULL){
      return -1;
   }
   filein->first=NULL;
   filein->size=0;

   for(int i=1;i<argc;i++){ //parcours des arguments
      if(strcmp(argv[i],"-t")==0){
         nthreads=atoi(argv[i+1]); //nombre de threads maximales
         i++;
      }
      else if(strcmp(argv[i],"-c")==0){
         voyelle=0; //les consonnes sont maintenant comptées
      }
      else if(strcmp(argv[i],"-o")==0){
         fileo=1;   // fichier de sortie pour y écrire les canditats finaux
         fileout=argv[i+1];
         i++;
      }
      else{  //les autres arguments sont les fichiers d'entrée
         node_t* nod = (node_t*)malloc(sizeof(node_t));
         if(nod==NULL){
            return -1;
         }
         nod->value=argv[i];
         nod->next=filein->first;
         filein->first=nod;
         filein->size++;
      }
   }
   uint8_t* buffer1[] = (uint8_t*)malloc(nthreads*sizeof(uint8_t)*32); //creation du premier buffer
   if(buffer1==NULL){
      return -1;
   }
   for(int i=0;i<nthreads;i++){ //mise a zero du premier buffer
      buffer1[i]=NULL;
   }
   char* buffer2[] = (char*)malloc(16*nthreads*sizeof(char)); //creation du second buffer
   if(buffer2==NULL){ 
      return -1;
   }
   for(int i=0;i<nthreads;i++){ //mise à zero du second buffer
      buffer2[i]=NULL;
   }
   list_t* candi=(list_t*)malloc(sizeof(list_t)); //creation de la liste de candidats
   if(candi==NULL){
      return -1;
   }
   candi->first=NULL;
   candi->size=0;

   pthread_t lect; //premiere type de thread, lisant les fichiers d'entrée
   pthread_t calc[nthreads]; //second type de thread, crackant les mots de passe
   pthread_t cand; // troisième type de thread, triant les candidats

   pthread_mutex_init(&mutex1,NULL); //initialisation des mutex et sémaphores
   sem_init(&empty1,0,nthreads);
   sem_init(&full1,0,0);
   pthread_mutex_init(&mutex2,NULL);
   sem_init(&empty2,0,nthreads);
   sem_init(&full2,0,0);

   params1_t* paramlect=(params1_t*)malloc(sizeof(params1_t)); //creation des arguments donnés au premier type de thread
   if(paramlect==NULL){
      return -1;
   }
   paramlect->fich = *filein;
   paramlect->buffer1 = buffer1;

   params2_t* paramcalc=(params2_t*)malloc(sizeof(params2_t)); //creation des arguments donnés au second type de thread
   if(paramcalc==NULL){
      return -1;
   }
   paramcalc->buffer1=buffer1;
   paramcalc->buffer2=buffer2;

   params3_t* paramcand=(params3_t*)malloc(sizeof(params3_t)); //creation des arguments donnés au troisième type de thread
   if(paramcand==NULL){
      return -1;
   }
   paramcand->buffer2=buffer2;
   paramcand->candi=*candi;

   int lect1= pthread_create(&lect,NULL,&lecture,(void*)paramlect); //creation du thread de lecture
   if(lect1!=0){
      return -2;
   }
   for(int i=0;i<nthreads;i++){  //creation des threads de calcul
      int calc1= pthread_create(&calc[i],NULL,&calcul,(void*)paramcalc);
      if(calc1!=0)
         return-2;
      }
   }
   int cand1= pthread_create(&cand,NULL,&candidat,(void*)paramcand); //creation du thread de tri
   if(cand1!=0){
      return -2;
   }

   int ptj = pthread_join(cand,NULL);
   if(ptj!=0){
      return -5;
   }
   if(fileo!=0){ //ouverture du fichier de sortie
      int fd=open(fileout,O_RDONLY|O_CREAT|O_TRUNC);
      if(fd==-1){
         return -3;
      }
   }
   else{
      fd=1;
   }
   for(int i=0;i<(candi->size);i++){  //ecriture des resultats
      int erf = write(fd,(void*) candi->value, strlen(candi->value));
      if(erf==-1){
         return -7;
      }
      candi=candi->next;
   }
   int clos=close(fd);
   if(clos==-1){
      return -6;
   }
   while(node_t curr1=(filein->first))!=NULL){    //libération de la mémoire allouée par malloc
      filein->first=(filein->first)->next;
      free(curr1);
   }
   free(filein);
   for(int i=0;i<nthreads;i++){
      free(buffer1[i]);
      free(buffer2[i]);
   }
   while(node_t curr2=(candi->first))!=NULL){
      candi->first=(candi->first)->next;
      free(curr2);
   }
   free(candi);
   return(EXIT_SUCCESS);
}




void *lecture(void* param){  //fonction executée par le thread de lecture
   param=(params1_t*)param;
   node_t noch = param->filein->first;
   while(noch!=NULL){
      int fd = open(noch->value,O_RDONLY); //ouverture d'un fichier entrant
      if(fd==-1){
         return -3; 
      }
      uint8_t a[32];
      int s=-1;
      while(s!=0){
         s=read(fd,&a,32*sizeof(uint8_t)); //lecture d'un mot de passe crypté
         if(s==-1){
            return -3;
         }
         sem_wait(&empty1);
         pthread_mutex_lock(&mutex);
         for(int i=0;i<nthreads;i++){
            if((param->buffer1)[i]==NULL){ //le mot de passe crypté est mis dans le buffer1
               (param->buffer1)[i]=a;
               i=nthreads;
            }
         }
         pthread_mutex_unlock(&mutex1);
         sem_post(&full1);
      }
      int clos = close(fd);
      if(clos==-1){
          return -6;
      }
      noch=noch->next; //passage au prochain fichier entrant
   }
   fini1=1;
   pthread_exit(); //fermeture du thread quand tous les fichiers ont été lus
}



void *calcul(void* param){ //fonction executée par le thread de calcul
   param=(params2_t*)param;
   uint8_t a[32];
   char* b;
   while(true){
      sem_wait(&full1);
      pthread_mutex_lock(&mutex1);
      for(int i=0;i<nthreads;i++){ 
         if((param->buffer1)[i]!=NULL){ //lecture d'un mot de passe crypté du buffer1
            a=(param->buffer1)[i];
            (param->buffer1)[i]=NULL;
            i=nthreads;
         }
      }
      pthread_mutex_unlock(&mutex1);
      sem_post(&empty1);
      bool okay = reversehash(&a,&b,16*sizeof(char)); //crackage du mot de passe
      if(!okay){
         return -4;
      }
      sem_wait(&empty2);
      pthread_mutex_lock(&mutex2);
      for(int i=0;i<nthreads;i++){
         if((param->buffer2)[i]==NULL){ //ecriture du mot de passe décrypté dans le buffer2
            (param->buffer2)[i]=b;
            i=nthreads;
         }
      }
      pthread_mutex_unlock(&mutex2);
      sem_post(&full2);
   }
   if(fini1==1){ //si le thread de lecture est fermé et le buffer1 est vide on peut fermer les threads de calcul
      int vide1=1;
      for(int i=0;i<nthreads;i++){
         if((param->buffer1)[i]!=NULL){
            vide1=0;
         }
      }
      if(vide1==1){
         fini2++;
         pthread_exit();
      }
   }
}

void* candidat(void* param){ //fonction utilisée par le thread de tri
   param=(params3_t*)param;
   int nMax=1; //nombre maximale de voyelle (ou de consonne)
   char* b;

   while(true){
      sem_wait(&full2);
      pthread_mutex_lock(&mutex2);
      for(int i=0;i<nthreads;i++){
         if((param->buffer2)[i]!=NULL){ //lecture d'un mot de passe décrypté du buffer2
            b=(param->buffer2)[i];
            (param->buffer2)[i]=NULL;
            i=nthreads;
         }
      }
      pthread_mutex_unlock(&mutex2);
      sem_post(&empty2);
      int n=0;
      for(int j=0; *(b+j)!='\0';j++){ //comptage du nombre de voyelle ou de consonne
         if(voyelle==1){
            if(*(b+j)=='a'||*(b+j)=='e'||*(b+j)=='i'||*(b+j)=='o'||*(b+j)=='u'||*(b+j)=='y'){
               n=n+1;
            }
         }
         else {
            if(!(*(b+j)=='a'||*(b+j)=='e'||*(b+j)=='i'||*(b+j)=='o'||*(b+j)=='u'||*(b+j)=='y')){
               n=n+1;
            }
         }
      }
      if(nMax==n){ //le mot de passe est ajouté à la liste de candidats
         node_t* nod = (node_t*)malloc(sizeof(node_t));
         if(nod==NULL){
            return -1;
         }
         nod->value=b;
         nod->next=(param->candi)->first;
         (param->candi)->first=nod;
         (param->candi)->size++;
      }
      if(n>Nmax){ //la liste de candidats est réinitialisée et le mot de passe y est stocké
         nMax=n;
         (param->candi)->first=NULL;
         (param->candi)->size=0;
         node_t* nod = (node_t*)malloc(sizeof(node_t));
         if(nod==NULL){
            return -1;
         }
         nod->value=b;
         nod->next=(param->candi)->first;
         (param->candi)->first=nod;
         (param->candi)->size++;
      }
   }
   if(fini2==nthreads){ //si les threads de calcul sont fermés et le buffer2 est vide on peut fermer le thread de tri 
      int vide2=1;
      for(int i=0;i<nthreads;i++){
         if((param->buffer2)[i]!=NULL){
            vide2=0;
         }
      }
      if(vide2==1){
         pthread_exit();
      }
   }
}

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

int nthreads=1;
int voyelle=1;

pthread_mutex_t mutex1;
sem_t empty1;
sem_t full1;

pthread_mutex_t mutex2;
sem_t empty2;
sem_t full2;

int main(int argc, char* argv[]) {

typedef struct node {
   struct node *next;
   char value[];
} node_t;

typedef struct list {
   struct node *first;
   int size;
} list_t;

typedef struct mesParams1{
   list_t fich;
   uint8_t* buffer1[];
} params1_t;

typedef struct mesParams2{
   uint8_t* buffer1[];
   char* buffer2[];
} params2_t;

typedef struct mesParams3{
   char* buffer2[];
   list_t candi;
} params3_t;

int* fileo=malloc(sizeof(int));
if(fileo==NULL){
   return -1;
}
*fileo=0;
char* fileout;
list_t* filein = (list_t*)malloc(sizeof(list_t));
if(filein==NULL){
   return -1;
}
filein->first=NULL;
filein->size=0;
for(i=1;i<argc;i++){
   if(strcmp(argv[i],"-t")==0){
      nthreads=atoi(argv[i+1]);
      i++;
   }
   else if(strcmp(argv[i],"-c")==0){
      voyelle=0;
   }
   else if(strcmp(argv[i],"-o")==0){
      *fileo=1;
      fileout=(char*)malloc(sizeof(char)*strlen(argv[i+1]);
      fileout=argv[i+1];
      i++;
   }
   else{
   node_t* nod = (node_t*)malloc(sizeof(node_t));
   if(nod==NULL){
   return -1;
   }
   nod->value=argv[i];
   nod->next=filein->first;
   filein->first=nod;
   filein->size++;
   }

uint8_t* buffer1[] = (uint8_t*)malloc(nthreads*sizeof(uint8_t)*32);
if(buffer1==NULL){
   return -1;
}
for(int i=0;i<nthreads;i++){
   *buffer1[i]=NULL;
}
char* buffer2[] = (char*)malloc(16*nthreads*sizeof(char));
if(buffer2==NULL){
   return -1;
}
for(int	i=0;i<nthreads;i++){
   *buffer2[i]=NULL;
}
list_t* candi=(list_t*)malloc(sizeof(list_t));
if(candi==NULL){
   return -1;
}
candi->first=NULL;
candi->size=0;

pthread_t lect;
pthread_t calc[nthreads];
pthread_t cand;

pthread_mutex_init(&mutex1,NULL);
sem_init(&empty1,0,nthreads);
sem_init(&full1,0,0);
pthread_mutex_init(&mutex2,NULL);
sem_init(&empty2,0,nthreads);
sem_init(&full2,0,0);

params1_t* paramlect=(params1_t*)malloc(sizeof(params1_t));
if(paramlect==NULL){
   return -1;
}
paramlect->fich = filein;
paramlect->buffer1 = buffer1;

params2_t* paramcalc=(params2_t*)malloc(sizeof(params2_t));
if(paramcalc==NULL){
   return -1;
}
paramcalc->buffer1=buffer1;
paramcalc->buffer2=buffer2;

params3_t* paramcand=(params3_t*)malloc(sizeof(params3_t));
if(paramcand==NULL){
   return -1;
}
paramcand->buffer2=buffer2;
paramcand->candi=candi;

int lect1= pthread_create(&lect,NULL,&lecture,(void*)paramlect);
if(lect1!=0){
 return -2;
}
for(int i=0;i<nthreads;i++){
int calc1= pthread_create(&calc[i],NULL,&calcul,(void*)paramcalc);
if(calc1!=0)
   return-2;
}
int cand1= pthread_create(&cand,NULL,&candidat,(void*)paramcand);
if(cand1!=0){
   return -2;
}

}




void *lecture(void* param){
param=(params1_t*)param;
node_t noch = param->filein->first;
while(noch!=NULL){
   int fd = open(noch->value,O_RDONLY);
   if(fd==-1){
      return -3; 
   }
   uint8_t a[32];
   int s=-1;
   while(s!=0){
      s=read(fd,&a,32*sizeof(uint8_t));
      if(s==-1){
         return -3;
      }
      sem_wait(&empty1);
      pthread_mutex_lock(&mutex);
      for(int i=0;i<nthreads;i++){
         if((param->buffer1)[i]==NULL){
            (param->buffer1)[i]=a;
            i=nthreads;
         }
      }
      pthread_mutex_unlock(&mutex1);
      sem_post(&full1);
   }
   noch=noch->next;
}
pthread_exit();
}



void *calcul(void* param){
param=(params2_t*)param;
uint8_t a[32];
char* b;
while(true){
   sem_wait(&full1);
   pthread_mutex_lock(&mutex1);
   for(int i=0;i<nthreads;i++){
      if((param->buffer1)[i]!=NULL){
         a=(param->buffer1)[i];
         (param->buffer1)[i]=NULL;
         i=nthreads;
      }
      if(i==(nthreads-1){
      pthread_exit();
      }
   }
   pthread_mutex_unlock(&mutex1);
   sem_post(&empty1);
   bool okay = reversehash(&a,&b,16*sizeof(char)); 
   if(!okay){
      return -4;
   }
   sem_wait(&empty2);
   pthread_mutex_lock(&mutex2);
   for(int i=0;i<nthreads;i++){
      if((param->buffer2)[i]==NULL){
         (param->buffer2)[i]=b;
         i=nthreads;
      }
   }
   pthread_mutex_unlock(&mutex2);
   sem_post(&full2);
}
}

void* candidat(void* param){
param=(params3_t*)param;
int nMax=1;
char* b;

while(true){
   sem_wait(&full2);
   pthread_mutex_lock(&mutex2);
   for(int i=0;i<nthreads;i++){
      if((param->buffer2)[i]!=NULL){
         b=(param->buffer2)[i];
         (param->buffer2)[i]=NULL;
         i=nthreads;
      }
   }
   pthread_mutex_unlock(&mutex2);
   sem_post(&empty2);
   int n=0;
   for(int j=0; *(b+j)!='\0';j++){
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
   if(nMax==n){
      node_t* nod = (node_t*)malloc(sizeof(node_t));
      if(nod==NULL){
         return -1;
      }
      nod->value=b;
      nod->next=(param->candi)->first;
      (param->candi)->first=nod;
      (param->candi)->size++;
   }
   if(n>Nmax){
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

      
   






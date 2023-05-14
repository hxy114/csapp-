#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
int s,E,b;
typedef struct node{
    unsigned long tag;
    struct node *next;
    struct node * pre;
}node;
void load(unsigned  long address,node *freelist,node *lrulist,int *hits,int *misses,int *evictions);
void store(unsigned  long address,node *freelist,node *lrulist,int *hits,int *misses,int *evictions);
void jiexi(const char *strLine,int *mode,unsigned long *address);
void init(int s,int E,node *freelist,node * lrulist);
int main(int argc, char *  argv[])
{
    node freelist[100];
    node lrulist[100];
    int o;

    int hits=0,misses=0,evictions=0;
    char trace[100];
    const char *optstring = "s:E:b:t:"; // 有三个选项-abc，其中c选项后有冒号，所以后面必须有参数
    while ((o = getopt(argc, argv, optstring)) != -1) {
        switch (o) {
            case 's':
                s= atoi(optarg);
                break;
            case 'E':
                E= atoi(optarg);
                break;
            case 'b':
                b= atoi(optarg);
                break;
            case 't':
                strcpy(trace,optarg);
                break;
            default:
                break;
        }

    }
    printf("%d\n",s);
    printf("%d\n",E);
    printf("%d\n",b);
    printf("%s\n",trace);
    init(s,E,freelist,lrulist);
    FILE *fptr;
    char strLine[100];
    fptr = fopen(trace, "r");
    while (!feof(fptr))									//循环读取每一行，直到文件尾
    {
        strLine[0]='I';
        fgets(strLine,100,fptr);					    //将fp所指向的文件一行内容读到strLine缓冲区
        if(strLine[0]!='I'){
            int mode;
            unsigned long address=0;
            jiexi(strLine,&mode,&address);
            if(mode==0){
                load(address,freelist,lrulist,&hits,&misses,&evictions);
            }else if(mode==1){
                store(address,freelist,lrulist,&hits,&misses,&evictions);

            }else if(mode==2){
                printf("M---------------------------------------\n");
                load(address,freelist,lrulist,&hits,&misses,&evictions);
                store(address,freelist,lrulist,&hits,&misses,&evictions);
                printf("M---------------------------------------\n");

            }
        }
          					//输出所读到的内容
        //DO SOMETHING ELSE
    }
    fclose(fptr);






    printSummary(hits, misses, evictions);
    return 0;
}
void init(int s,int E,node *freelist,node * lrulist){
    int set=1<<s;
    for(int i=0;i<set;i++){
        lrulist[i].next=NULL;
        lrulist[i].pre=NULL;
        freelist[i].next=NULL;
        freelist[i].pre=NULL;
        for(int j=0;j<E;j++){
            node *ptr=(node *) malloc(sizeof(node));
            ptr->next=freelist[i].next;
            ptr->pre=&freelist[i];
            if(freelist[i].next!=NULL){
                freelist[i].next->pre=ptr;
            }
            freelist[i].next=ptr;
        }
    }
}
void jiexi(const char * strLine,int *mode,unsigned long *address){
    if( strLine[1] == 'L'){
        *mode=0;
    }else if(strLine[1]=='S'){
        *mode=1;
    }else{
        *mode=2;
    }
    for(int i=3;i<100;i++){
        if(strLine[i]!=','){
            if(strLine[i]>='0'&&strLine[i]<='9'){
                *address=*address*16+(strLine[i]-'0');
            }else if(strLine[i]>='a'&&strLine[i]<='f'){
                *address=*address*16+(strLine[i]-'a'+10);
            }
        }else{
            break;
        }

    }
}
void load(unsigned  long address,node *freelist,node *lrulist,int *hits,int *misses,int *evictions){
    printf("load address:%lx",address);
    unsigned long tag=address>>(s+b),temp=address>>b;
    int set=0;
    for(int i=0;i<s;i++){
        set=set+((temp&0x1)<<i);
        temp=temp>>1;
    }
    node *lru=lrulist[set].next;
    node *prelru=&lrulist[set];
    while(lru!=NULL){
        if(lru->tag==tag){
            printf("  hit\n");
            (*hits)++;
            lru->pre->next=lru->next;
            if(lru->next!=NULL){
                lru->next->pre=lru->pre;
            }
            lru->next=lrulist[set].next;
            lru->pre=&lrulist[set];
            if(lrulist[set].next!=NULL){
                lrulist[set].next->pre=lru;
            }

            lrulist[set].next=lru;
            return;
        }
        prelru=lru;
        lru=lru->next;
    }
    if(freelist[set].next!=NULL){
        printf("  miss\n");
        (*misses)++;
        node *ptr=freelist[set].next;
        ptr->tag=tag;
        if(ptr->next!=NULL){
            ptr->next->pre=&freelist[set];
        }

        freelist[set].next=ptr->next;

        ptr->next=lrulist[set].next;
        ptr->pre=&lrulist[set];
        if(lrulist[set].next!=NULL){
            lrulist[set].next->pre=ptr;
        }


        lrulist[set].next=ptr;
        return;
    }
    printf("  miss evictions \n");
    (*evictions)++;
    (*misses)++;
    prelru->tag=tag;
    prelru->pre->next=prelru->next;


    prelru->next=lrulist[set].next;
    prelru->pre=&lrulist[set];
    if(lrulist[set].next!=NULL){
        lrulist[set].next->pre=prelru;
    }

    lrulist[set].next=prelru;
    return;

}
void store(unsigned  long address,node *freelist,node *lrulist,int *hits,int *misses,int *evictions){
    printf("store address:%lx",address);
    unsigned long tag=address>>(s+b),temp=address>>b;
    int set=0;
    for(int i=0;i<s;i++){
        set=set+((temp&0x1)<<i);
        temp=temp>>1;
    }
    node *lru=lrulist[set].next;
    node *prelru=&lrulist[set];
    while(lru!=NULL){
        if(lru->tag==tag){
            printf("  hit\n");
            (*hits)++;
            lru->pre->next=lru->next;
            if(lru->next!=NULL){
                lru->next->pre=lru->pre;
            }
            lru->next=lrulist[set].next;
            lru->pre=&lrulist[set];
            if(lrulist[set].next!=NULL){
                lrulist[set].next->pre=lru;
            }

            lrulist[set].next=lru;
            return;
        }
        prelru=lru;
        lru=lru->next;
    }
    if(freelist[set].next!=NULL){
        printf("  miss\n");
        (*misses)++;
        node *ptr=freelist[set].next;
        ptr->tag=tag;
        if(ptr->next!=NULL){
            ptr->next->pre=&freelist[set];
        }

        freelist[set].next=ptr->next;

        ptr->next=lrulist[set].next;
        ptr->pre=&lrulist[set];
        if(lrulist[set].next!=NULL){
            lrulist[set].next->pre=ptr;
        }


        lrulist[set].next=ptr;
        return;
    }
    printf("  miss evictions \n");
    (*evictions)++;
    (*misses)++;
    prelru->tag=tag;
    prelru->pre->next=prelru->next;


    prelru->next=lrulist[set].next;
    prelru->pre=&lrulist[set];
    if(lrulist[set].next!=NULL){
        lrulist[set].next->pre=prelru;
    }

    lrulist[set].next=prelru;
    return;
}
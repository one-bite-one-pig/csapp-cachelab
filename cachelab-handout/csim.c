#include "cachelab.h"
#include <getopt.h> // getopt
#include <stdlib.h> // atoi
#include <stdio.h>
#include <unistd.h>  


int s,E,b;
int curtime=0;
int hit=0,miss=0,eviction=0;


struct line{
    int valid;
    int tag;
    int time;
};

struct line **cache;


void init_cache(){
    cache=(struct line **)malloc (sizeof (struct line*)*(1<<s));
    for (int i=0;i<(1<<s);i++){
        cache[i]=(struct line*)malloc(sizeof (struct line)*E);
    }
    for(int i=0;i<(1<<s);i++){
        for(int j=0;j<E;j++){
            cache[i][j].valid=0;
            cache[i][j].tag=-1;
            cache[i][j].time=-1;
        }
    }

}







void get(size_t address,int *tag,int *set,int* block){
    *block=address&((1<<(b))-1);
    *set=(address>>b)&((1<<(s))-1);
    *tag=(address>>(b+s));
}


void load(size_t address){
    int tag,set,block;
    get(address,&tag,&set,&block);
    for(int i=0;i<E;i++){
        if(cache[set][i].valid==1&&cache[set][i].tag==tag){
            hit++;
            cache[set][i].time=curtime;
            return;
        }
        
    }
    
    miss++;
    int sign_of_empty=0;
    int empty_pos=-1;
    for(int i=0;i<E;i++){
        if(cache[set][i].valid==0){
            empty_pos=i;
            sign_of_empty=1;
        }
    }
    if(sign_of_empty){
        cache[set][empty_pos].valid=1;
        cache[set][empty_pos].tag=tag;
        cache[set][empty_pos].time=curtime;
    }
    else{
        eviction++;
        int the_latest_time=cache[set][0].time;
        int the_latest_pos=0;
        for(int i=1;i<E;i++){
            if(cache[set][i].time<the_latest_time){
                the_latest_time=cache[set][i].time;
                the_latest_pos=i;
            }
        }
        cache[set][the_latest_pos].time=curtime;
        cache[set][the_latest_pos].tag=tag;
    }

        
}

void modify(size_t address){
    load(address);
    hit++;
}


int main(int argc,char*argv[])
{   
    int option;
    FILE* trace_file;
    while ((option = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (option) {
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            trace_file = fopen(optarg, "r");
            break;
        default:
            exit(0);
        }
    }
    int size;
    char operation;
    size_t address;
    init_cache();
    while (fscanf(trace_file, "%s %lx,%d\n", &operation, &address, &size) == 3) {
        ++curtime;
        switch (operation) {
        case 'I':
            continue;
        case 'M': // Modify = Load + Store
            modify(address);
            break;
        case 'L':
            load(address);
            break;
            // Load
        case 'S': // Store
            load(address);
        }
    }

    for(int i=0;i<(1<<s);i++)
        free(cache[i]);
    free(cache);
    printSummary(hit, miss, eviction);
    return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include "cachelab.h"
#include <string.h>
#include<malloc.h>
#include<limits.h>
void callprint()
{
    printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n"
            "Options:\n"
            "  -h         Print this help message.\n"
            "  -v         Optional verbose flag.\n"
            "  -s <num>   Number of set index bits.\n"
            "  -E <num>   Number of lines per set.\n"
            "  -b <num>   Number of block offset bits.\n"
            "  -t <file>  Trace file.\n\n"
            "Examples:\n"
            "  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n"
            "  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}
typedef struct 
{
    int tag;
    int valid_bit;
    int timestamp;
}cache_line,*cache_set,**cache;
int hit,miss,evict;

cache init_cache(int s,int E,int b){
    cache cache_sim = (cache)malloc(sizeof(cache_set)*s);
    for(int i=0;i<s;i++){
        cache_set set = (cache_set)malloc(sizeof(cache_line)*E);
        cache_sim[i] = set;
        for(int j=0;j<E;j++){
            cache_sim[i][j].tag = -1;
            cache_sim[i][j].valid_bit = 0;
            cache_sim[i][j].timestamp = -1;
        }
    }
    return cache_sim;
}
int check_hit(cache cache_sim,int set_index,int E,int true_tag,int *hit){
  //  printf("enter checkhit%d %d\n",E,set_index);
    for(int i=0;i<E;i++){
        if(cache_sim[set_index][i].tag == true_tag){
            cache_sim[set_index][i].timestamp = 0;
            *hit += 1;
            return 1;
        }
    }
    return 0;
}
int check_empty(cache cache_sim, int set_index,int E,int true_tag,int *miss){
  //  printf("enter checkempty\n");
    for(int i=0;i<E;i++){
        if(cache_sim[set_index][i].valid_bit == 0){
            cache_sim[set_index][i].valid_bit = 1;
            cache_sim[set_index][i].tag = true_tag;
            cache_sim[set_index][i].timestamp = 0;
            *miss += 1;
         //   printf("empty success, miss: %d\n",*miss);
            return 1;
        }
    }
    return 0;
}
void update(cache cache_sim,int set_index,int E,int true_tag,int s,int *hit,int *miss,int *evict){
  //  printf("enter checkupdate\n");
    if(check_hit(cache_sim,set_index,E,true_tag,hit)){
        return;
    }
    else if( check_empty(cache_sim,set_index,E,true_tag,miss)){
        return;
    }
    else{
        *miss += 1;

        // case of eviction
        int max_stamp = INT_MIN;
        int evi_line;
        for(int i=0;i<E;i++){
            if(cache_sim[set_index][i].timestamp > max_stamp){
                evi_line = i;
                max_stamp = cache_sim[set_index][i].timestamp;
            }
        }
        cache_sim[set_index][evi_line].timestamp = 0;
        cache_sim[set_index][evi_line].tag = true_tag;
        *evict += 1;
    //    printf("miss:%d evict:%d",*miss,*evict);
    }

}
void update_time(cache cache_sim,int E,int s){
    int S = 1<<s;
    for(int i=0;i<S;i++){
        for(int j=0;j<E;j++){
            if(cache_sim[i][j].valid_bit ==1){
                cache_sim[i][j].timestamp += 1;
            }
        }
    }
}
int main(int argc,char **argv)
{
    int opt,s,E,b;
  ///  int *hit,*evict,*miss;
    int hit=0,evict=0,miss=0;
    char t[1000];
   // int verbose=0;
    while((opt=getopt(argc,argv,"hvs:E:b:t:"))!=-1){
		switch(opt)
		{
			case 'h':
			//	h = 1;
				callprint();
				break;
			case 'v':
			//	v = 1;
				callprint();
				break;
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
				strcpy(t, optarg);
				break;
			default:
				callprint();
				break;
		}
        
    }
  //  printf("finished");
    int S = 1<<s;
    printf("%d %d %d \n",s,E,b);
    char op;
    unsigned int addr;
    int size;
    cache cache_sim;
    cache_sim = init_cache(S,E,b);
    int tag;
    int set_index;
    hit = 0;miss=0;evict=0;
    FILE* fp = fopen(t, "r"); 
	if(fp == NULL)
	{
		printf("open error");
		exit(-1);
	}
    while(fscanf(fp, " %c %xu,%d\n", &op, &addr, &size) > 0){
        tag = addr >>(s+b);
        set_index = (addr >> b)&((-1U)>>(64-s));
        if(op=='I') continue;
        if(op == 'L'){
            update(cache_sim,set_index,E,tag,s,&hit,&miss,&evict);
        }
        if(op=='S'){
            update(cache_sim,set_index,E,tag,s,&hit,&miss,&evict);
        }
        if(op=='M'){
            update(cache_sim,set_index,E,tag,s,&hit,&miss,&evict);
            update(cache_sim,set_index,E,tag,s,&hit,&miss,&evict);
        }
        update_time(cache_sim,E,s);
    }
    printSummary(hit,miss,evict);
   // printf("hits:%d misses:%d evictions:%d\n",hit,miss,evict); 
   	fclose(fp);
	for(int i = 0; i < S; ++i)
		free(cache_sim[i]);
	free(cache_sim);      
}

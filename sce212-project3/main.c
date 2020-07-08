#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for getopt()

#define BYTES_PER_WORD 4
// #define DEBUG

/*
 * Cache structures
 */
int time = 0;

typedef struct
{
    int age; //LRU
    int valid;
    int dirty;
    uint32_t tag;
} cline;

typedef struct
{
    cline *lines;
} cset;

typedef struct
{
    int idxb; // index bits
    int way; // way
    int offs; // block offset bits
    cset *sets;
} cache;

int index_bit(int n){
    int cnt = 0;

    while(n) {
        cnt++;
        n = n >> 1;
    }

    return cnt-1;
}

/***************************************************************/
/*                                                             */
/* Procedure : build_cache                                     */
/*                                                             */
/* Purpose   : Initialize cache structure                      */
/*                                                             */
/* Parameters:                                                 */
/*     int bit: The set of cache                                 */
/*     int way: The associativity way of cache                   */
/*     int offs: The blocksize of cache                           */
/*                                                             */
/***************************************************************/
cache build_cache(int set, int way)
{
	cache Cache;
    Cache.sets = (cset*)malloc(sizeof(cset)*set); // cache set mallocation

    for(int i = 0; i < set; i++)
    {
        Cache.sets[i].lines = (cline*)malloc(sizeof(cline)*way); //1set, 8way
        Cache.offs = 3;
        Cache.idxb = 4;
        Cache.way = way;  //set basic value
        for(int j = 0; j < way; j++)
        
        {   
            Cache.sets[i].lines[j].age = 0;
            Cache.sets[i].lines[j].dirty = 0;
            Cache.sets[i].lines[j].tag = 0;
            Cache.sets[i].lines[j].valid = 0; //everything is 0 at first
        }
    }
    return Cache;
}

/***************************************************************/
/*                                                             */
/* Procedure : access_cache                                    */
/*                                                             */
/* Purpose   : Update cache stat and content                   */
/*                                                             */
/* Parameters:                                                 */
/*     cache *L: An accessed cache                             */
/*     int op: Read/Write operation                            */
/*     uint32_t addr: The address of memory access             */
/*     int *hit: The number of cache hit                       */
/*     int *miss: The number of cache miss                     */
/*     int *wb: The number of write-back                       */
/*                                                             */
/***************************************************************/
void access_cache(cache *cache, int op, uint32_t addr, int *read, int *write, int *readhit, int *writehit, int *readmiss, int *writemiss, int *wb)
{
    int total_read = *read;
    int total_write = *write;
    int read_hit = *readhit;
    int write_hit = *writehit;
    int read_miss = *readmiss;
    int write_miss = *writemiss;
    int writeback = *wb; //dirty

    uint32_t index = addr << 25; 
    index = index >> (25 + cache->offs);  // index bit = 4bit 
    uint32_t tag = addr >> (cache->idxb + cache->offs);  //tag = 25bit
    int wayIdx = 0;
    
    if(op == 1) //Read
    {   
        total_read++; //total read num up
        while(wayIdx < 8)
        {   
            if(cache->sets[index].lines[wayIdx].valid == 1) //valid
            {
                if(cache->sets[index].lines[wayIdx].tag == tag)
                {
                    read_hit++; time++;
                    cache->sets[index].lines[wayIdx].age = time;
                    break;
                }
            }
            else if(cache->sets[index].lines[wayIdx].valid == 0)
            {
                read_miss++; time++;
                cache->sets[index].lines[wayIdx].age = time;
                cache->sets[index].lines[wayIdx].valid = 1; // now valid
                cache->sets[index].lines[wayIdx].tag = tag; //set tag 
                break;
            }
            wayIdx++;
        }
        if(wayIdx == 8)// no room. have to evict.
        {
            read_miss++; time++;
            int old = 0, LRU = 201721747;
            for( int i = 0; i < 8; i++) 
            {
                if(cache->sets[index].lines[i].age < LRU)
                {
                    LRU = cache->sets[index].lines[i].age;
                    old = i;
                }
            }
            if(cache->sets[index].lines[old].dirty == 1)
            writeback++;
            cache->sets[index].lines[old].tag = tag;
            cache->sets[index].lines[old].age = time;
            cache->sets[index].lines[old].dirty = 0;
        }
    }

    else //Write
    {
        total_write++; //total write num up
        while(wayIdx < 8)
        {   
            if(cache->sets[index].lines[wayIdx].valid == 1) //valid
            {
                if(cache->sets[index].lines[wayIdx].tag == tag)
                {
                    write_hit++; time++;
                    cache->sets[index].lines[wayIdx].age = time;
                    cache->sets[index].lines[wayIdx].dirty = 1;
                    break;
                }
            }
            else if(cache->sets[index].lines[wayIdx].valid == 0)
            {   
                write_miss++; time++;
                cache->sets[index].lines[wayIdx].dirty = 1;
                cache->sets[index].lines[wayIdx].age = time;;
                cache->sets[index].lines[wayIdx].valid = 1; // now valid
                cache->sets[index].lines[wayIdx].tag = tag; //set tag 
                break;
            }
            wayIdx++;
        }
        if(wayIdx == 8)// no room. have to evict.
        {
            write_miss++; time++;
            int old = 0, LRU = 201721747;
            for( int i = 0; i < 8; i++) 
            {
                if(cache->sets[index].lines[i].age < LRU)
                {
                    LRU = cache->sets[index].lines[i].age;
                    old = i;
                }
            }
            if(cache->sets[index].lines[old].dirty == 1)
            writeback++;
            cache->sets[index].lines[old].tag = tag;
            cache->sets[index].lines[old].age = time;
            cache->sets[index].lines[old].dirty = 1;
        }
    }

    *read = total_read;
    *write = total_write;
    *readhit = read_hit; 
    *writehit = write_hit; 
    *readmiss = read_miss; 
    *writemiss = write_miss;
    *wb = writeback; 
}

/***************************************************************/
/*                                                             */
/* Procedure : cdump                                           */
/*                                                             */
/* Purpose   : Dump cache configuration                        */
/*                                                             */
/***************************************************************/
void cdump(int capacity, int assoc, int blocksize)
{

    printf("Cache Configuration:\n");
    printf("-------------------------------------\n");
    printf("Capacity: %dB\n", capacity);
    printf("Associativity: %dway\n", assoc);
    printf("Block Size: %dB\n", blocksize);
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : sdump                                           */
/*                                                             */
/* Purpose   : Dump cache stat		                           */
/*                                                             */
/***************************************************************/
void sdump(int total_reads, int total_writes, int write_backs,
           int reads_hits, int write_hits, int reads_misses, int write_misses)
{
    printf("Cache Stat:\n");
    printf("-------------------------------------\n");
    printf("Total reads: %d\n", total_reads);
    printf("Total writes: %d\n", total_writes);
    printf("Write-backs: %d\n", write_backs);
    printf("Read hits: %d\n", reads_hits);
    printf("Write hits: %d\n", write_hits);
    printf("Read misses: %d\n", reads_misses);
    printf("Write misses: %d\n", write_misses);
    printf("\n");
}


/***************************************************************/
/*                                                             */
/* Procedure : xdump                                           */
/*                                                             */
/* Purpose   : Dump current cache state                        */
/*                                                             */
/* Cache Design                                                */
/*                                                             */
/*      cache[set][assoc][word per block]                      */
/*                                                             */
/*                                                             */
/*       ----------------------------------------              */
/*       I        I  way0  I  way1  I  way2  I                 */
/*       ----------------------------------------              */
/*       I        I  word0 I  word0 I  word0 I                 */
/*       I  set0  I  word1 I  word1 I  work1 I                 */
/*       I        I  word2 I  word2 I  word2 I                 */
/*       I        I  word3 I  word3 I  word3 I                 */
/*       ----------------------------------------              */
/*       I        I  word0 I  word0 I  word0 I                 */
/*       I  set1  I  word1 I  word1 I  work1 I                 */
/*       I        I  word2 I  word2 I  word2 I                 */
/*       I        I  word3 I  word3 I  word3 I                 */
/*       ----------------------------------------              */
/*                                                             */
/*                                                             */
/***************************************************************/
void xdump(cache* L)
{
    int i, j, k = 0;
    int b = L->offs, s = L->idxb;
    int way = L->way, set = 1 << s;
    uint32_t line;

    printf("Cache Content:\n");
    printf("-------------------------------------\n");

    for(i = 0; i < way; i++) {
        if(i == 0) {
            printf("    ");
        }
        printf("      WAY[%d]", i);
    }
    printf("\n");

    for(i = 0; i < set; i++) {
        printf("SET[%d]:   ", i);

        for(j = 0; j < way; j++) {
            if(k != 0 && j == 0) {
                printf("          ");
            }
            if(L->sets[i].lines[j].valid) {
                line = L->sets[i].lines[j].tag << (s + b);
                line = line | (i << b);
            }
            else {
                line = 0;
            }
            printf("0x%08x  ", line);
        }
        printf("\n");
    }
    printf("\n");
}


int main(int argc, char *argv[])
{
    int capacity=1024;
    int way=8;
    int blocksize=8;
    int set;

    // Cache
    cache simCache;
    
    // Counts
    int read=0, write=0, writeback=0;
    int readhit=0, writehit=0;
    int readmiss=0, writemiss = 0;

    // Input option
    int opt = 0;
    char* token;
    int xflag = 0;

    // Parse file
    char *trace_name = (char*)malloc(32);
    FILE *fp;
    char line[16];
    char *op;
    uint32_t addr;

    /* You can define any variables that you want */

    trace_name = argv[argc-1];
    if (argc < 3) {
        printf("Usage: %s -c cap:assoc:block_size [-x] input_trace \n", argv[0]);
        exit(1);
    }

    while((opt = getopt(argc, argv, "c:x")) != -1) {
        switch(opt) {
            case 'c':
                token = strtok(optarg, ":");
                capacity = atoi(token);
                token = strtok(NULL, ":");
                way = atoi(token);
                token = strtok(NULL, ":");
                blocksize  = atoi(token);
                break;

            case 'x':
                xflag = 1;
                break;

            default:
                printf("Usage: %s -c cap:assoc:block_size [-x] input_trace \n", argv[0]);
                exit(1);

        }
    }

    // Allocate
    set = capacity / way / blocksize;
   
   /* TODO: Define a cache based on the struct declaration */
    simCache = build_cache(set, way);

    // Simulate
    fp = fopen(trace_name, "r"); // read trace file
    if(fp == NULL) {
        printf("\nInvalid trace file: %s\n", trace_name);
        return 1;
    }

    cdump(capacity, way, blocksize);

    /* TODO: Build an access function to load and store data from the file */
    while (fgets(line, sizeof(line), fp) != NULL) {
        op = strtok(line, " ");
        int opc = 0;
        if(strcmp(op, "R") == 0) //R = 1, W = 1;
        {
            opc = 1;
        }

        addr = strtoull(strtok(NULL, ","), NULL, 16);

#ifdef DEBUG
        // You can use #define DEBUG above for seeing traces of the file.
        fprintf(stderr, "op: %s\n", op);
        fprintf(stderr, "addr: %x\n", addr);
#endif
        
        access_cache(&simCache, opc, addr, &read, &write, &readhit, &writehit, &readmiss, &writemiss, &writeback);
        
    }

    // test example
    sdump(read, write, writeback, readhit, writehit, readmiss, writemiss);
    if (xflag) {
        xdump(&simCache);
    }

    return 0;
}
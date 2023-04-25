// Pre-Processor Directives
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants
#define CACHE_SIZE 32
#define BLOCK_SIZE 4

// Structures
typedef struct cache{
    int size;
    int assoc;
    int numSets;
    int blockSize;
    int replacement;
    int hit;
    int miss;
    int **tag;
    int **valid;
    char **data;
    int *lru;
}Cache;

// Function Prototypes
int hexToInt(char* hex);
void inCache(Cache * cache, int asssoc, int rp);
void accessCache(Cache * cache, int address);

// Main
int main(){
    Cache cache;
    char hex[9];
    int address, i;

    FILE * ifp = fopen("traces.txt", "r");

    cache.size = CACHE_SIZE;
    cache.blockSize = BLOCK_SIZE;
    inCache(&cache, 1, 0);

    for (int i = 0; i < 16; i++){
        fscanf(ifp, "%s", hex);

        address = hexToInt(hex);

        accessCache(&cache, address);
    }

    prinf("Hits: %d\n", cache.hit);
    printf("Misses: %d\n", cache.miss);

    return 0;
}

// Convert hexadecimal strings to integers
int hexToInt(char* hex){
    return (int)strtol(hex, NULL, 16);
}

// Initialize cache
void inCache(Cache * cache, int assoc, int rp){
    cache->assoc = assoc;
    cache->numSets = cache->size / (assoc * cache->blockSize);
    cache->replacement = rp;
    cache->hit = 0;
    cache->tag = (int**)malloc(cache->numSets * sizeof(int*));
    cache->valid = (int**)malloc(cache->numSets * sizeof(int*));
    cache->data = (char**)malloc(cache->numSets * sizeof(char*));

    if (rp == 0){
        cache->lru = (int*)malloc(cache->numSets * sizeof(int));
        memset(cache->lru, 0, cache->numSets * sizeof(int));
    }

    for (int i = 0; i < cache->numSets; i++){
        cache->tag[i] = (int*)malloc(assoc * sizeof(int));
        cache->valid[i] = (int*)malloc(assoc * sizeof(int));
        cache->data[i] = (char*)malloc(cache->blockSize * assoc * sizeof(char));
        for (int j = 0; j < assoc; j++){
            cache->tag[i][j] = -1;
            cache->valid [i][j] = 0;
            memset(&cache->data[i][j * cache->blockSize], 0, cache->blockSize * sizeof(char));
        }
    }
}


// Access the Cache
void accessCache(Cache * cache, int address){
    int setIndex = (address / cache->blockSize) % cache->numSets;
    int tag = (address / cache->blockSize) / cache->numSets;
    int lruIndex, randomIndex;

    for (int i = 0; i < cache->assoc; i++){
        if (cache->valid[setIndex][i] && cache->tag[setIndex][i] == tag){
            cache->hit++;
            if (cache->replacement == 0){
                cache->lru[setIndex] = i;
            }
            return;
        }
    }

    cache->miss++;

    if (cache->replacement == 0){
        lruIndex = cache->lru[setIndex];
        for (int i = 0; i < cache->assoc; i++){
            if (!cache->valid[setIndex][i]){
                lruIndex = i;
                break;
            }

            if (cache->lru[setIndex] > cache->lru[setIndex + i]){
                lruIndex = i;
            }
        }

        cache->lru[setIndex] = cache->lru[setIndex] + 1;
        if (lruIndex == cache->assoc){
            lruIndex = 0;
            cache->lru[setIndex] = 0;
        }

        cache->tag[setIndex][lruIndex] = tag;
        cache->valid[setIndex][lruIndex] = 1;
        memcpy(&cache->data[setIndex][randomIndex * cache->blockSize], &address, cache->blockSize * sizeof(char));
    }
}
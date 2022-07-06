#include <stdio.h>
#include "set.h"
#include <memory.h>
#include <stdlib.h>
int Cachefill(int address, int way, int index, int tag) {
    int StartAddr = address & 0xffffffc0;
    cachemiss++;
    cache2way[way].cacheline[index].valid = 1;
    cache2way[way].cacheline[index].tag = tag;
    for (int i = 0; i < 16; i++) {
        cache2way[way].cacheline[index].data[i] = memory[(StartAddr / 4) + i];
    }

    return;

}


int Cache4fill(int address, int way, int index, int tag) {
    int StartAddr = address & 0xffffffc0;
    cachemiss++;
    cache4way[way].cacheline[index].valid = 1;
    cache4way[way].cacheline[index].tag = tag;
    for (int i = 0; i < 16; i++) {
        cache4way[way].cacheline[index].data[i] = memory[(StartAddr / 4) + i];
    }
    
    return;

}
int Cache8fill(int address, int way, int index, int tag) {
    int StartAddr = address & 0xffffffc0;
    cachemiss++;
   /*
    if (cache8way[way].cacheline[index].dirty == 1) {
        for (int i = 0; i < 16; i++) {
            memory[(StartAddr / 4) + i]=cache8way[way].cacheline[index].data[i];
        }
        cache8way[way].cacheline[index].dirty = 0;
    }*/

    cache8way[way].cacheline[index].valid = 1;
    cache8way[way].cacheline[index].tag = tag;
    for (int i = 0; i < 16; i++) {
        cache8way[way].cacheline[index].data[i] = memory[(StartAddr / 4) + i];
    }

    return;

}

int DMCachefill(int address,  int index, int tag) {
    int StartAddr = address & 0xffffffc0;
    cachemiss++;
    directmapcache.cacheline[index].valid = 1;
    directmapcache.cacheline[index].tag = tag;
    for (int i = 0; i < 16; i++) {
        directmapcache.cacheline[index].data[i] = memory[(StartAddr / 4) + i];
    }
    return;
}

int FACachefill(int address,int index,  int tag) {
    int StartAddr = address & 0xffffffc0;
    cachemiss++;
    FAcache[index].valid = 1;
    FAcache[index].tag = tag;
   

    for (int i = 0; i < 16; i++) {
        FAcache[index].data[i] = memory[(StartAddr / 4) + i];
    }
    return;

}

int replace(int way,int index) {
    int num;
    int oldest = 0;
    switch (RPType)
    {
    case 0:
        //**********************random*********************************
        num = rand() % way;
        return num;
        break;
    case 1:
        //*********************fifo*********************     
        switch (MappingType)
        {
        case 0:
            oldest = 0;
            oldest = delete(FAcachequeue.queue, rear, front);
            return oldest;
            break;
        case 2:
            oldest = 0;
            oldest = delete(cache2queue[index].queue, rear, front);
            return oldest;
            break;
        case 4:
            oldest = 0;
            oldest = delete(cache4queue[index].queue, rear, front);
            return oldest;
            break;
        case 8:
            oldest = 0;
            oldest = delete(cache8queue[index].queue, rear, front);
            return oldest;
            break;
        }
        break;
    case 2:
        //*********************lru*********************
        switch (MappingType)
        {
        case 0:
            oldest = (FAcachelru.oldest % 128);
            FAcachelru.oldest++;
            FAcachelru.oldest = oldest % 128;
            return oldest;
            break;
        case 2:
            oldest = (cache2lru[index].oldest % 2);
            cache2lru[index].oldest++;
            cache2lru[index].oldest = cache2lru[index].oldest % 2;
            return oldest;
            return oldest;
        case 4:
            oldest = (cache4lru[index].oldest % 4);
            cache4lru[index].oldest++;
            cache4lru[index].oldest = cache4lru[index].oldest % 4;
            return oldest;
            return oldest;
            break;
        case 8:
            oldest = (cache8lru[index].oldest % 8);
            cache8lru[index].oldest++;
            cache8lru[index].oldest = oldest % 8;
            return oldest;
            return oldest;
            break;
        }
        break;

    case 3:
        //*********************sca*********************
        switch (MappingType)
        {
        case 0:
            oldest = (FAcachelru.oldest % 128);
            while (1) {
                if (FAcache[oldest].sca == 0) {
                    break;
                }
                else {
                    FAcache[oldest].sca = 0;
                    oldest++;
                    oldest = oldest % 128;
                }
            }
            FAcachelru.oldest = oldest % 128;
            return oldest;
            break;
        case 2:
            oldest = (cache2lru[index].oldest % 2);
            while (1) {
                if (cache2way[oldest].cacheline[index].sca == 0) {
                    break;
                }
                else {
                    cache2way[oldest].cacheline[index].sca = 0;
                    oldest++;
                    oldest = oldest % 2;
                }
            }
            cache4lru[index].oldest = oldest % 4;
            return oldest;
            break;
        case 4:
            oldest = (cache4lru[index].oldest % 4);
            while (1) {
                if (cache4way[oldest].cacheline[index].sca == 0) {
                    break;
                }
                else {
                    cache4way[oldest].cacheline[index].sca = 0;
                    oldest++;
                    oldest = oldest % 4;
                }
            }
            cache4lru[index].oldest = oldest % 4;
            return oldest;
            break;
        case 8:
            oldest = (cache8lru[index].oldest % 8);
            while (1) {
                if (cache8way[oldest].cacheline[index].sca == 0) {
                    break;
                }
                else {
                    cache8way[oldest].cacheline[index].sca = 0;
                    oldest++;
                    oldest = oldest % 8;
                }
            }
            cache8lru[index].oldest = oldest % 8;
            return oldest;
            break;
        }
        break;
    }
    

    
    
    

}

#include <stdio.h>
#include "set.h"
#include <memory.h>
#include <stdlib.h>
int flag1 = 0;

int FALWCache(int address) {
    //fully
    int tempaddr = address;
    int offset = tempaddr & 0x3f;
    int tag = (tempaddr >> 6) & 0x3ffffff;
    //FA
    for (int i = 0; i < 128; i++) {
        if (FAcache[i].valid != 0) {
            if (FAcache[i].tag == tag) {
                cachehit++;
                return FAcache[i].data[offset / 4];
            }
        }
        else {
            //cold miss
            coldMiss++;
            FACachefill(address, i, tag);
            return FAcache[i].data[offset / 4];
        }
    }
    cachereplace++;
    int index = replace(128, 0);
    FACachefill(address, index, tag);
    return NULL;

}
int DMLWCache(int address) {
    //1 way  tag 19 index 7
    int tempaddr = address;
    int offset = tempaddr & 0x3f;
    int index = (tempaddr >> 6) & 0x7f;
    int tag = (tempaddr >> 7) & 0x7ffff;
    //Dm
    for (int i = 0; i < 1; i++) {
        if (directmapcache.cacheline[index].valid != 0) {
            if (directmapcache.cacheline[index].tag == tag) {
                cachehit++;
                return directmapcache.cacheline[index].data[offset / 4];
            }
        }
        else {
            //cold miss
            coldMiss++;
            DMCachefill(address, index, tag);
            return directmapcache.cacheline[index].data[offset / 4];
        }
    }
    cachereplace++;
    DMCachefill(address, index, tag);
    return NULL;

}

int LWCache(int address) {
    //2 way  tag 20 index 6
    int tempaddr = address;
    int offset = tempaddr & 0x3f;
    int index = (tempaddr >> 6) & 0x3f;
    int tag = (tempaddr >> 6) & 0xfffff;
    for (int i = 0; i < 2; i++) {
        if (cache2way[i].cacheline[index].valid != 0) {
            if (cache2way[i].cacheline[index].tag == tag) {
                cachehit++;
                return cache2way[i].cacheline[index].data[offset / 4];
            }
        }
        else {
            //cold miss
            coldMiss++;
            Cachefill(address, i, index, tag);
            return cache2way[i].cacheline[index].data[offset / 4];
        }
    }
    cachereplace++;
    int way = replace(4, index);
    Cachefill(address, way, index, tag);
    return NULL;
}


int LWCache4(int address) {
    //4 way  tag 21 index 5
    int tempaddr = address;
    int offset = tempaddr & 0x3f;
    int index = (tempaddr >> 6) & 0x1f;
    int tag = (tempaddr >> 5) & 0x1fffff;
    for (int i = 0; i < 4; i++) {
        if (cache4way[i].cacheline[index].valid != 0) {
            if (cache4way[i].cacheline[index].tag == tag) {
                cachehit++;
                return cache4way[i].cacheline[index].data[offset / 4];

            }
        }
        else {
            //cold miss
            coldMiss++;
            Cache4fill(address, i, index, tag);
            return cache4way[i].cacheline[index].data[offset / 4];
        }

    }
    cachereplace++;
    Cache4fill(address, replace(4, index), index, tag);
    return NULL;
}

int LWCache8(int address) {
    //8 way  tag 22 index 4
    int tempaddr = address;
    int offset = tempaddr & 0x3f;
    int index = (tempaddr >> 6) & 0xf;
    int tag = (tempaddr >> 4) & 0x3fffff;
    for (int i = 0; i < 8; i++) {
        if (cache8way[i].cacheline[index].valid != 0) {
            if (cache8way[i].cacheline[index].tag == tag) {
                cachehit++;
                return cache8way[i].cacheline[index].data[offset / 4];

            }
        }
        else {
            //cold miss
            coldMiss++;
            Cache8fill(address, i, index, tag);
            return cache8way[i].cacheline[index].data[offset / 4];
        }

    }
    cachereplace++;
    int way = replace(8, index);
    Cache8fill(address, way, index, tag);
    //latch4[0].mReadData = cache8way[0].cacheline[index].data[offset / 4];
    return NULL;
}

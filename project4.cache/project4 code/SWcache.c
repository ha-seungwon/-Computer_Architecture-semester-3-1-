#include <stdio.h>
#include "set.h"
#include <memory.h>
#include <stdlib.h>

int flag = 0;

int FASWCache(int address, int data) {
    int tempaddr = address;
    int offset = tempaddr & 0x3f;
    int tag = (tempaddr >> 6) & 0x3ffffff;
    for (int i = 0; i < 128; i++) {
        if (FAcache[i].valid != 0) {
            if (FAcache[i].tag == tag) {
                cachehit++;
                FAcache[i].data[offset/4] = data;
                //FAcache[i].dirty = 1;
                FAcache[i].sca = 1;
                return;

            }
        }
        else {
            coldMiss++;
            //cold miss
            init(FAcachequeue.queue, i, rear, front);
            FACachefill(address, i, tag);
            return;
        }

    }
    int index = replace(128, 0);
    FACachefill(address,index, tag);
    return;

}
int DMSWCache(int address, int data) {
    //1 way  tag 19 index 7
    int tempaddr = address;
    int offset = tempaddr & 0x3f;
    int index = (tempaddr >> 6) & 0x7f;
    int tag = (tempaddr >> 7) & 0x7ffff;
    //DM
    for (int i = 0; i < 1; i++) {
        if (directmapcache.cacheline[index].valid != 0) {
            if (directmapcache.cacheline[index].tag == tag) {
                cachehit++;
                directmapcache.cacheline[index].data[offset / 4] = data;
                return;
            }

        }
        else {
            coldMiss++;
            //cold miss
            DMCachefill(address, index, tag);
            return;
        }
    }
    DMCachefill(address, index, tag);
    return;

}

int SWCache(int address, int data) {
    //2 way  tag 20 index 6
    int tempaddr = address;
    int offset = tempaddr & 0x3f;
    int index = (tempaddr >> 6) & 0x3f;
    int tag = (tempaddr >> 6) & 0x7ffff;
    int way = 0;
    for (int i = 0; i < 2; i++) {
        if (cache2way[i].cacheline[index].valid != 0) {
            if (cache2way[i].cacheline[index].tag == tag) {
                cachehit++;
                cache2way[i].cacheline[index].data[offset / 4] = data;
                cache2way[i].cacheline[index].dirty = 1;
                cache2way[i].cacheline[index].sca = 1;
                return;
            }
        }
        else {
            coldMiss++;
            //cold miss
            init(cache2queue[index].queue,i,rear,front);
            Cachefill(address, i, index, tag);
            return;
        }
    }
    way = replace(2, index);
    Cachefill(address, way, index, tag);
    return;
}


int SWCache4(int address, int data) {
    //4way  tag 21 index 5
    int tempaddr = address;
    int offset = tempaddr & 0x3f;
    int index = (tempaddr >> 6) & 0x1f;
    int tag = (tempaddr >> 5) & 0x1fffff;
    int way = 0;
    for (int i = 0; i < 4; i++) {
        if (cache4way[i].cacheline[index].valid != 0) {
            if (cache4way[i].cacheline[index].tag == tag) {
                cachehit++;
                cache4way[i].cacheline[index].data[offset / 4] = data;
                cache4way[i].cacheline[index].dirty = 1;
                cache4way[i].cacheline[index].sca = 1;
                return;
            }

        }
        else {
            coldMiss++;
            //cold miss
            init(cache4queue[index].queue, i, rear, front);
            Cache4fill(address, i, index, tag);
            return;
        }
    }
    Cache4fill(address, replace(4, index), index, tag);
    return;
}


int SWCache8(int address, int data) {
    //8ay  tag 22 index 4
    int tempaddr = address;
    int offset = tempaddr & 0x3f;
    int index = (tempaddr >> 6) & 0xf;
    int tag = (tempaddr >> 4) & 0x3fffff;
    int way = 0;
    for (int i = 0; i < 8; i++) {
        if (cache8way[i].cacheline[index].valid != 0) {
            if (cache8way[i].cacheline[index].tag == tag) {
                cachehit++;
                cache8way[i].cacheline[index].data[offset / 4] = data;
                cache8way[i].cacheline[index].dirty = 1;
                cache8way[i].cacheline[index].sca = 1;
                return;
            }

        }
        else {
            //cold miss
            coldMiss++;
            init(cache8queue[index].queue, i, rear, front);
            Cache8fill(address, i, index, tag);
            return;
        }
    }
    way = replace(8, index);
    Cache8fill(address, replace(8, index), index, tag);
    return;
}

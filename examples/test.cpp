//
//  test.cpp
//  test
//
//  Created by John Kaniarz on 9/22/18.
//  Copyright © 2018 John Kaniarz. All rights reserved.
//

#include "MTypes.h"
#include "MLoader.h"
#include "MSaver.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <memory.h>

using namespace std;

void printBuggedTeleporters(){
    Dungeon d = loadDungeon();
    int fnum=0;
    for(auto f:d){
        fnum++;
        for(auto t:f.teleporters){
            if(t.z2>0 && t.z2!=fnum){
                int j =(t.x-1) + (t.y -1)*30;
                int room = f.fields[j].areaID;
                Floor &f2 = d[t.z2-1];
                size_t i=0;
                for(i = 0; i< f2.fields.size();i++){
                    if(f2.fields[i].areaID == room)
                        break;
                }
                cout<<"T "<<t.x<<", "<<t.y<<", "<< fnum<<" => " << ((i%30)+1)<<", "<<((i/30)+1)<<", "<<t.z2<<" (room #"<<(room+1)<<")";
                if(f.areas[room].spawnMask & (1<<MON_DRAGON))
                    cout<<" Dragon";
                if(f.areas[room].spawnMask & (1<<MON_MYTHICAL))
                    cout<<" Mythical";
                if(f.areas[room].spawnMask & (1<<MON_INDIGINI))
                    cout<<" Indigini";
                cout<<endl;
            }
        }
        /*
        for(auto c:f.chutes){
            int room = f.fields[(c.x-1) + (c.y -1)*30].areaID;
            Floor &f2 = d[fnum+c.depth-1];
            size_t i=0;
            for(i = 0; i< f2.fields.size();i++){
                if(f2.fields[i].areaID == room)
                    break;
            }
            cout<<"C "<<c.x<<", "<<c.y<<", "<< fnum<<" => " << ((i%30)+1)<<", "<<((i/30)+1)<<", "<<(fnum+c.depth)<<" (room #"<<(room+1)<<")"<<endl;
        }
        */
    }
}

int main(int argc,char ** argv){
    printBuggedTeleporters();
    return 0;
}

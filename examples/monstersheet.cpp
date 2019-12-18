//
//  monstersheet.cpp
//  libDejenol
//
//  Created by John Kaniarz on 12/17/19.
//  Copyright © 2019 John Kaniarz. All rights reserved.
//

#include "MTypes.h"
#include "MLoader.h"
#include "MSaver.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <sstream>

std::vector<std::string> getNextLineAndSplitIntoTokens(std::istream& str)
{
    std::vector<std::string>   result;
    std::string                line;
    std::getline(str,line);

    std::stringstream          lineStream(line);
    std::string                cell;

    while(std::getline(lineStream,cell, ','))
    {
        result.push_back(cell);
    }
    // This checks for a trailing comma with no data after it.
    if (!lineStream && cell.empty())
    {
        // If there was a trailing comma then add an empty element.
        result.push_back("");
    }
    return result;
}

int main(int argc, char ** argv){
    std::filesystem::path in = "MDATA5.MDR";
    std::filesystem::path out;
    
    if(argc>1) in = argv[1];
    if(argc>2) out = argv[2];
    
    if(in.extension() == ".mdr" || in.extension() == ".MDR"){
        Monsters monsters = loadMonsters(in.string());
        if(out.empty()){
            out=in;
            out.replace_extension(".csv");
        }
        std::ofstream f(out);
        f<<"name, att, def, id, hits, numGroups, picID, lockedChance, levelFound, FIRE, COLD, ELECTRIC, MIND, DISEASE, POISON, MAGIC, STONE, PARALYSIS, DRAIN, ACID, specialPropertyFlags, specialAttackFlags, spellFlags, chance, boxChance1, boxChance2, boxChance3, alignment, ingroup, goldFactor, trapFlags, guildlevel, STR, INT, WIS, CON, CHA, DEX, type, damageMod, compType, compSpawnMode, compID, item1, item2, item3, item4, item5, item6, item7, item8, item9, item10, subtype, compSubtype, size, itemDropLevel, itemChance,"<<std::endl;
        for(auto m:monsters){
            f<<m.name<<", ";
            f<<m.att<<", ";
            f<<m.def<<", ";
            f<<m.id<<", ";
            f<<m.hits<<", ";
            f<<m.numGroups<<", ";
            f<<m.picID<<", ";
            f<<m.lockedChance<<", ";
            f<<m.levelFound<<", ";

            for(int i=0;i<11;i++){
                f<<m.resistances[i]<<", ";
            }
            f<<m.specialPropertyFlags<<", ";
            f<<m.specialAttackFlags<<", ";
            f<<m.spellFlags<<", ";

            f<<m.chance<<", ";
            for(int i=0;i<3;i++){
                f<<m.boxChance[i]<<", ";
            }
            f<<m.alignment<<", ";
            f<<m.ingroup<<", ";
            f<<m.goldFactor<<", ";
            f<<m.trapFlags<<", ";
            f<<m.guildlevel<<", ";
            
            for(int i=0;i<6;i++){
                f<<m.stats[i]<<", ";
            }
            f<<m.type<<", ";
            f<<m.damageMod<<", ";
            f<<m.companionType <<", ";
            f<<m.companionSpawnMode <<", ";
            f<<m.companionID<<", ";
            for(int i=0;i<10;i++){
                f<<m.items[i]<<", ";
            }
            f<<m.subtype<<", ";
            f<<m.companionSubtype<<", ";
            f<<m.size<<", ";
            f<<m.itemDropLevel<<", ";
            f<<m.itemChance<<std::endl;
        }
        f.close();
    } else if (in.extension() == ".csv" || in.extension() == ".CSV"){
        if(out.empty()){
            out=in;
            out.replace_extension(".mdr");
        }
        std::ifstream f(in);
        Monsters monsters;
        monsters.reserve(401);
        
        getNextLineAndSplitIntoTokens(f);
        while(f){
            Monster m;
            std::vector<std::string> t = getNextLineAndSplitIntoTokens(f);
            if(t.size()<57) break;
            m.name = t[0];
            m.att = std::stoi(t[1]);
            m.def = std::stoi(t[2]);
            m.id = std::stoi(t[3]);
            m.hits = std::stoi(t[4]);
            m.numGroups = std::stoi(t[5]);
            m.picID = std::stoi(t[6]);
            m.lockedChance = std::stoi(t[7]);
            m.levelFound = std::stoi(t[8]);

            for(int i =0;i<11;i++){
                m.resistances[i] = std::stoi(t[9+i]);
            }
            m.specialPropertyFlags = std::stoi(t[20]);
            m.specialAttackFlags = std::stoi(t[21]);
            m.spellFlags = std::stoi(t[22]);

            m.chance = std::stoi(t[23]);
            for(int i =0;i<3;i++){
                m.boxChance[i] = std::stoi(t[24+i]);
            }
            m.alignment = std::stoi(t[27]);
            m.ingroup = std::stoi(t[28]);
            m.goldFactor = std::stoi(t[29]);
            m.trapFlags = std::stoi(t[30]);
            m.guildlevel = std::stoi(t[31]);
            
            for(int i =0;i<6;i++){
                m.stats[i] = std::stoi(t[32+i]);
            }
            m.type = std::stoi(t[38]);
            m.damageMod = std::stof(t[39]);
            m.companionType  = std::stoi(t[40]);
            m.companionSpawnMode  = std::stoi(t[41]);
            m.companionID = std::stoi(t[42]);
            for(int i =0;i<10;i++){
                m.items[i] = std::stoi(t[43+i]);
            }
            m.subtype = std::stoi(t[53]);
            m.companionSubtype = std::stoi(t[54]);
            m.size = std::stoi(t[55]);
            m.itemDropLevel = std::stoi(t[56]);
            m.itemChance = std::stoi(t[57]);
            
            monsters.push_back(m);
        }
        saveMonsters(monsters, out.string());
    } else {
        std::cout<<"usage:"<< argv[0]<<" infile [outfile]"<<std::endl;;
    }
    return 0;
}

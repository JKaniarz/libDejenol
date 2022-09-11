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
    std::filesystem::path in = "MDATA3.MDR";
    std::filesystem::path out;
    
    if(argc>1) in = argv[1];
    if(argc>2) out = argv[2];
    
    if(in.extension() == ".mdr" || in.extension() == ".MDR"){
        Items items = loadItems(in.string());
        if(out.empty()){
            out=in;
            out.replace_extension(".csv");
        }
        std::ofstream f(out);
        std::string starter;
        std::string ender;
        std::string ender2;
        bool quotes = false;
        if(out.extension() == ".js") {
            f<<"itemVarNames = [\"name\", \"ID\", \"att\", \"def\", \"price\", \"floor\", \"rarity\", \"abilities\", \"swings\", \"specialType\", \"spellIndex\", \"spellID\", \"charges\", \"guilds\", \"levelScale\", \"damageMod\", \"alignmentFlags\", \"nHands\", \"type\", \"resistanceFlags\", \"StrReq\", \"IntReq\", \"WisReq\", \"ConReq\", \"ChaReq\", \"DexReq\", \"StrMod\", \"IntMod\", \"WisMod\", \"ConMod\", \"ChaMod\", \"DexMod\", \"cursed\", \"spellLvl\", \"classRestricted\"]\n";
            f<<"items = ["<<std::endl;
            starter = "\t[";
            ender = "],\n";
            ender2="]\n]";
            quotes=true;
        } else if( out.extension()==".json") {
            f<<"["<<std::endl;
            starter = "\t[";
            ender = "],\n";
            ender2="]\n]";
            quotes=true;
        } else if(out.extension()==".csv"){
            f<<"name, ID, att, def, price, floor, rarity, abilities, swings, specialType, spellIndex, spellID, charges, guilds, levelScale, damageMod, alignmentFlags, nHands, type, resistanceFlags, StrReq, IntReq, WisReq, ConReq, ChaReq, DexReq, StrMod, IntMod, WisMod, ConMod, ChaMod, DexMod, cursed, spellLvl, classRestricted"<<std::endl;
            ender = "\n";
            ender2 = "\n";
        }
        bool first =true;
        for(auto item:items){
            if(!first) f<<ender;
            first=false;
            f<<starter;
            if(quotes){
                f << "\""<<item.name << "\",";
            } else {
                f << item.name << ",";
            }
            
            f << item.ID << ",";
            f << item.att << ",";
            f << item.def << ",";
            f << item.price << ",";
            f << item.floor << ",";
            f << item.rarity << ",";
            f << item.abilities << ",";
            f << item.swings << ",";
            f << item.specialType << ",";
            f << item.spellIndex << ",";
            f << item.spellID << ",";
            f << item.charges << ",";
            //f << item.deleted << ",";
            f << item.guilds << ",";
            f << item.levelScale << ",";
            f << item.damageMod << ",";
            f << item.alignmentFlags << ",";
            f << item.nHands << ",";
            f << item.type << ",";
            f << item.resistanceFlags << ",";
            for(int i=0;i<6;i++){
                f << item.statsRequired[i] << ",";
            }
            for(int i=0;i<6;i++){
                f << item.statsMod[i] << ",";
            }
            f << item.cursed << ",";
            f << item.spellLvl << ",";
            f << item.classRestricted;
        }
        f<<ender2;
        f.close();
    } else {
        std::cout<<"usage:"<< argv[0]<<" infile [outfile]"<<std::endl;;
    }
    return 0;
}


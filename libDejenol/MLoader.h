#pragma once
#include "MTypes.h"
#include <vector>
#include <string>


extern GameData loadGameInfo(std::string filename = "MDATA1.MDR"); //Not Implemented
extern Spells loadSpells(std::string filename = "MDATA2.MDR");
extern Items loadItems(std::string filename = "MDATA3.MDR");
extern Characters loadCharacters(std::string filename = "MDATA4.MDR"); //Not Implemented
extern Monsters loadMonsters(std::string filename = "MDATA5.MDR");
//General Store
//Guildmasters
extern Automap loadAutomap(std::string filename = "MDATA8.MDR");
extern GuildLogs loadGuildLogs(std::string filename = "MDATA9.MDR");
//Dungeon state
extern Dungeon loadDungeon(std::string filename = "MDATA11.MDR");
//Parties
extern Library loadLibrary(std::string filename = "MDATA13.MDR");
extern HallRecords loadHallRecords(std::string filename = "MDATA14.MDR");
//Confinement

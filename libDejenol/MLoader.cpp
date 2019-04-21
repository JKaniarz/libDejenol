#include "MTypes.h"
#include "MLoader.h"
#include "RecordReader.h"
#include <vector>

typedef RecordReader<20> MapRecordReader;

//MDATA1
GameData loadGameInfo(std::string filename) {
	assert(false); //Function not implemented
	RecordReader<260> rr(filename);
	GameData gameInfo = {};
	return gameInfo;
}

//MDATA2
Spells loadSpells(std::string filename) {
	RecordReader<75> buff(filename);
	Spells spells;
	buff.read();
	std::string version = buff.getString();
	assert(version == "1.1");
	buff.read();
	uint16_t numSpells = buff.getWord();
	buff.read();
	spells.clear();
	spells.reserve(numSpells);
	for (size_t i = 0; i < numSpells; i++) {
		Spell s;
		buff.read();
		buff.get(s.name);
		buff.get(s.ID);
		buff.get(s.category);
		buff.get(s.level);
		buff.get(s.u4);
		buff.get(s.killEffect);
		buff.get(s.affectMonster);
		buff.get(s.affectGroup);
		buff.get(s.damage1);
		buff.get(s.damage2);
		buff.get(s.specialEffect);
		for (size_t i = 0; i < 7; i++)
			buff.get(s.required[i]);
		buff.get(s.resistedBy);
		spells.push_back(s);
	}
	return spells;
}

//MDATA3
Items loadItems(std::string filename) {
	Items items;
	RecordReader<125> rr(filename);
	rr.read();
	std::string version = rr.getString();
	assert(version == "1.1");
	rr.read();
	uint16_t storeCode = rr.getWord();
	rr.read();
	uint16_t nItems = rr.getWord();
	items.reserve(nItems);
	for (size_t i = 0; i < nItems; i++) {
		rr.read();
		Item item;
		item.name = rr.getString();
		item.ID = rr.getWord();
		item.att = rr.getWord();
		item.def = rr.getWord();
		item.price = rr.getDword();
		item.floor = rr.getWord();
		item.rarity = rr.getWord();
		item.abilities = rr.getDword();
		item.swings = rr.getWord();
		item.specialType = rr.getWord();
		item.spellIndex = rr.getWord();
		item.spellID = rr.getWord();
		item.charges = rr.getDword();
		item.guilds = rr.getDword();
		item.levelScale = rr.getWord();
		item.damageMod = rr.getFloat();
		item.alignmentFlags = rr.getDword();
		item.nHands = rr.getWord();
		item.type = rr.getWord();
		item.resistanceFlags = rr.getDword();
		for (size_t s = 0; s < 7; s++) {
			item.stats[s] = rr.getWord();
		}
		for (size_t s = 0; s < 7; s++) {
			rr.get(item.statsmod[s]);
		}
		item.cursed = rr.getWord();
		item.spellLvl = rr.getWord();
		item.classRestricted = rr.getWord();
		items.push_back(item);
	}
	return items;
}

//MDATA4
Characters loadCharacters(std::string filename) {
	assert(false); //Function not implemented
	Characters characters = {};
	return characters;
}

//MDATA5
Monsters loadMonsters(std::string filename) {
	RecordReader<160> buff(filename);
	Monsters monsters;

	buff.read();
	std::string version = buff.getString();
	assert(version == "1.1");
	buff.read();

	buff.read();
	size_t numMonsters = buff.getWord();
	monsters.reserve(numMonsters);
	for (size_t i = 0; i < numMonsters; i++) {
		Monster m;
		buff.read();
		m.name = buff.getString();
		buff.get(m.att);
		buff.get(m.def);
		buff.get(m.id);
		buff.get(m.hits);
		buff.get(m.numGroups);
		buff.get(m.picID);
		buff.get(m.lockedChance);
		buff.get(m.levelFound);

		for (size_t i = 0; i < 12; i++)
			buff.get(m.resistances[i]);

		buff.get(m.specialPropertyFlags);
		buff.get(m.specialAttackFlags);
		buff.get(m.spellFlags);

		buff.get(m.chance);
		for (size_t i = 0; i < 4; i++)
			buff.get(m.boxChance[i]);


		buff.get(m.alignment);
		buff.get(m.ingroup);
		buff.get(m.goldFactor);
		buff.get(m.trapFlags);
		buff.get(m.guildlevel);

		for (size_t i = 0; i < 7; i++)
			buff.get(m.stats[i]);

		buff.get(m.type);
		buff.get(m.damageMod);
		buff.get(m.companionType);
		buff.get(m.companionSpawnMode);
		buff.get(m.companionID);

		for (size_t i = 0; i < 11; i++)
			buff.get(m.items[i]);

		buff.get(m.subtype);
		buff.get(m.companionSubtype);
		buff.get(m.size);
		buff.get(m.itemDropLevel);
		buff.get(m.itemChance);
		monsters.push_back(m);
	}
	return monsters;
}

Automap loadAutomap(std::string filename) {
	Automap a;
	RecordReader<10> buff(filename);
	buff.read();
	std::string version = buff.getString();
	assert(version == "1.1");
	buff.read();
	a.deepestLevel = buff.getWord();
	a.flags.resize(15);
	for (size_t floor = 0; floor < 15; floor++) {
		for (size_t i = 0; i < 900; i++) {
			buff.read();
			buff.get(a.flags[floor][i]);
		}
	}
	return a;
}

GuildLogs loadGuildLogs(std::string filename) {
	std::ifstream f(filename, std::ios::binary);
	std::vector<GuildLog> guildLogs;
	uint16_t numRecs;
	f.read((char*)& numRecs, 2);
	guildLogs.reserve(numRecs);
	std::vector<char> buffer;
	for (size_t i = 0; i < numRecs; i++) {
		GuildLog l;
		f.read((char*)& l.guildID, 2);
		f.read((char*)& l.date, 4);
		uint16_t len;
		f.read((char*)& len, 2);
		buffer.resize(len);
		f.read(&buffer[0], len);
		l.message = std::string(buffer.begin(), buffer.end());
		guildLogs.push_back(l);
	}
	return guildLogs;
}

void loadFloor(MapRecordReader & buff, Floor & f) {
	//load header
	buff.read();
	f.width = buff.getWord();
	f.height = buff.getWord();
	f.levelNum = buff.getWord();

	uint16_t numAreas = buff.getWord();
	uint16_t numChutes = buff.getWord();
	uint16_t numTeleporters = buff.getWord();
	uint16_t count;

	//load field data
	f.fields.clear();
	f.fields.reserve(f.width * f.height);
	for (int i = 0; i < f.width * f.height; i++) {
		buff.read();
		f.fields.push_back({ buff.getWord(),buff.getIntCurrency() });
	}

	//load area data
	buff.read();
	count = buff.getWord();
	assert(count == numAreas);
	f.areas.clear();
	f.areas.reserve(201);
	for (size_t i = 0; i < 201; i++) {
		buff.read();
		f.areas.push_back({ buff.getDword(), buff.getWord() });
	}
	//f.areas.resize(numAreas+1); //numAreas is wrong.

	//load teleporter data
	buff.read();
	count = buff.getWord();
	assert(count == numTeleporters);
	f.teleporters.clear();
	f.teleporters.reserve(numTeleporters);
	for (size_t i = 0; i < numTeleporters; i++) {
		buff.read();
		Floor::Teleporter t = { buff.getWord(),buff.getWord(),buff.getWord(),buff.getWord(),buff.getWord() };
		if (t.x > 0 && t.y > 0)
			f.teleporters.push_back(t);


	}

	//load chute data
	buff.read();
	count = buff.getWord();
	assert(count == numChutes);
	f.chutes.clear();
	f.chutes.reserve(numChutes);
	for (size_t i = 0; i < numChutes; i++) {
		buff.read();
		Floor::Chute c = { buff.getWord(),buff.getWord(),buff.getWord() };
		if (c.x > 0 && c.y > 0)
			f.chutes.push_back(c);
	}
}
Dungeon loadDungeon(std::string filename) {
	Dungeon dungeon;
	MapRecordReader buff(filename);
	buff.read();
	uint16_t numLevels = buff.getWord();

	std::vector<uint16_t> offsets;
	for (size_t i = 0; i < numLevels; i++) {
		buff.read();
		offsets.push_back(buff.getWord());
	}

	dungeon.clear();
	dungeon.resize(numLevels);
	for (size_t i = 0; i < numLevels; i++) {
		buff.seek(offsets[i]);
		loadFloor(buff, dungeon[i]);
	}
	return dungeon;
}

Library loadLibrary(std::string filename) {
	RecordReader<100> buff(filename);
	int16_t numMonsters;
	int16_t numItems;
	int16_t monstersFound;
	int16_t itemsFound;
	std::vector<Library::Record> monsters;
	std::vector<Library::Record> items;

	buff.read();
	std::string ver = buff.getString();
	assert(ver == "1.1");
	buff.read();
	buff.get(numMonsters);
	monsters.reserve(numMonsters);
	buff.read();
	buff.get(numItems);
	items.reserve(numItems);
	buff.read();
	buff.get(monstersFound);
	buff.read();
	buff.get(itemsFound);
	for (int i = 0; i < numMonsters; i++) {
		buff.read();
		monsters.push_back({ buff.getWord(),buff.getWord(),buff.getCurrency(),buff.getString(30),buff.getWord(),buff.getDword(),buff.getString(8),buff.getWord() });
	}
	for (int i = 0; i < numItems; i++) {
		buff.read();
		items.push_back({ buff.getWord(),buff.getWord(),buff.getCurrency(),buff.getString(30),buff.getWord(),buff.getDword(),buff.getString(8),buff.getWord() });
	}
	return Library({ monstersFound,itemsFound,monsters,items });
}

HallRecords loadHallRecords(std::string filename) {
	HallRecords records(13);
	RecordReader<45> buff(filename);
	for (size_t i = 0; i < 13; i++) {
		buff.read();
		records[i].value = buff.getCurrency();
		records[i].name = buff.getString(30);
		records[i].date = buff.getDword();
	}
	return records;
};

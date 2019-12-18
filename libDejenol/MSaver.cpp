#include "MSaver.h"
#include "RecordWriter.h"


void saveMonsters(const Monsters &monsters, std::string filename){
    RecordWriter<160> buff(filename);
    
    buff.set("1.1");
    buff.writeRecord();
    
    buff.writeRecord();
    
    buff.set((uint16_t)monsters.size());
    buff.writeRecord();
    for(auto m:monsters){
        buff.set(m.name);
        buff.set(m.att);
        buff.set(m.def);
        buff.set(m.id);
        buff.set(m.hits);
        buff.set(m.numGroups);
        buff.set(m.picID);
        buff.set(m.lockedChance);
        buff.set(m.levelFound);

        for (size_t i = 0; i < 12; i++)
            buff.set(m.resistances[i]);

        buff.set(m.specialPropertyFlags);
        buff.set(m.specialAttackFlags);
        buff.set(m.spellFlags);

        buff.set(m.chance);
        for (size_t i = 0; i < 4; i++)
            buff.set(m.boxChance[i]);

        buff.set(m.alignment);
        buff.set(m.ingroup);
        buff.set(m.goldFactor);
        buff.set(m.trapFlags);
        buff.set(m.guildlevel);

        for (size_t i = 0; i < 7; i++)
            buff.set(m.stats[i]);

        buff.set(m.type);
        buff.set(m.damageMod);
        buff.set(m.companionType);
        buff.set(m.companionSpawnMode);
        buff.set(m.companionID);

        for (size_t i = 0; i < 11; i++)
            buff.set(m.items[i]);

        buff.set(m.subtype);
        buff.set(m.companionSubtype);
        buff.set(m.size);
        buff.set(m.itemDropLevel);
        buff.set(m.itemChance);
        buff.writeRecord();
    }
}

void saveAutomap(const Automap& a, std::string filename) {
    RecordWriter<10> buff(filename);
    buff.set("1.1");
    buff.writeRecord();
    buff.set(a.deepestLevel);
    buff.writeRecord();
	for (auto floor : a.flags) {
		for (auto flag : floor) {
            buff.set(flag);
            buff.writeRecord();
        }
    }
}

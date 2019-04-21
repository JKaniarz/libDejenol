//
//  revealmap.cpp
//  libDejenol
//
//  Created by John Kaniarz on 9/19/18.
//  Copyright © 2018 John Kaniarz. All rights reserved.
//


#include "MTypes.h"
#include "MLoader.h"
#include "MSaver.h"

#include <iostream>
#include <algorithm>

Automap a;
Dungeon d;

constexpr size_t mapIndex(size_t x, size_t y) {
	return y * 30 + x;
}

void revealSquare(size_t floor, size_t x, size_t y) {
	size_t i = mapIndex(x, y);
	uint64_t flags = d[floor].fields[i].flags;
	if ((flags & 1 << Floor::ROCK) == 0) {
		flags |= (i / 30 == 29 ? 1 << Floor::WALL_NORTH : 0) | (i % 30 == 29 ? 1 << Floor::WALL_EAST : 0) | (1 << Floor::EXPLORED);
	}
	a.flags[floor][i] = uint32_t(flags);
}

void revealFloor(size_t floor) {
	a.deepestLevel = std::max(a.deepestLevel, int16_t(floor + 1));
	for (int y = 0; y < d[floor].height; y++) {
		for (int x = 0; x < d[floor].width; x++) {
			revealSquare(floor, x, y);
		}
	}
}

bool canBeExplored(size_t floor, size_t x, size_t y) {
	if (!(d[floor].fields[mapIndex(x, y)].flags & (1 << Floor::ROCK)))
		return true;

	int64_t adjacent[4] = {
		d[floor].fields[mapIndex((x + 1) % 30, y)].flags,
		d[floor].fields[mapIndex((x + 29) % 30, y)].flags,
		d[floor].fields[mapIndex(x, (y + 1) % 30)].flags,
		d[floor].fields[mapIndex(x, (y + 29) % 30)].flags
	};
	for (auto flags : adjacent) {
		if (!(flags & (1 << Floor::ROCK | 1 << Floor::TELEPORTER | 1 << Floor::ANTIMAGIC)))
			return true;
	}
	return false;
}

bool isExplored(size_t floor, size_t x, size_t y) {
	static constexpr uint64_t EXPLORED_MASK = ~0x3F; //Ignore walls and doors
	return (a.flags[floor][mapIndex(x, y)] & EXPLORED_MASK) > 0;
}

void smartRevealSquare(size_t floor, size_t x, size_t y) {
	//reveal only squares that can't be explored
	if (canBeExplored(floor, x, y))
		return;
	//and only if all adjacent, explorable squares have been explored
	if (!isExplored(floor, (x + 1) % 30, y) && canBeExplored(floor, (x + 1) % 30, y))
		return;
	if (!isExplored(floor, (x + 29) % 30, y) && canBeExplored(floor, (x + 29) % 30, y))
		return;
	if (!isExplored(floor, x, (y + 1) % 30) && canBeExplored(floor, x, (y + 1) % 30))
		return;
	if (!isExplored(floor, x, (y + 29) % 30) && canBeExplored(floor, x, (y + 29) % 30))
		return;
	revealSquare(floor, x, y);
}

void smartRevealFloor(size_t floor) {
	for (int y = 0; y < d[floor].height; y++) {
		for (int x = 0; x < d[floor].width; x++) {
			smartRevealSquare(floor, x, y);
		}
	}
}

int main(int argc, char** argv) {
	if (std::rename("MDATA8.MDR", "MDATA8.BAK")) {
		std::cout << "Failed to make MDATA8.BAK" << std::endl;
		return 0;
	}
	a = loadAutomap("MDATA8.BAK");
	d = loadDungeon();
	if (argc == 1) {
		for (int floor = 0; floor <= a.deepestLevel; floor++) {
			smartRevealFloor(floor);
		}
	}
	else {
		for (int i = 1; i < argc; i++) {
			size_t floor = atoi(argv[i]);
			revealFloor(floor);
		}
	}
	
	saveAutomap(a, "MDATA8.MDR");
	return 0;
}

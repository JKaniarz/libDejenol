#include "MTypes.h"
#include "MLoader.h"

#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <map>
#include <array>
#include <list>
#include <string>
#include <bitset>
#include <cmath>
#include "lodepng.h"
#include "RecordWriter.h"



uint32_t popcount(uint32_t x) {
	uint32_t count;
	for (count = 0; x; count++)
		x &= x - 1; //clear lowest set bit
	return count;
}

uint32_t probabilityToColor(double probability) {
	if (probability <= .01) {
		return 0;
	}
	else {
		uint8_t red = uint8_t(std::pow(probability, .5) * 255);
		uint8_t green = 0;
		uint8_t blue = 255 - red;
		uint8_t alpha = 255;
		return (alpha << 24) + (blue << 16) + (green << 8) + red;
	}
}

const size_t NUM_HEATMAPS = MON_NUMKINDS + 1;
Dungeon dungeon = loadDungeon();
uint32_t heatmap[NUM_HEATMAPS][30 * 30 * 15];

int main() {
	memset(heatmap, 0, sizeof(heatmap));
	int index = 0;
	for (size_t level = 0; level < 15; level++) {
		for (size_t y = 0; y < 30; y++) {
			for (size_t x = 0; x < 30; x++) {
				Floor::Field& field = dungeon[level].fields[(29 - y) * 30 + x];
				Floor::Area & area = dungeon[level].areas[field.areaID];
				double probability = (.99f / popcount(area.spawnMask) + (.01 / MON_NUMKINDS)); //even chance of all allowed types + 1% chance of anything
				if (area.lairID > 0) {
					probability *= .25f;
					heatmap[NUM_HEATMAPS-1][index] = 0xFF0000FF;
				}
				if (field.flags & (1 << Floor::ROCK)) probability = 0.0f;

				for (size_t spawn = 0; spawn < MON_NUMKINDS; spawn++) {
					heatmap[spawn][index] = probabilityToColor(area.spawnMask & (1 << spawn) ? probability : 0.0);
				}
				index++;
			}
		}
	}
	for (size_t i = 0; i < NUM_HEATMAPS; i++) {
		size_t j = i + 1;
		lodepng::encode(std::to_string(j / 10) + std::to_string(j % 10) + ".png", reinterpret_cast<uint8_t*>(&heatmap[i][0]), 30, 30 * 15);
	}
}

#include "MLoader.h"
#include "MTypes.h"

#include "lodepng.h"

#include <array>
#include <cassert>
#include <cmath>
#include <string>
#include <vector>

using namespace std;

const size_t MAX_LEVEL = 16;
const uint32_t NUM_CELLS = 30 * 30 * 15;

Dungeon dungeon;
Monsters monsters;

uint32_t monWeights[MON_NUMKINDS][MAX_LEVEL];

void buildWeights() {
	for (auto m : monsters) {
		monWeights[m.type][m.levelFound] += m.chance;
	}
}

array<float, MON_NUMKINDS> &operator+=(array<float, MON_NUMKINDS> &a, const array<float, MON_NUMKINDS> &b) {
	for (size_t i = 0; i < a.size(); i++) {
		a[i] += b[i];
	}
	return a;
}

array<float, MON_NUMKINDS> &operator*=(array<float, MON_NUMKINDS> &a, float b) {
	for (size_t i = 0; i < a.size(); i++) {
		a[i] *= b;
	}
	return a;
}

array<float, MON_NUMKINDS> getOdds(uint32_t monsterFlags, size_t startLevel, size_t endLevel, float weight = 1.0) {
	array<float, MON_NUMKINDS> odds = {};
	array<float, MON_NUMKINDS> weights = {};

	startLevel = max(1ul, startLevel);
	endLevel = min(MAX_LEVEL, endLevel);

	monsterFlags &= (1 << MON_NUMKINDS) - 1;
	if (!monsterFlags) return odds;

	size_t weightTotal = 0;
	size_t prevWeight = 0;
	for (size_t i = 0; i < MON_NUMKINDS; i++) {
		if (~monsterFlags & (1 << i)) continue;
		for (size_t j = startLevel; j <= endLevel; j++) {
			weights[i] += monWeights[i][j];
		}
		if (!prevWeight) prevWeight = weights[i]; // Initialize to first non-zero weight
		weightTotal += weights[i];
	}
	if (weightTotal == 0) return odds;

	float oddsAccum = 0.0f;
	size_t lastValidIndex = 0;
	for (size_t i = 0; i < MON_NUMKINDS; i++) {
		if (~monsterFlags & (1 << i)) continue;
		if (weights[i] == 0) continue;

		lastValidIndex = i;
		odds[i] = (100.0f / 101.0f) * prevWeight / weightTotal;
		prevWeight = weights[i];

		if (odds[i] + oddsAccum > 1.0f) {
			odds[i] = 1.0f - oddsAccum;
			oddsAccum = 1.0f;
		} else {
			oddsAccum += odds[i];
		}
		odds[i] *= weight;
	}

	if (oddsAccum < 1.0f) {
		odds[lastValidIndex] += (1.0f - oddsAccum) * weight;
	}

	return odds;
}

uint32_t probabilityToColor(float probability) {
	constexpr float minRange = 0.01f;
	constexpr float maxRange = 0.75f;
	constexpr float gamma = 0.5f;

	if (probability <= minRange) return 0;

	probability = min((probability - minRange) / (maxRange - minRange), 1.0f);
	probability = probability * (1 - minRange) + minRange;
	probability = pow(probability, gamma);

	uint8_t red = uint8_t(probability * 255);
	uint8_t green = 0x00;
	uint8_t blue = 0xFF - red;
	uint8_t alpha = 0xFF;
	return (alpha << 24) + (blue << 16) + (green << 8) + red;
}

int main() {
	array<std::array<uint32_t, NUM_CELLS>, MON_NUMKINDS + 1> image = {};
	dungeon = loadDungeon();
	monsters = loadMonsters();
	buildWeights();
	int index = -1;
	for (size_t l = 0; l < 15; l++) {
		for (size_t y = 0; y < 30; y++) {
			for (size_t x = 0; x < 30; x++) {
				index++;
				std::array<float, MON_NUMKINDS> odds = {};
				Floor::Field &field = dungeon[l].fields[(29 - y) * 30 + x];
				Floor::Area &area = dungeon[l].areas[field.areaID];

				if (field.flags & (1 << Floor::ROCK)) continue;

				size_t level = l + 1;
				size_t span = (level + 2) / 5;
				if (field.flags & (1 << Floor::STUD)) {
					odds += getOdds(area.spawnMask, level, level + 1, 2.0f / 3.0f);
					odds += getOdds(area.spawnMask, level + 1 - span, level + 1, 0.51f / 3.0f);
					odds += getOdds(area.spawnMask, level + 1 - span - 1, level + 1, 0.49f / 3.0f);
				} else {
					odds += getOdds(area.spawnMask, level - span, level, 0.51f);
					odds += getOdds(area.spawnMask, level - span - 1, level, 0.49f);
				}

				if (l > 0 || field.areaID > 18) {
					// 1% chance of anything if outside starter zone
					for (size_t i = 0; i < MON_NUMKINDS; i++) {
						odds[i] = (odds[i] * 0.99f + 0.01f);
					}
				}

				// lairs spawn a specific monster 75% of the time
				if (area.lairID > 0) {
					odds *= 0.25;
					image[MON_NUMKINDS][index] = probabilityToColor(.75);
				}

				for (size_t i = 0; i < odds.size(); i++) {
					image[i][index] = probabilityToColor(odds[i]);
				}
			}
		}
	}
	for (size_t i = 0; i < image.size(); i++) {
		size_t j = i + 1;
		lodepng::encode(string("SuperSeer/") + std::to_string(j / 10) + std::to_string(j % 10) + ".png",
		                reinterpret_cast<uint8_t *>(image[i].data()), 30, 30 * 15);
	}
}

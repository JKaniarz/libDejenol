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

#define FLAG(x) (1<<x)

enum SpecialType {
	STAIRS,
	TELEPORTER,
	CHUTE
};

struct Shortcut {
	int x1, y1, z1;
	int x2, y2, z2;
	int kind;
};

enum FillType {
	EMPTY,
	SOLID,
	SPECIAL
};


Dungeon dungeon;

bool hwalls[15][31][30];
bool vwalls[15][30][31];
int fill[15][30][30];
std::vector<Shortcut> shortcuts;

void convertDungeon() {
	memset(hwalls, 0, sizeof(hwalls));
	memset(vwalls, 0, sizeof(vwalls));
	memset(fill, 0, sizeof(fill));
	for (int z = 0; z < 15; z++) {
		Floor& floor = dungeon[z];
		for (int y = 0; y < 30; y++) {
			//the border walls aren't necessarily marked
			hwalls[z][0][y] = true;
			hwalls[z][30][y] = true;
			vwalls[z][y][0] = true;
			vwalls[z][y][30] = true;
			for (int x = 0; x < 30; x++) {
				uint64_t flags = floor.fields[y * 30 + x].flags;
				if (flags & FLAG(Floor::WALL_NORTH)) hwalls[z][y + 1][x] = true;
				if (flags & FLAG(Floor::WALL_EAST)) vwalls[z][y][x + 1] = true;

				if (flags & FLAG(Floor::ROCK)) fill[z][y][x] = SOLID;
				else if (flags & FLAG(Floor::STAIRS_UP) && z > 0) { //ignore stairs to town
					shortcuts.push_back({ x, y, z,x,y,z - 1, STAIRS });
					Shortcut& s = shortcuts.back();
					fill[s.z1][s.y1][s.x1] = SPECIAL;
					fill[s.z2][s.y2][s.x2] = SPECIAL;
				}
				else if (flags & FLAG(Floor::STAIRS_DOWN)) {
					shortcuts.push_back({ x, y, z,x,y,z + 1, STAIRS });
					Shortcut& s = shortcuts.back();
					fill[s.z1][s.y1][s.x1] = SPECIAL;
					fill[s.z2][s.y2][s.x2] = SPECIAL;
				}
			}
		}
		for (auto t : floor.teleporters) {
			if (t.x2 == 0 && t.y2 == 0 && t.z2 == 0) {
				fill[z][t.y - 1][t.x - 1] = SOLID; //mark random teleporters as rock
			}
			else {
				shortcuts.push_back({ t.x - 1, t.y - 1, z, t.x2 - 1, t.y2 - 1, t.z2 > 0 ? t.z2 - 1 : z, TELEPORTER });
				Shortcut& s = shortcuts.back();
				fill[s.z1][s.y1][s.x1] = SPECIAL;
				fill[s.z2][s.y2][s.x2] = SPECIAL;
			}
		}
		for (auto c : floor.chutes) {
			shortcuts.push_back({ c.x - 1,c.y - 1,z,c.x - 1,c.y - 1, z + c.depth, CHUTE });
			Shortcut& s = shortcuts.back();
			fill[s.z1][s.y1][s.x1] = SPECIAL;
			fill[s.z2][s.y2][s.x2] = SPECIAL;
		}
	}
	for (auto s : shortcuts) {
		assert(s.z2 >= 0);
	}
}

typedef std::tuple<int, int, int> Coord;

struct PathTile {
	bool visited;
	int distance;
	int x, y, z;
	PathTile() :visited(false), distance(0), x(0), y(0), z(0) {}
	PathTile(bool visited, int distance, int x, int y, int z) :visited(visited), distance(distance), x(x), y(y), z(z) {}
};

class ForwardSolver {
	std::list<Coord> frontier;
public:
	PathTile path[15][30][30];

	ForwardSolver(int x1, int y1, int z1) {
		frontier.clear();
		memset(path, 0, sizeof(path));

		path[z1][y1][x1] = PathTile(true, 0, -1, -1, -1);
		frontier.push_back({ x1,y1,z1 });
		while (!frontier.empty()) {
			int x, y, z;
			std::tie(x, y, z) = frontier.front();
			frontier.pop_front();
			//if (x == x2 && y == y2 && z == z2) break;
			explore(x, y, z);
		}
	}
	std::list<Coord> findPathTo(int x2, int y2, int z2) {
		std::list<Coord> thePath;
		if (!path[z2][y2][x2].visited) { //unable to reach destination
			return thePath;
		}
		int x = x2, y = y2, z = z2;
		while (z >= 0) {
			thePath.push_front({ x,y,z });
			PathTile& p = path[z][y][x];
			x = p.x;
			y = p.y;
			z = p.z;

		}
		return thePath;
	}
private:
	void explore(int x, int y, int z) {
		int distance = path[z][y][x].distance;
		switch (fill[z][y][x]) {
		case SPECIAL:
			for (auto s : shortcuts) {
				if (s.x1 == x && s.y1 == y && s.z1 == z) {
					switch (s.kind) {
					case TELEPORTER:
						if (!path[s.z2][s.y2][s.x2].visited) {
							path[s.z2][s.y2][s.x2] = { true, distance, x,y,z };
							frontier.push_front({ s.x2,s.y2,s.z2 }); //push front because the distance doesn't increase
						}
						return;
					case CHUTE:
						if (false) {
							return; //chutes are to be avoided
						}
						//fall through to treat like stairs
					case STAIRS:
						if (!path[s.z2][s.y2][s.x2].visited) {
							path[s.z2][s.y2][s.x2] = { true, distance + 1, x,y,z };
							frontier.push_back({ s.x2,s.y2,s.z2 });
						}
					}
				}
			}
			//Fall through
		case EMPTY:
			if (!hwalls[z][y][x] && !path[z][y - 1][x].visited) {
				path[z][y - 1][x] = { true, distance + 1, x,y,z };
				frontier.push_back({ x,y - 1,z });
			}
			if (!hwalls[z][y + 1][x] && !path[z][y + 1][x].visited) {
				path[z][y + 1][x] = PathTile(true, distance + 1, x, y, z);
				frontier.push_back({ x,y + 1,z });
			}
			if (!vwalls[z][y][x] && !path[z][y][x - 1].visited) {
				path[z][y][x - 1] = { true, distance + 1, x,y,z };
				frontier.push_back({ x - 1,y,z });
			}
			if (!vwalls[z][y][x + 1] && !path[z][y][x + 1].visited) {
				path[z][y][x + 1] = { true, distance + 1, x,y,z };
				frontier.push_back({ x + 1,y,z });
			}
			break;
		case SOLID:
			path[z][y][x].distance = INT16_MAX;
			return;
		default:
			assert(false);
		};
	}
};

class ReverseSolver {
	std::list<Coord> frontier;
public:
	PathTile path[15][30][30];
	ReverseSolver(int x2, int y2, int z2) {
		frontier.clear();
		memset(path, 0, sizeof(path));

		path[z2][y2][x2] = { true, 0, -1, -1, -1 };
		frontier.push_back({ x2,y2,z2 });
		while (!frontier.empty()) {
			int x, y, z;
			std::tie(x, y, z) = frontier.front();
			frontier.pop_front();
			reverseExplore(x, y, z);
		}
	}
	std::list<Coord> findPathFrom(int x1, int y1, int z1) {
		std::list<Coord> thePath;
		if (!path[z1][y1][x1].visited) { //unable to reach destination
			return thePath;
		}
		int x = x1, y = y1, z = z1;
		while (z >= 0) {
			thePath.push_back({ x,y,z });
			PathTile& p = path[z][y][x];
			x = p.x;
			y = p.y;
			z = p.z;

		}
		return thePath;
	}
private:
	bool canReverseExplore(int x, int y, int z) {
		if (path[z][y][x].visited) return false;
		switch (fill[z][y][x]) {
		case SPECIAL:
			for (auto s : shortcuts) {
				if (s.kind == TELEPORTER && s.x1 == x && s.y1 == y && s.z1 == z) {
					return false;
				}
			}
			//fall through
		case EMPTY:
			return true;
		case SOLID:
			path[z][y][x] = { true, INT32_MAX, -1,-1,-1 };
			return false;
		default:
			assert(false);
			return false;
		}
	}

	void reverseExplore(int x, int y, int z) {
		Coord bad = Coord({ -1,-2,0 });
		int distance = path[z][y][x].distance;
		if (!hwalls[z][y][x] && canReverseExplore(x, y - 1, z)) {
			path[z][y - 1][x] = { true, distance + 1, x,y,z };
			frontier.push_back({ x,y - 1,z });
			assert(frontier.back() != bad);
		}
		if (!hwalls[z][y + 1][x] && canReverseExplore(x, y + 1, z)) {
			path[z][y + 1][x] = PathTile(true, distance + 1, x, y, z);
			frontier.push_back({ x,y + 1,z });
			assert(frontier.back() != bad);
		}
		if (!vwalls[z][y][x] && canReverseExplore(x - 1, y, z)) {
			path[z][y][x - 1] = { true, distance + 1, x,y,z };
			frontier.push_back({ x - 1,y,z });
			assert(frontier.back() != bad);
		}
		if (!vwalls[z][y][x + 1] && canReverseExplore(x + 1, y, z)) {
			path[z][y][x + 1] = { true, distance + 1, x,y,z };
			frontier.push_back({ x + 1,y,z });
			assert(frontier.back() != bad);
		}
		if (fill[z][y][x] == SPECIAL) {
			for (auto s : shortcuts) {
				if (s.x2 == x && s.y2 == y && s.z2 == z) {
					if (path[s.z1][s.y1][s.x1].visited)    continue;
					switch (s.kind) {
					case TELEPORTER:
						path[s.z1][s.y1][s.x1] = { true, distance, x,y,z };
						frontier.push_front({ s.x1,s.y1,s.z1 }); //push front because the distance doesn't increase
						assert(frontier.back() != bad);
						continue;
					case CHUTE:
						if (false) {
							return; //chutes are to be avoided
						}
						//fall through to treat like stairs
					case STAIRS:
						path[s.z1][s.y1][s.x1] = { true, distance + 1, x,y,z };
						frontier.push_back({ s.x1, s.y1, s.z1 });
						assert(frontier.back() != bad);
					}
				}
			}
		};
	}
};

int main() {
	dungeon = loadDungeon();
	convertDungeon();

	ForwardSolver *fs = new ForwardSolver(8, 10, 0);
	ReverseSolver *rs = new ReverseSolver(12, 3, 14);
	int a[3];
	int b[3];
	int mindist = INT32_MAX;

	int dirs[4][2] = {
		{1,0},
		{0,1},
		{29,0},
		{0,29}
	};
	for (int z = 0; z < 15; z++) {
		for (int y = 0; y < 30; y++) {
			for (int x = 0; x < 30; x++) {
				for (auto d : dirs) {
					int x2 = (x + d[0]) % 30;
					int y2 = (y + d[1]) % 30;
					PathTile & p1 = fs->path[z][y][x];
					PathTile & p2 = rs->path[z][y2][x2];
					if (!p1.visited || !p2.visited) continue;
					int dist = p1.distance + p2.distance;
					if (dist < mindist && fill[z][y][x] != SOLID && fill[z][y2][x2] != SOLID) {
						mindist = dist;
						a[0] = x;
						a[1] = y;
						a[2] = z;
						b[0] = x2;
						b[1] = y2;
						b[2] = z;
					}
				}
			}
		}
	}
	for (auto t : fs->findPathTo(20, 2, 9)) {
		int x, y, z;
		std::tie(x, y, z) = t;
		std::cout << x + 1 << "," << y + 1 << "," << z + 1 << std::endl;
	}
	std::cout << "--------" << std::endl;
	//    for (auto t : fs.findPathTo(a[0],a[1],a[2])) {
	//        int x, y, z;
	//        std::tie(x, y, z) = t;
	//        std::cout << x+1<<","<<y+1<<","<<z+1 << std::endl;
	//    }
	//    for (auto t : rs.findPathFrom(b[0],b[1],b[2])) {
	//        int x, y, z;
	//        std::tie(x, y, z) = t;
	//        std::cout << x+1<<","<<y+1<<","<<z+1 << std::endl;
	//    }
}

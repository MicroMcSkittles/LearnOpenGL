#include <iostream>
#include "WAD.h"

int main() {


	WAD wad;
	Map map;
	GameConfig config;

	map.name = "E1M1";

	LoadWAD("assets/doom.wad", &wad);
	LoadMap(&wad, &map);
	LoadGameConfig(&wad, &config);

	std::cout << GenerateConsoleText(&wad, &map, &config) << std::endl;

	return 0;
}
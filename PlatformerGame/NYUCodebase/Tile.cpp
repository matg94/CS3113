#include "Tile.h"


Tile::Tile(int tpe) : type(tpe), size(0.5f) {
	if (tpe == 1 || tpe == 2) {
		textureIndex = 3;
	}
	else {
		textureIndex = 0;
	}
}
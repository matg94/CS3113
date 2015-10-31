#include "Tile.h"


Tile::Tile(int tpe) : type(tpe), size(0.5f), xPos(0.0f), yPos(0.0f) {
	if (tpe == 1) {
		textureIndex = 17;
	}
	else if (tpe == 2) {
		textureIndex = 164;
	}
	else {
		textureIndex = 0;
	}
}

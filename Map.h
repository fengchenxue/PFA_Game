#pragma once
#include "GamesEngineeringBase.h"
#include "DataStructure.h"
#include "Character.h"
#include <fstream>
#include<iostream>

class Tile {
public:
	GamesEngineeringBase::Image Texture;
	Vec2 Position;
	bool hasCollision = false;

	Tile() :Texture(), Position(0, 0) {}

	bool isInView(GamesEngineeringBase::Window& canvas, Player& player);

	void draw(GamesEngineeringBase::Window& canvas, Player& player);
};

class TileSet {
public:
	myVector<Tile> tiles;
	TileSet() :tiles(25) { tiles.Initialize(); }

	void load();

	Tile* operator[](int index) {
		return tiles[index];
	}
};

class World {
public:
	int templateTiles[50][50] = { 0 };
	TileSet tilesSet;
	
	Vec2 Position;


	bool endless;
	int tilesWide=0;
	int tilesHigh=0;
	int tileWidth=0;
	int tileHeight=0;

	World(std::string filename="Res/tiles.txt", bool _endless = false);
	
	void draw(GamesEngineeringBase::Window& canvas, Player& player);
	//check tile collision
	void checkCollision(GamesEngineeringBase::Window& canvas,Player& player,float _dt,Vec2 &Direction);
	//check boundary of the map
	void checkBoundary(GamesEngineeringBase::Window& canvas, Player& player);

	//if the player is out of the center templateTiles, update the Position of the map
	void updatePosition(Player& player);
	//check if the whole templateTiles is in the view of the player
	bool isInView(GamesEngineeringBase::Window& canvas, Player& player,Vec2 _Position);
	
	//calculate the index of the tile, if it is out of the templateTiles
	int calculateTileIndexX(int x) {
		if (x < 0) return x + tilesWide;
		if (x >= tilesWide) return x - tilesWide;
		return x;
	}
	int calculateTileIndexY(int y) {
		if (y < 0) return y + tilesHigh;
		if (y >= tilesHigh) return y - tilesHigh;
		return y;
	}
};
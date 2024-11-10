#include"Map.h"
#include<sstream>


bool Tile::isInView(GamesEngineeringBase::Window& canvas, Player& player) const
{
	if (Position.x + Texture.width < player.CameraPosition.x || player.CameraPosition.x + canvas.getWidth() < Position.x ||
		Position.y + Texture.height < player.CameraPosition.y || player.CameraPosition.y + canvas.getHeight() < Position.y) return false;
	return true;
}

void Tile::draw(GamesEngineeringBase::Window& canvas, Player& player)
{
	if (isInView(canvas, player)) {
		for (unsigned int x = 0; x < Texture.width; x++) {
			for (unsigned int y = 0; y < Texture.height; y++) {
				if (Position.x + x >= player.CameraPosition.x && Position.x + x <= player.CameraPosition.x + canvas.getWidth() - 1 &&
					Position.y + y >= player.CameraPosition.y && Position.y + y <= player.CameraPosition.y + canvas.getHeight() - 1) {

					canvas.draw(static_cast<int>(std::round(x + Position.x - player.CameraPosition.x)),
						static_cast<int>(std::round(y + Position.y - player.CameraPosition.y)),
						Texture.at(x, y));

				}
			}
		}
	}
}



void TileSet::load()
{
	for (unsigned int i = 0; i <= 23; i++) {
		tiles[i]->Texture.load("Res/" + std::to_string(i) + ".png");
		if (i >= 14 && i <= 22) {
			tiles[i]->hasCollision = true;
		}
	}

}

void World::initialize(bool _endless)
{
	endless = _endless;
	tilesSet.load();
	std::ifstream infile("Res/tiles.txt");
	if (!infile.is_open()) {
		throw std::runtime_error("Cannot open file");
	}

	//get every line of the file
	std::string inputLine;
	std::stringstream ss;
	std::string temp;
	while (std::getline(infile, inputLine)) {
		if (inputLine.find("tileswide") != std::string::npos) {
			tilesWide = std::stoi(inputLine.substr(inputLine.find(" ") + 1));
		}
		else if (inputLine.find("tileshigh") != std::string::npos) {
			tilesHigh = std::stoi(inputLine.substr(inputLine.find(" ") + 1));
		}
		else if (inputLine.find("tilewidth") != std::string::npos) {
			tileWidth = std::stoi(inputLine.substr(inputLine.find(" ") + 1));
		}
		else if (inputLine.find("tileheight") != std::string::npos) {
			tileHeight = std::stoi(inputLine.substr(inputLine.find(" ") + 1));
		}
		else if (inputLine.find("layer 0") != std::string::npos) {
			//get the tile data
			for (int i = 0; i < tilesHigh; i++) {
				std::getline(infile, inputLine);
				ss.str(inputLine);
				for (int j = 0; j < tilesWide; j++) {
					std::getline(ss, temp, ',');
					if (!temp.empty()) {
						//std::cout << temp << " ";
						templateTiles[j][i] = std::stoi(temp);
					}

				}
			}
		}
	}
}

void World::draw(GamesEngineeringBase::Window& canvas, Player& player)
{
	if (endless) updatePosition(player);
	//draw center tilateTiles 
	for (int i = 0; i < tilesWide; i++) {
		for (int j = 0; j < tilesHigh; j++) {
			tilesSet[templateTiles[i][j]]->Position = { Position.x + static_cast<float>(i * 32), Position.y + static_cast<float>(j * 32) };
			tilesSet[templateTiles[i][j]]->draw(canvas, player);
		}
	}
	//if endless, use 9 templateTiles to draw the map
	if (endless) {
		//draw left
		if (isInView(canvas, player, Position - Vec2(static_cast<float>(tilesWide * tileWidth), 0))) {
			for (int i = 0; i < tilesWide; i++) {
				for (int j = 0; j < tilesHigh; j++) {
					tilesSet[templateTiles[i][j]]->Position = { Position.x + static_cast<float>(i * 32 - tilesWide * tileWidth),Position.y + static_cast<float>(j * 32) };
					tilesSet[templateTiles[i][j]]->draw(canvas, player);
				}
			}
		}
		//draw right
		if (isInView(canvas, player, Position + Vec2(static_cast<float>(tilesWide * tileWidth), 0))) {
			for (int i = 0; i < tilesWide; i++) {
				for (int j = 0; j < tilesHigh; j++) {
					tilesSet[templateTiles[i][j]]->Position = { Position.x + static_cast<float>(i * 32 + tilesWide * tileWidth),Position.y + static_cast<float>(j * 32) };
					tilesSet[templateTiles[i][j]]->draw(canvas, player);
				}
			}
		}
		//draw up
		if (isInView(canvas, player, Position - Vec2(0, static_cast<float>(tilesHigh * tileHeight)))) {
			for (int i = 0; i < tilesWide; i++) {
				for (int j = 0; j < tilesHigh; j++) {
					tilesSet[templateTiles[i][j]]->Position = { Position.x + static_cast<float>(i * 32),Position.y + static_cast<float>(j * 32 - tilesHigh * tileHeight) };
					tilesSet[templateTiles[i][j]]->draw(canvas, player);
				}
			}
		}
		//draw down
		if (isInView(canvas, player, Position + Vec2(0, static_cast<float>(tilesHigh * tileHeight)))) {
			for (int i = 0; i < tilesWide; i++) {
				for (int j = 0; j < tilesHigh; j++) {
					tilesSet[templateTiles[i][j]]->Position = { Position.x + static_cast<float>(i * 32),Position.y + static_cast<float>(j * 32 + tilesHigh * tileHeight) };
					tilesSet[templateTiles[i][j]]->draw(canvas, player);
				}
			}
		}
		//draw left up
		if (isInView(canvas, player, Position - Vec2(static_cast<float>(tilesWide * tileWidth), static_cast<float>(tilesHigh * tileHeight)))) {
			for (int i = 0; i < tilesWide; i++) {
				for (int j = 0; j < tilesHigh; j++) {
					tilesSet[templateTiles[i][j]]->Position = { Position.x + static_cast<float>(i * 32 - tilesWide * tileWidth),Position.y + static_cast<float>(j * 32 - tilesHigh * tileHeight) };
					tilesSet[templateTiles[i][j]]->draw(canvas, player);
				}
			}
		}
		//draw right up
		if (isInView(canvas, player, Position + Vec2(static_cast<float>(tilesWide * tileWidth), static_cast<float>(-tilesHigh * tileHeight)))) {
			for (int i = 0; i < tilesWide; i++) {
				for (int j = 0; j < tilesHigh; j++) {
					tilesSet[templateTiles[i][j]]->Position = { Position.x + static_cast<float>(i * 32 + tilesWide * tileWidth),Position.y + static_cast<float>(j * 32 - tilesHigh * tileHeight) };
					tilesSet[templateTiles[i][j]]->draw(canvas, player);
				}
			}
		}
		//draw left down
		if (isInView(canvas, player, Position - Vec2(static_cast<float>(tilesWide * tileWidth), static_cast<float>(-tilesHigh * tileHeight)))) {
			for (int i = 0; i < tilesWide; i++) {
				for (int j = 0; j < tilesHigh; j++) {
					tilesSet[templateTiles[i][j]]->Position = { Position.x + static_cast<float>(i * 32 - tilesWide * tileWidth),Position.y + static_cast<float>(j * 32 + tilesHigh * tileHeight) };
					tilesSet[templateTiles[i][j]]->draw(canvas, player);
				}
			}
		}
		//draw right down
        if (isInView(canvas, player, Position + Vec2(static_cast<float>(tilesWide * tileWidth), static_cast<float>(tilesHigh * tileHeight)))) {
			for (int i = 0; i < tilesWide; i++) {
				for (int j = 0; j < tilesHigh; j++) {
					tilesSet[templateTiles[i][j]]->Position = { Position.x + static_cast<float>(i * 32 + tilesWide * tileWidth),Position.y + static_cast<float>(j * 32 + tilesHigh * tileHeight) };
					tilesSet[templateTiles[i][j]]->draw(canvas, player);
				}
			}
		}

	}

}

void World::checkCollision(GamesEngineeringBase::Window& canvas, Player& player, float _dt, Vec2& Direction)
{
	//if the map is endless, update the Position of the Center templateTiles
	if (endless) {
		updatePosition(player);
		//calculate the index of the tile that the player is on
		int indexX = static_cast<int>((player.CenterPosition.x - Position.x) / tileWidth);
		int indexY = static_cast<int>((player.CenterPosition.y - Position.y) / tileHeight);
		//middle variables to store the position of the tile that the player is on
		Vec2 tempPosition;
		//check collisions of 5*5 tiles around the player
		for (int i = -2; i <= 2; i++) {
			for (int j = -2; j <= 2; j++) {
				//check if the tile has collision. The ocean is not accessible 
				if (tilesSet[templateTiles[calculateTileIndexX(indexX + i)][calculateTileIndexY(indexY + j)]]->hasCollision) {
					//middle variables to store the position of the tile that the player is on
					tempPosition = { Position.x + (indexX + i) * tileWidth,Position.y + (indexY + j) * tileHeight };
					//check if the player collides with the tile
					Vec2 overlap1{ max(player.Position.x,tempPosition.x),max(player.Position.y,tempPosition.y) };
					Vec2 overlap2{ min(player.Position.x + 2*player.collisionRadius,tempPosition.x + tileWidth),min(player.Position.y + 2*player.collisionRadius,tempPosition.y + tileHeight) };
					//push the player out of the tile
					if (overlap1.x < overlap2.x && overlap1.y < overlap2.y) {
						if (overlap2.x - overlap1.x > overlap2.y - overlap1.y) {
							if (abs(overlap1.y -player.Position.y)<=0.001) {
								player.Position.y += overlap2.y - overlap1.y;
							}
							else {
								player.Position.y -= overlap2.y - overlap1.y;
							}
						}
						else {
							if (abs(overlap1.x - player.Position.x) <= 0.001) {
								player.Position.x += overlap2.x - overlap1.x;
							}
							else {
								player.Position.x -= overlap2.x - overlap1.x;
							}
						}
						player.updateCameraAndCenter(canvas);
					}
				}
			}
		}
	}
	//if the map is not endless, check the collision with the center templateTiles
	else {
		int indexX = static_cast<int>((player.CenterPosition.x - Position.x) / tileWidth);
		int indexY = static_cast<int>((player.CenterPosition.y - Position.y) / tileHeight);
		//middle variables to store the position of the tile that the player is on
		Vec2 tempPosition;
		//check collisions of 5*5 tiles around the player
		for (int i = -2; i <= 2; i++) {
			for (int j = -2; j <= 2; j++) {
				//skip the tile that is out of the map
				if (indexX + i >= 0 && indexX + i < tilesWide && indexY + j >= 0 && indexY + j < tilesHigh) {
					//check if the tile has collision.The ocean is not accessible 
					if (tilesSet[templateTiles[indexX + i][indexY + j]]->hasCollision) {
						//middle variables to store the position of the tile that the player is on
						tempPosition = { Position.x + (indexX + i) * tileWidth,Position.y + (indexY + j) * tileHeight };
						//check if the player collides with the tile
						Vec2 overlap1{ max(player.Position.x,tempPosition.x),max(player.Position.y,tempPosition.y) };
						Vec2 overlap2{ min(player.Position.x + 2 * player.collisionRadius,tempPosition.x + tileWidth),min(player.Position.y + 2 * player.collisionRadius,tempPosition.y + tileHeight) };
						//push the player out of the tile
						if (overlap1.x < overlap2.x && overlap1.y < overlap2.y) {
							if (overlap2.x - overlap1.x > overlap2.y - overlap1.y) {
								if (abs(overlap1.y - player.Position.y) <= 0.001) {
									player.Position.y += overlap2.y - overlap1.y;
								}
								else {
									player.Position.y -= overlap2.y - overlap1.y;
								}
							}
							else {
								if (abs(overlap1.x - player.Position.x) <= 0.001) {
									player.Position.x += overlap2.x - overlap1.x;
								}
								else {
									player.Position.x -= overlap2.x - overlap1.x;
								}
							}
							player.updateCameraAndCenter(canvas);
						}
					}
				}
			}
		}
	}
}

void World::checkBoundary(GamesEngineeringBase::Window& canvas, Player& player) const
{
	if (!endless) {
		player.Position.x = max(player.Position.x, Position.x);
		player.Position.x = min(player.Position.x, Position.x + tilesWide * tileWidth - 2*player.collisionRadius);
		player.Position.y = max(player.Position.y, Position.y);
		player.Position.y = min(player.Position.y, Position.y + tilesHigh * tileHeight - 2*player.collisionRadius);
	}
}

void World::updatePosition(Player& player)
{
	if (endless) {
		if (player.CenterPosition.x < Position.x) {
			Position.x -= static_cast<float>(tilesWide * tileWidth);
		}
		else if (player.CenterPosition.x > Position.x + tilesWide * tileWidth) {
			Position.x += static_cast<float>(tilesWide * tileWidth);
		}
		if (player.CenterPosition.y < Position.y) {
			Position.y -= static_cast<float>(tilesHigh * tileHeight);
		}
		else if (player.CenterPosition.y > Position.y + tilesHigh * tileHeight) {
			Position.y += static_cast<float>(tilesHigh * tileHeight);
		}
	}
}


bool World::isInView(GamesEngineeringBase::Window& canvas, Player& player, Vec2 _Position) const
{
	if (_Position.x + tilesWide * tileWidth < player.CameraPosition.x || player.CameraPosition.x + canvas.getWidth() < _Position.x ||
		_Position.y + tilesHigh * tileHeight < player.CameraPosition.y || player.CameraPosition.y + canvas.getHeight() < _Position.y) return false;
	return true;
}

void World::save(std::string filename)
{
	std::ofstream file(filename);
	if (!file.is_open()) {
		throw std::runtime_error("Cannot open file");
	}
	file.write(reinterpret_cast<char*>(&endless), sizeof(endless));
}

void World::load(std::string filename)
{
	std::ifstream file(filename);
	if (!file.is_open()) {
		throw std::runtime_error("Cannot open file");
	}
	bool _endless=false;
	file.read(reinterpret_cast<char*>(&_endless), sizeof(_endless));
	initialize(_endless);
}

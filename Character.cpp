#include"Character.h"
#include<cmath>

void Character::updateAttackProgress(float _dt)
{
	attackProgress += _dt;
}

bool Character::canAttack(bool ifReset)
{
	if (attackProgress >= attackCooldown) {
		if (ifReset) attackProgress = 0;
		else attackProgress -= attackCooldown;
		return true;
	}
	return false;
}

bool Character::getDamage(int damage)
{
	HP -= damage;
	if (HP <= 0) {
		return true;
	}
	return false;
}


Player::Player(std::string filename, float PositionX, float PositionY, int _HP, float _MoveSpeed, int _AttackPower, float _collisionRadius, float _attackThreshold, int _AOECount)
{
	Texture.load(filename);
	Position.x = PositionX;
	Position.y = PositionY;
	CenterPosition.x = Position.x + static_cast<float>(Texture.width) / 2;
	CenterPosition.y = Position.y + static_cast<float>(Texture.height) / 2;
	HP = _HP;
	MoveSpeed = _MoveSpeed;
	AttackPower = _AttackPower;
	collisionRadius = _collisionRadius;
	attackCooldown = _attackThreshold;
	AOECount = _AOECount;
}

void Player::updateAttackProgress(float _dt)
{
	Character::updateAttackProgress(_dt);
	AOEProgress += _dt;
}

bool Player::canAOE()
{
	if (AOEProgress >= AOECoolDown) {
		AOEProgress = 0;
		return true;
	}
	return false;
}

void Player::getEXPAndScore(int _EXP,int _score)
{
	score += _score;
	EXP += _EXP;
	if (EXP >= 50*Level) {
		EXP -= 50*Level;
		Level++;
		AOECount = 2 + Level/2;
		attackCooldown = attackCooldown * 0.90f;
		HP = 100 + 10 * Level;
	}
	//std::cout <<" Level:" << Level << " attackcooldow" << attackCooldown << std::endl;
}



void Player::PlayerMovement(Vec2& MoveDirection, float dt)
{
	Position = Position + MoveDirection * MoveSpeed * dt;

}



void Player::checkBulletCollision(BulletPool& bulletPool)
{
	for (size_t i = 0; i < bulletPool.EnemyBullets.size; i++) {
		if ((CenterPosition - bulletPool.EnemyBullets.Data[i]->CenterPosition).length2() < std::pow(collisionRadius + bulletPool.EnemyBullets.Data[i]->Radius, 2)) {
			getDamage(bulletPool.EnemyBullets.Data[i]->AttackPower);
			bulletPool.releaseBullet(false, bulletPool.EnemyBullets.Data[i]);
			i--;
		}
	}
}

void Player::updateCameraAndCenter(GamesEngineeringBase::Window& canvas)
{
	CameraPosition.x = Position.x - static_cast<float>(canvas.getWidth()- Texture.width) / 2;
	CameraPosition.y = Position.y - static_cast<float>(canvas.getHeight()- Texture.height) / 2;
	CenterPosition.x = Position.x + static_cast<float>(Texture.width) / 2;
	CenterPosition.y = Position.y + static_cast<float>(Texture.height) / 2;
}


void Player::draw(GamesEngineeringBase::Window& canvas)
{
	for (unsigned int x = 0; x < Texture.width; x++) {
		for (unsigned int y = 0; y < Texture.height; y++) {
			if (Texture.hasAlpha()) {
				canvas.drawPNG(x + (canvas.getWidth() - Texture.width) / 2, y + (canvas.getHeight() - Texture.height) / 2, Texture.at(x, y));
			}
			else {
				canvas.draw(x + (canvas.getWidth() - Texture.width) / 2, y + (canvas.getHeight() - Texture.height), Texture.at(x, y));
			}

		}
	}
}

void Player::spawnBullet(EnemyPool& enemyPool, BulletPool& bulletPool, float _dt)
{
	updateAttackProgress(_dt);
	if (enemyPool.nearestEnemy)
	{
		if (canAttack(false)) {
			Vec2 Direction = enemyPool.nearestEnemy->CenterPosition - this->CenterPosition;
			Direction.normalize();
			bulletPool.spawnBullet(this->CenterPosition, Direction,AttackPower, true);
		}
	}
}


void Enemy::EnemyMovementAndUpdateCenter(Vec2& MoveDirection, float dt)
{
	Position = Position + MoveDirection * MoveSpeed * dt;
	CenterPosition.x = Position.x + static_cast<float>(Texture.width) / 2;
	CenterPosition.y = Position.y + static_cast<float>(Texture.height) / 2;
}

void Enemy::Initialize(unsigned int _EnemyType, float PositionX, float PositionY)
{
	Position.x = PositionX;
	Position.y = PositionY;
	CenterPosition.x = Position.x + static_cast<float>(Texture.width) / 2;
	CenterPosition.y = Position.y + static_cast<float>(Texture.height) / 2;
	EnemyType = _EnemyType;
	if (EnemyType == 1) {
		HP = 150;
		MoveSpeed = 100.f;
		AttackPower = 5;
		Texture.load("Res/Enemy1.png");
		collisionRadius = 45.f;
		attackProgress = 1.f;
		attackCooldown = 1.f;
		EXP = 20;
		score = 20;

	}
	else if (EnemyType == 2) {
		HP = 130;
		MoveSpeed = 120.f;
		AttackPower = 5;
		Texture.load("Res/Enemy2.png");
		collisionRadius = 70.f;
		attackProgress = 0.f;
		attackCooldown = 2.f;
		EXP = 40;
		score = 40;
	}
	else if (EnemyType == 3) {
		HP = 90;
		MoveSpeed = 200.f;
		AttackPower = 5;
		Texture.load("Res/Enemy3.png");
		collisionRadius = 50.f;
		attackProgress = 0.5f;
		attackCooldown = 0.5f;
		EXP = 30;
		score = 30;
	}
	else if (EnemyType == 4) {
		HP = 240;
		MoveSpeed = 100.f;
		AttackPower = 10;
		Texture.load("Res/Enemy4.png");
		collisionRadius = 75.f;
		attackProgress = 2.f;
		attackCooldown = 2.f;
		EXP = 50;
		score = 50;
	}
}


bool Enemy::isInView(GamesEngineeringBase::Window& canvas, Player& player)
{
	if (Position.x + Texture.width<player.CameraPosition.x || player.CameraPosition.x+canvas.getWidth()<Position.x ||
		Position.y + Texture.height<player.CameraPosition.y || player.CameraPosition.y+canvas.getHeight()<Position.y) return false;
	return true;
}

void Enemy::draw(GamesEngineeringBase::Window& canvas, Player& player)
{
	//because the enemy image is large, we need to check whether it is in the view instead of checking every pixel
	if (isInView(canvas, player)) {
		for (unsigned int x = 0; x < Texture.width; x++) {
			for (unsigned int y = 0; y < Texture.height; y++) {
				if (Position.x + x >= player.CameraPosition.x && Position.x + x <= player.CameraPosition.x + canvas.getWidth()-1 &&
					Position.y + y >= player.CameraPosition.y && Position.y + y <= player.CameraPosition.y + canvas.getHeight()-1) {
					if (Texture.hasAlpha()) {
						canvas.drawPNG(static_cast<int>(std::round(x + Position.x - player.CameraPosition.x)),
							           static_cast<int>(std::round(y + Position.y - player.CameraPosition.y)),
							           Texture.at(x, y));
					}
					else {
                        canvas.draw(static_cast<int>(std::round(x + Position.x - player.CameraPosition.x)), 
                                    static_cast<int>(std::round(y + Position.y - player.CameraPosition.y)),
                                    Texture.at(x, y));
					}
				}
			}
		}
	}

}


unsigned int EnemyPool::getEnemyType() const
{
	int random = rand() % EnemySpawnWeightSum;
	if (random < EnemySpawnWeight[1]) return 1;
	else if (random < EnemySpawnWeight[1] + EnemySpawnWeight[2]) return 2;
	else if (random < EnemySpawnWeight[1] + EnemySpawnWeight[2] + EnemySpawnWeight[3]) return 3;
	else return 4;
	return 0;
}

void EnemyPool::spawnEnemies(GamesEngineeringBase::Window& canvas, float _dt, float currentTime, Player& player)
{
	SpawnProgress += _dt;
	if (SpawnProgress >= SpawnTimeThreshold_Current) {
		//calculate the number of enemies to spawn
		int spawnCount = static_cast<int>(SpawnProgress / SpawnTimeThreshold_Current);
		for (int i = 1; i <= spawnCount; i++) {
			spawnOneEnemy(canvas, player);
		}
		//updata the spawn progress
		SpawnProgress -= spawnCount * SpawnTimeThreshold_Current;
		SpawnTimeThreshold_Current = SpawnTimeThreshold_Start - (SpawnTimeThreshold_Start - SpawnTimeThreshold_End) * min(1.f, currentTime / targetTime);
		//std::cout << SpawnTimeThreshold_Current << std::endl;
	}
}

void EnemyPool::spawnOneEnemy(GamesEngineeringBase::Window& canvas, Player& player)
{

	//get an inactivated enemy from the pool
	Enemy* newEnemy = InactivatedEnemies.acquire();

	//Every pixel of the canvas has the same probability to spawn an enemy
	unsigned int spawnSide = rand() % (2 * (canvas.getWidth() + canvas.getHeight()));
	if (spawnSide < canvas.getWidth()) {
		//spawn from the top side
		newEnemy->Initialize(getEnemyType(), player.CameraPosition.x + spawnSide, player.CameraPosition.y - 200);
		//std::cout << "top" << std::endl;
	}
	else if (spawnSide < 2 * canvas.getWidth()) {
		//spawn from the bottom side
		newEnemy->Initialize(getEnemyType(), player.CameraPosition.x + spawnSide - canvas.getWidth(), player.CameraPosition.y+canvas.getHeight() + 200);
		//std::cout << "bottom" << std::endl;
	}
	else if (spawnSide < 2 * canvas.getWidth() + canvas.getHeight()) {
		//spawn from the left side
		newEnemy->Initialize(getEnemyType(), player.CameraPosition.x - 200, player.CameraPosition.y + spawnSide - 2 * canvas.getWidth());
		//std::cout << "left" << std::endl;
	}
	else {
		//spawn from the right side
		newEnemy->Initialize(getEnemyType(), player.CameraPosition.x+canvas.getWidth() + 200, player.CameraPosition.y + spawnSide - 2 * canvas.getWidth() - canvas.getHeight());
		//std::cout << "right" << std::endl;
	}
	ActivatedEnemies.push_back(newEnemy);
	
}

void EnemyPool::EnemyMovementAndCheckCollision(Player& player, float _dt, BulletPool& bulletPool)
{
	if (ActivatedEnemies.size == 0) return;

	//used to move and attack
	Vec2 Direction = { 0,0 };
	//used to find the nearest enemy
	nearestEnemy = nullptr;
	float minDistance2 = 9000000000.f;
	float currentDistance2 = 0.f;

	for (size_t i = 0; i < ActivatedEnemies.size; i++) {
		//update the attack progress
		ActivatedEnemies.Data[i]->updateAttackProgress(_dt);
		
		//type 2 enemies do not move but check whether to attack player remotely
		if (ActivatedEnemies.Data[i]->EnemyType == 2) {
			//calculate the distance between the enemy and the player
			Direction = player.CenterPosition - ActivatedEnemies.Data[i]->CenterPosition;
			currentDistance2 = Direction.length2();
			Direction.normalize();
			//if the type 2 enemy is close enough, attack player remotely
			if (currentDistance2 < 200000) {
				if (ActivatedEnemies.Data[i]->canAttack(true)) {
					bulletPool.spawnBullet(ActivatedEnemies.Data[i]->CenterPosition, Direction, ActivatedEnemies.Data[i]->AttackPower, false);
				}
			}//if the type 2 enemy is not close enough, he will move towards the player
			else {
				ActivatedEnemies.Data[i]->EnemyMovementAndUpdateCenter(Direction, _dt);
			}
			//check whether it is the nearest enemy
            currentDistance2 = currentDistance2 - static_cast<float>(std::pow(ActivatedEnemies.Data[i]->MoveSpeed * _dt, 2));
			if (currentDistance2 < minDistance2) {
				minDistance2 = currentDistance2;
				nearestEnemy = ActivatedEnemies.Data[i];
			}
		}
		//As for other enemy types, they move and check collisions to attack player in melee
		else {
			//move the enemy
			Direction = player.CenterPosition - ActivatedEnemies.Data[i]->CenterPosition;
			Direction.normalize();
			ActivatedEnemies.Data[i]->EnemyMovementAndUpdateCenter(Direction, _dt);
			//after moving, check whether it is the nearest enemy
			currentDistance2 = (player.CenterPosition - ActivatedEnemies.Data[i]->CenterPosition).length2();
			if (currentDistance2 < minDistance2) {
				minDistance2 = currentDistance2;
				nearestEnemy = ActivatedEnemies.Data[i];
			}
			//check collision with player to attack player in melee
			if (currentDistance2 < std::pow((player.collisionRadius + ActivatedEnemies.Data[i]->collisionRadius), 2)) {
				if (ActivatedEnemies.Data[i]->canAttack(true)) {
					player.getDamage(ActivatedEnemies.Data[i]->AttackPower);
				}
			}
		}
		//All of enemies need to check whether they will get damage from player's bullets
		for (size_t j = 0; j < bulletPool.PlayerBullets.size; j++) {
			if ((ActivatedEnemies.Data[i]->CenterPosition - bulletPool.PlayerBullets.Data[j]->CenterPosition).length2() < std::pow(ActivatedEnemies.Data[i]->collisionRadius + bulletPool.PlayerBullets.Data[j]->Radius, 2)) {
				if (ActivatedEnemies.Data[i]->getDamage(bulletPool.PlayerBullets.Data[j]->AttackPower)) {
					//if the enemy is dead, release the enemy and the bullet and update the player's EXP and score
					player.getEXPAndScore(ActivatedEnemies.Data[i]->EXP, ActivatedEnemies.Data[i]->score);
					releaseEnemy(ActivatedEnemies.Data[i]);
					i--;
					bulletPool.releaseBullet(true, bulletPool.PlayerBullets.Data[j]);
					break;
				}
				else {
					//if the enemy is not dead, release the bullet
					bulletPool.releaseBullet(true, bulletPool.PlayerBullets.Data[j]);
					j--;
				}
			}
		}
	}
}


void EnemyPool::releaseEnemy(Enemy* enemy)
{
	size_t index = ActivatedEnemies.find(enemy);
	ActivatedEnemies.erase(index);
	InactivatedEnemies.push_back(enemy);
}

void EnemyPool::draw(GamesEngineeringBase::Window& canvas, Player& player)
{
	for (size_t i = 0; i < ActivatedEnemies.size; i++) {
		ActivatedEnemies.Data[i]->draw(canvas, player);
	}
}

void EnemyPool::AOE(Player& player, GamesEngineeringBase::Window& canvas)
{
	
	//check whether the player has enough enemies to attack
	if (ActivatedEnemies.size > player.AOECount) {
		//std::cout << "AOE1"<<ActivatedEnemies.size <<" " << player.AOECount << std::endl;
		//attack the enemies in the AOE range
		findNLargest(player.AOECount);
		size_t index = 0;
		for (size_t i = 0; i < MinHeap.size; i++) {
			index=ActivatedEnemies.find(MinHeap[i]);
			if (ActivatedEnemies[index]->getDamage(player.AttackPower*2)) {
				//if the enemy is dead, release the enemy and update the player's EXP and score
				player.getEXPAndScore(ActivatedEnemies[index]->EXP, ActivatedEnemies[index]->score);
				releaseEnemy(ActivatedEnemies[index]);
			}
		}
		MinHeap.size=0;
	}
	else {
		//std::cout << "AOE2" << std::endl;
		for (size_t i = 0; i < ActivatedEnemies.size; i++) {
			//std::cout << ActivatedEnemies[i]->HP << std::endl;
			if (ActivatedEnemies.Data[i]->getDamage(player.AttackPower*2)) {
				//if the enemy is dead, release the enemy and update the player's EXP and score
				player.getEXPAndScore(ActivatedEnemies.Data[i]->EXP, ActivatedEnemies.Data[i]->score);
				releaseEnemy(ActivatedEnemies.Data[i]);
				i--;
			}
		}
	}
}

void EnemyPool::heapify(size_t i, size_t size)
{
	size_t smallest = i;
	size_t left = 2 * i + 1;
	size_t right = 2 * i + 2;
	if (left < size) {
		if (*MinHeap[left] < *MinHeap[smallest]) smallest = left;
	}
	if (right < size) {
		if (*MinHeap[right] < *MinHeap[smallest]) smallest = right;
	}
	if (smallest != i) {
		Enemy* temp = MinHeap[i];
		MinHeap[i] = MinHeap[smallest];
		MinHeap[smallest] = temp;
		heapify(smallest, size);
	}
}

void EnemyPool::findNLargest(size_t N)
{
	MinHeap.size=0;
	size_t size = min(ActivatedEnemies.size, N);

	for (size_t i = 0; i < size; i++) {
		MinHeap.push_back(ActivatedEnemies.Data[i]);
	}
	for (size_t i = (size / 2) - 1; i !=SIZE_MAX; i--) {
		heapify(i, size);
	}
	for (size_t i = N; i < ActivatedEnemies.size; i++) {
		if (*MinHeap.Data[0] < *ActivatedEnemies.Data[i]) {
			MinHeap.Data[0] = ActivatedEnemies.Data[i];
			heapify(0, size);
		}
	}
	
}


void Bullet::Initialize(Vec2& Position, Vec2& Direction, int _AttackPower, bool _isPlayer)
{
	CenterPosition = Position;
	MoveDirection = Direction;
	AttackPower = _AttackPower;
	isPlayer = _isPlayer;
	lifeProgress = 0.f;
	if (isPlayer) {
		MoveSpeed = 300.f;
		Radius = 8.f;
		Texture.load("Res/PlayerBullet.png");
	}
	else {
		MoveSpeed = 150.f;
		Radius = 6.f;
		Texture.load("Res/EnemyBullet.png");
	}
}

bool Bullet::BulletMoveAndUpdateLife(float _dt, BulletPool& bulletPool)
{
	lifeProgress += _dt;
	if (lifeProgress >= lifeTime) {
		//destroy the bullet
		bulletPool.releaseBullet(isPlayer, this);
		return true;
	}
	CenterPosition = CenterPosition + MoveDirection * MoveSpeed * _dt;
	return false;
}

void Bullet::draw(GamesEngineeringBase::Window& canvas, Player& player)
{
	//because the bullet image is small, we don't need to check whether it is in the view. Just directly check every pixel.
    Vec2 Position = CenterPosition - Vec2(static_cast<float>(Texture.width) / 2, static_cast<float>(Texture.height) / 2);
	for (unsigned int x = 0; x < Texture.width; x++) {
		for (unsigned int y = 0; y < Texture.height; y++) {
			if (Position.x + x >= player.CameraPosition.x && Position.x + x <= player.CameraPosition.x + canvas.getWidth()-1 &&
				Position.y + y >= player.CameraPosition.y && Position.y + y <= player.CameraPosition.y + canvas.getHeight()-1) {
				if (Texture.hasAlpha()) {
					canvas.drawPNG(static_cast<int>(std::round(x + Position.x - player.CameraPosition.x)), 
						           static_cast<int>(std::round(y + Position.y - player.CameraPosition.y)), 
						           Texture.at(x, y));
				}
				else {
					canvas.draw(static_cast<int>(std::round(x + Position.x - player.CameraPosition.x)),
						        static_cast<int>(std::round(y + Position.y - player.CameraPosition.y)),
						        Texture.at(x, y));
				}
			}
		}
	}
}


void BulletPool::spawnBullet(Vec2& Position, Vec2& Direction, int _AttackPower,bool _isPlayer)
{
	Bullet* newBullet = InactivatedBullets.acquire();
	newBullet->Initialize(Position, Direction,_AttackPower, _isPlayer);
	if (_isPlayer) PlayerBullets.push_back(newBullet);
	else EnemyBullets.push_back(newBullet);
}



void BulletPool::BulletMoveAndUpdateLife(float _dt)
{
	//move and update the life of the bullets
	for (size_t i = 0; i < EnemyBullets.size; i++) {
		if (EnemyBullets.Data[i]->BulletMoveAndUpdateLife(_dt, *this)) i--;
	}
	for (size_t i = 0; i < PlayerBullets.size; i++) {
		if (PlayerBullets.Data[i]->BulletMoveAndUpdateLife(_dt, *this)) i--;
	}
}

void BulletPool::releaseBulletWithIndex(bool isPlayer, size_t index)
{
	if (isPlayer) {
		InactivatedBullets.push_back(PlayerBullets.erase(index));
	}
	else {
		InactivatedBullets.push_back(EnemyBullets.erase(index));
	}
}

void BulletPool::releaseBullet(bool isPlayer, Bullet* bullet)
{
	if (isPlayer) {
		//std::cout << bullet->lifeProgress << std::endl;
		size_t index=PlayerBullets.find(bullet);
		InactivatedBullets.push_back(PlayerBullets.erase(index));
	}
	else {
		size_t index = EnemyBullets.find(bullet);
		InactivatedBullets.push_back(EnemyBullets.erase(index));
	}
}

void BulletPool::draw(GamesEngineeringBase::Window& canvas, Player& player)
{
	for (size_t i = 0; i < EnemyBullets.size; i++) {
		EnemyBullets.Data[i]->draw(canvas, player);
	}
	for (size_t i = 0; i < PlayerBullets.size; i++) {
		PlayerBullets.Data[i]->draw(canvas, player);
	}
}

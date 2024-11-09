#pragma once
#include "GamesEngineeringBase.h"
#include "DataStructure.h"

class BulletPool;
class EnemyPool;
class Character {
public:
	GamesEngineeringBase::Image Texture;
	//Position using [float], when showing the character on the screen, we need to convert it to [int]
	Vec2 Position = { 0,0 };
	Vec2 CenterPosition = { 0,0 };
	
	int EXP = 0;
	int score = 0;

	int HP=100;
	float MoveSpeed=10.f;
	int AttackPower=10;
	float collisionRadius=5.f;
	

	//Both ranged and melee attack, if attackProgress is larger than attackCooldown, the character can attack.Then attackProgress = 0
	float attackProgress = 1.f;
	float attackCooldown = 1.f;

	Character() :Texture() {}
	//update cooldown
	void updateAttackProgress(float _dt);

	//return true if the character can attack, if ifReset is true, reset the attackProgress=0, otherse attackProgress -= attackCooldown
	//As for player,ifReset=false, because the player attack speed is quick. When frame rate fluctuation, the player may miss some attacks
	//As for enemy,ifReset=true, because the enemy attack speed is slow. Frame rate fluctuation won't cause many negative effects
	bool canAttack(bool ifReset);
	
	bool getDamage(int damage);
};

class Player : public Character
{
public:
	Vec2 CameraPosition = { 0,0 };
	
	int Level = 1;
	int AOECount = 2;
	float AOECoolDown = 10.f;
	float AOEProgress = 10.f;

	Player(std::string filename, float PositionX, float PositionY, int _HP, float _MoveSpeed, int _AttackPower, float _collisionRadius, float _attackThreshold, int _AOECount);
	
	void updateAttackProgress(float _dt);
	//return true if the player can use AOE
	bool canAOE();

	void getEXPAndScore(int _EXP,int _score);

	//Move the player according to the MoveDirection
	void PlayerMovement(Vec2& MoveDirection, float dt);
	
	//check collision
	void checkBulletCollision(BulletPool& bulletPool);

	//update the camera position and player center position
	void updateCameraAndCenter(GamesEngineeringBase::Window& canvas);

	//draw the player on the screen
	void draw(GamesEngineeringBase::Window& canvas);

	//spawn bullets towards the nearest enemy
	void spawnBullet(EnemyPool& enemyPool,BulletPool& bulletPool, float _dt);

};

class Enemy :public Character {
public:
	unsigned int EnemyType=1;

	//Move enemies and update the center position
	void EnemyMovementAndUpdateCenter(Vec2& MoveDirection, float dt);
	
	// After getting the enemy from the pool, it needs to be initialized
	void Initialize(unsigned int _EnemyType,float PositionX,float PositionY);
	
	//Overload the < operator to compare the HP of the enemies
	bool operator<(const Enemy& other) {
		return HP < other.HP;
	}
	bool operator<(const Enemy* other) {
		return HP < other->HP;
	}

	//check if the enemy is in the view of the camera
	bool isInView(GamesEngineeringBase::Window& canvas, Player& player);

	void draw(GamesEngineeringBase::Window& canvas, Player&player);

};

//Use a pool to store activated and inactivated enemies to prevent frequent memory allocation and deallocation
class EnemyPool {
public:
	myVector<Enemy> InactivatedEnemies;
	myVector<Enemy> ActivatedEnemies;
	Enemy* nearestEnemy = nullptr;
	myVector<Enemy> MinHeap;

	// these variables are used to calculate the number of enemies to spawn
	float SpawnProgress = 0.f;
	float SpawnTimeThreshold_Current = 1.f;
	float SpawnTimeThreshold_Start = 1.f;
	float SpawnTimeThreshold_End = 0.1f;
	float targetTime = 0.f;
	//these variables are used to calculate the type of enemies to spawn
	int EnemySpawnWeight[5] = { 0 };
	int EnemySpawnWeightSum = 0;

	//When constructing the EmemyPool, configure enemy spawn parameters
	EnemyPool(float _targetTime, float _thresholdStart, float _thresholdEnd, int weight1,int weight2,int weight3,int weight4) :
		targetTime(_targetTime),
		SpawnTimeThreshold_Current(_thresholdStart),
		SpawnTimeThreshold_Start(_thresholdStart),
		SpawnTimeThreshold_End(_thresholdEnd), 
		SpawnProgress(_thresholdStart),
		MinHeap(16){

		EnemySpawnWeight[1] = weight1;
		EnemySpawnWeight[2] = weight2;
		EnemySpawnWeight[3] = weight3;
		EnemySpawnWeight[4] = weight4;
		EnemySpawnWeightSum = weight1 + weight2 + weight3 + weight4;
		
		//fill the InactivatedEnemies with default enemies
		InactivatedEnemies.Initialize();
	}

	//get the enemy type by cauculating the weight
	unsigned int getEnemyType() const;

	//Spawn enemies
	void spawnEnemies(GamesEngineeringBase::Window& canvas, float _dt, float currentTime, Player& player);
	void spawnOneEnemy(GamesEngineeringBase::Window& canvas,Player&player);

	//1.Enemy move 2.Attack player (melee + ranged) 3. find the nearest enemy 4. check collisions with player's bullets
	void EnemyMovementAndCheckCollision(Player&player,float _dt,BulletPool & bulletPool);

	//reset an activated enemy and move it back to the InactivatedEnemies
	void releaseEnemy(Enemy* enemy);

	void draw(GamesEngineeringBase::Window& canvas, Player& player);

	void AOE(Player& player, GamesEngineeringBase::Window& canvas);
	
	//find enemies with largest HP
	void heapify(size_t i, size_t size); 
	void findNLargest(size_t N);

};

class Bullet {
public:
	Vec2 CenterPosition = { 0,0 };
	Vec2 MoveDirection = { 0,0 };
	float MoveSpeed = 10.f;
	float Radius = 5.f;
	int AttackPower = 10;
	//whenlifeProgress is larger than lifeTime, the bullet will be destroyed
	float lifeTime = 10.f;
	float lifeProgress = 0.f;
	GamesEngineeringBase::Image Texture;
	bool isPlayer = false;

	Bullet() :Texture() {}

	//fetch the bullet from the InactivatedBullets and initialize it
	void Initialize(Vec2& Position, Vec2& Direction,int _AttackPower, bool _isPlayer);
	//move the bullet and update the lifeProgress,return true if the bullet is destroyed
	bool BulletMoveAndUpdateLife(float _dt,BulletPool&bulletPool);

	void draw(GamesEngineeringBase::Window& canvas, Player& player);
};

//use a pool to store activated and inactivated bullets to prevent frequent memory allocation and deallocation
class BulletPool {
public:
	myVector<Bullet> EnemyBullets;
	myVector<Bullet> PlayerBullets;
	myVector<Bullet> InactivatedBullets;

	BulletPool() :EnemyBullets(32), PlayerBullets(16), InactivatedBullets(64) {
		//fill the InactivatedBullets with default bullets
		InactivatedBullets.Initialize();
	}

	void spawnBullet(Vec2& Position, Vec2& Direction,int _AttackPower,bool _isPlayer);
	
	//Bullets move and update their lives
	void BulletMoveAndUpdateLife(float _dt);
	
	//reset an activated bullet and move it back to the InactivatedBullets
	void releaseBulletWithIndex(bool isPlayer, size_t index);
	void releaseBullet(bool isPlayer, Bullet * bullet);

	void draw(GamesEngineeringBase::Window& canvas,Player&player);
};
#include"GamesEngineeringBase.h"
#include"Character.h"
#include"Map.h"
#include<cmath>
#include<iostream>

constexpr int ScreenWidth = 1280;
constexpr int ScreenHeight = 720;

int main() {
	//create a window
    GamesEngineeringBase::Window canvas;
    canvas.create(ScreenWidth,ScreenHeight, "PFAGame");
    
	//initial random seed
	srand(static_cast<unsigned int>(time(0)));

	//Initial Map
	bool endless = true;
	World world("Res/tiles.txt", endless);

	//Initial Player
    //constructor: Player(std::string filename, float PositionX, float PositionY, int _HP, float _MoveSpeed, int _AttackPower, float _collisionRadius, float _attackThreshold, int _AOECount);
	Player player("Res/cat.png",800.f,800.f,100, 200.f, 100,28.f,1.f,2);
	player.updateCameraAndCenter(canvas);
    Vec2 PlayerDirection;

    //Initial Timer
    GamesEngineeringBase::Timer timer;
    int FPS = 0;
	float dt = 0.f;

	//Initial EnemyPool and BulletPool
    //constructor : EnemyPool(float _targetTime, float _thresholdStart, float _thresholdEnd, int weight1,int weight2,int weight3,int weight4)
    EnemyPool enemyPool(90.f, 2.0f, 0.5f,10,2,4,1);
    BulletPool bulletPool;

    //program loop
    bool running = true;
    while (running)
    {
        canvas.clear();
        canvas.checkInput();
        if (canvas.keyPressed(VK_ESCAPE)) break;

        //timer and FPS
        dt = timer.dt();
        FPS = static_cast<int>(std::round(1.f / dt));
        //--------check if the player uses AOE----------------
        if (canvas.keyPressed(VK_SPACE)) {
            if (player.canAOE()) {
                enemyPool.AOE(player, canvas);
            }
        }
        //-------All of the bullets move WITHOUT collision detection----------------
        bulletPool.BulletMoveAndUpdateLife(dt);

        //-------1.Player move 2.check collision with the tilemap and bullets----------
        PlayerDirection.clear();
        //Use the direction to get correct result for the player's diagonal movement  
        //For example, if speed is 1, diagonal movement distance will be 1*dt, instead of sqrt(2)*dt    
        //(1,0) is right, (-1,0) is left, (0,1) is down, (0,-1) is up
        if (canvas.keyPressed('W')) PlayerDirection.y -= 1;
        if (canvas.keyPressed('S')) PlayerDirection.y += 1;
        if (canvas.keyPressed('A')) PlayerDirection.x -= 1;
        if (canvas.keyPressed('D')) PlayerDirection.x += 1;
        //Because of the accuracy of float, we need to set a threshold to check if the player is moving
        if (abs(PlayerDirection.x) > 0.000001 || abs(PlayerDirection.y) > 0.000001) {
            PlayerDirection.normalize();
            player.PlayerMovement(PlayerDirection, dt);
            player.updateCameraAndCenter(canvas);
        }
        //After the player moves, check if the player collides with the map and bullets
        world.checkBoundary(canvas, player);
        player.updateCameraAndCenter(canvas);
        world.checkCollision(canvas, player, dt, PlayerDirection);
        player.checkBulletCollision(bulletPool);


        //-----1.Enemy move 2.Attack player (melee + ranged) 3. find the nearest enemy 4. check collisions with player's bullets--------
        enemyPool.EnemyMovementAndCheckCollision(player, dt, bulletPool);


        //------spawn player's bullets--------------
        player.spawnBullet(enemyPool, bulletPool, dt);

        //------spawn new enemies-------------------
        enemyPool.spawnEnemies(canvas, dt, timer.getCurrentTime(), player);

		//draw map
		world.draw(canvas, player);
		//draw player
		player.draw(canvas);
		//draw enemies
		enemyPool.draw(canvas,player);
		//draw bullets
		bulletPool.draw(canvas,player);

        canvas.present();
    }
    return 0;
}
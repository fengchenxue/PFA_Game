#include"GamesEngineeringBase.h"
#include"Character.h"
#include"Map.h"
#include<cmath>
#include<iostream>

constexpr int ScreenWidth = 1280;
constexpr int ScreenHeight = 720;

//Because atexit() can only assignedwith a function with no parameters, we need to use global variables to store data
//Because the player and world have image, we need to initialize them after the window is created
//But we can initialize enemyPool and bulletPool before the window is created
Player player;
EnemyPool enemyPool(90.f, 2.0f, 0.5f, 10, 2, 4, 1);
BulletPool bulletPool;
World world;
void saveGame() {
	player.save("Res/Player.dat");
	enemyPool.save("Res/Enemy.dat");
	bulletPool.save("Res/Bullet.dat");
	world.save("Res/World.dat");
}

int main() {
    //create a window
    GamesEngineeringBase::Window canvas;
    canvas.create(ScreenWidth, ScreenHeight, "PFAGame");

	//save game when exit
	atexit(saveGame);
    //select map mode
    int mode;
    bool endless;
    while (true) {
		system("cls");
        std::cout << "input a number:1. for normal mode 2. for endless mode 3. load game\n";
        std::cin >> mode;
        switch (mode)
        {
        case 1:
            endless = false;
			//Because the player's and tiles' images can only be loaded after the window is created
			//We initialize the player and world at this moment after the window is created
            player.initialize(800.f, 800.f, 100, 200.f, 100, 28.f, 1.f, 2);
			world.initialize(endless);
            break;
        case 2:
            endless = true;
            player.initialize(800.f, 800.f, 100, 200.f, 100, 28.f, 1.f, 2);
            world.initialize(endless);
            break;
        case 3:
			player.load("Res/Player.dat");
			enemyPool.load("Res/Enemy.dat");
			bulletPool.load("Res/Bullet.dat");
			world.load("Res/World.dat");
            break;
        default:
            break;
        }
        if (mode>=1&&mode<=3) break;
    }

	//initial random seed
	srand(static_cast<unsigned int>(time(0)));
	
	//update the camera and center of the player
	player.updateCameraAndCenter(canvas);
    Vec2 PlayerDirection;

    //Initial Timer
    GamesEngineeringBase::Timer timer;
    int FPS = 0;
	float dt = 0.f;

    //program loop
    bool running = true;
    while (running)
    {
        canvas.clear();
        canvas.checkInput();
        if (canvas.keyPressed(VK_ESCAPE)) break;
		if (player.HP <= 0) break;

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

		//show FPS and other information
		system("cls");
        std::cout << "FPS:" << FPS << std::endl;
		std::cout << "Score:" << player.score << std::endl;
		std::cout << "Enemies:" << enemyPool.ActivatedEnemies.size << std::endl;
        std::cout << "HP:" << player.HP <<std::endl;
        std::cout << "Level:" << player.Level << std::endl; 
        std::cout << "Attack Interval:" << player.attackCooldown << std::endl;
        std::cout<< "AOECount:" << player.AOECount << std::endl<<std::endl;
		if (player.AOEProgress >= player.AOECoolDown) std::cout << "AOE Ready" << std::endl;
		else std::cout << "AOE not ready"<< std::endl;

        canvas.present();
    }
    return 0;
}
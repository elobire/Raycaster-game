#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <stdlib.h>
#include <math.h>

#include <iostream>
#include <sstream>

#define mapWidth 24
#define mapHeight 24
#define screenWidth 640
#define screenHeight 480
#define texWidth 512
#define texHeight 512
#define const int texture_wall_size = 128;
double ZBuffer[1920];

const float cameraHeight = 0.66f; // height of player camera(1.0 is ceiling, 0.0 is floor)

sf::Color floorC = sf::Color::Cyan;

std::string ConvertToString(int number){
    std::ostringstream buff;
    buff<<number;
    return buff.str();
}



int worldMap[mapWidth][mapHeight]=
{
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


// define world sprites
struct Sprite
{
	double x;
	double y;
	int texture;
};

#define numSprites 2

Sprite sprite[numSprites] =
{
	{ 8, 7, 10 },
	{20, 4.5, 10}
};

int spriteOrder[numSprites];
double spriteDistance[numSprites];

void combSort(int* order, double* dist, int amount)
{
	int gap = amount;
	bool swapped = false;
	while (gap > 1 || swapped)
	{
		//shrink factor 1.3
		gap = (gap * 10) / 13;
		if (gap == 9 || gap == 10) gap = 11;
		if (gap < 1) gap = 1;
		swapped = false;
		for (int i = 0; i < amount - gap; i++)
		{
			int j = i + gap;
			if (dist[i] < dist[j])
			{
				std::swap(dist[i], dist[j]);
				std::swap(order[i], order[j]);
				swapped = true;
			}
		}
	}
}




int main()
{

	sf::Color color;
	color = sf::Color::White;
	
	sf::Texture floorTex;

	if (!floorTex.loadFromFile("stone.gif"))
	{
		std::cout << "Cannot load floor texture" << std::endl;
	}

	sf::Texture orangeWallTex;

	if (!orangeWallTex.loadFromFile("orangeWall.jpg"))
	{
		std::cout << "Cannot load orange wall texture" << std::endl;
	}

	sf::Texture lichenBrickTex;

	if (!lichenBrickTex.loadFromFile("lichenbrick.jpg"))
	{
		std::cout << "Cannot load lichen brick texture" << std::endl;
	}

	sf::Texture drawnTex;

	if (!drawnTex.loadFromFile("drawn2.jpg"))
	{
		std::cout << "Cannot load das brick texture" << std::endl;
	}

	sf::Texture rockyWallTex;

	if (!rockyWallTex.loadFromFile("rockyWall.jpg"))
	{
		std::cout << "Cannot rocky wall texture" << std::endl;
	}

	sf::Texture spriteTex;

	if (!spriteTex.loadFromFile("troll.png"))
	{
		std::cout << "Cannot load sprite texture" << std::endl;
	}

	sf::Texture greenBrickTex;

	if (!greenBrickTex.loadFromFile("greenBrick.jpg"))
	{
		std::cout << "Cannot load green brick texture" << std::endl;
	}
	//texture.setSmooth(true);
	sf::Texture hitlerTexture;

	if (!hitlerTexture.loadFromFile("hitler.gif"))
	{
		std::cout << "Cannot load hitler texture" << std::endl;
	}
   
	sf::Texture wallTex;

	if (!wallTex.loadFromFile("wall2.jpg"))
	{
		std::cout << "Cannot load  wall texture" << std::endl;
	}

	sf::Texture brickTex;

	if (!brickTex.loadFromFile("brick.gif"))
	{
		std::cout << "Cannot load brick texture" << std::endl;
	}

	sf::Texture textures[4];
	textures[0] = lichenBrickTex;
	textures[1] = orangeWallTex;
	textures[2] = wallTex;
	textures[3] = drawnTex;
    int w = 1920, h = 1080;
   
	//floor colors
	sf::Color color1(128, 128, 128);
	sf::Color color2(64, 64, 64);

    double posX = 22, posY = 12;  //x and y start position
    double dirX = -1, dirY = 0; //initial direction vector
    double planeX = 0, planeY = 0.66; //the 2d raycaster version of camera plane
   
    double time = 0; //time of current frame
    double oldTime = 0; //time of previous frame
   
    // Create the main window
	sf::RenderWindow window(sf::VideoMode(w, h), "SFML window", sf::Style::Fullscreen);
   
    window.setFramerateLimit(60);
    sf::Clock clock = sf::Clock();
    sf::Time fps;
   
    // Start the game loop
	while (window.isOpen()) {
		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close window: exit
			if (event.type == sf::Event::Closed)
				window.close();

			// Escape key: exit
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
				window.close();
		}

		window.clear();
		sf::VertexArray floorLines(sf::Lines, 18 * screenWidth);

		for (int x = 0; x < w; x++)
		{
			//calculate ray position and direction
			double cameraX = 2 * x / double(w) - 1; //x-coordinate in camera space
			double rayPosX = posX;
			double rayPosY = posY;
			double rayDirX = dirX + planeX * cameraX;
			double rayDirY = dirY + planeY * cameraX;

			//which box of the map we're in
			int mapX = int(rayPosX);
			int mapY = int(rayPosY);

			//length of ray from current position to next x or y-side
			double sideDistX;
			double sideDistY;

			//length of ray from one x or y-side to next x or y-side
			double deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
			double deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
			double perpWallDist;

			//what direction to step in x or y-direction (either +1 or -1)
			int stepX;
			int stepY;
			bool nextIsWall = false;

			int hit = 0; //was there a wall hit?
			int side; //was a NS or a EW wall hit?

			//calculate step and initial sideDist
			if (rayDirX < 0)
			{
				stepX = -1;
				sideDistX = (rayPosX - mapX) * deltaDistX;
			}
			else
			{
				stepX = 1;
				sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
			}
			if (rayDirY < 0)
			{
				stepY = -1;
				sideDistY = (rayPosY - mapY) * deltaDistY;
			}
			else
			{
				stepY = 1;
				sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
			}

			//float perpWallDist = 0.0f; // wall distance, projected on camera direction
			bool horizontal; // was a horizontal side hit?
			int groundPixel = screenHeight;
			int wallHeight = 0; // height of wall to draw on the screen at each distance
			int nWallHeight;
			double nPerpWalldist;
			int prevGround = 0;
			int prevCeiling = 0;

			bool first = true;

			//perform DDA
			while (hit == 0)
			{

				sf::Color floorC = ((mapX % 2 == 0 && mapY % 2 == 0) ||
					(mapX % 2 == 1 && mapY % 2 == 1)) ? color1 : color2;
				first = false;

				//jump to next map square, OR in x-direction, OR in y-direction
				if (sideDistX < sideDistY)
				{
					sideDistX += deltaDistX;
					mapX += stepX;
					side = 0;
					horizontal = true;
					perpWallDist = (mapX - rayPosX + (1 - stepX) / 2) / rayDirX;
				}
				else
				{
					sideDistY += deltaDistY;
					mapY += stepY;
					side = 1;
					horizontal = false;
					perpWallDist = (mapY - rayPosY + (1 - stepY) / 2) / rayDirY;
				}

				if (worldMap[mapX][mapY] > 0)
				{
					nextIsWall = true;
				}
				else {
					nextIsWall = false;
				}


				//	wallHeight = screenHeight / perpWallDist;


				int lineH = abs(int(h / perpWallDist));
				groundPixel = int(lineH * 0.5f + h * 0.5f);

				double floorHit;
				if (side == 0)
				{
					floorHit = rayPosY + perpWallDist * rayDirY;
				}
				else {
					floorHit = rayPosY + perpWallDist * rayDirX;
				}
				floorHit -= floor((floorHit));

				floorHit = floorHit * texWidth;

				//if (!nextIsWall)
				if (!first)
				{
					sf::Vertex floorL[] =
					{
						sf::Vertex(sf::Vector2f(x, groundPixel), floorC),
						sf::Vertex(sf::Vector2f(x, prevGround), floorC)
					};
					//	floorL[0].texCoords = sf::Vector2f(floorHit, 0);
					//floorL[1].texCoords = sf::Vector2f(floorHit, 64);
					//window.draw(floorL, 2, sf::Lines);
				}




				wallHeight = abs(int(h / perpWallDist));
				//int ceilingPix = int(-wallHeight *(1.0f - cameraHeight) + screenHeight * 0.5f);
				int ceilingPix = int(-wallHeight * (1.0f - 0.5f) + h * 0.5f);
				if (!first)
				{
					sf::Vertex ceilingL[] =
					{
						sf::Vertex(sf::Vector2f(x, ceilingPix), floorC),
						sf::Vertex(sf::Vector2f(x, prevGround), floorC)
					};
					//ceilingL[0].texCoords = sf::Vector2f(floorHit, 0);
					//ceilingL[1].texCoords = sf::Vector2f(floorHit, 64);
					//window.draw(ceilingL, 2, sf::Lines);
				}



				floorC = (floorC == color1) ? color2 : color1;

				//Check if ray has hit a wall
				prevGround = groundPixel;
				if (worldMap[mapX][mapY] > 0) hit = 1;
			}



			//Calculate distance projected on camera direction (oblique distance will give fisheye effect!)
			if (side == 0)
				perpWallDist = fabs((mapX - rayPosX + (1 - stepX) / 2) / rayDirX);
			else
				perpWallDist = fabs((mapY - rayPosY + (1 - stepY) / 2) / rayDirY);

			//Calculate height of line to draw on screen
			int lineHeight = abs(int(h / perpWallDist));

			//calculate lowest and highest pixel to fill in current stripe
			int drawStart = -lineHeight / 2 + h / 2;
			if (drawStart < 0)drawStart = 0;
			int drawEnd = lineHeight / 2 + h / 2;
			if (drawEnd >= h)drawEnd = h - 1;



			//choose texture
			int i = 0;
			switch (worldMap[mapX][mapY])
			{
			case 1:  i = 0;  break;
			case 2:  i = 1;  break;
			case 3:  i = 2;   break;
			case 4:  i = 3;  break;
			default: i = 0; break;
			}

			//give x and y sides different brightness
			if (side == 1) {
				color = sf::Color(color.r / 2, color.g / 2, color.b / 2);
			}



			//texturing calculations
			int texNum = worldMap[mapX][mapY] - 1; //1 subtracted from it so that texture 0 can be used!

			//calculate value of wallX
			double wallX; //where exactly the wall was hit
			if (side == 0)
			{
				wallX = rayPosY + perpWallDist * rayDirY;
			}
			else
			{
				wallX = rayPosX + perpWallDist * rayDirX;
			}

			wallX -= floor((wallX));

			int texX = int(wallX * double(texWidth));
			if (side == 0 && rayDirX > 0)
			{
				texX = texWidth - texX - 1;
			}
			if (side == 1 && rayDirY < 0)
			{
				texX = texWidth - texX - 1;
			}

			int ceilingPixel = int(-lineHeight * (1.0f - 0.5f) + h * 0.5f);
			//changed 
			groundPixel = int(lineHeight * 0.5f + h * 0.5f);


			//x coordinate on the texture
			//int texX = int(wallX * double(texWidth));
			//if (side == 0 && rayDirX > 0) texX = texWidth - texX - 1;
			//	if (side == 1 && rayDirY < 0) texX = texWidth - texX - 1;
			wallX = wallX * float(texWidth);

			//draw the pixels of the stripe as a vertical line        
			sf::VertexArray lineArray(sf::Lines, 2);
			lineArray[0].position = sf::Vector2f(x, ceilingPixel);
			lineArray[1].position = sf::Vector2f(x, groundPixel);
			lineArray[0].texCoords = sf::Vector2f(texX, 0);
			lineArray[1].texCoords = sf::Vector2f(texX, texWidth);



			//draw walls to screen
			window.draw(lineArray, &textures[i]);

			//SET THE ZBUFFER FOR THE SPRITE CASTING
			ZBuffer[x] = perpWallDist; //perpendicular distance is used

		}


			//draw sprites ----------------------------------------------------------------------------------------------------
			for (int k = 0; k < numSprites; k++)
			{
				spriteOrder[k] = k;
				//spriteDistance[i] = ((posX - sprite[i].x) * (posX - sprite[i].x) + (posY - sprite[i].y) * (posY - sprite[i].y)); //sqrt not taken, unneeded
			}

			combSort(spriteOrder, spriteDistance, numSprites);


			//after sorting the sprites, do the projection and draw them
			for (int i = 0; i < numSprites; i++)
			{
				//translate sprite position to relative to camera
				double spriteX = sprite[spriteOrder[i]].x - posX;
				double spriteY = sprite[spriteOrder[i]].y - posY;

				//transform sprite with the inverse camera matrix
				// [ planeX   dirX ] -1                                       [ dirY      -dirX ]
				// [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
				// [ planeY   dirY ]                                          [ -planeY  planeX ]

				double invDet = 1.0 / (planeX * dirY - dirX * planeY); //required for correct matrix multiplication

				double transformX = invDet * (dirY * spriteX - dirX * spriteY);
				double transformY = invDet * (-planeY * spriteX + planeX * spriteY); //this is actually the depth inside the screen, that what Z is in 3D

				int spriteScreenX = int((w / 2) * (1 + transformX / transformY));

				//calculate height of the sprite on screen
				int spriteHeight = abs(int(h / (transformY))); //using "transformY" instead of the real distance prevents fisheye
				//calculate lowest and highest pixel to fill in current stripe
				int drawStartY = -spriteHeight / 2 + h / 2;
				if (drawStartY < 0) drawStartY = 0;
				int drawEndY = spriteHeight / 2 + h / 2;
				if (drawEndY >= h) drawEndY = h - 1;


				//calculate width of the sprite
				int spriteWidth = abs(int(h / (transformY)));
				int drawStartX = -spriteWidth / 2 + spriteScreenX;
				if (drawStartX < 0) drawStartX = 0;
				int drawEndX = spriteWidth / 2 + spriteScreenX;
				if (drawEndX >= w) drawEndX = w - 1;


				sf::Sprite testSprite;
				testSprite.setTexture(spriteTex);

				for (int stripe = drawStartX; stripe < drawEndX; stripe++)
				{
					int texX = int(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * texWidth / spriteWidth) / 256;
					//the conditions in the if are:
					//1) it's in front of camera plane so you don't see things behind you
					//2) it's on the screen (left)
					//3) it's on the screen (right)
					//4) ZBuffer, with perpendicular distance
					if (transformY > 0 && stripe > 0 && stripe < w && transformY < ZBuffer[stripe])
					{
						testSprite.setPosition(sf::Vector2f(drawStartX, drawStartY)); // absolute position
					}
					window.draw(testSprite);
				}
			}
        
       


        fps = clock.getElapsedTime();
        float fpsValue = 1000000/fps.asMicroseconds();
        clock.restart();

		// Declare and load a font
		sf::Font font;
		font.loadFromFile("arial.ttf");
		// Create a text
		sf::Text text(std::to_string(fpsValue), font);
		text.setCharacterSize(30);
		text.setStyle(sf::Text::Bold);
		text.setColor(sf::Color::Red);
		// Draw it
		window.draw(text);
		//window.draw(floorLines);

       
        double moveSpeed = fps.asSeconds() * 5.0; //the constant value is in squares/second
        double rotSpeed = fps.asSeconds() * 2.0; //the constant value is in radians/second

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))) {
			if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) {

				posX += dirX * moveSpeed;
			}
				double oldDirX = dirX;
				dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
				dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
				double oldPlaneX = planeX;
				planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
				planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
			if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) {
				if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += planeX * moveSpeed;
				if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += planeY * moveSpeed;
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) {
				if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) posX -= planeX * moveSpeed;
				if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY -= planeY * moveSpeed;
			}
		}
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            //both camera direction and camera plane must be rotated
            double oldDirX = dirX;
            dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
            dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
            planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            //both camera direction and camera plane must be rotated
            double oldDirX = dirX;
            dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
            dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
            planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            if(worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += dirX * moveSpeed;
            if(worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            if(worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
            if(worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
        }
       
        window.display();
        window.clear();
    }

    return EXIT_SUCCESS;
}
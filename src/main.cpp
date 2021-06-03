#include <iostream>
#include <vector>
#include <cmath>
#include <thread>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>

using namespace std;

struct Chunk
{
	int *cells;
	Chunk (int, int, int);
};

Chunk::Chunk(int cx=0, int cy=0, int chunkWidth=0)
{
	int val = abs(cx * cy);
	cells = new int[chunkWidth * chunkWidth];
	for (int i = 0; i < chunkWidth; i++)
	for (int j = 0; j < chunkWidth; j++)
	{	
		int cellIndex = i * chunkWidth + j;
		cells[cellIndex] = val;
	}
}

struct Chunker 
{
	bool isRendering;

	int renderDistance;

	int mapWidth;
	int mapWidthChunks;
	int mapWidthCells;
	int mapWidthPx;
	int mapChunkNum;
	int chunkWidth;
	int chunkWidthPx;
	int chunkCellNum;
	int cellWidth;

	int cursx;
	int cursy;

	int curcx;
	int curcy;

	int curpx;
	int curpy;

	int dcx;
	int dcy;

	int sx;
	int sy;

	bool alive = true;

	sf::RenderTexture tMap;
	sf::Sprite sMap;
	Chunk *map;

	Chunker (int, int, int);
	void move(int, int);
	void changeStart();
};

Chunker::Chunker(int renderDistance, int chunkWidth, int cellWidth)
{
	this->isRendering = false;

	this->renderDistance = renderDistance;

	this->mapWidth = renderDistance * 2 + 1;
	this->mapWidthCells = mapWidth * chunkWidth;
	this->mapWidthPx = mapWidthCells * cellWidth;
	this->mapChunkNum = pow(mapWidth, 2);
	this->chunkWidth = chunkWidth;
	this->chunkWidthPx = chunkWidth * cellWidth;
	this->chunkCellNum = pow(chunkWidth, 2);
	this->cellWidth = cellWidth;

	this->cursx = 0;
	this->cursy = 0;

	this->curcx = 0;
	this->curcy = 0;

	this->curpx = 0;
	this->curpy = 0;

	this->dcx = 0;
	this->dcy = 0;

	this->sx = 0;
	this->sy = 0;

	this->tMap.create(mapWidthPx, mapWidthPx);
	this->sMap.setTexture(tMap.getTexture());
	this->map = new Chunk[mapChunkNum];
	for (int i = 0; i < mapWidth; i++)
	for (int j = 0; j < mapWidth; j++)
	{

		int chunkIndex = i * mapWidth + j;
		map[chunkIndex];
		Chunk chunk = Chunk(j, i, chunkWidth);
		map[chunkIndex] = chunk;
	}
}

void Chunker::move(int px, int py)
{
	this->curpx = px;
	this->curpy = py;

	// calculating new cx, cy
	int ncx = curpx / (cellWidth * chunkWidth);
	if (curpx < 0) ncx -= 1;
	int ncy = curpy / (cellWidth * chunkWidth);
	if (curpy < 0) ncy -= 1;

	// calculating differences
	this->dcx = ncx - curcx;
	this->dcy = ncy - curcy;

	//assigning
	this->curcx = ncx;
	this->curcy = ncy;

}

void Chunker::changeStart()
{	
	this->sx = curcx * chunkWidthPx - (renderDistance * chunkWidthPx);
	this->sy = curcy * chunkWidthPx - (renderDistance * chunkWidthPx);	
	sMap.setPosition(sx, sy);
}


void update(Chunker &c, sf::RenderTexture &otMap, sf::Sprite &osMap)
{
	c.isRendering = true;
	if (c.dcx == 0 && c.dcy == 0) 
	{
		c.isRendering = false;
		return;
	}	

	// copy value from c here
	int renderDistance = c.renderDistance;
	int mapWidth = c.mapWidth; 
	int mapWidthCells = c.mapWidthCells;
	int mapWidthPx = c.mapWidthPx; 
	int mapChunkNum = c.mapChunkNum; 
	int chunkWidth = c.chunkWidth; 
	int chunkWidthPx = c.chunkWidthPx; 
	int cellWidth = c.cellWidth; 
	int curcx = c.curcx;
	int curcy = c.curcy;

	Chunk *map = new Chunk[c.mapChunkNum];
	// for (int i = 0; i < mapWidth; i++)
	// for (int j = 0; j < mapWidth; j++)
	// {
	// 	int chunkIndex = i * mapWidth + j;
	// 	map[chunkIndex] = c.map[chunkIndex];
	// }

	// creating the new map based on curcx, curcy
	for (int i = -renderDistance; i < renderDistance + 1; i++)
	{
	for (int j = -renderDistance; j < renderDistance + 1; j++)
	{
		int chunkIndex = ((i + renderDistance) * mapWidth) + (j + renderDistance);
		Chunk chunk = Chunk(curcx + j, curcy - i, chunkWidth);
		map[chunkIndex] = chunk;
	}
	}

	// drawing the map to a texture
	using namespace sf;
	RenderTexture tRect;
	tRect.create(1, 1);
	Sprite sRect;
	sRect.setScale(cellWidth, cellWidth);
	sRect.setTexture(tRect.getTexture());
	RenderTexture tMap;
	tMap.create(mapWidthPx, mapWidthPx);
	Sprite sMap;
	sMap.setTexture(tMap.getTexture());

	for (int i = 0; i < mapWidthCells; i++)
	for (int j = 0; j < mapWidthCells; j++)
	{
		int cx = j / chunkWidth;
		int cy = i / chunkWidth;


		int cellx = j % chunkWidth;
		int celly = i % chunkWidth;

		int chunkIndex = cy * mapWidth + cx;
		int cellIndex = celly * chunkWidth + cellx;

		Chunk chunk = map[chunkIndex];
		int val = chunk.cells[cellIndex];
		int col = 51 * val;
		tRect.clear(Color(col, col, col));
		sRect.setPosition(cx * chunkWidthPx + cellx * cellWidth, cy * chunkWidthPx + celly * cellWidth);
		tRect.display();
		
		tMap.draw(sRect);
	}
	tMap.display();
	std::copy(map, map+mapChunkNum, c.map);

	c.isRendering = false;
	c.changeStart();
	osMap.setPosition(c.sx, c.sy);
	otMap.draw(sMap);
}

int intx = 0;
int inty = 0;
Chunker c(3, 10, 50);
sf::RenderTexture tMap;
sf::Sprite sMap;
void chunkThread()
{
tMap.create(c.mapWidthPx, c.mapWidthPx);
sMap.setTexture(tMap.getTexture());
	while (c.alive)
	{
		c.move(intx, inty);
		update(c, tMap, sMap);
	}
}

int main() {
	using namespace sf;
	srand(time(0));
	RenderWindow window;
	window.create(VideoMode(300, 300), "WINDOW");


	RenderTexture textureRect;
	textureRect.create(1, 1);
	textureRect.clear(Color(255,0,0));
	Sprite spriteRect;
	spriteRect.setTexture(textureRect.getTexture());

	View playerView;
	playerView.setSize(300, 300);

	Clock clock;
	Time dt = seconds(0);
	Time timeNow = seconds(0);
	Time lastFrame = seconds(0);
	Time lastSecond = seconds(0);
	int fps = 0;

	bool up, down, left, right;
	up = down = left = right = false;
	bool kw, ks, ka, kd;
	kw = ks = ka = kd = false;
	float x, y;
	x = y = 0;
	
	std::thread t(chunkThread);

	Event event;
	while(window.isOpen()) {
		dt = clock.getElapsedTime() - timeNow;
		timeNow += dt; 

		// poll events
		while(window.pollEvent(event)) 
		{
			switch (event.type) 
			{
			case Event::Closed:
				window.close();
				c.alive = false;
				break;
			case Event::KeyPressed:
				switch (event.key.code)
				{
				case Keyboard::Up:	up	= true; break;
				case Keyboard::Down:	down	= true; break;
				case Keyboard::Left:	left	= true; break;
				case Keyboard::Right:	right	= true; break;
				case Keyboard::W:	kw	= true; break;
				case Keyboard::S:	ks	= true; break;
				case Keyboard::A:	ka	= true; break;
				case Keyboard::D:	kd	= true; break;
				}
				break;
			case Event::KeyReleased:
				switch (event.key.code)
				{
				case Keyboard::Up:	up	= false; break;
				case Keyboard::Down:	down	= false; break;
				case Keyboard::Left:	left	= false; break;
				case Keyboard::Right:	right	= false; break;
				case Keyboard::W:	kw	= false; break;
				case Keyboard::S:	ks	= false; break;
				case Keyboard::A:	ka	= false; break;
				case Keyboard::D:	kd	= false; break;
				}
				break;
			}
		}

		float n = 300.f*dt.asSeconds();
		float dcx, dcy;
		dcx = dcy = 0;
		if (up) dcy -= n;
		if (down) dcy += n;
		if (right) dcx += n;
		if (left) dcx -= n;
		if (ka) playerView.rotate(360.f*dt.asSeconds());
		if (kd) playerView.rotate(-360.f*dt.asSeconds());
		if (ks) playerView.zoom(1-0.9f*dt.asSeconds()); 
		if (kw) playerView.zoom(1+0.9f*dt.asSeconds()); 
		x += dcx; y += dcy;
		playerView.setCenter(x, y);
		intx = x; inty = y;



		if (timeNow - lastSecond >= seconds(1))
		{
			lastSecond = timeNow;
			fps = 0;
		}

		if (timeNow - lastFrame >= milliseconds(17))
		{
			lastFrame = timeNow;
			fps += 1;
			
			window.clear(Color(100,200,255));

			window.setView(playerView);

			window.draw(sMap);

			spriteRect.setPosition(x-25, y-25);
			spriteRect.setScale(50, 50);
			textureRect.clear(Color(150,255,0));
			window.draw(spriteRect);

			textureRect.clear(Color::Yellow);
			spriteRect.setPosition(0, 0);
			spriteRect.setScale(c.cellWidth*4, c.cellWidth*4);
			window.draw(spriteRect);
			window.display();

		}

	}

	return 0;
}

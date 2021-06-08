#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <map>
#include <string>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>

using namespace std;

void printmany(vector<float> m)
{
	for (float i : m)
		cout << i << "\t";
	cout << endl;
}

sf::Mutex mutex;

vector<vector<int>> cCoor;
vector<vector<int>> cCells;
int renderDistance;
int cellWpx = 25;
int chunkWcell = 4;
int chunkWpx = chunkWcell * cellWpx;
int ppx = 0;
int ppy = 0;
bool running = true;
float high = 0.f;
float low = 0.f;

vector<sf::Sprite> tileSprites;

float interpolate(float s, float e, float x)
{
	return (e - s) * x + s;
}

struct Gradient{
	float x, y;
};

float grads[4][2] = 
{
	{.7071, .7071},
	{.7071, -.7071},
	{-.7071, -.7071},
	{-.7071, .7071}
};

Gradient randomGradient(int cx, int cy) 
{
	int random = 2920.f * sin(cx * 21942.f + cy * 171324.f + 8912.f) * cos(cx * 23157.f * cy * 217832.f + 9758.f);
	Gradient grad; 
	grad.x = grads[random % 4][0];//cos(random);
	grad.y = grads[random % 4][1];//sin(random);
	return grad;
}

float dot(int cx, int cy, float lx, float ly) {
    Gradient gradient = randomGradient(cx, cy);

    float dx = lx;
    float dy = ly;

    return (dx*gradient.x + dy*gradient.y);
}

float perlin(int cx, int cy, int cellx, int celly) {
	float lx = (cellx + 0.5) / chunkWcell;
	float ly = (celly + 0.5) / chunkWcell;

	float tl = dot(cx, cy, -lx, -ly);
	float tr = dot(cx + 1, cy, 1 - lx, -ly);
	float dl = dot(cx, cy + 1, -lx, 1 - ly);
	float dr = dot(cx + 1, cy + 1, 1 - lx, 1 - ly);

	float top = interpolate(tl, tr, lx);
	float bot = interpolate(dl, dr, lx);
	float mid = interpolate(top, bot, ly);

	if (mid > high) high = mid;
	else if (mid < low) low = mid;
	int val = (mid + 1) * tileSprites.size() / 2;
	return val;
}

tuple<int, int> getChunk (int ppx, int ppy)
{
	int cx = ppx / (chunkWcell * cellWpx);
	int cy = ppy / (chunkWcell * cellWpx);
	if (ppx < 0) cx -= 1;
	if (ppy < 0) cy -= 1;
	tuple<int, int> tup(cx, cy);
	return tup;
}

void renderChunks()
{
	int lcx, lcy;
	lcx = lcy = 1;
	while (running)
	{
		vector<vector<int>> scCoor;
		vector<vector<int>> scCells;
		int cx, cy;
		tie(cx, cy) = getChunk(ppx, ppy);
		if (cx == lcx && cy == lcy) continue;
		lcx = cx; lcy = cy;
		for (int i = -renderDistance; i < renderDistance + 1; i++)
			for (int j = -renderDistance; j < renderDistance + 1; j++)
			{
				int ccx, ccy;
				ccx = cx + j;
				ccy = cy + i;
				vector<int> coor = {ccx, ccy};
				vector<int> cells;
				for (int k = 0; k < chunkWcell * chunkWcell; k++)
				{
					int cellx = k % chunkWcell;
					int celly = k / chunkWcell;
					int val = perlin(cx+j, cy+i, cellx, celly);
					cells.push_back(val);
				}
				scCoor.push_back(coor);
				scCells.push_back(cells);
			}
		mutex.lock();
		cCoor = scCoor;
		cCells = scCells;
		mutex.unlock();
	}
}


int main() {
	cout << "Enter render distance" << endl;
	cin >> renderDistance;
	using namespace sf;
	srand(time(0));

	int screenW = 800;
	int screenH = 600;

	RenderWindow window;
	window.create(VideoMode(screenW, screenH), "WINDOW");

	Texture bumblebee;
	bumblebee.loadFromFile("rsrc/bumblebee.png");
	Sprite sbumblebee(bumblebee);

	// loading map textures
	Texture tile0; tile0.loadFromFile("rsrc/tile0.png");
	Texture tile1; tile1.loadFromFile("rsrc/tile1.png");
	Texture tile2; tile2.loadFromFile("rsrc/tile2.png");
	Sprite stile0(tile0);
	Sprite stile1(tile1);
	Sprite stile2(tile2);
	tileSprites.push_back(stile0);
	tileSprites.push_back(stile0);
	tileSprites.push_back(stile0);
	tileSprites.push_back(stile0);
	tileSprites.push_back(stile1);
	tileSprites.push_back(stile2);
	tileSprites.push_back(stile2);
	tileSprites.push_back(stile2);
	tileSprites.push_back(stile2);

	View playerView;
	playerView.setSize(screenW, screenH);

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
	
	Thread t(&renderChunks);
	t.launch();

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
				running = false;
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
		if (ka) playerView.rotate(90.f*dt.asSeconds());
		if (kd) playerView.rotate(-90.f*dt.asSeconds());
		if (ks) playerView.zoom(1-0.9f*dt.asSeconds()); 
		if (kw) playerView.zoom(1+0.9f*dt.asSeconds()); 
		x += dcx; y += dcy;
		playerView.setCenter(x, y);
		ppx = x; ppy = y;



		if (timeNow - lastSecond >= seconds(1))
		{
			lastSecond = timeNow;
			std::cout << "FPS: " << fps  << " high: " << high << " low: " << low << std::endl;
			fps = 0;
		}

		if (timeNow - lastFrame >= milliseconds(17))
		{
			lastFrame = timeNow;
			fps += 1;
			
			window.clear(Color(100,200,255));

			window.setView(playerView);

			mutex.lock();
			for (int i = 0; i < cCoor.size(); i++)
			{
				int sx = cCoor[i][0] * chunkWpx;
				int sy = cCoor[i][1] * chunkWpx;

				for (int j = 0; j < cCells[i].size(); j++)
				{
					int cellx = j % chunkWcell;
					int celly = j / chunkWcell;
					int bx = sx + cellx * cellWpx;
					int by = sy + celly * cellWpx;
					tileSprites[cCells[i][j]].setPosition(bx, by);
					window.draw(tileSprites[cCells[i][j]]);
				}
				// std::cout << cCoor[i][0] << " " << cCoor[i][1] << std::endl;
			}
			mutex.unlock();

			// drawing the character
			sbumblebee.setPosition(x-cellWpx/2, y-cellWpx/2);
			window.draw(sbumblebee);

			window.display();
		}

	}

	return 0;
}

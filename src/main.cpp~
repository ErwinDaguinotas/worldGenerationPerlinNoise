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
				int val = abs((cx + j) * (cy + i)) % 5;
				int ccx, ccy;
				ccx = cx + j;
				ccy = cy + i;
				vector<int> coor = {ccx, ccy};
				vector<int> cells;
				for (int k = 0; k < chunkWcell * chunkWcell; k++)
					cells.push_back(val);
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
	RenderWindow window;
	window.create(VideoMode(300, 300), "WINDOW");

	RenderTexture textureRect;
	textureRect.create(1, 1);
	Sprite spriteRect;
	spriteRect.setTexture(textureRect.getTexture());

	// loading map textures
	RenderTexture a; a.create(cellWpx, cellWpx); a.clear(Color(255, 0, 0));
	RenderTexture b; b.create(cellWpx, cellWpx); b.clear(Color(200, 55, 0));
	RenderTexture c; c.create(cellWpx, cellWpx); c.clear(Color(150, 105, 0));
	RenderTexture d; d.create(cellWpx, cellWpx); d.clear(Color(100, 155, 0));
	RenderTexture e; e.create(cellWpx, cellWpx); e.clear(Color(50, 205, 0));
	Sprite aa(a.getTexture());
	Sprite bb(b.getTexture());
	Sprite cc(c.getTexture());
	Sprite dd(d.getTexture());
	Sprite ee(e.getTexture());
	vector<Sprite> tileSprites = {aa, bb, cc, dd, ee};

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
		if (ka) playerView.rotate(360.f*dt.asSeconds());
		if (kd) playerView.rotate(-360.f*dt.asSeconds());
		if (ks) playerView.zoom(1-0.9f*dt.asSeconds()); 
		if (kw) playerView.zoom(1+0.9f*dt.asSeconds()); 
		x += dcx; y += dcy;
		playerView.setCenter(x, y);
		ppx = x; ppy = y;



		if (timeNow - lastSecond >= seconds(1))
		{
			lastSecond = timeNow;
			std::cout << "FPS: " << fps << std::endl;
			fps = 0;
		}

		if (timeNow - lastFrame >= milliseconds(17))
		{
			lastFrame = timeNow;
			fps += 1;
			
			window.clear(Color(100,200,255));

			window.setView(playerView);

			mutex.lock();
			spriteRect.setScale(cellWpx, cellWpx);
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
			spriteRect.setPosition(x-25, y-25);
			spriteRect.setScale(50, 50);
			textureRect.clear(Color(150,255,0));
			window.draw(spriteRect);

			window.display();
		}

	}

	return 0;
}

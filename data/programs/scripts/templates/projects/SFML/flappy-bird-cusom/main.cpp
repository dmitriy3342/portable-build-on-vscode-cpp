#include <SFML/Graphics.hpp>
#include <iostream>

template <class B, typename T>
bool instanceOf(T imp)
{
	if (dynamic_cast<B *>(imp) != nullptr)
	{
		// imp is instance of B
		return true;
	}
	// imp is NOT instance of B
	return false;
}

sf::IntRect getIntRect(sf::RectangleShape &shape)
{
	return sf::IntRect(shape.getPosition().x, shape.getPosition().y, shape.getSize().x, shape.getSize().y);
}

class Entity
{
public:
	virtual void draw() = 0;		  //Отрисовка объектов
	virtual void process() = 0;		  //Обработка логики объектов
	virtual void processEvents() = 0; //Логика обработки событий системы
	virtual bool intersects(sf::IntRect &rect) = 0;
};

class Grass : public Entity
{
	sf::RenderWindow &window;
	sf::Event &event;
	sf::Texture texture;
	sf::RectangleShape shape;
	float coefGrassZoom;
	float grassWidth;
	float сoefGrassWidth;
	int numberGrassInWidth;
	float grassSpeed;

public:
	Grass(sf::RenderWindow &window, sf::Event &event) : window(window), event(event)
	{
		texture.loadFromFile("imgs/grass-1.png");
		texture.setRepeated(true);

		shape.setSize(sf::Vector2f(window.getSize().x * 3, 80));
		shape.setTexture(&texture);
		shape.setPosition(0, window.getSize().y - shape.getSize().y + 5);

		coefGrassZoom = 988 / 80;
		grassWidth = 3227 / coefGrassZoom;
		сoefGrassWidth = window.getSize().x * 3 / grassWidth;
		numberGrassInWidth = window.getSize().x / grassWidth;
		grassSpeed = 1;

		shape.setTextureRect(sf::IntRect(0, 0, 3227 * сoefGrassWidth, 988));
	}

	virtual void draw()
	{
		window.draw(shape);
	}

	virtual void process()
	{
		if (shape.getPosition().x < -(numberGrassInWidth + 1) * grassWidth)
		{
			shape.setPosition(0, shape.getPosition().y);
		}

		shape.move(-grassSpeed, 0);
	}

	virtual void processEvents()
	{
	}

	virtual bool intersects(sf::IntRect &rect)
	{
		return getIntRect(shape).intersects(rect);
	};
};

class FPS : public Entity
{
	sf::RenderWindow &window;
	sf::Text text;
	sf::Clock clock;
	int fps;
	int countFps;
	unsigned int size;

public:
	FPS(sf::RenderWindow &window, sf::Font &font) : window(window)
	{
		fps = 0;
		countFps = 0;
		size = 20;
		text.setFont(font);
		text.setFillColor(sf::Color(255, 166, 0));
		text.setCharacterSize(size);
	}

	virtual void draw()
	{
		window.draw(text);
	}

	virtual void process()
	{
		sf::Time elapsed = clock.getElapsedTime();
		if (elapsed.asMilliseconds() >= 1000)
		{
			fps = countFps;
			countFps = -1;
			clock.restart();
		}
		text.setString("FPS: " + std::to_string(fps));

		countFps++;
	}

	virtual void processEvents()
	{
	}

	virtual bool intersects(sf::IntRect &rect)
	{
		return false;
	};
};

class Column : public Entity
{
	sf::RenderWindow &window;
	sf::Event &event;
	sf::Texture texture;
	sf::RectangleShape shapeTop;
	sf::RectangleShape shapeBottom;
	int speed;
	int spaceBetweenShapes;
	int spaceBetweenColumns;
	int width;

	void animation()
	{
		shapeTop.move(-speed, 0);
		shapeBottom.move(-speed, 0);

		if (shapeTop.getPosition().x + shapeTop.getSize().x < 0)
		{
			shapeTop.setPosition(window.getSize().x + spaceBetweenColumns, 0);
			shapeTop.setSize(sf::Vector2f(width, randomHeight()));

			shapeBottom.setSize(sf::Vector2f(width, window.getSize().y - shapeTop.getSize().y - spaceBetweenShapes));
			shapeBottom.setPosition(shapeTop.getPosition().x, shapeTop.getSize().y + spaceBetweenShapes);

			countColumn += 1;
		}
	}

	int randomHeight()
	{
		return rand() % (window.getSize().y - 200) + 50;
	}

public:
	static int countColumn;

	Column(sf::RenderWindow &window, sf::Event &event, int startPositionX)
		: window(window), event(event)
	{
		texture.loadFromFile("imgs/column.png");

		spaceBetweenShapes = 100;
		width = 100;
		spaceBetweenColumns = 200;
		speed = 2;

		shapeTop.setTexture(&texture);
		shapeTop.setSize(sf::Vector2f(width, randomHeight()));
		shapeTop.setPosition(startPositionX, 0);

		shapeBottom.setTexture(&texture);
		shapeBottom.setSize(sf::Vector2f(width, window.getSize().y - shapeTop.getSize().y - spaceBetweenShapes));
		shapeBottom.setPosition(shapeTop.getPosition().x, shapeTop.getSize().y + spaceBetweenShapes);
	}

	virtual void draw()
	{
		window.draw(shapeTop);
		window.draw(shapeBottom);
	}

	virtual void process()
	{
		animation();
	}

	virtual void processEvents()
	{
	}

	virtual bool intersects(sf::IntRect &rect)
	{

		return getIntRect(shapeTop).intersects(rect) || getIntRect(shapeBottom).intersects(rect);
	};
};
int Column::countColumn = 0;

class Bird : public Entity
{
	sf::RenderWindow &window;
	sf::Event &event;
	std::vector<Entity *> &entities;
	sf::Texture texture;
	sf::RectangleShape shape;
	int index;
	int speed;

	sf::IntRect getTextureRectForBird(int index)
	{
		int row = index / 5;
		int column = index % 5;
		return sf::IntRect(178 * column, 166 * row, 180, 166);
	}

	void animation()
	{
		shape.setTextureRect(getTextureRectForBird(index / 2));
		index++;

		if (index > 13 * 2)
		{
			index = 0;
		}
	}

public:
	Bird(sf::RenderWindow &window, sf::Event &event, std::vector<Entity *> &entities)
		: window(window), event(event), entities(entities)
	{
		texture.loadFromFile("./imgs/sprite-bird-animated.png");

		shape.setSize(sf::Vector2f(60, 60));
		shape.setTexture(&texture);

		shape.setPosition(window.getSize().x / 2 - 75, window.getSize().y / 2 - 75);

		index = 0;
		speed = 30;
	}

	virtual void draw()
	{
		window.draw(shape);
		//shape.getTextureRect().intersects(df)
	}

	virtual void process()
	{
		animation();

		for (int i = 0; i < entities.size(); i++)
		{

			if (!instanceOf<Bird>(entities[i]))
			{
				sf::IntRect rect = getIntRect(shape);
				if (entities[i]->intersects(rect))
				{
					std::cout << " collision" << std::endl;
				}
			}
		}
	}

	virtual void processEvents()
	{
		if (event.KeyPressed && (event.key.code == sf::Keyboard::W))
		{
			if (shape.getPosition().y > 0)
			{
				shape.move(0, -speed);
			}
			else
			{
				shape.setPosition(shape.getPosition().x, 0);
			}
		}
		if (event.KeyPressed && (event.key.code == sf::Keyboard::S))
		{
			if (shape.getPosition().y + shape.getSize().y < window.getSize().y)
			{
				shape.move(0, speed);
			}
			else
			{
				shape.setPosition(shape.getPosition().x, window.getSize().y - shape.getSize().y);
			}
		}
		if (event.KeyPressed && (event.key.code == sf::Keyboard::A))
		{
			if (shape.getPosition().x > 0)
			{
				shape.move(-speed, 0);
			}
			else
			{
				shape.setPosition(0, shape.getPosition().y);
			}
		}
		if (event.KeyPressed && (event.key.code == sf::Keyboard::D))
		{
			if (shape.getPosition().x + shape.getSize().x < window.getSize().x)
			{
				shape.move(speed, 0);
			}
			else
			{
				shape.setPosition(window.getSize().x - shape.getSize().x, shape.getPosition().y);
			}
		}
	}

	virtual bool intersects(sf::IntRect &rect)
	{
		return getIntRect(shape).intersects(rect);
	};
};

class Statistic : public Entity
{
	sf::RenderWindow &window;
	sf::Text text;
	unsigned int size;

public:
	Statistic(sf::RenderWindow &window, sf::Font &font) : window(window)
	{

		size = 20;
		text.setFont(font);
		text.setFillColor(sf::Color(255, 166, 0));
		text.setCharacterSize(size);
		text.move(0, 20);
	}

	virtual void draw()
	{
		window.draw(text);
	}

	virtual void process()
	{
		text.setString("ACCOUNT: " + std::to_string(Column::countColumn));
	}

	virtual void processEvents()
	{
	}

	virtual bool intersects(sf::IntRect &rect)
	{
		return false;
	};
};

int main()
{
	//Скрыть консоль
	//FreeConsole();

	//Создаем окно
	int HEIGHT = 800;
	int WIDTH = 1200;
	int FPS_MAX = 60;
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!");
	window.setFramerateLimit(FPS_MAX);

	sf::Event event; //Событие системы

	std::vector<Entity *> entities;

	sf::Font fontLight, fontBold, fontRegular;

	if (fontLight.loadFromFile("./fonts/Roboto-Light-webfont.woff"))
		std::cout << "Font Light was downloaded!" << std::endl;

	if (fontBold.loadFromFile("./fonts/Roboto-Bold-webfont.woff"))
		std::cout << "Font Bold was downloaded!" << std::endl;

	if (fontRegular.loadFromFile("./fonts/Roboto-Regular-webfont.woff"))
		std::cout << "Font Regular was downloaded!" << std::endl;

	entities.push_back(new Column(window, event, WIDTH + 300 * 0));
	entities.push_back(new Column(window, event, WIDTH + 300 * 1));
	entities.push_back(new Column(window, event, WIDTH + 300 * 2));
	entities.push_back(new Column(window, event, WIDTH + 300 * 3));
	entities.push_back(new Column(window, event, WIDTH + 300 * 4));

	//Птичка
	entities.push_back(new Bird(window, event, entities));

	//Травка
	entities.push_back(new Grass(window, event));

	//FPS
	entities.push_back(new FPS(window, fontRegular));

	// Статистика
	entities.push_back(new Statistic(window, fontRegular));

	//Основной цикл приложения
	while (window.isOpen())
	{
		//Цикл обработки событий системы
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			//Логика обработки событий

			for (int i = 0; i < entities.size(); i++)
			{
				entities[i]->processEvents();
			}
		}

		//Начало блока логики программы

		for (int i = 0; i < entities.size(); i++)
		{
			entities[i]->process();
		}
		//Конец блока логики программы

		//Очистка экрана, путем заливки цветом
		window.clear(sf::Color::White);

		//Рисование
		for (int i = 0; i < entities.size(); i++)
		{
			entities[i]->draw();
		}

		//Показать все отрисованные элементы
		window.display();
	}
	return 0;
}

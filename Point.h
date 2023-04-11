#pragma once

class Point
{
public:
	int x, y;
	Point(int x, int y) {
		this->x = x;
		this->y = y;
	}

	Point() {
		this->x = 0;
		this->y = 0;
	}

	Point operator+(const Point& p) {
		return Point(this->x + p.x, this->y + p.y);
	}
};


#pragma once

#include <vector>
#include <iostream>
#include <algorithm>

class Cell;

class Rectangle
{
private:
	int colorNumber;
	int mechanicNumber;

	std::vector<Cell*> cells;
	std::vector<Rectangle*> neighbers;

	inline void sortCells();

public:
	Rectangle();

	friend std::ostream& operator<<(std::ostream&, const Rectangle&);

	bool operator==(const Rectangle& region) const;

	bool operator<(const Rectangle&) const;

	int Width() const;
	int Height() const;

	int CountNeighbors() const;

	int GetMechanic() const;
	int GetColor() const;

	void ExitCell(Cell* const);
	void AddCell(Cell* const);
	void FindNeigbers();
	Rectangle* GetNeighbor(int) const;

	void SetMechanic(int MechanicNumber);
	void SetColor(int ColorNumber);

	Cell* const BaseCell() const;

	inline int Area() const;
};
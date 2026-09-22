#pragma once

#include <vector>
#include <iostream>
#include <algorithm>

class Cell;
class Region;

class Rectangle
{
private:
	int colorNumber;
	int mechanicLayer0;
	int mechanicLayer1;
	int mechanicLayer2;

	std::vector<Cell*>* cells;
	std::vector<Rectangle*> neighbers;
	std::vector<Rectangle*> topNeighbors;
	std::vector<Rectangle*> leftNeighbors;

	Region* region;

	inline void sortCells();
	void recalculateSize();

	int width;
	int height;

	const int minWidth = 1, minHeight = 1;

public:
	Rectangle(std::vector<Cell*>* Cells);
	Rectangle(int Width, int Height);
	~Rectangle();

	std::ostream& Layer0(std::ostream&);
	std::ostream& Layer1(std::ostream&);
	std::ostream& Layer2(std::ostream&);

	bool operator==(const Rectangle& region) const;

	bool operator<(const Rectangle&) const;

	int Width() const;
	int Height() const;

	int CountNeighbors() const;
	int CountTopNeighbors() const;
	int CountLeftNeighbors() const;

	int GetMechanicLayer0() const;
	int GetMechanicLayer1() const;
	int GetMechanicLayer2() const;
	int GetColor() const;

	void ExitCell(Cell* const);
	void AddCell(Cell* const);

	Rectangle* GetNeighbor(int) const;
	Rectangle* GetTopNeighbor(int) const;
	Rectangle* GetLeftNeighbor(int) const;

	void SetMechanicLayer0(int MechanicNumber);
	void SetMechanicLayer1(int MechanicNumber);
	void SetMechanicLayer2(int MechanicNumber);
	void SetColor(int ColorNumber);

	Cell* const BaseCell() const;

	inline int Area() const;

	Cell* getCell(int X, int Y);

	const std::vector<Cell*>* getCells() const;

	Rectangle* Split();

	//bool TestNeighbors();

	Region* GetRegion() const;
	void SetRegion(Region* Reg);

	void FindNeigbers();

	std::vector<Cell*>* GetCells();
};
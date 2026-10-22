#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <string>

class Cell;
class Region;

class Rectangle
{
private:
	int colorNumber;

	std::string mechanicLayer0;
	std::string mechanicLayer1;
	std::string mechanicLayer2;

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

	std::string GetMechanicLayer0() const;
	std::string GetMechanicLayer1() const;
	std::string GetMechanicLayer2() const;
	int GetColor() const;

	void ExitCell(Cell* const);
	void AddCell(Cell* const);

	Rectangle* GetNeighbor(int) const;
	Rectangle* GetTopNeighbor(int) const;
	Rectangle* GetLeftNeighbor(int) const;

	void SetMechanicLayer0(std::string MechanicNumber);
	void SetMechanicLayer1(std::string MechanicNumber);
	void SetMechanicLayer2(std::string MechanicNumber);
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
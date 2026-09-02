#pragma once

#include <vector>
#include <iostream>
#include <algorithm>

class Cell;

class Region
{
private:
	int colorNumber;
	int mechanicNumber;

	std::vector<Cell*> cells;
	std::vector<Region*> neighbers;

	inline void sortCells();

public:
	Region();

	friend std::ostream& operator<<(std::ostream&, const Region&);

	bool operator==(const Region& region) const;

	bool operator<(const Region&) const;

	int Width() const;
	int Height() const;

	int CountNeighbors() const;

	int GetMechanic() const;
	int GetColor() const;

	void ExitCell(Cell* const);
	void AddCell(Cell* const);
	void FindNeigbers();
	Region* GetNeighbor(int) const;

	void SetMechanic(int MechanicNumber);
	void SetColor(int ColorNumber);

	Cell* const BaseCell() const;

	inline int Area() const;
};
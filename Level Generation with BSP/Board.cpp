#include "Board.h"
#include <iomanip>
#include <random>

Board::Board(int Width, int Height) :
    height{ Height }, width{ Width }, rectangles{}, cells{}, minHeight{ 1 }, minWidth{ 1 }, rng{ std::random_device{}() }, regions{}
{
    Rectangle* baseRectangle = new Rectangle(Width, Height);
    rectangles.push_back(baseRectangle);

    cells = *baseRectangle->getCells();
}

Board::~Board()
{
    for (int n = 0; n < rectangles.size(); n++)
    {
        delete rectangles.at(n);
    }

    for (int i = 0; i < regions.size(); i++)
    {
        delete regions.at(i);
    }
}

Cell* const Board::getCell(int X, int Y) const
{
	return cells.at(X + Y * width);
}

std::ostream& operator<<(std::ostream& Output, const Board& board)
{
    for (int y = 0; y < board.height; y++)
    {
        for (int x = 0; x < board.width; x++)
        {
            Output
                << std::setw(2)
                << *board.getCell(x, y)
                << ",";
        }

        //for (int x = board.width - 2; x >= 0; x--)
        //{
        //    Output
        //        << std::setw(2)
        //        << *board.getCell(x, y)
        //        << ",";
        //}
        Output << "\n";
    }
	return Output;
}

void Board::mesh(int ColorCount)
{
    rectangleSort();

    for (int i = 0; i < rectangles.size(); i++)
    {
        rectangles.at(i)->SetColor(i);
    }

    for (int n = 0; n < rectangles.size() - ColorCount; n++)
    {
        std::vector<Rectangle*> validRectangles;

        // Find rectangles that have at least one neighbor
        // with a different mechanic.
        for (Rectangle* rectangle : rectangles)
        {
            for (int i = 0; i < rectangle->CountNeighbors(); i++)
            {
                Rectangle* neighbor = rectangle->GetNeighbor(i);

                if (neighbor->GetColor() != rectangle->GetColor())
                {
                    validRectangles.push_back(rectangle);
                    break;
                }
            }
        }

        if (validRectangles.empty())
            break;

        // Choose a base rectangle.
        int baseIndex = randomInt(0, validRectangles.size() - 1);
        Rectangle* baseRectangle = validRectangles.at(baseIndex);

        // Find neighbors with a different mechanic.
        std::vector<Rectangle*> differentNeighbors;

        for (int i = 0; i < baseRectangle->CountNeighbors(); i++)
        {
            Rectangle* neighbor = baseRectangle->GetNeighbor(i);

            if (neighbor->GetColor() != baseRectangle->GetColor())
            {
                differentNeighbors.push_back(neighbor);
            }
        }

        if (differentNeighbors.empty())
            continue;

        // Choose one different-mechanic neighbor.
        int neighborIndex =
            randomInt(0, differentNeighbors.size() - 1);

        Rectangle* neighbor = differentNeighbors.at(neighborIndex);

        int baseColor = baseRectangle->GetColor();
        int neighborColor = neighbor->GetColor();

        // Merge the entire neighbor mechanic group
        // into the base mechanic group.
        for (Rectangle* rectangle : rectangles)
        {
            if (rectangle->GetColor() == neighborColor)
            {
                rectangle->SetColor(baseColor);
            }
        }
    }
    findRegion();

    int m = 0;
    for (auto* rect : rectangles)
    {
        rect->SetMechanic(m++);
    }
}

void Board::findRegion()
{
    // Clear previous color groups
    for (auto* region : regions)
    {
        delete region;
    }

    regions.clear();

    // Group rectangles by color
    for (auto* rectangle : rectangles)
    {
        bool found = false;

        for (auto* region : regions)
        {
            if (region->Size() == 0)
                continue;

            // All rectangles in this vector have the same color.
            if (region->GetColor() == rectangle->GetColor())
            {
                region->AddRecangle(rectangle);
                rectangle->SetRegion(region);
                found = true;
                break;
            }
        }

        // No existing group for this color
        if (!found)
        {
            auto* newRegion = new Region();
            newRegion->AddRecangle(rectangle);
            rectangle->SetRegion(newRegion);
            regions.push_back(newRegion);
        }
    }
}

void Board::setColorMechanic(std::vector<Rectangle*>* ColorRectangle, int Mechanic)
{
    for (Rectangle* rectangle : *ColorRectangle)
    {
        rectangle->SetMechanic(Mechanic);
    }
}

int Board::getColorColorRectangle(std::vector<Rectangle*>* ColorRectangle)
{
    return ColorRectangle->at(0)->GetColor();
}

bool Board::touchesBoardEdge(Rectangle* Rectangle)
{
    int x = Rectangle->BaseCell()->X();
    int y = Rectangle->BaseCell()->Y();

    int right = x + Rectangle->Width();
    int bottom = y + Rectangle->Height();

    return
        x == 0 ||
        y == 0 ||
        right == width ||
        bottom == height;
}

bool Board::paint(bool M2, bool M3, bool M4, bool M5)
{
    for (auto* region : regions)
    {
        region->SetMechanic(-1);
    }

    std::vector<int> m{ 0, 1, 2, 3, 4, 5 };
    std::vector<Region*> availableRegions{ regions };

    // ------------------------------------------------
    // m0
    // ------------------------------------------------

    if (availableRegions.empty())
        return false;

    int smallestArea = availableRegions.at(0)->Area();
    int indexM0 = 0;

    for (int i = 1; i < static_cast<int>(availableRegions.size()); i++)
    {
        if (availableRegions.at(i)->Area() < smallestArea)
        {
            indexM0 = i;
            smallestArea = availableRegions.at(i)->Area();
        }
    }

    availableRegions.at(indexM0)->SetMechanic(m.at(0));

    availableRegions.erase(
        availableRegions.begin() + indexM0
    );


    // ------------------------------------------------
    // m1
    // ------------------------------------------------

    int m1Area = 0;

    while (m1Area < smallestM1Area)
    {
        std::vector<Region*> m1Candidates;

        for (auto* region : availableRegions)
        {
            bool valid = true;

            for (auto* rectangle : region->GetConvex())
            {
                for (int i = 0;
                    i < rectangle->CountTopNeighbors();
                    i++)
                {
                    Rectangle* neighbor =
                        rectangle->GetTopNeighbor(i);

                    // Ignore rectangles belonging
                    // to the same region.
                    if (neighbor->GetRegion() == region)
                        continue;

                    int mechanic =
                        neighbor->GetMechanic();

                    // Existing external top neighbors
                    // must be m0 or m1.
                    if (mechanic != m.at(0) &&
                        mechanic != m.at(1))
                    {
                        valid = false;
                        break;
                    }
                }

                if (!valid)
                    break;
            }

            if (valid)
            {
                m1Candidates.push_back(region);
            }
        }

        if (m1Candidates.empty())
            return false;

        int candidateIndex =
            randomInt(
                0,
                static_cast<int>(m1Candidates.size()) - 1
            );

        Region* selectedRegion =
            m1Candidates.at(candidateIndex);

        selectedRegion->SetMechanic(m.at(1));

        m1Area += selectedRegion->Area();

        auto it = std::find(
            availableRegions.begin(),
            availableRegions.end(),
            selectedRegion
        );

        if (it != availableRegions.end())
        {
            availableRegions.erase(it);
        }
    }


    // ------------------------------------------------
    // m2 / m3 / m4 / m5 candidates
    // ------------------------------------------------

    std::vector<Region*> m2Candidates;
    std::vector<Region*> m3Candidates;
    std::vector<Region*> m4Candidates;
    std::vector<Region*> m5Candidates;

    int countSelectedMechanics = 0;


    // m2 can use every remaining region
    if (M2)
    {
        countSelectedMechanics++;

        for (auto* region : availableRegions)
        {
            if (isM2Candidate(region))
            {
                m2Candidates.push_back(region);
            }
        }
    }


    // m3 can use every remaining region
    if (M3)
    {
        m3Candidates.clear();

        for (auto* candidate : availableRegions)
        {
            if (candidate->GetMechanic() != -1)
                continue;

            if (isM3Candidate(candidate))
            {
                m3Candidates.push_back(candidate);
            }
        }
    }


    // m4 can use every remaining region
    if (M4)
    {
        countSelectedMechanics++;

        for (auto* region : availableRegions)
        {
            if (isM4Candidate(region))
            {
                m4Candidates.push_back(region);
            }
        }
    }


    // m5 only accepts even rectangular regions
    if (M5)
    {
        countSelectedMechanics++;

        for (auto* region : availableRegions)
        {
            if (region->IsEvenRectangle())
            {
                m5Candidates.push_back(region);
            }
        }
    }


    // ------------------------------------------------
    // Validate candidates
    // ------------------------------------------------

    if (M2 && m2Candidates.empty())
        return false;

    if (M3 && m3Candidates.empty())
        return false;

    if (M4 && m4Candidates.empty())
        return false;

    if (M5 && m5Candidates.empty())
        return false;


    // No m2-m5 requested.
    if (countSelectedMechanics == 0)
    {
        return availableRegions.empty();
    }


    // ------------------------------------------------
    // Assign m2 / m3 / m4 / m5
    // ------------------------------------------------

    std::vector<int> validIndeces;
    std::vector<int> selected;

    for (auto* region : availableRegions)
    {
        validIndeces.clear();

        if (selected.size() ==
            static_cast<size_t>(countSelectedMechanics))
        {
            selected.clear();
        }


        // ----------------
        // m5
        // ----------------

        if (M5 &&
            std::find(
                selected.begin(),
                selected.end(),
                m.at(5)
            ) == selected.end() &&
            std::find(
                m5Candidates.begin(),
                m5Candidates.end(),
                region
            ) != m5Candidates.end())
        {
            validIndeces.push_back(m.at(5));
        }


        // ----------------
        // m2
        // ----------------

        if (M2 &&
            std::find(
                selected.begin(),
                selected.end(),
                m.at(2)
            ) == selected.end() &&
            std::find(
                m2Candidates.begin(),
                m2Candidates.end(),
                region
            ) != m2Candidates.end())
        {
            validIndeces.push_back(m.at(2));
        }


        // ----------------
        // m3
        // ----------------

        if (M3 &&
            std::find(
                selected.begin(),
                selected.end(),
                m.at(3)
            ) == selected.end() &&
            std::find(
                m3Candidates.begin(),
                m3Candidates.end(),
                region
            ) != m3Candidates.end())
        {
            validIndeces.push_back(m.at(3));
        }


        // ----------------
        // m4
        // ----------------

        if (M4 &&
            std::find(
                selected.begin(),
                selected.end(),
                m.at(4)
            ) == selected.end() &&
            std::find(
                m4Candidates.begin(),
                m4Candidates.end(),
                region
            ) != m4Candidates.end())
        {
            validIndeces.push_back(m.at(4));
        }


        // No mechanic can be assigned to this region.
        if (validIndeces.empty())
        {
            return false;
        }


        int index =
            randomInt(
                0,
                static_cast<int>(validIndeces.size()) - 1
            );

        int selectedMechanic =
            validIndeces.at(index);

        selected.push_back(selectedMechanic);

        region->SetMechanic(selectedMechanic);
    }


    // ------------------------------------------------
    // Make sure every enabled mechanic exists
    // ------------------------------------------------

    bool m2Valid = !M2;
    bool m3Valid = !M3;
    bool m4Valid = !M4;
    bool m5Valid = !M5;

    int m2Area = 0;
    int m3Area = 0;
    int m4Area = 0;
    int m5Area = 0;

    for (auto* region : regions)
    {
        int mechanic =
            region->GetMechanic();

        if (mechanic == m.at(2))
            m2Area += region->Area();

        if (mechanic == m.at(3))
            m3Area += region->Area();

        if (mechanic == m.at(4))
            m4Area += region->Area();

        if (mechanic == m.at(5))
            m5Area += region->Area();
    }

    if (M2 && m2Area > smallestM2Area)
        m2Valid = true;

    if (M3 && m3Area > smallestM3Area)
        m3Valid = true;

    if (M4 && m4Area > smallestM4Area)
        m4Valid = true;

    if (M5 && m5Area > smallestM5Area)
        m5Valid = true;


    return
        m2Valid &&
        m3Valid &&
        m4Valid &&
        m5Valid;
}

void Board::LevelGenerate(int RectangleCount, int ColorCount, bool M2, bool M3, bool M4, bool M5)
{
    resetRectangle();

    for (int n = 0; n < RectangleCount - 1; n++)
    {
        for (int i = 0; i < tryLimit; i++)
        {
            Rectangle* newRecangle = rectangles.at(randomInt(0, rectangles.size() - 1))->Split();
            if (newRecangle != nullptr)
            {
                rectangles.push_back(newRecangle);
                break;
            }
        }
        for (auto* rectangle : rectangles)
        {
            rectangle->FindNeigbers();
        }
    }

    mesh(ColorCount);

    int i{0};

    for (; i < tryLimit; i++)
    {
        if (paint(M2, M3, M4, M5)) break;
    }

    if (i == tryLimit) LevelGenerate(RectangleCount, ColorCount, M2, M3, M4, M5);
}

void Board::TestAllRectangleNeighbors()
{
    bool allCorrect = true;
    int rectangleIndex = 0;

    for (auto* region : regions)
    {
        for (auto* rectangle : region->GetConvex())
        {
            std::cout
                << "\n\n######## RECTANGLE "
                << rectangleIndex
                << " ########\n";

            bool result =
                rectangle->TestNeighbors();

            if (!result)
                allCorrect = false;

            rectangleIndex++;
        }
    }

    std::cout
        << "\n\n====================================\n";

    if (allCorrect)
    {
        std::cout
            << "ALL RECTANGLE NEIGHBORS ARE CORRECT\n";
    }
    else
    {
        std::cout
            << "SOME RECTANGLE NEIGHBORS ARE WRONG\n";
    }

    std::cout
        << "====================================\n";
}

bool Board::isM2Candidate(Region* region)
{
    for (auto* rectangle : region->GetConvex())
    {
        for (int i = 0; i < rectangle->CountTopNeighbors(); i++)
        {
            Rectangle* neighbor =
                rectangle->GetTopNeighbor(i);

            // Ignore another rectangle belonging to same region.
            if (neighbor->GetRegion() == region)
                continue;

            int mechanic =
                neighbor->GetMechanic();

            // m0, m1 and m2 are valid.
            if (mechanic != 0 &&
                mechanic != 1 &&
                mechanic != 2)
            {
                return false;
            }
        }
    }

    // No external top neighbors is also valid.
    return true;
}

bool Board::isM4Candidate(Region* region)
{
    bool leftValid = true;
    bool rightValid = true;

    // --------------------------------
    // Check LEFT side
    // --------------------------------

    for (auto* rectangle : region->GetConvex())
    {
        for (int i = 0; i < rectangle->CountLeftNeighbors(); i++)
        {
            Rectangle* neighbor =
                rectangle->GetLeftNeighbor(i);

            // Ignore another rectangle inside the same region.
            if (neighbor->GetRegion() == region)
                continue;

            int mechanic =
                neighbor->GetMechanic();

            // Only m0 or m4 can be on the left.
            if (mechanic != 0 &&
                mechanic != 4)
            {
                leftValid = false;
                break;
            }
        }

        if (!leftValid)
            break;
    }


    // --------------------------------
    // Check RIGHT side
    //
    // We don't store right neighbors.
    // If another rectangle has one of our
    // rectangles as its LEFT neighbor,
    // then that rectangle is on our RIGHT.
    // --------------------------------

    for (auto* otherRegion : regions)
    {
        if (otherRegion == region)
            continue;

        for (auto* otherRectangle : otherRegion->GetConvex())
        {
            bool isRightNeighbor = false;

            for (int i = 0;
                i < otherRectangle->CountLeftNeighbors();
                i++)
            {
                Rectangle* leftNeighbor =
                    otherRectangle->GetLeftNeighbor(i);

                if (leftNeighbor->GetRegion() == region)
                {
                    isRightNeighbor = true;
                    break;
                }
            }

            if (!isRightNeighbor)
                continue;

            int mechanic =
                otherRectangle->GetMechanic();

            // Only m0 or m4 can be on the right.
            if (mechanic != 0 &&
                mechanic != 4)
            {
                rightValid = false;
                break;
            }
        }

        if (!rightValid)
            break;
    }


    // At least one side must be valid.
    return leftValid || rightValid;
}

bool Board::isM3Candidate(Region* region)
{
    for (auto* rectangle : region->GetConvex())
    {
        for (int i = 0; i < rectangle->CountTopNeighbors(); i++)
        {
            Rectangle* neighbor =
                rectangle->GetTopNeighbor(i);

            // Ignore rectangles inside the same region.
            if (neighbor->GetRegion() == region)
                continue;

            int mechanic =
                neighbor->GetMechanic();

            // Valid mechanics above/below in your naming convention:
            // m0, m1, m2, m3, m5
            if (mechanic != 0 &&
                mechanic != 1 &&
                mechanic != 2 &&
                mechanic != 3 &&
                mechanic != 5)
            {
                return false;
            }
        }
    }

    // No external top neighbors is also valid.
    return true;
}
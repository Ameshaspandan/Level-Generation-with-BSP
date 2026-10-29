#include "Board.h"
#include <iomanip>
#include <functional>
#include <stdexcept>
#include <random>

Board::Board(int Width, int Height) :
    height{ Height }, width{ Width }, rectangles{}, cells{}, minHeight{ 1 }, minWidth{ 1 }, rng{ std::random_device{}() }, regions{}
{
    if (!IsSupportedSize(Width, Height)) throw std::invalid_argument("Supported sizes: 9x9, 9x10, 9x11, 8x11.");
    Rectangle* baseRectangle = new Rectangle(Width, Height);
    rectangles.push_back(baseRectangle);

    cells = *baseRectangle->getCells();
}

Board::~Board()
{
    for (auto* region : regions) delete region;
    for (int n = 0; n < rectangles.size(); n++)
    {
        delete rectangles.at(n);
    }

}

Cell* const Board::getCell(int X, int Y) const
{
	return cells.at(X + Y * width);
}

//std::ostream& operator<<(std::ostream& Output, const Board& board)
//{
//    const int width = board.width;
//    const int height = board.height;
//
//    if (width < 1 || width > 9 ||
//        height < 1 || height > 9)
//    {
//        throw std::runtime_error(
//            "Compressed board format only supports width/height from 1 to 9."
//        );
//    }
//
//    // Sort pointers without modifying board.rectangles.
//    std::vector<Rectangle*> rectangles = board.rectangles;
//
//    std::sort(
//        rectangles.begin(),
//        rectangles.end(),
//        [](const Rectangle* a, const Rectangle* b)
//        {
//            const Cell* cellA = a->BaseCell();
//            const Cell* cellB = b->BaseCell();
//
//            if (cellA->Y() != cellB->Y())
//                return cellA->Y() < cellB->Y();
//
//            return cellA->X() < cellB->X();
//        }
//    );
//
//    // Encode all rectangles for one mechanic layer.
//    auto encodeLayer = [&](auto getLayerValue) -> std::string
//        {
//            std::string result;
//            result.reserve(rectangles.size() * 3);
//
//            for (const Rectangle* rectangle : rectangles)
//            {
//                const std::string value = getLayerValue(rectangle);
//
//                if (value.size() != 1)
//                {
//                    throw std::runtime_error(
//                        "Layer value must contain exactly one character."
//                    );
//                }
//
//                const int rectWidth = rectangle->Width();
//                const int rectHeight = rectangle->Height();
//
//                if (rectWidth < 1 || rectWidth > 9 ||
//                    rectHeight < 1 || rectHeight > 9)
//                {
//                    throw std::runtime_error(
//                        "Compressed rectangle dimensions must be from 1 to 9."
//                    );
//                }
//
//                result += value[0];
//                result += static_cast<char>('0' + rectWidth);
//                result += static_cast<char>('0' + rectHeight);
//            }
//
//            return result;
//        };
//
//    Output << width << 'x' << height << '/';
//
//    Output << encodeLayer(
//        [](const Rectangle* rectangle)
//        {
//            return rectangle->GetMechanicLayer0();
//        }
//    );
//
//    Output << '/';
//
//    Output << encodeLayer(
//        [](const Rectangle* rectangle)
//        {
//            return rectangle->GetMechanicLayer1();
//        }
//    );
//
//    Output << '/';
//
//    Output << encodeLayer(
//        [](const Rectangle* rectangle)
//        {
//            return rectangle->GetMechanicLayer2();
//        }
//    );
//
//    return Output;
//}

std::ostream& operator<<(std::ostream& Output, const Board& board)
{
    const int width = board.width;
    const int height = board.height;

    // Board limits:
    // width  = 1..9
    // height = 1..11
    if (width < 1 || width > 9 ||
        height < 1 || height > 11)
    {
        throw std::runtime_error(
            "Compressed board format supports "
            "width 1..9 and height 1..11."
        );
    }

    const int cellCount = width * height;


    // =========================================================
    // Encode one layer
    //
    // Format:
    //
    // mechanic + width + height
    //
    // Examples:
    //
    // A34
    //     A = mechanic
    //     3 = width
    //     4 = height
    //
    // A310
    //     A  = mechanic
    //     3  = width
    //     10 = height
    //
    // AA211
    //     AA = mechanic
    //     2  = width
    //     11 = height
    //
    // *911
    //     *  = empty
    //     9  = width
    //     11 = height
    // =========================================================

    auto encodeLayer =
        [&](auto getLayerValue) -> std::string
        {
            // -----------------------------------------------------
            // Cache mechanic of every cell.
            //
            // Mechanics internally may be:
            //
            // 0
            // A
            // B
            // C
            // D
            // E
            // AA
            // AAA
            //
            // "0" will be converted to "#" when serialized.
            // -----------------------------------------------------

            std::vector<std::string> values(
                cellCount
            );

            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    std::string value =
                        getLayerValue(
                            board.getCell(x, y)
                        );

                    if (value.empty())
                    {
                        throw std::runtime_error(
                            "Cell mechanic cannot be empty."
                        );
                    }

                    // Validate mechanic.
                    //if (value != "0" &&
                    //    value != "#" &&
                    //    value != "A" &&
                    //    value != "B" &&
                    //    value != "C" &&
                    //    value != "D" &&
                    //    value != "E" &&
                    //    value != "AA" &&
                    //    value != "AAA")
                    //{
                    //    throw std::runtime_error(
                    //        "Invalid mechanic in board: " +
                    //        value
                    //    );
                    //}

                    values[
                        y * width + x
                    ] = value;
                }
            }


            // -----------------------------------------------------
            // Keeps track of cells already represented by
            // rectangles.
            // -----------------------------------------------------

            std::vector<unsigned char> used(
                cellCount,
                0
            );


            std::string result;

            // Estimate only.
            result.reserve(
                cellCount * 5
            );


            int cursor = 0;


            while (cursor < cellCount)
            {
                // -------------------------------------------------
                // Find first unused cell.
                //
                // Row-major:
                //
                // left -> right
                // top  -> bottom
                // -------------------------------------------------

                while (
                    cursor < cellCount &&
                    used[cursor]
                    )
                {
                    ++cursor;
                }


                if (cursor == cellCount)
                    break;


                const int startX =
                    cursor % width;

                const int startY =
                    cursor / width;


                const std::string& value =
                    values[cursor];


                // -------------------------------------------------
                // Find largest rectangle of the same mechanic
                // beginning at this first unused cell.
                // -------------------------------------------------

                int bestWidth = 1;
                int bestHeight = 1;
                int bestArea = 1;


                int maxWidth =
                    width - startX;


                for (
                    int rectHeight = 1;
                    startY + rectHeight <= height;
                    ++rectHeight
                    )
                {
                    const int rowStart =
                        (
                            startY +
                            rectHeight -
                            1
                            )
                        * width
                        + startX;


                    int rowWidth = 0;


                    while (
                        rowWidth < maxWidth
                        )
                    {
                        const int index =
                            rowStart +
                            rowWidth;


                        if (
                            used[index] ||
                            values[index] != value
                            )
                        {
                            break;
                        }


                        ++rowWidth;
                    }


                    // Any further row cannot make the
                    // rectangle wider than this.
                    maxWidth = rowWidth;


                    if (maxWidth == 0)
                        break;


                    const int area =
                        maxWidth *
                        rectHeight;


                    if (area > bestArea)
                    {
                        bestArea =
                            area;

                        bestWidth =
                            maxWidth;

                        bestHeight =
                            rectHeight;
                    }
                }


                // -------------------------------------------------
                // Mark rectangle as used.
                // -------------------------------------------------

                for (
                    int y = startY;
                    y < startY + bestHeight;
                    ++y
                    )
                {
                    const int rowStart =
                        y * width +
                        startX;


                    for (
                        int x = 0;
                        x < bestWidth;
                        ++x
                        )
                    {
                        used[
                            rowStart + x
                        ] = 1;
                    }
                }


                // -------------------------------------------------
                // Serialize mechanic.
                //
                // Internal:
                //     0
                //
                // Stored:
                //     #
                // -------------------------------------------------
                result += value;


                // -------------------------------------------------
                // Width
                //
                // Always exactly one digit: 1..9
                // -------------------------------------------------

                result +=
                    std::to_string(
                        bestWidth
                    );


                // -------------------------------------------------
                // Height
                //
                // 1..9:
                //
                //     4
                //
                // 10..11:
                //
                //     10
                //     11
                //
                // std::to_string() gives exactly the format
                // we want.
                // -------------------------------------------------

                result +=
                    std::to_string(
                        bestHeight
                    );
            }


            return result;
        };


    // =========================================================
    // Board size
    // =========================================================

    Output
        << width
        << 'x'
        << height
        << '/';


    // =========================================================
    // Layer -1
    // =========================================================

    Output << encodeLayer(
        [](const Cell* cell)
        {
            return cell
                ->GetRectangle()
                ->GetMechanicLayer0();
        }
    );


    Output << '/';


    // =========================================================
    // Layer 0
    // =========================================================

    Output << encodeLayer(
        [](const Cell* cell)
        {
            return cell
                ->GetRectangle()
                ->GetMechanicLayer1();
        }
    );


    Output << '/';


    // =========================================================
    // Layer +1
    // =========================================================

    Output << encodeLayer(
        [](const Cell* cell)
        {
            return cell
                ->GetRectangle()
                ->GetMechanicLayer2();
        }
    );


    return Output;
}

void Board::mesh(int ColorCount)
{
    if (ColorCount < 1 || ColorCount > static_cast<int>(rectangles.size()))
        throw std::invalid_argument("Color count exceeds available rectangles.");
    rectangleSort();

    for (int i = 0; i < rectangles.size(); i++)
    {
        rectangles.at(i)->SetColor(i);
    }

    for (int n = 0; n < static_cast<int>(rectangles.size()) - ColorCount; n++)
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

    //int m = 0;
    //for (auto* rect : rectangles)
    //{
    //    rect->SetMechanicLayer1(m++);
    //}
}

void Board::findRegion()
{
    for (auto* rectangle : rectangles) rectangle->SetRegion(nullptr);
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
                region->AddRectangle(rectangle);
                rectangle->SetRegion(region);
                found = true;
                break;
            }
        }

        // No existing group for this color
        if (!found)
        {
            auto* newRegion = new Region();
            newRegion->AddRectangle(rectangle);
            rectangle->SetRegion(newRegion);
            regions.push_back(newRegion);
        }
    }
}

void Board::setColorMechanic(std::vector<Rectangle*>* ColorRectangle, std::string Mechanic)
{
    for (Rectangle* rectangle : *ColorRectangle)
    {
        rectangle->SetMechanicLayer1(Mechanic);
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

bool Board::paint(bool M0, bool M2, bool M3, bool M4, bool M5, bool Md, bool Mu)
{
    for (auto* region : regions)
    {
        region->SetMechanicLayer0(m.at(0));
        region->SetMechanicLayer1("-1");
        region->SetMechanicLayer2(m.at(0));
    }

    std::vector<Region*> availableRegions{ regions };

    // ------------------------------------------------
    // m0 -> sum of smaller Regions
    // ------------------------------------------------
    
    if (M0)
    {
        int m0Area{};
        while (m0Area < smallestM0Area)
        {
            if (availableRegions.empty())
                return false;

            int smallestRegionArea = availableRegions.at(0)->Area();
            int indexM0 = 0;

            //index starts 1 that scape smallest region
            for (int i = 1; i < static_cast<int>(availableRegions.size()); i++)
            {
                if (availableRegions.at(i)->Area() < smallestRegionArea)
                {
                    indexM0 = i;
                    smallestRegionArea = availableRegions.at(i)->Area();
                }
            }

            availableRegions.at(indexM0)->SetMechanicLayer1(m.at(0));

            m0Area += availableRegions.at(indexM0)->Area();

            availableRegions.erase(
                availableRegions.begin() + indexM0
            );
        }
    }

    //-------------------------------------------------
    //Specify Md and Mu Regions 
    //-------------------------------------------------

    std::vector<Region*> upDownLayer{ availableRegions };

    // ------------------------------------------------
    // m1
    // ------------------------------------------------

    int m1Area = 0;

    while (m1Area < smallestM1Area)
    {
        std::vector<Region*> m1Candidates;

        for (auto* region : availableRegions)
        {
            if (m1Area + region->Area() > largestM1Area)
                continue;

            bool valid = true;

            for (auto* rectangle : region->GetConvex())
            {
                for (int i = 0;
                    i < rectangle->CountTopNeighbors();
                    ++i)
                {
                    Rectangle* neighbor =
                        rectangle->GetTopNeighbor(i);

                    if (neighbor->GetRegion() == region)
                        continue;

                    std::string mechanic =
                        neighbor->GetMechanicLayer1();

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
                m1Candidates.push_back(region);
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

        selectedRegion->SetMechanicLayer1(m.at(1));

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

    if (m1Area > largestM1Area) 
        return false;

    // Bounded constraint search permits mechanics to repeat and checks
    // neighboring assignments as they become known.
    std::vector<int> enabled;
    if (M2) enabled.push_back(2);
    if (M3) enabled.push_back(3);
    if (M4) enabled.push_back(4);
    if (M5) enabled.push_back(5);
    const int minimum[] = {0, 0, smallestM2Area, smallestM3Area, smallestM4Area, smallestM5Area};
    int assignedArea[6] = {};
    int nodes = 0;
    auto partialValid = [&]() {
        for (auto* region : regions) {
            const auto mechanic = region->GetMechanicLayer1();
            if (mechanic == m[2] || mechanic == m[3] || mechanic == m[5]) {
                if (mechanic == m[5] && !region->IsEvenRectangle()) return false;
                for (auto* rectangle : region->GetConvex())
                    for (int i=0; i<rectangle->CountTopNeighbors(); ++i) {
                        auto* neighbor = rectangle->GetTopNeighbor(i);
                        if (neighbor->GetRegion() == region) continue;
                        const auto top = neighbor->GetMechanicLayer1();
                        if (top == "-1" || top == m[0] || top == m[1] || top == m[2]) continue;
                        if (mechanic != m[2] && (top == m[3] || top == m[5])) continue;
                        return false;
                    }
            }
            if (mechanic == m[4]) {
                bool left = true, right = true;
                auto allowed = [&](Rectangle* r) {
                    auto value = r->GetMechanicLayer1();
                    return value == "-1" || value == m[0] || value == m[4];
                };
                for (auto* rectangle : region->GetConvex())
                    for (int i=0; i<rectangle->CountLeftNeighbors(); ++i) {
                        auto* neighbor = rectangle->GetLeftNeighbor(i);
                        if (neighbor->GetRegion() != region && !allowed(neighbor)) left = false;
                    }
                for (auto* other : rectangles) {
                    if (other->GetRegion() == region) continue;
                    for (int i=0; i<other->CountLeftNeighbors(); ++i)
                        if (other->GetLeftNeighbor(i)->GetRegion() == region && !allowed(other)) right = false;
                }
                if (!left && !right) return false;
            }
        }
        return true;
    };
    std::function<bool(size_t)> assign = [&](size_t index) {
        if (++nodes > 20000) return false;
        int remaining = 0;
        for (size_t i=index; i<availableRegions.size(); ++i) remaining += availableRegions[i]->Area();
        int deficit = 0;
        for (int mechanic : enabled) deficit += std::max(0, minimum[mechanic]-assignedArea[mechanic]);
        if (deficit > remaining) return false;
        if (index == availableRegions.size()) return deficit == 0 && partialValid();
        auto choices = enabled;
        std::shuffle(choices.begin(), choices.end(), rng);
        auto* region = availableRegions[index];
        for (int mechanic : choices) {
            region->SetMechanicLayer1(m[mechanic]);
            assignedArea[mechanic] += region->Area();
            if (partialValid() && assign(index+1)) return true;
            assignedArea[mechanic] -= region->Area();
        }
        region->SetMechanicLayer1("-1");
        return false;
    };
    if (!assign(0)) return false;

    // Subset-sum chooses overlays that satisfy their area bounds.
    auto overlay = [&](bool enabledFlag, int mechanic, int minimumArea, int maximumArea) {
        if (!enabledFlag) return true;
        std::vector<Region*> candidates;
        for (auto* region : upDownLayer)
            if (mechanic == 6 ? isMdCandidate(region) : isMuCandidate(region)) candidates.push_back(region);
        std::shuffle(candidates.begin(), candidates.end(), rng);
        std::vector<int> previous(maximumArea+1, -1), chosen(maximumArea+1, -1);
        previous[0] = 0;
        for (int i=0; i<static_cast<int>(candidates.size()); ++i) {
            int area = candidates[i]->Area();
            for (int sum=maximumArea; sum>=area; --sum)
                if (previous[sum] == -1 && previous[sum-area] != -1) {
                    previous[sum] = sum-area;
                    chosen[sum] = i;
                }
        }
        std::vector<int> totals;
        for (int sum=minimumArea; sum<=maximumArea; ++sum) if (previous[sum] != -1) totals.push_back(sum);
        if (totals.empty()) return false;
        int sum = totals[randomInt(0, static_cast<int>(totals.size())-1)];
        while (sum > 0) {
            auto* region = candidates[chosen[sum]];
            if (mechanic == 6) region->SetMechanicLayer0(m[6]);
            else region->SetMechanicLayer2(m[7]);
            sum = previous[sum];
        }
        return true;
    };
    return overlay(Md, 6, smallestMdArea, largestMdArea) &&
        overlay(Mu, 7, smallestMuArea, largestMuArea);
}

void Board::LevelGenerate(int RectangleCount, int ColorCount, bool M0, bool M2, bool M3, bool M4, bool M5, bool Md, bool Mu, bool VerticalSymmetry)
{
    const int area = width * height;
    const int requiredRegions = 1 + int(M0) + int(M2) + int(M3) + int(M4) + int(M5);
    const int requiredArea = smallestM1Area + (M0 ? smallestM0Area : 0) +
        (M2 ? smallestM2Area : 0) + (M3 ? smallestM3Area : 0) +
        (M4 ? smallestM4Area : 0) + (M5 ? smallestM5Area : 0);
    if (RectangleCount < 1 || RectangleCount > area || ColorCount < requiredRegions ||
        ColorCount > RectangleCount || requiredArea > area)
        throw std::invalid_argument("Rectangle/color counts cannot satisfy the selected mechanics.");
    if (!M0 && !M2 && !M3 && !M4 && !M5 && area > largestM1Area)
        throw std::invalid_argument("Enable a mechanic to cover cells outside M1.");
    // An even-width M5 rectangle cannot straddle an odd-width board's axis.
    if (VerticalSymmetry && width % 2 != 0 && M5 && ColorCount < requiredRegions + 1)
        throw std::invalid_argument("Vertical symmetry with M5 on a 9-column board requires an extra region for its mirror.");
    for (int attempt=0; attempt<tryLimit; ++attempt) {
        resetRectangle();
        if (VerticalSymmetry) {
            if (!splitSymmetric(RectangleCount)) continue;
            // A symmetric partition can admit many different connected region
            // groupings. Explore those before discarding its geometry.
            for (int grouping=0; grouping<16; ++grouping) {
                if (!meshSymmetric(ColorCount)) continue;
                if (paintSymmetric(M0, M2, M3, M4, M5, Md, Mu)) return;
            }
            continue;
        }
        while (static_cast<int>(rectangles.size()) < RectangleCount) {
            std::vector<Rectangle*> splittable;
            for (auto* rectangle : rectangles) if (rectangle->Area()>1) splittable.push_back(rectangle);
            if (splittable.empty()) throw std::logic_error("No rectangle can be split.");
            auto* rectangle = splittable[randomInt(0, static_cast<int>(splittable.size())-1)];
            rectangles.push_back(rectangle->Split());
        }
        for (auto* rectangle : rectangles) rectangle->FindNeigbers();
        mesh(ColorCount);
        if (paint(M0, M2, M3, M4, M5, Md, Mu)) return;
    }
    LevelGenerate(RectangleCount, ColorCount, M0, M2, M3, M4, M5, Md, Mu, VerticalSymmetry);
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

            std::string mechanic =
                neighbor->GetMechanicLayer1();

            // m0, m1 and m2 are valid.
            if (mechanic != m.at(0) &&
                mechanic != m.at(1) &&
                mechanic != m.at(2))
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

            std::string mechanic =
                neighbor->GetMechanicLayer1();

            // Only m0 or m4 can be on the left.
            if (mechanic != m.at(0) &&
                mechanic != m.at(4))
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

            std::string mechanic =
                otherRectangle->GetMechanicLayer1();

            // Only m0 or m4 can be on the right.
            if (mechanic != m.at(0) &&
                mechanic != m.at(4))
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

            std::string mechanic =
                neighbor->GetMechanicLayer1();

            // Valid mechanics top:
            // m0, m1, m2, m3, m5  
            if (mechanic != m.at(0) &&
                mechanic != m.at(1) &&
                mechanic != m.at(2) &&
                mechanic != m.at(3) &&
                mechanic != m.at(5))
            {
                return false;
            }
        }
    }

    // Color UP Layer

    // Color Down Layer

    // No external top neighbors is also valid.
    return true;
}

bool Board::isMuCandidate(Region* region)
{
    return region->GetMechanicLayer1() != m.at(1);
}

bool Board::isMdCandidate(Region* region)
{
    return region->GetMechanicLayer1() != m.at(4);
}

bool Board::splitSymmetric(int count)
{
    struct Box { int x, y, w, h; };
    std::vector<Box> boxes{{0, 0, width, height}};
    while (static_cast<int>(boxes.size()) < count) {
        std::vector<std::vector<Box>> choices;
        for (int i=0; i<static_cast<int>(boxes.size()); ++i) {
            const auto a = boxes[i];
            int mirror = -1;
            for (int j=i; j<static_cast<int>(boxes.size()); ++j)
                if (boxes[j].x == width-a.x-a.w && boxes[j].y == a.y &&
                    boxes[j].w == a.w && boxes[j].h == a.h) { mirror=j; break; }
            if (mirror < 0) continue; // This pair was already visited.
            auto add = [&](std::vector<Box> pieces) {
                std::vector<Box> next;
                for (int j=0; j<static_cast<int>(boxes.size()); ++j)
                    if (j!=i && j!=mirror) next.push_back(boxes[j]);
                next.insert(next.end(), pieces.begin(), pieces.end());
                if (static_cast<int>(next.size())<=count) choices.push_back(std::move(next));
            };
            for (int cut=1; cut<a.h; ++cut) {
                std::vector<Box> pieces{{a.x,a.y,a.w,cut},{a.x,a.y+cut,a.w,a.h-cut}};
                if (mirror!=i) {
                    const auto b=boxes[mirror];
                    pieces.push_back({b.x,b.y,b.w,cut});
                    pieces.push_back({b.x,b.y+cut,b.w,b.h-cut});
                }
                add(std::move(pieces));
            }
            if (mirror==i) {
                for (int cut=1; cut<=a.w/2; ++cut) {
                    std::vector<Box> pieces{{a.x,a.y,cut,a.h},{a.x+a.w-cut,a.y,cut,a.h}};
                    if (a.w>2*cut) pieces.push_back({a.x+cut,a.y,a.w-2*cut,a.h});
                    add(std::move(pieces));
                }
            } else {
                const auto b=boxes[mirror];
                for (int cut=1; cut<a.w; ++cut)
                    add({{a.x,a.y,cut,a.h},{a.x+cut,a.y,a.w-cut,a.h},
                         {b.x,b.y,b.w-cut,b.h},{b.x+b.w-cut,b.y,cut,b.h}});
            }
        }
        if (choices.empty()) return false;
        boxes=std::move(choices[randomInt(0,static_cast<int>(choices.size())-1)]);
    }
    auto* original=rectangles.front();
    // Board's cell index and neighbor pointers stay intact.
    original->GetCells()->clear();
    delete original;
    rectangles.clear();
    for (const auto& box:boxes) {
        auto* group=new std::vector<Cell*>;
        for (int y=box.y; y<box.y+box.h; ++y)
            for (int x=box.x; x<box.x+box.w; ++x) group->push_back(getCell(x,y));
        rectangles.push_back(new Rectangle(group));
    }
    for (auto* rectangle:rectangles) rectangle->FindNeigbers();
    return true;
}

bool Board::meshSymmetric(int count)
{
    const int n=static_cast<int>(rectangles.size());
    std::vector<int> mirror(n), labels(n);
    std::vector<std::pair<int,int>> edges;
    auto indexOf=[&](Rectangle* r) {
        return static_cast<int>(std::find(rectangles.begin(),rectangles.end(),r)-rectangles.begin());
    };
    for (int i=0;i<n;++i) {
        labels[i]=i;
        auto* r=rectangles[i];
        mirror[i]=indexOf(getCell(width-1-r->BaseCell()->X(),r->BaseCell()->Y())->GetRectangle());
        for(int j=0;j<r->CountNeighbors();++j) edges.push_back({i,indexOf(r->GetNeighbor(j))});
    }
    auto distinct=[](std::vector<int> values) {
        std::sort(values.begin(),values.end());
        return static_cast<int>(std::unique(values.begin(),values.end())-values.begin());
    };
    int remaining=n;
    while(remaining>count) {
        std::vector<std::vector<int>> choices;
        for(const auto& edge:edges) {
            auto trial=labels;
            auto unite=[&](int a,int b) {
                int from=trial[b],to=trial[a];
                for(auto& label:trial) if(label==from) label=to;
            };
            unite(edge.first,edge.second);
            unite(mirror[edge.first],mirror[edge.second]);
            int after=distinct(trial);
            if(after<remaining && after>=count) choices.push_back(std::move(trial));
        }
        if(choices.empty()) return false;
        labels=std::move(choices[randomInt(0,static_cast<int>(choices.size())-1)]);
        remaining=distinct(labels);
    }
    for(int i=0;i<n;++i) rectangles[i]->SetColor(labels[i]);
    findRegion();
    return true;
}

bool Board::paintSymmetric(bool M0,bool M2,bool M3,bool M4,bool M5,bool Md,bool Mu)
{
    // Each unit is either a self-mirrored region or two reflected regions.
    // Paired regions remain separate, connected regions in the hierarchy.
    std::vector<std::vector<Region*>> units;
    std::vector<Region*> visited;
    for(auto* region:regions) {
        region->SetMechanicLayer0(m[0]);
        region->SetMechanicLayer1("-1");
        region->SetMechanicLayer2(m[0]);
    }
    for(auto* region:regions) {
        if(std::find(visited.begin(),visited.end(),region)!=visited.end()) continue;
        auto* cell=region->GetConvex().front()->BaseCell();
        auto* mirror=getCell(width-1-cell->X(),cell->Y())->GetRectangle()->GetRegion();
        units.push_back({region});
        visited.push_back(region);
        if(mirror!=region) {units.back().push_back(mirror);visited.push_back(mirror);}
    }
    auto top=[](const std::vector<Region*>& unit) {
        int y=100;
        for(auto* region:unit) for(auto* r:region->GetConvex()) y=std::min(y,r->BaseCell()->Y());
        return y;
    };
    std::stable_sort(units.begin(),units.end(),[&](const auto& a,const auto& b){return top(a)<top(b);});
    std::vector<int> enabled{1};
    if(M0) enabled.push_back(0);
    if(M2) enabled.push_back(2);
    if(M3) enabled.push_back(3);
    if(M4) enabled.push_back(4);
    if(M5) enabled.push_back(5);
    if(units.size()<enabled.size()) return false;
    const int minimum[]={smallestM0Area,smallestM1Area,smallestM2Area,smallestM3Area,smallestM4Area,smallestM5Area};
    std::vector<int> areas;
    for(const auto& unit:units) {int area=0;for(auto* r:unit) area+=r->Area();areas.push_back(area);}
    auto set=[&](size_t index,int mechanic) {
        for(auto* region:units[index]) region->SetMechanicLayer1(mechanic<0 ? "-1" : m[mechanic]);
    };
    auto valid=[&]() {
        for(auto* region:regions) {
            const auto value=region->GetMechanicLayer1();
            if(value=="-1" || value==m[0]) continue;
            if(value==m[5] && !region->IsEvenRectangle()) return false;
            if(value==m[4]) {
                bool left=true,right=true;
                auto allowed=[&](Rectangle* r) {
                    const auto other=r->GetMechanicLayer1();
                    return other=="-1" || other==m[0] || other==m[4];
                };
                for(auto* r:region->GetConvex()) for(int i=0;i<r->CountLeftNeighbors();++i) {
                    auto* neighbor=r->GetLeftNeighbor(i);
                    if(neighbor->GetRegion()!=region && !allowed(neighbor)) left=false;
                }
                for(auto* r:rectangles) if(r->GetRegion()!=region)
                    for(int i=0;i<r->CountLeftNeighbors();++i)
                        if(r->GetLeftNeighbor(i)->GetRegion()==region && !allowed(r)) right=false;
                if(!left && !right) return false;
            } else {
                for(auto* r:region->GetConvex()) for(int i=0;i<r->CountTopNeighbors();++i) {
                    auto* neighbor=r->GetTopNeighbor(i);
                    if(neighbor->GetRegion()==region) continue;
                    const auto other=neighbor->GetMechanicLayer1();
                    if(other=="-1" || other==m[0] || other==m[1]) continue;
                    if(value!=m[1] && other==m[2]) continue;
                    if((value==m[3] || value==m[5]) && (other==m[3] || other==m[5])) continue;
                    return false;
                }
            }
        }
        return true;
    };
    // Overlay subsets also operate on whole reflection pairs.
    auto overlay=[&](bool use,int mechanic,int lower,int upper) {
        if(!use) return true;
        std::vector<int> candidates;
        for(int i=0;i<static_cast<int>(units.size());++i) {
            bool accepts=true;
            for(auto* region:units[i])
                if(region->GetMechanicLayer1()==m[0] ||
                    !(mechanic==6 ? isMdCandidate(region) : isMuCandidate(region))) accepts=false;
            if(accepts) candidates.push_back(i);
        }
        std::shuffle(candidates.begin(),candidates.end(),rng);
        std::vector<int> previous(upper+1,-1),chosen(upper+1,-1);
        previous[0]=0;
        for(int i:candidates) for(int sum=upper;sum>=areas[i];--sum)
            if(previous[sum]<0 && previous[sum-areas[i]]>=0) {previous[sum]=sum-areas[i];chosen[sum]=i;}
        std::vector<int> totals;
        for(int sum=lower;sum<=upper;++sum) if(previous[sum]>=0) totals.push_back(sum);
        if(totals.empty()) return false;
        int sum=totals[randomInt(0,static_cast<int>(totals.size())-1)];
        while(sum>0) {
            for(auto* region:units[chosen[sum]])
                if(mechanic==6) region->SetMechanicLayer0(m[6]); else region->SetMechanicLayer2(m[7]);
            sum=previous[sum];
        }
        return true;
    };
    int assigned[6]={},nodes=0;
    std::function<bool(size_t)> search=[&](size_t index) {
        if(++nodes>20000) return false;
        int remaining=0,deficit=0,missing=0;
        for(size_t i=index;i<areas.size();++i) remaining+=areas[i];
        for(int mechanic:enabled) {
            deficit+=std::max(0,minimum[mechanic]-assigned[mechanic]);
            if(assigned[mechanic]==0) ++missing;
        }
        if(deficit>remaining || missing>static_cast<int>(units.size()-index)) return false;
        if(index==units.size()) {
            if(deficit!=0 || !valid()) return false;
            for(auto* region:regions) {region->SetMechanicLayer0(m[0]);region->SetMechanicLayer2(m[0]);}
            return overlay(Md,6,smallestMdArea,largestMdArea) && overlay(Mu,7,smallestMuArea,largestMuArea);
        }
        auto choices=enabled;
        std::shuffle(choices.begin(),choices.end(),rng);
        for(int mechanic:choices) {
            if(mechanic==1 && assigned[1]+areas[index]>largestM1Area) continue;
            set(index,mechanic);assigned[mechanic]+=areas[index];
            if(valid() && search(index+1)) return true;
            assigned[mechanic]-=areas[index];
        }
        set(index,-1);
        return false;
    };
    return search(0);
}



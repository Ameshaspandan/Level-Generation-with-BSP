#pragma once

#include <random>
#include <string>
#include <vector>
#include <stdexcept>

class Random
{
public:
	Random(unsigned seed = std::random_device{}());

	int randomInt(int min, int max)
	{
		std::uniform_int_distribution<int> dist(min, max);
		return dist(rng);
	}
private:
	std::mt19937 rng;
};

Random::Random(unsigned seed) : rng{ seed }
{
}

using Layer = std::vector<std::vector<char>>;

struct Layout
{
    int width;
    int height;

    Layer layerMinus1;
    Layer layer0;
    Layer layerPlus1;
};


// ------------------------------------------------------------
// Find first cell that hasn't been encoded/decoded yet.
// Scan order:
// left -> right
// top -> bottom
// ------------------------------------------------------------
bool FindFirstEmpty(
    const std::vector<std::vector<bool>>& used,
    int width,
    int height,
    int& outX,
    int& outY)
{
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (!used[y][x])
            {
                outX = x;
                outY = y;
                return true;
            }
        }
    }

    return false;
}


// ------------------------------------------------------------
// Encode one layer.
//
// Does NOT write width/height.
// They are written once by EncodeLayout().
//
// Format:
// A68B19C29D61
// ------------------------------------------------------------
std::string EncodeLayer(
    const Layer& layer,
    int width,
    int height)
{
    if (layer.size() != height)
        throw std::runtime_error("Invalid layer height");

    for (const auto& row : layer)
    {
        if (row.size() != width)
            throw std::runtime_error("Invalid layer width");
    }

    std::vector<std::vector<bool>> used(
        height,
        std::vector<bool>(width, false)
    );

    std::string result;

    while (true)
    {
        int startX;
        int startY;

        if (!FindFirstEmpty(
            used,
            width,
            height,
            startX,
            startY))
        {
            break;
        }

        const char value = layer[startY][startX];

        int bestWidth = 1;
        int bestHeight = 1;
        int bestArea = 1;

        // Try all possible rectangles starting from
        // the first unused cell.
        for (int rectWidth = 1;
            startX + rectWidth <= width;
            ++rectWidth)
        {
            for (int rectHeight = 1;
                startY + rectHeight <= height;
                ++rectHeight)
            {
                bool valid = true;

                for (int y = startY;
                    y < startY + rectHeight && valid;
                    ++y)
                {
                    for (int x = startX;
                        x < startX + rectWidth;
                        ++x)
                    {
                        if (used[y][x] ||
                            layer[y][x] != value)
                        {
                            valid = false;
                            break;
                        }
                    }
                }

                if (!valid)
                    continue;

                int area = rectWidth * rectHeight;

                if (area > bestArea)
                {
                    bestArea = area;
                    bestWidth = rectWidth;
                    bestHeight = rectHeight;
                }
            }
        }

        // Mark rectangle as used.
        for (int y = startY;
            y < startY + bestHeight;
            ++y)
        {
            for (int x = startX;
                x < startX + bestWidth;
                ++x)
            {
                used[y][x] = true;
            }
        }

        result += value;
        result += static_cast<char>('0' + bestWidth);
        result += static_cast<char>('0' + bestHeight);
    }

    return result;
}


// ------------------------------------------------------------
// Decode one layer.
// ------------------------------------------------------------
Layer DecodeLayer(
    const std::string& encoded,
    int width,
    int height)
{
    if (encoded.size() % 3 != 0)
        throw std::runtime_error(
            "Invalid encoded layer"
        );

    Layer layer(
        height,
        std::vector<char>(width, '\0')
    );

    std::vector<std::vector<bool>> used(
        height,
        std::vector<bool>(width, false)
    );

    for (size_t i = 0; i < encoded.size(); i += 3)
    {
        char value = encoded[i];

        int rectWidth =
            encoded[i + 1] - '0';

        int rectHeight =
            encoded[i + 2] - '0';

        if (rectWidth < 1 ||
            rectWidth > width ||
            rectHeight < 1 ||
            rectHeight > height)
        {
            throw std::runtime_error(
                "Invalid rectangle size"
            );
        }

        int startX;
        int startY;

        if (!FindFirstEmpty(
            used,
            width,
            height,
            startX,
            startY))
        {
            throw std::runtime_error(
                "Too many rectangles"
            );
        }

        if (startX + rectWidth > width ||
            startY + rectHeight > height)
        {
            throw std::runtime_error(
                "Rectangle outside layout"
            );
        }

        // Check overlap.
        for (int y = startY;
            y < startY + rectHeight;
            ++y)
        {
            for (int x = startX;
                x < startX + rectWidth;
                ++x)
            {
                if (used[y][x])
                {
                    throw std::runtime_error(
                        "Rectangle overlap"
                    );
                }
            }
        }

        // Fill rectangle.
        for (int y = startY;
            y < startY + rectHeight;
            ++y)
        {
            for (int x = startX;
                x < startX + rectWidth;
                ++x)
            {
                layer[y][x] = value;
                used[y][x] = true;
            }
        }
    }

    int x;
    int y;

    if (FindFirstEmpty(
        used,
        width,
        height,
        x,
        y))
    {
        throw std::runtime_error(
            "Encoded layer does not fill entire layout"
        );
    }

    return layer;
}


// ============================================================
// FUNCTION 1
//
// THREE LAYERS -> STRING
//
// Format:
//
// WH[L-1]/[L0]/[L+1]
//
// Example:
//
// 99A99/A68B19C29D61/A99
// ============================================================
std::string EncodeLayout(
    const Layer& layerMinus1,
    const Layer& layer0,
    const Layer& layerPlus1)
{
    if (layer0.empty() ||
        layer0[0].empty())
    {
        throw std::runtime_error(
            "Layout cannot be empty"
        );
    }

    int height =
        static_cast<int>(layer0.size());

    int width =
        static_cast<int>(layer0[0].size());

    // This format uses one digit for width/height.
    if (width < 1 || width > 9 ||
        height < 1 || height > 9)
    {
        throw std::runtime_error(
            "Width and height must be between 1 and 9"
        );
    }

    std::string result;

    result += static_cast<char>('0' + width);
    result += static_cast<char>('0' + height);

    result += EncodeLayer(
        layerMinus1,
        width,
        height);

    result += '/';

    result += EncodeLayer(
        layer0,
        width,
        height);

    result += '/';

    result += EncodeLayer(
        layerPlus1,
        width,
        height);

    return result;
}


// ============================================================
// FUNCTION 2
//
// STRING -> THREE LAYERS
// ============================================================
Layout DecodeLayout(
    const std::string& encoded)
{
    if (encoded.size() < 2)
    {
        throw std::runtime_error(
            "Invalid encoded layout"
        );
    }

    int width =
        encoded[0] - '0';

    int height =
        encoded[1] - '0';

    if (width < 1 || width > 9 ||
        height < 1 || height > 9)
    {
        throw std::runtime_error(
            "Invalid layout size"
        );
    }

    size_t firstSlash =
        encoded.find('/', 2);

    if (firstSlash == std::string::npos)
    {
        throw std::runtime_error(
            "Missing first '/'"
        );
    }

    size_t secondSlash =
        encoded.find(
            '/',
            firstSlash + 1);

    if (secondSlash == std::string::npos)
    {
        throw std::runtime_error(
            "Missing second '/'"
        );
    }

    if (encoded.find(
        '/',
        secondSlash + 1)
        != std::string::npos)
    {
        throw std::runtime_error(
            "Too many '/' characters"
        );
    }

    std::string minus1 =
        encoded.substr(
            2,
            firstSlash - 2);

    std::string zero =
        encoded.substr(
            firstSlash + 1,
            secondSlash - firstSlash - 1);

    std::string plus1 =
        encoded.substr(
            secondSlash + 1);

    Layout result;

    result.width = width;
    result.height = height;

    result.layerMinus1 =
        DecodeLayer(
            minus1,
            width,
            height);

    result.layer0 =
        DecodeLayer(
            zero,
            width,
            height);

    result.layerPlus1 =
        DecodeLayer(
            plus1,
            width,
            height);

    return result;
}
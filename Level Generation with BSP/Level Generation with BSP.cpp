#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <atomic>
#include <thread>
#include <chrono>
#include <algorithm>
#include <stdexcept>
#include <cctype>
#include <map>

#include "Board.h"

void ShowProgress(int current, int total)
{
    constexpr int barWidth = 50;

    if (total <= 0)
        return;

    current = std::clamp(current, 0, total);

    float progress = static_cast<float>(current) / total;
    int position = static_cast<int>(barWidth * progress);

    std::cout << "\r[";

    for (int i = 0; i < barWidth; ++i)
    {
        if (i < position)
            std::cout << '=';
        else if (i == position)
            std::cout << '>';
        else
            std::cout << ' ';
    }

    std::cout << "] "
        << static_cast<int>(progress * 100.0f)
        << "% (" << current << "/" << total << ")"
        << std::flush;

    if (current == total)
        std::cout << '\n';
}

// Include your Board header here.
// #include "Board.h"

// Keep your existing ShowProgress() declaration here.

bool ParseBool(const char* arg)
{
    const std::string value = arg;

    if (value == "true" || value == "1")
        return true;

    if (value == "false" || value == "0")
        return false;

    throw std::invalid_argument(
        "Boolean arguments must be true, false, 1, or 0."
    );
}

int ParseInt(const char* arg)
{
    std::size_t pos = 0;
    const std::string value = arg;
    const int result = std::stoi(value, &pos);

    if (pos != value.size())
        throw std::invalid_argument("Invalid integer argument.");

    return result;
}

// Reads a simple INI file.
// Supports sections, key=value pairs, and comments beginning with # or ;.
std::map<std::string, std::string> LoadIni(
    const std::string& fileName)
{
    std::ifstream file(fileName);

    if (!file.is_open())
        throw std::runtime_error(
            "Cannot open configuration file: " + fileName);

    std::map<std::string, std::string> values;
    std::string line;
    std::string section;

    while (std::getline(file, line))
    {
        // Trim leading and trailing whitespace.
        const auto first = line.find_first_not_of(" \t\r\n");

        if (first == std::string::npos)
            continue;

        const auto last = line.find_last_not_of(" \t\r\n");
        line = line.substr(first, last - first + 1);

        if (line[0] == '#' || line[0] == ';')
            continue;

        if (line.front() == '[' && line.back() == ']')
        {
            section = line.substr(1, line.size() - 2);
            continue;
        }

        const auto equals = line.find('=');

        if (equals == std::string::npos)
            continue;

        std::string key = line.substr(0, equals);
        std::string value = line.substr(equals + 1);

        auto trim = [](std::string& s)
            {
                const auto begin = s.find_first_not_of(" \t\r\n");
                if (begin == std::string::npos)
                {
                    s.clear();
                    return;
                }

                const auto end = s.find_last_not_of(" \t\r\n");
                s = s.substr(begin, end - begin + 1);
            };

        trim(key);
        trim(value);

        if (key.empty())
            continue;

        // Store keys using their section, e.g. Generation.width.
        values[section.empty() ? key : section + "." + key] = value;
    }

    return values;
}

int ReadInt(
    const std::map<std::string, std::string>& config,
    const std::string& key)
{
    const auto it = config.find(key);

    if (it == config.end())
        throw std::runtime_error("Missing configuration key: " + key);

    std::size_t pos = 0;
    const int result = std::stoi(it->second, &pos);

    if (pos != it->second.size())
        throw std::runtime_error("Invalid integer for: " + key);

    return result;
}

bool ReadBool(
    const std::map<std::string, std::string>& config,
    const std::string& key)
{
    const auto it = config.find(key);

    if (it == config.end())
        throw std::runtime_error("Missing configuration key: " + key);

    std::string value = it->second;

    std::transform(
        value.begin(), value.end(), value.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (value == "true" || value == "1")
        return true;

    if (value == "false" || value == "0")
        return false;

    throw std::runtime_error("Invalid boolean for: " + key);
}

int main(int argc, char* argv[])
{
    // Default configuration file.
    // Optionally accept a custom path as the first argument.
    if (argc > 2)
    {
        std::cerr
            << "Usage: " << argv[0]
            << " [config-file.ini]\n";
        return 1;
    }

    const std::string configFile =
        (argc == 2) ? argv[1] : "generator.ini";

    int levelCount;
    int boardWidth;
    int boardHeight;
    int rectangleCount;
    int colorCount;

    bool M0, M2, M3, M4, M5, Md, Mu;

    try
    {
        const auto config = LoadIni(configFile);

        levelCount = ReadInt(config, "Generation.levelCount");
        boardWidth = ReadInt(config, "Generation.width");
        boardHeight = ReadInt(config, "Generation.height");
        rectangleCount = ReadInt(config, "Generation.rectangleCount");
        colorCount = ReadInt(config, "Generation.colorCount");

        M0 = ReadBool(config, "Mechanics.M0");
        M2 = ReadBool(config, "Mechanics.M2");
        M3 = ReadBool(config, "Mechanics.M3");
        M4 = ReadBool(config, "Mechanics.M4");
        M5 = ReadBool(config, "Mechanics.M5");
        Md = ReadBool(config, "Mechanics.Md");
        Mu = ReadBool(config, "Mechanics.Mu");
    }
    catch (const std::exception& e)
    {
        std::cerr << "Configuration error: "
            << e.what() << '\n';
        return 1;
    }

    if (levelCount <= 0 ||
        boardWidth <= 0 ||
        boardHeight <= 0 ||
        rectangleCount <= 0 ||
        colorCount <= 0)
    {
        std::cerr
            << "All numeric configuration values must be positive.\n";
        return 1;
    }

    std::cout << "Configuration: " << configFile << '\n';
    std::cout << "Levels to generate: " << levelCount << '\n';
    std::cout << "Board size: "
        << boardWidth << 'x' << boardHeight << '\n';

    const std::string fileName = "layouts.csv";

    // 1. Load existing layouts.

    std::unordered_set<std::string> existingLayouts;
    int layoutCount = 0;

    {
        std::ifstream file(fileName);
        std::string line;

        while (std::getline(file, line))
        {
            if (line.rfind("Layout ", 0) == 0)
            {
                std::string layout;

                if (std::getline(file, layout))
                {
                    existingLayouts.insert(layout);
                    ++layoutCount;
                }
            }
        }
    }

    std::cout << "Existing layouts: " << layoutCount << '\n';
    std::cout << "Generating " << levelCount << " layouts...\n";

    // 2. Generate layouts in parallel.

    std::vector<std::string> results(levelCount);

    std::atomic<int> nextIndex{ 0 };
    std::atomic<int> completed{ 0 };

    const unsigned int hardwareThreads =
        std::max(1u, std::thread::hardware_concurrency());

    const int workerCount =
        std::min(levelCount, static_cast<int>(hardwareThreads));

    std::vector<std::thread> threads;
    threads.reserve(workerCount);

    for (int i = 0; i < workerCount; ++i)
    {
        threads.emplace_back([&]()
            {
                while (true)
                {
                    const int index =
                        nextIndex.fetch_add(1, std::memory_order_relaxed);

                    if (index >= levelCount)
                        break;

                    Board board{ boardWidth, boardHeight };

                    board.LevelGenerate(
                        rectangleCount,
                        colorCount,
                        M0, M2, M3, M4, M5, Md, Mu
                    );

                    std::ostringstream stream;
                    stream << board;

                    results[index] = stream.str();

                    completed.fetch_add(1, std::memory_order_relaxed);
                }
            });
    }

    // 3. Show generation progress.

    int lastProgress = -1;

    while (completed.load(std::memory_order_relaxed) < levelCount)
    {
        const int progress =
            completed.load(std::memory_order_relaxed);

        if (progress != lastProgress)
        {
            ShowProgress(progress, levelCount);
            lastProgress = progress;
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(50)
        );
    }

    for (auto& t : threads)
        t.join();

    ShowProgress(levelCount, levelCount);

    // 4. Append only unique layouts.

    std::ofstream file(fileName, std::ios::app);

    if (!file.is_open())
    {
        std::cerr << "\nFailed to open file for appending.\n";
        return 1;
    }

    int addedCount = 0;

    for (const auto& layout : results)
    {
        if (existingLayouts.insert(layout).second)
        {
            file << "Layout " << ++layoutCount << '\n';
            file << layout << '\n';

            ++addedCount;
        }
    }

    file.close();

    if (!file)
    {
        std::cerr << "\nFailed to write layouts.csv.\n";
        return 1;
    }

    std::cout << "\nGeneration completed.\n";
    std::cout << "Generated candidates: " << levelCount << '\n';
    std::cout << "New unique layouts added: " << addedCount << '\n';
    std::cout << "Duplicates skipped: "
        << levelCount - addedCount << '\n';

    return 0;
}
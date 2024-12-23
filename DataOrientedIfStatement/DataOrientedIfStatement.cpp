#include <iostream>
#include <windows.h>
#include <string>
#include <thread>
#include <vector>
#include <functional>

enum Season
{
    spring = 0,
    summer = 1,
    autumn = 2,
    winter = 3
};

struct Data {
    std::string id;
    Season season;
    bool seen;
};

std::string getGUID()
{
    std::string result{};
    GUID guid;
    CoCreateGuid(&guid);
    char guidStr[37];
    sprintf_s(
        guidStr,
        "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
        guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    result = guidStr;

    return result;
}

std::vector<Data> generateData(const int dataLength) {
    std::cout << "Generating data...\n";
    std::vector<Data> data(dataLength);

    const char threadCount = 12;
    std::thread threads[threadCount];

    for (int threadIndex = 0; threadIndex < threadCount; threadIndex++) {
        int dataIndex = (dataLength / threadCount) * threadIndex;
        int runs = dataLength / threadCount;
        threads[threadIndex] = std::thread([&data, dataIndex, runs] {
            for (int currentDataIndex = dataIndex; currentDataIndex < dataIndex + runs; currentDataIndex++)
            {
                data[currentDataIndex] = Data{ getGUID(), Season((rand() % 3)), currentDataIndex % 10 == 0 };
            }
            });
    }

    for (int threadIndex = 0; threadIndex < threadCount; threadIndex++) {
        threads[threadIndex].join();
    }

    std::cout << "Generating data done!\n";

    return data;
}

void run(std::vector<Data> data, std::function<int(std::vector<Data>&)> runFunction) {
    auto start = std::chrono::high_resolution_clock::now();

    int foundCount = runFunction(data);

    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << "Found: " << foundCount << "\n";

    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    std::cout << microseconds.count() << "µs\n";
    std::cout << microseconds.count() / 1000 << "ms\n";
    std::cout << microseconds.count() / 1000000 << "s\n";
}

int runWithANormalIfStatement(std::vector<Data>& data) {
    int foundCount = 0;

    for (int dataIndex = 0; dataIndex < data.size(); dataIndex++)
    {
        if (data[dataIndex].id.find('A') != std::string::npos && data[dataIndex].season == Season::autumn && data[dataIndex].seen) {
            foundCount++;
        }
    }

    return foundCount;
}

int runWithADataOrientedIfStatement(std::vector<Data>& data) {
    int foundCount = 0;

    for (int dataIndex = 0; dataIndex < data.size(); dataIndex++)
    {
        if (data[dataIndex].seen && data[dataIndex].season == Season::autumn && data[dataIndex].id.find('A') != std::string::npos) {
            foundCount++;
        }
    }

    return foundCount;
}

int main()
{
    std::cout << "Data oriented if statement!\n";

    std::vector<Data> data = generateData(10000000);

    std::cout << "\n\nRun with a normal if statement:\n";
    run(data, runWithANormalIfStatement);

    std::cout << "\n\nRun with a data oriented if statement:\n";
    run(data, runWithADataOrientedIfStatement);
}

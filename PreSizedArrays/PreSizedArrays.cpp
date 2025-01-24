#include <iostream>
#include <windows.h>
#include <string>
#include <thread>
#include <vector>
#include <functional>
#include <list>

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

void run(const int dataLength, std::function<int(const int)> runFunction) {
    auto start = std::chrono::high_resolution_clock::now();

    int foundCount = runFunction(dataLength);

    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << "Data size: " << foundCount << "\n";

    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    std::cout << microseconds.count() << "µs\n";
    std::cout << microseconds.count() / 1000 << "ms\n";
    std::cout << microseconds.count() / 1000000 << "s\n";
}

int generateDataWithAList(const int dataLength) {
    std::list<Data> data{};

    for (int dataIndex = 0; dataIndex < dataLength; dataIndex++)
    {
        Data newData { getGUID(), Season((rand() % 3)), dataIndex % 10 == 0 };
        data.push_back(newData);
    }

    return data.size();
}

int generateDataWithAPreSizedArray(const int dataLength) {
    Data* data = new Data[dataLength];

    for (int dataIndex = 0; dataIndex < dataLength; dataIndex++)
    {
        data[dataIndex].id = getGUID();
        data[dataIndex].season = Season((rand() % 3));
        data[dataIndex].seen = dataIndex % 10 == 0;
    }

    delete[] data;
    return dataLength;
}

int main()
{
    std::cout << "Unlocking the Power of Pre-Sized Arrays!\n";

    const int dataLength = 8128000;

    std::cout << "\n\nGenerate data with a list:\n";
    run(dataLength, generateDataWithAList);

    std::cout << "\n\nGenerate data with a Pre-Sized array:\n";
    run(dataLength, generateDataWithAPreSizedArray);
}

#include <iostream>
#include <vector>
#include <list>
#include <random>
#include <functional>
#include <chrono>

void run(const int dataLength, std::function<void(const int)> runFunction) {
    auto start = std::chrono::high_resolution_clock::now();

    runFunction(dataLength);
    auto finish = std::chrono::high_resolution_clock::now();

    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    std::cout << microseconds.count() << "µs\n";
    std::cout << microseconds.count() / 1000 << "ms\n";
    std::cout << microseconds.count() / 1000000 << "s\n";
}

void run(std::function<void()> runFunction) {
    auto start = std::chrono::high_resolution_clock::now();

    runFunction();
    auto finish = std::chrono::high_resolution_clock::now();

    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    std::cout << microseconds.count() / 1000 << "ms ";
    std::cout << microseconds.count() << "µs\n";
}

std::vector<int> generateRandomContiguousMemoryList(int size) {
    std::vector<int> contiguousMemoryList;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 10000);

    for (int i = 0; i < size; ++i) {
        contiguousMemoryList.push_back(distrib(gen));
    }
    return contiguousMemoryList;
}

std::list<int> generateRandomDoubleLinkedList(int size) {
    std::list<int> doubleLinkedList;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 10000);

    for (int i = 0; i < size; ++i) {
        doubleLinkedList.push_back(distrib(gen));
    }
    return doubleLinkedList;
}

void sort(std::vector<int>& contiguousMemoryList) {
    std::sort(contiguousMemoryList.begin(), contiguousMemoryList.end(), std::greater<int>());
}

void sort(std::list<int>& doubleLinkedList) {
    doubleLinkedList.sort();
}

void removeThisValueRange(std::vector<int>& contiguousMemoryList, int start, int until) {
    for (int index = 0; index < contiguousMemoryList.size(); index++) {
        int value = contiguousMemoryList.at(index);

        if (value >= start && value <= until) {
            contiguousMemoryList.erase(contiguousMemoryList.begin() + index);
            index--;
        }
    }
}

void removeThisValueRange(std::list<int>& doubleLinkedList, int start, int until) {
    doubleLinkedList.remove_if([&](const int& value) -> bool {
        return value >= start && value <= until;
    });
}

void addingDataOnMod5RemoveThree(std::vector<int>& contiguousMemoryList) {
    for (int number = 20000; number <= 30000; number++) {
        contiguousMemoryList.push_back(number);
        if (number % 5 == 0) {
            //for (int toRemoveNumber = number; toRemoveNumber >= number - 3; toRemoveNumber--) {
                //contiguousMemoryList.pop_back();
                contiguousMemoryList.erase(contiguousMemoryList.begin() + contiguousMemoryList.size() - 4);
            //}
        }
    }
}

void addingDataOnMod5RemoveThree(std::list<int>& doubleLinkedList) {
    for (int number = 20000; number <= 30000; number++) {
        doubleLinkedList.push_back(number);
        if (number % 5 == 0) {
            for (int toRemoveNumber = number; toRemoveNumber >= number - 3; toRemoveNumber--) {
                doubleLinkedList.pop_back();
            }
        }
    }
}

void contiguousMemoryListDataMutations(int dataLength) {
    std::vector<int> contiguousMemoryList;

    run([&dataLength, &contiguousMemoryList]() -> void {
        std::cout << "generateRandom:               ";
        contiguousMemoryList = generateRandomContiguousMemoryList(dataLength);
    });

    run([&contiguousMemoryList]() -> void {
        std::cout << "removeThisValueRange:         ";
        removeThisValueRange(contiguousMemoryList, 2000, 3999);
    });

    run([&contiguousMemoryList]() -> void {
        std::cout << "sort:                         ";
        sort(contiguousMemoryList);
    });

    run([&contiguousMemoryList]() -> void {
        std::cout << "adding on mod 5 remove three: ";
        addingDataOnMod5RemoveThree(contiguousMemoryList);
    });

    std::cout << std::endl;
}

void doubleLinkedListDataMutations(int dataLength) {
    std::list<int> doubleLinkedList;

    run([&dataLength, &doubleLinkedList]() -> void {
        std::cout << "generateRandom:               ";
        doubleLinkedList = generateRandomDoubleLinkedList(dataLength);
    });
    
    run([&doubleLinkedList]() -> void {
        std::cout << "removeThisValueRange:         ";
        removeThisValueRange(doubleLinkedList, 2000, 3999);
    });

    run([&doubleLinkedList]() -> void {
        std::cout << "sort:                         ";
        sort(doubleLinkedList);
    });

    run([&doubleLinkedList]() -> void {
        std::cout << "adding on mod 5 remove three: ";
        addingDataOnMod5RemoveThree(doubleLinkedList);
    });

    std::cout << std::endl;
}

int main() {
    std::cout << "The art of data structure selection!\n";

    const int dataLength = 812800;

    std::cout << "\n\Data mutations with a contiguous memory list:\n";
    run(dataLength, contiguousMemoryListDataMutations);

    std::cout << "\n\Data mutations with double linked list:\n";
    run(dataLength, doubleLinkedListDataMutations);
}
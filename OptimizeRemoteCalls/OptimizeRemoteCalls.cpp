/*#include <iostream>
#include <vector>
#include <list>
#include <random>
#include <functional>
#include <chrono>

void run(std::function<void()> runFunction) {
    auto start = std::chrono::high_resolution_clock::now();

    runFunction();
    auto finish = std::chrono::high_resolution_clock::now();

    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    std::cout << microseconds.count() / 1000 << "ms ";
    std::cout << microseconds.count() << "µs\n";
}

int main() {
    std::cout << "Optimize remote calls!\n";

    std::cout << "\n\:\n";
    //run(dataLength, contiguousMemoryListDataMutations);

    std::cout << "\n\:\n";
    //run(dataLength, doubleLinkedListDataMutations);
}*/

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {
    std::string url = "http://localhost:5125/invoice";

    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        std::string readBuffer;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else {
            try {
                json j = json::parse(readBuffer);

                std::cout << "Count: " << j["count"] << std::endl;

            }
            catch (const json::exception& e) {
                std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            }
        }

        curl_easy_cleanup(curl);
    }

    return 0;
}
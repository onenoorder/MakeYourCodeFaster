#include <iostream>
#include <list>
#include <functional>
#include <chrono>
#include <format>
#include <ctime>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void run(std::function<int()> runFunction) {
    auto start = std::chrono::high_resolution_clock::now();

    int result = runFunction();
    auto finish = std::chrono::high_resolution_clock::now();

    std::cout << "Total amount: " << result << std::endl;

    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    std::cout << microseconds.count() / 1000 << "ms ";
    std::cout << microseconds.count() << "µs\n";
}

std::list<std::string> GetAllDatesOfLastYear() {
    std::list<std::string> dates{};

    tm begin_datum = { 0 };
    begin_datum.tm_year = 2024 - 1900;
    begin_datum.tm_mon = 0;
    begin_datum.tm_mday = 0;

    time_t begin_tijd = mktime(&begin_datum);

    for (int i = 0; i < 366; ++i) {
        begin_tijd += 24 * 60 * 60;

        tm* dt = new tm;
        localtime_s(dt, &begin_tijd);

        std::ostringstream stringStream;
        stringStream << dt->tm_mon + 1 << "-" << dt->tm_mday << "-" << dt->tm_year + 1900;
        std::string dateOfTheYear = stringStream.str();
        dates.push_back(dateOfTheYear);
    }

    return dates;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

struct InvoiceLine {
    int Amount;
};

struct Invoice {
    std::string Date;
    std::vector<InvoiceLine> InvoiceLines;
};

struct RemoteCallData {
    std::string FromDate;
    std::string ToDate;
    int Count;
    std::vector<Invoice> Invoices;
};

void from_json(const json& jsonInput, InvoiceLine& invoiceLine)
{
    invoiceLine.Amount = jsonInput["amount"].get<int>();
}

void from_json(const json& jsonInput, Invoice& invoice)
{
    invoice.Date = jsonInput["date"].get<std::string>();
    invoice.InvoiceLines = jsonInput["invoiceLines"].get<std::vector<InvoiceLine>>();
}

void from_json(const json& jsonInput, RemoteCallData& remoteCallData)
{
    remoteCallData.Invoices = jsonInput.get<std::vector<Invoice>>();
}

RemoteCallData ParseJson(json jsonInput) {
    RemoteCallData remoteCallData {
        jsonInput["fromDate"].template get<std::string>(),
        jsonInput["toDate"].template get<std::string>(),
        jsonInput["count"].template get<int>(),
        jsonInput["invoices"].template get<std::vector<Invoice>>()
    };

    return remoteCallData;
}

RemoteCallData RunRemoteCall(std::string url) {
    RemoteCallData remoteCallData;

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
                remoteCallData = ParseJson(json::parse(readBuffer));
                //std::cout << "Count: " << remoteCallData.Count << std::endl;
            }
            catch (const json::exception& e) {
                std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            }
        }

        curl_easy_cleanup(curl);
    }

    return remoteCallData;
}

int workingWithRemoteCalls() {
    std::string url = "https://localhost:7079/invoice/";
    int totalAmount = 0;

    for (std::string dateOfTheYear : GetAllDatesOfLastYear())
    {
        std::string newUrl = url + dateOfTheYear;
        RemoteCallData remoteCallData = RunRemoteCall(newUrl);

        for (Invoice invoice : remoteCallData.Invoices) {
            for (InvoiceLine invoiceLine : invoice.InvoiceLines) {
                totalAmount += invoiceLine.Amount;
            }
        }
    }

    return totalAmount;
}

int workingWithOptimizeRemoteCalls() {
    std::string url = "https://localhost:7079/invoice";
    int totalAmount = 0;

    RemoteCallData remoteCallData = RunRemoteCall(url);

    for (Invoice invoice : remoteCallData.Invoices) {
        for (InvoiceLine invoiceLine : invoice.InvoiceLines) {
            totalAmount += invoiceLine.Amount;
        }
    }

    return totalAmount;
}

int main() {
    std::cout << "Optimize remote calls!\n";

    std::cout << "\n\Working with remote calls:\n";
    run(workingWithRemoteCalls);

    std::cout << "\n\Working with optimize remote calls:\n";
    run(workingWithOptimizeRemoteCalls);
}
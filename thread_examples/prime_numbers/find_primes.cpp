#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include <utility>

/* Code from stackoverflow article: https://stackoverflow.com/questions/66210728/find-all-primes-in-a-range-between-two-numbers-using-a-given-amount-of-threads
   All credits to the author: https://stackoverflow.com/users/9666018/a-m
*/

// Calculate start and end index for all chunks
std::vector<std::pair<size_t, size_t>> calculatePairs(const size_t low, const size_t high, const size_t numberOfGroups) {

    // Here we will store the resulting pairs with start and end values
    std::vector<std::pair<size_t, size_t>> pairs{};

    // Calculate chung size and remainder
    const size_t delta = high+1 - low;
    const size_t chunk = delta / numberOfGroups;
    size_t remainder = delta % numberOfGroups;

    // Calculate the chunks start and end addresses for all chunks
    size_t startIndex{}, endIndex{};
    for (size_t i = 0; i < numberOfGroups; ++i) {

        // Calculate end address and distribute remainder equally
        endIndex = startIndex + chunk + (remainder ? 1 : 0) - 1;
        // Store a new pair of start and end indices
        pairs.emplace_back(startIndex, endIndex);
        // Nexct start index
        startIndex = endIndex + 1;
        // We now consumed 1 remainder
        if (remainder) --remainder;
    }
    return pairs;
}


void find_prime(const size_t low, const size_t high, std::promise<std::vector<int>>&& promisePrimes) {
    bool is_prime = true;

    std::vector<int> primes{};

    for (size_t i = low; i <= high; i++) {
        if (i == 1 || i == 0)
            continue;

        is_prime = true;

        for (size_t j = 2; j <= i / 2; ++j) {
            if (i % j == 0) {
                is_prime = false;
                break;
            }
        }

        if (is_prime) {
            primes.push_back(i);
        }
    }
    promisePrimes.set_value(std::move(primes));
}

void print(std::vector<int> const& primes) {
    for (std::vector<int>::size_type i = 0; i < primes.size(); i++) {
        std::cout << primes.at(i) << ' ';
    }
}

int main() {

    constexpr size_t low = 0, high = 100, num_threads = 4;

    
    std::vector<int> primes;
    std::vector<std::thread> threads;

    std::vector<std::pair<size_t, size_t>> pairs = calculatePairs(low, high, num_threads);

    std::vector<std::promise<std::vector<int>>> promisePrimes(num_threads);
    std::vector<std::shared_future<std::vector<int>>> futurePrimes(num_threads);

    for (size_t i = 0; i < num_threads; i++) {

        futurePrimes[i] = promisePrimes[i].get_future();

        std::thread t(find_prime, pairs[i].first, pairs[i].second, std::move(promisePrimes[i]));
        threads.push_back(std::move(t));
    }

    for (size_t i = 0; i < num_threads; i++) {
        std::vector<int> temp = futurePrimes[i].get();
        primes.insert(primes.end(), temp.begin(), temp.end());
        threads[i].join();
    }

    print(primes);
    std::cout << "\nSize: " << primes.size();
    
    return 0;
}
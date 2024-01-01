#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <random>

const int BUFFER_SIZE = 100;
const int NUM_PRODUCERS = 5;
const int NUM_CONSUMERS = 5;
const int NUM_VALUES_TO_PRODUCE = 50;

std::mutex mtx;
std::condition_variable bufferEmpty, bufferFull;

std::vector<int> buffer;
int bufferCount = 0;
int producerCount = 0;
int consumerCount = 0;

void writeToBuffer(int value, int producerId) {
    std::unique_lock<std::mutex> lock(mtx);
    buffer.push_back(value);
    ++bufferCount;
    if (bufferCount == BUFFER_SIZE) {
        bufferFull.notify_all();
    }
    std::cout << "Producer " << producerId << " produced: " << value << std::endl;
}

int readFromBuffer() {
    std::unique_lock<std::mutex> lock(mtx);
    while (bufferCount == 0) {
        bufferEmpty.wait(lock);
    }
    int value = buffer.front();
    buffer.erase(buffer.begin());
    --bufferCount;
    if (bufferCount == BUFFER_SIZE - 1) {
        bufferFull.notify_all();
    }
    return value;
}

void producerFunc(int producerId) {
    for (int i = 0; i < NUM_VALUES_TO_PRODUCE; ++i) {
        int randValue = rand() % 100 + 1; // Generate a random number between 1 and 100
        writeToBuffer(randValue, producerId);
    }
    ++producerCount;
    if (producerCount == NUM_PRODUCERS) {
        bufferFull.notify_all();
    }
}

void consumerFunc(int consumerId) {
    std::ofstream file("Numbers.txt", std::ios::app);
    if (!file.is_open()) {
        std::cout << "Error opening file!" << std::endl;
        return;
    }

    while (true) {
        int value = readFromBuffer();
        file << "Consumer " << consumerId << " consumed: " << value << std::endl;
        if (bufferCount == 0 && producerCount == NUM_PRODUCERS) {
            break;
        }
    }
    ++consumerCount;
    if (consumerCount == NUM_CONSUMERS) {
        file.close();
        bufferEmpty.notify_all();
    }
}

int main() {
    std::vector<std::thread> producers, consumers;

    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        producers.emplace_back(producerFunc, i);
    }

    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        consumers.emplace_back(consumerFunc, i);
    }

    for (auto &producer : producers) {
        producer.join();
    }

    for (auto &consumer : consumers) {
        consumer.join();
    }

    return 0;
}

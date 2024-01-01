#include "ProducerConsumer.h"
#include <iostream>
#include <fstream>
#include <random>

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
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(1, 100);

	for (int i = 0; i < NUM_VALUES_TO_PRODUCE; ++i) {
		int randValue = dis(gen); // Generate a random number between 1 and 100
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

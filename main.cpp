#include "ProducerConsumer.h"
#include <thread>

int main() {
	std::vector<std::thread> producers, consumers;

	// Create producer threads
	for (int i = 0; i < NUM_PRODUCERS; ++i) {
		producers.emplace_back(producerFunc, i);
	}

	// Create consumer threads
	for (int i = 0; i < NUM_CONSUMERS; ++i) {
		consumers.emplace_back(consumerFunc, i);
	}

	// Join producer threads
	for (auto &producer : producers) {
		producer.join();
	}

	// Join consumer threads
	for (auto &consumer : consumers) {
		consumer.join();
	}

	return 0;
}

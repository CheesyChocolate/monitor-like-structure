#ifndef PRODUCER_CONSUMER_H
#define PRODUCER_CONSUMER_H

#include <fstream>
#include <vector>
#include <mutex>
#include <condition_variable>

const int BUFFER_SIZE = 100;
const int NUM_PRODUCERS = 5;
const int NUM_CONSUMERS = 5;
const int NUM_VALUES_TO_PRODUCE = 50;

extern std::mutex mtx;
extern std::condition_variable bufferEmpty, bufferFull;
extern std::vector<int> buffer;
extern int bufferCount, producerCount, consumerCount;

void writeToBuffer(int value, int producerId);
int readFromBuffer();
void producerFunc(int producerId);
void consumerFunc(int consumerId);

#endif // PRODUCER_CONSUMER_H

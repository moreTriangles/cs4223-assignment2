#include "processor_impl.hpp"
#include "protocol.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

ProcessorImpl::ProcessorImpl(int pid, string filepath, unsigned int cacheSize,
                             unsigned int associativity, unsigned int blockSize,
                             shared_ptr<Bus> bus, shared_ptr<Protocol> protocol)
    : pid(pid), protocol(protocol), bus(bus) {
    l1Data = make_shared<Cache>(cacheSize, associativity, blockSize);
    ifstream file(filepath); // just let runtime exception throw if fail
    string line;
    unsigned int type, value;
    while (getline(file, line)) {
        stringstream ss(line);
        ss >> type;
        string s;
        ss >> s;
        if (s.starts_with("0x")) s = s.substr(2);
        stringstream(s) >> hex >> value;
        stream.push_back(make_pair(type, value));
    }
}

void ProcessorImpl::executeCycle() {
    auto pair = this->stream[streamIndex];
    unsigned int type = pair.first;
    unsigned int value = pair.second;
    switch (state) {
    case FREE:
        execute(type, value);
        break;
    case STORE:
        if (currRequest->isDone()) {
            state = FREE;
        }
        break;
    case LOAD:
        if (currRequest->isDone()) {
            state = FREE;
        }
        break;
    case NON_MEMORY:
        if (!nonMemCounter) {
            state = FREE;
        }
        break;
    case MEM_ACCESS:
        break;
    }
    cycles++;
}

void ProcessorImpl::execute(unsigned int type, unsigned int value) {
    switch (type) {
    case 0: // load
        protocol->onLoad(value, bus, l1Data);
        break;
    case 1: // store
        protocol->onStore(value, bus, l1Data);
        state = STORE;
    case 2: // non-memory instructions
        state = NON_MEMORY;
        nonMemCounter = value;
        break;
    default:
        throw logic_error("invalid instruction type");
    }
}

void ProcessorImpl::invalidateCache() {}

bool ProcessorImpl::onBusRd(unsigned int address) {
    return protocol->onBusRd(address, bus, l1Data);
}

bool ProcessorImpl::isDone() { return done; }

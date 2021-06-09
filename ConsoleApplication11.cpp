#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <time.h>
#include <memory>
#include <chrono>

struct successorListBlock {
    int value;
    struct successorListBlock* nextSuccessor;
};

struct successorListNode {
    int value;
    struct successorListNode* nextElement;
    struct successorListBlock* firstSuccessor;
};

struct successorListBlock* newBlockGenerator(int newValue) {
    struct successorListBlock* newBlock = new successorListBlock;
    newBlock->value = newValue;
    return newBlock;
}

std::vector<std::vector<int>> generateGraph(double saturation, int size) {
    int counter = 0, numberEdges = 0;
    std::vector<std::vector<int>> adjacencyMatrix(size);
    std::vector<int> slotList;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            adjacencyMatrix[i].push_back(0);
            if (i < j) {
                slotList.push_back(size * i + j);
            }
        }
    }
    counter = 0;
    random_shuffle(slotList.begin(), slotList.end());
    while (numberEdges < saturation * size * (size - 1) / 2) {
        int j = slotList[counter] % size;
        int i = slotList[counter] / size;
        if (i != j) {
            adjacencyMatrix[i][j] = 1;
            adjacencyMatrix[j][i] = 1;
            numberEdges++;
        }
        counter++;
    }
    return adjacencyMatrix;
}

struct successorListNode* generateDigraph(double saturation, int size) {
    int counter = 0, numberEdges = 0;
    std::vector<std::vector<int>> adjacencyMatrix(size);
    std::vector<int> slotList(size * size);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            adjacencyMatrix[i].push_back(0);
            slotList[size * i + j] = size * i + j;
        }
    }
    random_shuffle(slotList.begin(), slotList.end());
    while (numberEdges < saturation * size * (size - 1)) {
        int j = slotList[counter] % size;
        int i = slotList[counter] / size;
        if (i != j) {
            adjacencyMatrix[i][j] = 1;
            numberEdges++;
        }
        counter++;
    }
    struct successorListNode* beginList = new successorListNode;
    beginList->nextElement = nullptr;
    beginList->firstSuccessor = nullptr;
    struct successorListNode* currentElementNode = beginList;
    for (int i = 0; i < size; i++) {
        currentElementNode->value = i;
        currentElementNode->firstSuccessor = nullptr;
        int j = 0;
        while (j < size) {
            if (adjacencyMatrix[i][j] == 1) {
                currentElementNode->firstSuccessor = newBlockGenerator(j);
                break;
            }
            j++;
        }
        if (currentElementNode->firstSuccessor != nullptr) {
            struct successorListBlock* currentElementBlock = currentElementNode->firstSuccessor;
            for (j++; j < size; j++) {
                if (adjacencyMatrix[i][j] == 1) {
                    currentElementBlock->nextSuccessor = newBlockGenerator(j);
                    currentElementBlock = currentElementBlock->nextSuccessor;
                }
            }
            currentElementBlock->nextSuccessor = nullptr;
        }
        if (i != size - 1) {
            currentElementNode->nextElement = new successorListNode;
            currentElementNode = currentElementNode->nextElement;
        }
        else {
            currentElementNode->nextElement = nullptr;
        }
    }
    return beginList;
}

bool hamiltonCycleSearchDigraph(int node, bool* visitedTable, int size, int* nVisited, struct successorListNode* nodeList, std::vector<int>& resultCycle) {
    visitedTable[node] = true;
    *nVisited += 1;
    struct successorListNode* currentNode = nodeList;
    while (currentNode->value != node) {
        currentNode = currentNode->nextElement;
    }
    if (currentNode->firstSuccessor != nullptr) {
        successorListBlock* currentSuccessor = currentNode->firstSuccessor;
        do {
            if (currentSuccessor->value == 0 && *nVisited == size ) {
                resultCycle.push_back(node);
                return true;
            }
            if (!visitedTable[currentSuccessor->value]) {
                if (hamiltonCycleSearchDigraph(currentSuccessor->value, visitedTable, size, nVisited, nodeList, resultCycle)) {
                    resultCycle.push_back(node);
                    return true;
                }
            }
            currentSuccessor = currentSuccessor->nextSuccessor;
        } while (currentSuccessor != nullptr);
    }
    visitedTable[node] = false;
    *nVisited -= 1;
    return false;
}

bool hamiltonCycleSearchUndigraph(int node, bool* visitedTable, int size, int* nVisited, std::vector<std::vector<int>>& adjacencyMatrix, std::vector<int>& resultCycle) {
    visitedTable[node] = true;
    *nVisited += 1;
    for (int i = 0; i < size; i++) {
        if (adjacencyMatrix[node][i]) {
            if (i == 0 && *nVisited == size) {
                resultCycle.push_back(node);
                return true;
            }
            if (!visitedTable[i]) {
                if (hamiltonCycleSearchUndigraph(i, visitedTable, size, nVisited, adjacencyMatrix, resultCycle)) {
                    resultCycle.push_back(node);
                    return true;
                }
            }
        }
    }
    visitedTable[node] = false;
    *nVisited -= 1;
    return false;
}

void eulerCycleSearchUndiGraph(int node, std::vector<std::vector<int>>& adjacencyMatrix, std::vector<int>& resultCycle) {
    for (int i = 0; i < adjacencyMatrix.size(); i++) {
        if (adjacencyMatrix[node][i]) {
            adjacencyMatrix[node][i] = 0;
            adjacencyMatrix[i][node] = 0;
            eulerCycleSearchUndiGraph(i, adjacencyMatrix, resultCycle);
        }
    }
    resultCycle.push_back(node);
}

void eulerCycleSearchDigraph(int node, struct successorListNode* nodeList, std::vector<int>& resultCycle) {
    struct successorListNode* currentNode = nodeList;
    while (currentNode->value != node) {
        currentNode = currentNode->nextElement;
    }
    while (currentNode->firstSuccessor != nullptr) {
        int savedValue = currentNode->firstSuccessor->value;
        currentNode->firstSuccessor = currentNode->firstSuccessor->nextSuccessor;
        eulerCycleSearchDigraph(savedValue, nodeList, resultCycle);
    }
    resultCycle.push_back(node);
}

void deleteDigraph(struct successorListNode* digraph) {
    struct successorListNode* currentNode = digraph;
    while (currentNode->nextElement != nullptr) {
        while (currentNode->firstSuccessor != nullptr) {
            struct successorListBlock* currentBlock = currentNode->firstSuccessor->nextSuccessor;
            delete currentNode->firstSuccessor;
            currentNode->firstSuccessor = currentBlock;
        }
        struct successorListNode* newElement = currentNode->nextElement;
        delete currentNode;
        currentNode = newElement;
    }
    while (currentNode->firstSuccessor != nullptr) {
        struct successorListBlock* currentBlock = currentNode->firstSuccessor->nextSuccessor;
        delete currentNode->firstSuccessor;
        currentNode->firstSuccessor = currentBlock;
    }
    delete currentNode;
}

int main()
{
    std::fstream file;
    file.open("wyniki.txt", std::ios::out);
    srand(time(NULL));
    for (int i = 10; i < 100; i += 10) {
        for (int j = 200; j <= 1200; j += 100) {
            for (int m = 0; m < 20; m++) {
                struct successorListNode* newGraph = generateDigraph(float(i / 100), j);
                std::vector<std::vector<int>> newGraphOther = generateGraph(float(i / 100), j);

                std::vector<int> hamiltonUndigraph;
                std::vector<int> hamiltonDigraph;
                std::vector<int> eulerUndigraph;
                std::vector<int> eulerDigraph;

                int nVisited = 0;
                bool* visitedTable = new bool[j];
                for (int k = 0; k < j; k++) {
                    visitedTable[k] = false;
                }

                auto t1 = std::chrono::high_resolution_clock::now();
                hamiltonCycleSearchUndigraph(0, visitedTable, j, &nVisited, newGraphOther, hamiltonUndigraph);
                auto t2 = std::chrono::high_resolution_clock::now();
                float result = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
                std::string output = std::to_string(i) + ", " + std::to_string(j) + ", " + "HAMILTON - UNDIGRAPH, " + std::to_string(result) + "\n";
                file << output;
                std::cout << i << " " << j << " " << "HAMILTON - UNDIGRAPH\n";

                for (int k = 0; k < j; k++) {
                    visitedTable[k] = false;
                }

                t1 = std::chrono::high_resolution_clock::now();
                hamiltonCycleSearchDigraph(0, visitedTable, j, &nVisited, newGraph, hamiltonDigraph);
                t2 = std::chrono::high_resolution_clock::now();
                result = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
                output = std::to_string(i) + ", " + std::to_string(j) + ", " + "HAMILTON - DIGRAPH, " + std::to_string(result) + "\n";
                file << output;
                std::cout << i << " " << j << " " << "HAMILTON - DIGRAPH\n";

                t1 = std::chrono::high_resolution_clock::now();
                eulerCycleSearchUndiGraph(0, newGraphOther, eulerUndigraph);
                t2 = std::chrono::high_resolution_clock::now();
                result = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
                output = std::to_string(i) + ", " + std::to_string(j) + ", " + "EULER - UNDIGRAPH, " + std::to_string(result) + "\n";
                file << output;
                std::cout << i << " " << j << " " << "EULER - UNDIGRAPH\n";

                t1 = std::chrono::high_resolution_clock::now();
                eulerCycleSearchDigraph(0, newGraph, eulerDigraph);
                t2 = std::chrono::high_resolution_clock::now();
                result = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
                output = std::to_string(i) + ", " + std::to_string(j) + ", " + "EULER - DIGRAPH, " + std::to_string(result) + "\n";
                file << output;
                std::cout << i << " " << j << " " << "EULER - DIGRAPH\n";

                deleteDigraph(newGraph);
                delete[] visitedTable;
            }
        }
    }
    file.close();
}

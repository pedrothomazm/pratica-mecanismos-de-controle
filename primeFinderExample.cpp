#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <fstream>
#include <math.h>
#include <vector>

using namespace std;

// Número máximo a determinar se é primo
const int MAX_NUMBER = 100;

// Número máximo de threads
const int MAX_THREADS = 10;

// Número de repetições para calcular o tempo médio
const int REPETITIONS = 10;

// Nomes dos arquivos de saída
const string UNBALANCED_OUTPUT_FILE = "primesUnbalanced.txt";
const string BALANCED_OUTPUT_FILE = "primesBalanced.txt";

/**
 * @brief Verifica se um número é primo de forma lenta.
 *
 * Esta função verifica se um número é primo de forma lenta, ou seja, ela
 * verifica se o número é divisível por todos os números menores que ele,
 * além de dormir por 1ms a cada iteração para simular uma função lenta.
 *
 * @param n O número a ser verificado.
 * @return true se o número é primo, false caso contrário.
 */
bool isPrimeSlow(int n) {
    if (n < 2) return false;
    for (int i = 2; i < n; i++) {
        // Dorme por 1ms para simular uma função lenta
        this_thread::sleep_for(chrono::milliseconds(1));
        if (n % i == 0) return false;
    }
    return true;
}

/**
 * @brief Verifica se os números de um bloco são primos.
 *
 * Esta função verifica se os números de um bloco são primos e armazena
 * os resultados em um vetor de booleanos.
 *
 * @param start O início do bloco.
 * @param end O final do bloco.
 * @param results O vetor de booleanos para armazenar os resultados.
 */
void findPrimesBlock(int start, int end, bool *results) {
    for (int i = start; i < end; i++) {
        results[i] = isPrimeSlow(i);
    }
}

/**
 * @brief Verifica se os números de um bloco são primos.
 *
 * Esta função verifica se os números de um bloco são primos e armazena
 * os resultados em um vetor de booleanos. Ela utiliza um mutex para
 * controlar o acesso ao índice do próximo número a ser verificado.
 *
 * @param index O índice do próximo número a ser verificado.
 * @param end O final do bloco.
 * @param results O vetor de booleanos para armazenar os resultados.
 * @param m O mutex para controlar o acesso ao índice.
 */
void findPrimesThread(int *index, int end, bool *results, mutex *m) {
    while (true) {
        // Bloqueia o mutex para acessar o índice
        m->lock();
        int current = *index;
        *index = *index + 1;
        m->unlock();

        if (current >= end) break;
        results[current] = isPrimeSlow(current);
    }
}

/**
 * @brief Encontra os números primos de 0 a maxNumber.
 * 
 * Esta função encontra os números primos de 0 a maxNumber utilizando
 * numThreads threads de forma desbaçanceada. O resultado é armazenado
 * em um arquivo de saída.
 * 
 * @param maxNumber O número máximo a ser verificado.
 * @param numThreads O número de threads a ser utilizado.
 * @param outputFileName O nome do arquivo de saída.
 */
void findPrimesUnbalanced(int maxNumber, int numThreads, string outputFileName) {
    // Inicializa o vetor de resultados e as threads
    bool results[maxNumber];
    thread threads[numThreads];

    // Inicia as threads
    float blockSize = (float) maxNumber / numThreads;
    int block_start;
    int block_end = 0;
    for (int i = 0; i < numThreads; i++) {
        block_start = block_end;
        block_end = (int) round(blockSize * (i + 1));
        threads[i] = thread(
            findPrimesBlock,
            block_start,
            block_end,
            (bool*) results
        );
    }

    // Espera as threads terminarem
    for (int i = 0; i < numThreads; i++) {
        threads[i].join();
    }

    // Escreve os resultados em um arquivo
    ofstream file;
    file.open(outputFileName);
    for (int i = 0; i < maxNumber; i++) {
        if (results[i]) {
            file << i << endl;
        }
    }
    file.close();
}

/**
 * @brief Encontra os números primos de 0 a maxNumber.
 * 
 * Esta função encontra os números primos de 0 a maxNumber utilizando
 * numThreads threads de forma balanceada. O resultado é armazenado
 * em um arquivo de saída.
 * 
 * @param maxNumber O número máximo a ser verificado.
 * @param numThreads O número de threads a ser utilizado.
 * @param outputFileName O nome do arquivo de saída.
 */
void findPrimesBalanced(int maxNumber, int numThreads, string outputFileName) {
    // Inicializa o vetor de resultados e as threads
    bool results[maxNumber];
    thread threads[numThreads];

    // Inicializa o índice e o mutex
    int index = 0;
    mutex m;

    // Inicia as threads
    for (int i = 0; i < numThreads; i++) {
        threads[i] = thread(
            findPrimesThread,
            &index,
            maxNumber,
            (bool*) results,
            &m
        );
    }

    // Espera as threads terminarem
    for (int i = 0; i < numThreads; i++) {
        threads[i].join();
    }

    // Escreve os resultados em um arquivo
    ofstream file;
    file.open(outputFileName);
    for (int i = 0; i < maxNumber; i++) {
        if (results[i]) {
            file << i << endl;
        }
    }
    file.close();
}

int main() {
    // Cria um arquivo para armazenar os resultados
    ofstream file;
    file.open("threadsUnbalanced.csv");
    file << "Threads,Tempo" << endl;
    
    for (int i = 1; i <= MAX_THREADS; i++) {
        // Calcula o tempo médio para cada número de threads
        auto start = chrono::high_resolution_clock::now();
        for (int j = 0; j < REPETITIONS; j++) {
            findPrimesUnbalanced(MAX_NUMBER, i, UNBALANCED_OUTPUT_FILE);
        }
        auto end = chrono::high_resolution_clock::now();

        chrono::duration<double> avg = (end - start) / REPETITIONS;
        file << i << "," << avg.count() << endl;

        // Imprime os resultados
        cout << "Threads: " << i << endl;
        cout << "Tempo médio: " << avg.count() << "s" << endl;
        cout << "Quantidade de números avaliados: " << MAX_NUMBER << endl;

        // Lê os números primos do arquivo e imprime os resultados
        ifstream inputFile(UNBALANCED_OUTPUT_FILE);
        string line;
        vector<int> primes;
        while (getline(inputFile, line)) {
            primes.push_back(stoi(line));
        }
        cout << "Quantidade de números primos: " << primes.size() << endl;
        cout << "Números primos: ";
        for (int i = 0; i < primes.size(); i++) {
            cout << primes[i] << " ";
        }
        cout << endl << endl;
    }
    file.close();

    // Repete o processo para o caso balanceado
    file.open("threadsBalanced.csv");
    file << "Threads,Tempo" << endl;

    for (int i = 1; i <= MAX_THREADS; i++) {
        // Calcula o tempo médio para cada número de threads
        auto start = chrono::high_resolution_clock::now();
        for (int j = 0; j < REPETITIONS; j++) {
            findPrimesBalanced(MAX_NUMBER, i, BALANCED_OUTPUT_FILE);
        }
        auto end = chrono::high_resolution_clock::now();

        chrono::duration<double> avg = (end - start) / REPETITIONS;
        file << i << "," << avg.count() << endl;

        // Imprime os resultados
        cout << "Threads: " << i << endl;
        cout << "Tempo médio: " << avg.count() << "s" << endl;
        cout << "Quantidade de números avaliados: " << MAX_NUMBER << endl;

        // Lê os números primos do arquivo e imprime os resultados
        ifstream inputFile(BALANCED_OUTPUT_FILE);
        string line;
        vector<int> primes;
        while (getline(inputFile, line)) {
            primes.push_back(stoi(line));
        }
        cout << "Quantidade de números primos: " << primes.size() << endl;
        cout << "Números primos: ";
        for (int i = 0; i < primes.size(); i++) {
            cout << primes[i] << " ";
        }
        cout << endl << endl;
    }
    file.close();

    return 0;
}
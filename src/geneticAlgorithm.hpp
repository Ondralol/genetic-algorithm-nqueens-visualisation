/**
 * @file geneticAlgorithm.hpp
 * @author Ondrej
 * @brief Genetic algorithm class for solving N-queens problem
 *
*/

#pragma once

#include <vector>
#include <map>
#include <cstddef>
#include <mutex>
#include <atomic>
#include <mutex>

#define POPULATION_SIZE 500 // Population might be +- 1 than POPULATION_SIZE due to crossover, but that is not a problem
#define GENERATIONS 10000
#define CROSSOVER_RATE 0.85f
#define MUTATION_RATE 0.0235f
#define PREVIOUS_GEN_COUNT 25 // Needs to be lower than population_size
#define PREVIOUS_GEN_CROSSOVER_COUNT 125 // Needs to be lower than population_size
#define TOURNAMENT_SIZE 10

using Individual = std::pair<std::vector<size_t>, double>;

/** Represents one generation */
class Generation
{
public:
  Generation(size_t index, float mutationRate, float crossoverRate)
    : m_generationIndex(index),
      m_mutationRate(mutationRate),
      m_crossoverRate(crossoverRate)
  {};

  /** Returns number of positions that queen can be attack from */
  size_t attackCount(size_t row, std::vector<size_t> & individual);

  /** Gets fitness  for individual */
  double getFitness(std::vector<size_t> & individual);

  /** Adds individual to the generation */
  void addIndividual(std::vector<size_t> individual);

  /** Gets the average fitness */
  double fitnessAverage(void);

  /** Gets the best fitness (could be calculated continuouly i guess, but this will do for now */
  double fitnessBest(void);

  /** Returns N best individuals from generation */
  std::vector<std::vector<size_t>> getNBest(size_t n);

  /** Returns generations mutation rate */
  float getMutationRate(void);

  /** Returns generations crossover rate */
  float getCrossoverRate(void);

  /** Finds n random individuals and returns the one with the highest fitness */
  std::vector<size_t> getRandomTournament(size_t n);


private:
  /* Represents position on chess board, such that each element in vector is a row, index
    represents at which column the queen is, -1 represents that there is no queen in that row */
  std::vector<std::pair<std::vector<size_t>, double>> m_individuals;
  bool m_sorted = false;
  float m_mutationRate;
  float m_crossoverRate;

  // not using for now, can be used later for better performance
  double m_highestFitness;
  double m_averageFitness;

  size_t m_generationIndex;
};


class BoardVisualisation;

/** Genetic algorithm that solves N-Queens problem */
class Genetic
{
public:
  Genetic(size_t N)
    : m_dimension(N)
  {};

  /** Generate individual (random position of queens on chess board) */
  std::vector<size_t> generateIndividual();

  /** Crossover two individuals (combines their genes) with CROSSOVER_RATE probability  */
  std::pair<std::vector<size_t>, std::vector<size_t>> crossoverIndividuals(std::vector<size_t> individual1, std::vector<size_t> individual2);

  /** Mutate individual with MUTATION_RATE probability */
  std::vector<size_t> mutateIndividual(std::vector<size_t> individual);

  /** Returns Nth generation */
  Generation getNthGeneration(size_t N);

  /** Returns current mutatio*/

  /** Returns number of generations */
  size_t getGenerationsCount(void);

  /** Returns true if calculation is finished */
  bool isFinished();

  /** Runs the whole genetic algorithm */
  bool run(void);


private:
  size_t m_dimension;
  size_t m_generationIndex = 0;
  float m_mutationRate = MUTATION_RATE;
  float m_crossoverRate = CROSSOVER_RATE;
  bool m_finished = false;
  std::vector<Generation> m_generations;
  std::mutex m_mtx;
};

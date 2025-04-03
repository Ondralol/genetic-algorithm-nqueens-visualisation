/**
 * @file geneticAlgorithm.cpp
 * @author Ondrej
 * @brief Genetic algorithm class for solving N-queens problem
 *
*/

#include "geneticAlgorithm.hpp"

#include <cstdlib>
#include <cfloat>
#include <algorithm>
#include <random>
#include <iostream>
#include <cmath>

/* Implementation of Generation class*/

/** Returns number of positions that queen can be attack from */
size_t Generation::attackCount(size_t row, std::vector<size_t> & individual)
{
  size_t attacks = 0;
  /* Because of the way we store the queen positions, there is never gonna be more than one queen in the same row
     we need to check whether there are more queens in the same columns or if the queens can attack each other diagonaly.
     Another thing we need to think about is if counting two queens atacking each other "twice" is not actually bad for the algorithm */
  for (size_t i = row; i < individual.size(); i ++)
  {
    // Our own row
    if (i == row)
      continue;

    // On the same row
    if (individual[row] == individual[i])
      attacks ++;

    // If queen can be attacked diagonally
    if (std::abs(static_cast<int>(row) - static_cast<int>(i)) == std::abs(static_cast<int>(individual[row]) - static_cast<int>(individual[i])))
      attacks ++;
  }

  return attacks;
}

/** Gets fitness score for individual. Works such that fitness 0 means that no queens attack each other and N means that N queens attack each   other */
double Generation::getFitness(std::vector<size_t> & individual)
{
  double fitness = 0;
  for (size_t i = 0; i < individual.size(); i ++)
  {
    fitness += this -> attackCount(i, individual);
  }
  return fitness;
}


/** Adds individual to the generation */
void Generation::addIndividual(std::vector<size_t> individual)
{
  m_sorted = false;
  m_individuals.push_back(std::make_pair(individual, this -> getFitness(individual)));
}


/** Gets the average fitness */
double Generation::fitnessAverage(void)
{
  double sum = 0.0f;
  for (const auto & value: m_individuals)
  {
    sum += value.second;
  }

  return m_individuals.size() != 0 ? sum/m_individuals.size() : DBL_MAX;
}

/** Gets the best fitness (could be calculated continuouly i guess, but this will do for now */
double Generation::fitnessBest(void)
{
  double best = DBL_MAX;
  for (const auto & value: m_individuals)
  {
    best = std::min(best, value.second);
  }

  return best;
}


/** Returns N best individuals from generation */
std::vector<std::vector<size_t>> Generation::getNBest(size_t n)
{
  /* If n > size */
  if (n > m_individuals.size())
    return {};

  std::vector<std::vector<size_t>> returnVector;
  size_t i = 0;

  if (!m_sorted)
  {
    /* Sorted in ascending order (the lower the fitness, the better) */
    std::sort(m_individuals.begin(), m_individuals.end(),
              [] (const std::pair<std::vector<size_t>, double> & a, const std::pair<std::vector<size_t>, double> & b)
    {
      return a.second < b.second;
    });

    m_sorted = true;
  }

  for (size_t i = 0; i < n; i++)
  {
    returnVector.push_back(m_individuals[i].first);
  }

  return returnVector;
}


/** Returns generations mutation rate */
float Generation::getMutationRate(void)
{
  return m_mutationRate;
}


/** Returns generations crossover rate */
float Generation::getCrossoverRate(void)
{
  return m_crossoverRate;
}


/** Find two random individuals and returns the one with higher fitness */
std::vector<size_t> Generation::getRandomTournament(size_t n)
{
  if (m_individuals.size() == 0)
    return {};

  std::random_device rd;
  std::minstd_rand gen(rd());
  std::uniform_int_distribution<int> dist(0, m_individuals.size() - 1);

  Individual individual = m_individuals[dist(gen)];
  for (int i = 1; i < n; i ++)
  {
    individual = std::min(individual, m_individuals[dist(gen)],
                          [] (Individual x, Individual y)
    {
      return x.second < y.second;
    });
  }

  return individual.first;
}

/** Generate individual (random position of queens on chess board) */
std::vector<size_t> Genetic::generateIndividual()
{
  std::random_device rd;
  std::minstd_rand gen(rd());
  std::uniform_int_distribution<int> dist(0, m_dimension - 1);

  std::vector<size_t> individual;

  for (size_t i = 0; i < m_dimension; i++)
  {
    individual.emplace_back(dist(gen));
  }

  return individual;
}

/** Crossover two individuals (combines their genes) with CROSSOVER_RATE probablity */
std::pair<std::vector<size_t>, std::vector<size_t>> Genetic::crossoverIndividuals(std::vector<size_t> individual1, std::vector<size_t> individual2)
{
  std::pair<std::vector<size_t>, std::vector<size_t>> newIndividuals;
  std::vector<size_t> newIndividual;

  // Calculates the probability
  std::random_device rd;
  std::minstd_rand gen(rd());
  std::uniform_int_distribution<int> dist(0, 100);

  // If crossover is not happening
  if (dist(gen) > m_crossoverRate * 100)
  {
    newIndividuals.first = individual1;
    newIndividuals.second = individual2;
    return newIndividuals;
  }

  // Choose random point in m_dimension range to start the crossover
  dist = std::uniform_int_distribution<int>(0, m_dimension - 1);
  int crossoverStart = dist(gen);

  /* First crossover */

  // Get genes from the first individual
  for (int i = 0; i < crossoverStart; i ++)
  {
    newIndividual.push_back(individual1[i]);
  }

  // Get genes from the second individual
  for (int i = crossoverStart; i < m_dimension; i++)
  {
    newIndividual.push_back(individual2[i]);
  }

  newIndividuals.first = newIndividual;

  /* Second crossover */
  newIndividual.clear();

  // Get genes from the second individual
  for (int i = 0; i < crossoverStart; i ++)
  {
    newIndividual.push_back(individual1[i]);
  }

  // Get genes from the first individual
  for (int i = crossoverStart; i < m_dimension; i++)
  {
    newIndividual.push_back(individual2[i]);
  }

  newIndividuals.second = newIndividual;

  return newIndividuals;
}


/** Mutate individual with MUTATION_RATE probability. For each gene calculate probability of mutation, if mutation should happen
    generate gene in range [0, m_dimension - 1], else keep the gene                                                             */
std::vector<size_t> Genetic::mutateIndividual(std::vector<size_t> individual)
{
  std::vector<size_t> newIndividual;

  // Calculates the probability for each gene
  std::random_device rd;
  std::minstd_rand gen(rd());
  std::uniform_int_distribution<int> distProba(0, 100);
  std::uniform_int_distribution<int> distGene(0, m_dimension - 1);

  for (size_t i = 0; i < m_dimension; i ++)
  {
    // Keep the gene same
    if (distProba(gen) > m_mutationRate * 100)
    {
      newIndividual.push_back(individual[i]);
    }

    // Mutate the gene
    else
    {
      newIndividual.push_back(distGene(gen));
    }
  }

  return newIndividual;
}

/** Returns Nth generation */
Generation Genetic::getNthGeneration(size_t N)
{
  std::unique_lock<std::mutex> lock (m_mtx);
  if (N >= m_generations.size())
    throw std::out_of_range("Generation Out of range");

  return m_generations[N];
}

/** Returns number of generations */
size_t Genetic::getGenerationsCount(void)
{
  std::unique_lock<std::mutex> lock (m_mtx);
  return m_generations.size();
}

/** Returns true if calculation is finished */
bool Genetic::isFinished()
{
  std::unique_lock<std::mutex> lock (m_mtx);
  return m_finished;
}


/** Runs the whole genetic algorithm */
bool Genetic::run(void)
{
  /* Randomly generate the first generation */
  Generation gen(m_generationIndex ++, MUTATION_RATE, CROSSOVER_RATE);
  for (size_t i = 0; i < POPULATION_SIZE; i ++)
  {
    gen.addIndividual(this -> generateIndividual());
  }

  std::unique_lock<std::mutex> lock (m_mtx);
  m_generations.push_back(gen);
  lock.unlock();

  for (size_t i = 1; i < GENERATIONS; i ++)
  {
    /* Use simulated annealing to update mutation and crossover rates */
    m_mutationRate = MUTATION_RATE * std::exp(-static_cast<float>(i) / GENERATIONS);
    m_crossoverRate = CROSSOVER_RATE * std::exp(-static_cast<float>(i) / GENERATIONS * 1.0);

    lock.lock();
    Generation prevGen = m_generations[m_generationIndex - 1];
    /* If previous generation has low fitness, make crossover rate lower and mutation rate higher */
    lock.unlock();

    Generation newGen(m_generationIndex ++, m_mutationRate, m_crossoverRate);
    /* Add the best N individuals from the previous generation, but mutate their genes */
    for (auto & individual: prevGen.getNBest(PREVIOUS_GEN_COUNT))
    {
      newGen.addIndividual(this -> mutateIndividual(individual));
    }

    /* Crossover the best N individuals from the previous generation and mutate their genes */
    std::random_device rd;
    std::minstd_rand gen(rd());
    std::uniform_int_distribution<int> dist(0, PREVIOUS_GEN_COUNT - 1);

    std::vector<std::vector<size_t>> individuals = prevGen.getNBest(PREVIOUS_GEN_COUNT);
    std::pair<std::vector<size_t>, std::vector<size_t>> newIndividuals;



    for (int j = 0; j < PREVIOUS_GEN_CROSSOVER_COUNT / 2; j++)
    {
      newIndividuals = this -> crossoverIndividuals(individuals[dist(gen)], individuals[dist(gen)]);
      newGen.addIndividual(this -> mutateIndividual(newIndividuals.first));
      newGen.addIndividual(this -> mutateIndividual(newIndividuals.second));
    }


    // Add the rest of the individuals
    for (int j = 0; j < (POPULATION_SIZE - 2 * PREVIOUS_GEN_COUNT - 2 * (PREVIOUS_GEN_CROSSOVER_COUNT / 2)); j ++)
    {
      newIndividuals = this -> crossoverIndividuals(prevGen.getRandomTournament(TOURNAMENT_SIZE), prevGen.getRandomTournament(TOURNAMENT_SIZE));
      newGen.addIndividual(this -> mutateIndividual(newIndividuals.first));
      newGen.addIndividual(this -> mutateIndividual(newIndividuals.second));
    }

    lock.lock();
    m_generations.push_back(newGen);
    lock.unlock();
    //std::cout << "Generation: " << m_generationIndex << ", Average: " << newGen.fitnessAverage() << ", Best: " << newGen.fitnessBest()  << std::endl;

    if (newGen.fitnessBest() == 0.0f)
    {
      m_finished = true;
      std::cout << "Success" << std::endl;
      return true;
    }
  }

  std::cout << "Failure" << std::endl;
  return false;
}

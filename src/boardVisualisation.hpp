/**
 * @file boardVisualisation.hpp
 * @author Ondrej
 * @brief Class that visualises the chess board
 *
*/

#pragma once

#include "geneticAlgorithm.hpp"

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <chrono>

// Represents position on the chess board
using Position = std::pair<size_t, size_t>;

#define TOP_PADDING_TEXT 20.0f
#define LEFT_PADDING_TEXT 15.0f
#define TOP_PADDING 65.0f
#define LEFT_PADDING 15.0f
#define TOP_PADDING_GRAPH 450.0f
#define LEFT_PADDING_GRAPH 15.0f
#define GRAPH_SIZE_X 450.0f
#define GRAPH_SIZE_Y 300.0f

class BoardVisualisation
{
public:
  BoardVisualisation(size_t N, unsigned screenWidth, unsigned screenHeight)
    : m_window(sf::RenderWindow (sf::VideoMode({screenWidth, screenHeight}), "N-Queens Visualisation")),
  m_genetic(Genetic(N))
  {
    m_screenTitle = "N-Queens Visualisation";
    m_window.setFramerateLimit(360);

    m_paused = false;
    m_stepDelay = 0.0005f;
    m_dimension = N;
    m_font.loadFromFile("assets/open_sans");
    m_visualisationIndex = 0;
    m_startVisualisation = false;
  }

  /** Processes the user input during visualisation */
  void processInput(sf::Event & event);

  /** The main visualisation loop */
  void mainLoop(void);

  /** Loads texture from cache/file */
  std::shared_ptr<sf::Texture> loadTexture(std::string filename);

  /** Displays the whole chess board */
  void showBoard(void);

private:
  sf::RenderWindow m_window;
  std::string m_screenTitle;
  bool m_paused;

  std::unordered_map<std::string, std::shared_ptr<sf::Texture>> m_textureCache;
  sf::Font m_font;
  size_t m_dimension;
  std::vector<std::vector<size_t>> m_squares;
  float m_stepDelay;
  std::chrono::high_resolution_clock::time_point m_startTime;

  Genetic m_genetic;
  size_t m_visualisationIndex;
  bool m_startVisualisation;
};


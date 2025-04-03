/**
 * @file boardVisualisation.cpp
 * @author Ondrej
 * @brief Class that visualises the chess board
 *
*/

#include "boardVisualisation.hpp"

#include <memory>
#include <string>
#include <random>
#include <thread>
#include <sstream>

/** Processes all user input */
void BoardVisualisation::processInput(sf::Event & event)
{
  /* Close window if window is closed */
  if (event.type == sf::Event::Closed)
    m_window.close();

  /** Close window if ESC is pressed */
  else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    m_window.close();

  /** Pause animation if SPACE is pressed */
  else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space)
  {
    m_paused = !m_paused;
    if (m_paused)
      m_window.setTitle(m_screenTitle + " -  (PAUSED)");
    else
      m_window.setTitle(m_screenTitle);
  }

  /* Change speed (Faster) */
  else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D)
  {
    m_stepDelay = std::max(0.00025f, m_stepDelay / 1.5f);
  }

  /* Change speed (Slower) */
  else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A)
  {
    m_stepDelay = std::min(0.75f, m_stepDelay * 1.5f);
  }

  /* If R pressed, reset animation */
  else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
  {
    /* If visualisation already started, then reset it */
    if (m_startVisualisation)
      m_visualisationIndex = 0;
  }
}

/** The main loop */
void BoardVisualisation::mainLoop(void)
{
  /* Calculate the N-Queens problem */
  std::thread thr(std::bind(&Genetic::run, &m_genetic));
  m_startTime = std::chrono::high_resolution_clock::now();

  sf::Event event;
  sf::Clock clock;
  float currentStepTime = 0.0f;
  while (m_window.isOpen())
  {
    while (m_window.pollEvent(event))
    {
      this -> processInput(event);
    }

    float dt = clock.restart().asSeconds();
    currentStepTime += dt;

    /* If delta time & not paused */
    if (currentStepTime >= m_stepDelay && !m_paused)
    {
      /* Update step - due to multi threading there needs to be these conditions, explanation:
         we check whether we can take generation that has already been preprocessed in the thread, so it would be
         safe to work with or if have have finished all the generations, then we can taky any completed generation */
      if ((m_visualisationIndex + 1) < m_genetic.getGenerationsCount())
      {

        /* Initialize the start of the visualisation */
        if (m_startVisualisation)
          m_visualisationIndex ++;
        
        m_startVisualisation = true;
      }

      // Reset timer
      currentStepTime = 0.0f;
    }

    m_window.clear(sf::Color(39,36,33,255));

    showBoard();

    m_window.display();

  }
  thr.join();
}

/** Loads texture from cache/file */
std::shared_ptr<sf::Texture> BoardVisualisation::loadTexture(std::string filename)
{
  if (m_textureCache.find(filename) != m_textureCache.end())
    return m_textureCache[filename];

  auto texture = std::make_shared<sf::Texture>();
  if (!texture -> loadFromFile(filename))
    return nullptr;

  m_textureCache[filename] = texture;
  return texture;
}


/** Displays the whole board */
void BoardVisualisation::showBoard(void)
{
  unsigned int smallerWinSize = std::min(m_window.getSize().x, m_window.getSize().y);
  float squareSize = (smallerWinSize - 75) / m_dimension;
  float queenSize = 0.5 ;

  sf::VertexArray squares(sf::Quads, m_dimension * m_dimension * 4);
  size_t vertexIndex = 0;

  float X = LEFT_PADDING;
  float Y = TOP_PADDING;

  for (size_t i = 0; i < m_dimension; i ++)
  {
    for (size_t j = 0; j < m_dimension; j ++)
    {
      sf::Color color;
      /* Decide colour based on square positon */
      if ((i % 2 != 0 && j % 2 != 0)|| (i % 2 == 0 && j % 2 == 0) )
        color = sf::Color(238,238,210,255);
      else
        color = sf::Color(118,150,86,255);

      squares[vertexIndex].color = color;
      squares[vertexIndex].position = sf::Vector2f(X, Y);
      vertexIndex ++;

      squares[vertexIndex].color = color;
      squares[vertexIndex].position = sf::Vector2f(X + squareSize, Y);
      vertexIndex ++;

      squares[vertexIndex].position = sf::Vector2f(X + squareSize, Y + squareSize);
      squares[vertexIndex].color = color;
      vertexIndex ++;

      squares[vertexIndex].color = color;
      squares[vertexIndex].position = sf::Vector2f(X, Y + squareSize);
      vertexIndex ++;

      X += squareSize;
    }

    Y += squareSize;
    X = LEFT_PADDING;

  }

  m_window.draw(squares);


  /* Displays the queens */

  // If we can start the visualisation
  if (!m_startVisualisation)
    return;

  auto texture = this -> loadTexture("assets/queen_white");
  // If texture could not be loaded
  if (!m_startVisualisation)
    return;

  texture -> setSmooth(true);
  sf::Sprite sprite;
  sprite = sf::Sprite(*texture);
  sprite.setScale(squareSize / texture -> getSize().x, squareSize / texture -> getSize().y);

  Generation gen = m_genetic.getNthGeneration(m_visualisationIndex);
  std::vector<std::vector<size_t>> nbest = gen.getNBest(1);
  if (nbest.size() == 0)
    return;

  std::vector<size_t> queens = nbest[0];

  for (size_t i = 0; i < queens.size(); i ++)
  {
    sprite.setPosition(LEFT_PADDING + i * squareSize, TOP_PADDING + queens[i] * squareSize);
    m_window.draw(sprite);
  }

  /* Displays text*/
  sf::Text text;
  text.setFont(m_font);
  text.setCharacterSize(35);
  text.setFillColor(sf::Color::White);

  // Generation
  text.setString("Generation: " + std::to_string(m_visualisationIndex));
  text.setPosition(LEFT_PADDING + squareSize * m_dimension / 2 - 75, TOP_PADDING_TEXT);
  m_window.draw(text);

  text.setCharacterSize(25);

  //Other stats
  auto endTime = std::chrono::high_resolution_clock::now();
  auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_startTime);

  auto minutes = std::chrono::duration_cast<std::chrono::minutes>(elapsedTime);
  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsedTime);
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime);

  
  /* Currently useless
  std::ostringstream oss;
  oss << minutes.count() << ":" << seconds.count() << ":" << milliseconds.count();
  text.setString("Elapsed Time: " + oss.str());
  text.setPosition(LEFT_PADDING + squareSize * m_dimension + 100, TOP_PADDING_TEXT + 50);
  m_window.draw(text);
  */

  text.setString("Max Generations count: " + std::to_string(GENERATIONS));
  text.setPosition(LEFT_PADDING + squareSize * m_dimension + 100, TOP_PADDING_TEXT + 100);
  m_window.draw(text);

  text.setString("Population Size: " + std::to_string(POPULATION_SIZE));
  text.setPosition(LEFT_PADDING + squareSize * m_dimension + 100, TOP_PADDING_TEXT + 150);
  m_window.draw(text);

  text.setString("Mutation Rate: " + std::to_string(gen.getMutationRate()));
  text.setPosition(LEFT_PADDING + squareSize * m_dimension + 100, TOP_PADDING_TEXT + 200);
  m_window.draw(text);

  text.setString("Crossover Rate: " + std::to_string(gen.getCrossoverRate()));
  text.setPosition(LEFT_PADDING + squareSize * m_dimension + 100, TOP_PADDING_TEXT + 250);
  m_window.draw(text);


  text.setString("Average Fitness: " + std::to_string(gen.fitnessAverage()));
  text.setPosition(LEFT_PADDING + squareSize * m_dimension + 100, TOP_PADDING_TEXT + 300);
  m_window.draw(text);

  text.setString("Best Fitness: " + std::to_string(gen.fitnessBest()));
  text.setPosition(LEFT_PADDING + squareSize * m_dimension + 100, TOP_PADDING_TEXT + 350);
  m_window.draw(text);

}





/**
 * @file main.cpp
 * @author Ondrej
 * @brief Main function that parses input arguments and calls other functions/metods
*/

#include "boardVisualisation.hpp"

#include <iomanip>

/**
 * @brief Manages whole program
 * - Argument 1: Positive integer N that stands for chess board size (NxN)
*/
int main (int argc, char ** argv)
{
  // Default value if no arguments are passed
  size_t N = 8;

  // Incorrent number of arguments
  if (argc > 2)
    return EXIT_FAILURE;

  // If one argument is passed
  if (argc == 2)
  {
    std::istringstream parse(argv[1]);
    // If argument was not a number
    if (!(parse >> N))
      return EXIT_FAILURE;
  }

  /* Creates an instance of BoardVisualisation */
  unsigned screenWidth = sf::VideoMode::getDesktopMode().width;
  unsigned screenHeight = sf::VideoMode::getDesktopMode().height;
  BoardVisualisation board(N, screenWidth, screenHeight);

  /* Runs the main window loop*/
  board.mainLoop();

  return EXIT_SUCCESS;
}

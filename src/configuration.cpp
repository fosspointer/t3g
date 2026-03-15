#include "configuration.hpp"

namespace t3g
{
    std::size_t Configuration::boardWidth = 3ul;
    std::size_t Configuration::boardHeight = 3ul;
    std::size_t Configuration::winPileSize = 3ul;
    bool Configuration::enableColors = true;
    std::size_t Configuration::moveTimer = 0;
    char Configuration::firstPlayerSymbol = 'X';
    char Configuration::secondPlayerSymbol = 'O';
};

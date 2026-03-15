#pragma once
#include <cstddef>

namespace t3g
{
    class Configuration final
    {
    public:
        Configuration() = delete;
        static std::size_t boardWidth;
        static std::size_t boardHeight;
        static std::size_t winPileSize;
        static bool enableColors;
        static std::size_t moveTimer;
        static char firstPlayerSymbol;
        static char secondPlayerSymbol;
    };
}

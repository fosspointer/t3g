#pragma once
#include "configuration.hpp"
#include <cstdint>
#include <unordered_map>
#include <optional>

namespace t3g
{
    enum class BoardState : bool
    {
        FirstPlayer, SecondPlayer
    };

    inline BoardState operator!(BoardState state)
    {
        return static_cast<BoardState>(!static_cast<bool>(state));
    }

    class Board final
    {
    public:
        Board() = delete;

        static std::optional<BoardState> getCell(std::size_t position_x, std::size_t position_y);
        static bool setCell(std::size_t position_x, std::size_t position_y, BoardState board_state);
    private:
        static std::unordered_map<std::size_t, BoardState> s_board;
    };
}

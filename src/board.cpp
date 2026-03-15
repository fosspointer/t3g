#include "board.hpp"

namespace t3g
{
    std::unordered_map<std::size_t, BoardState> Board::s_board;

    std::optional<BoardState> Board::getCell(std::size_t position_x, std::size_t position_y)
    {
        auto absolute_position = position_x + position_y * Configuration::boardWidth;
        auto find = s_board.find(absolute_position);

        if(find == s_board.end())
            return std::nullopt;

        else return std::make_optional(find->second);
    }

    bool Board::setCell(std::size_t position_x, std::size_t position_y, BoardState board_state)
    {
        auto absolute_position = position_x + position_y * Configuration::boardWidth;
        auto find = s_board.find(absolute_position);

        if(find != s_board.end())
            return false;

        s_board.insert(std::make_pair(absolute_position, board_state));
        return true;
    }
}

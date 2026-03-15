#pragma once
#include <cstddef>
#include <cstdint>
#include <print>
#include "board.hpp"
#include "configuration.hpp"

namespace t3g
{
    class Game final
    {
    public:
        Game() = delete;

        static void start()
        {
            BoardState player_turn = BoardState::FirstPlayer;

            for(std::size_t turn_count{0ul}; turn_count < Configuration::boardWidth * Configuration::boardHeight; ++turn_count)
            {
                displayBoard();
                char player_character = player_turn == BoardState::FirstPlayer
                    ? Configuration::firstPlayerSymbol
                    : Configuration::secondPlayerSymbol;

                std::string prompt = std::format("{} to play: ", player_character);
                auto [move_x, move_y] = readMove(prompt);
                Board::setCell(move_x, move_y, player_turn);

                if(getVictoryState(move_x, move_y))
                {
                    displayBoard();
                    std::println("Player {} has won!", player_character);
                    return;
                }

                player_turn = !player_turn;
            }

            displayBoard();
            std::println("It's a draw!");
        }

    private:
        using MoveAxis = std::pair<std::ptrdiff_t, std::ptrdiff_t>;
        static constexpr std::array<MoveAxis, 4ul> s_moveAxes{
            std::make_pair(1l, 0l), // Horizontal
            std::make_pair(0l, 1l), // Vertical
            std::make_pair(1l, 1l), // First diagonal
            std::make_pair(-1l, 1l) // Second diagonal
        };

        static auto readMove(std::string_view prompt) -> std::pair<std::size_t, std::size_t> 
        {
            static char buffer[256];
            std::size_t row, column;
            std::string_view row_part{};

            bool has_valid_move{false};
            do
            {
                // show prompt and read to buffer
                std::print("{}", prompt);
                std::fgets(buffer, sizeof buffer, stdin);
                auto input = std::string_view{buffer};

                // prepare to iterate
                std::size_t index{0ul};

                // trim start space
                while(index < input.length() && std::isspace(input[index])) { ++index; }

                // validate column
                if(index < input.length() && !std::isalpha(input[index]))
                {
                    std::println("Every move must begin with a letter to indicate the column to play in, but `{}` is not a letter.", input[index]);
                    continue;
                }
                else if(index >= input.length() - 1ul)
                {
                    std::println("Move cannot be empty!");
                    continue;
                }

                column = std::toupper(input[index++]) - 'A';

                // validate row 
                auto start_row = index;
                while(index < input.length() && std::isdigit(input[index])) { ++index; }

                row_part = input.substr(start_row, index - start_row - 1ul);
                if(start_row == index)
                {
                    std::println("Every move must have a number corresponding to the relevant row to play in, but `{}` is not a valid number.", inputToFriendly(row_part));
                    continue;
                }

                if(index != input.length() - 1ul && !std::isspace(input[index]) && input[index] != '\n')
                {
                    std::println("Found unexpected characters at the end of your move: `{}`. A move should only consist of a letter (the target column) followed by a number (the target row) to move in.", inputToFriendly(input));
                    continue;
                }

                has_valid_move = true;
            }
            while(!has_valid_move);

            // it's safe to use .data() as the string given to strtoul does not have to be null-terminated
            // we have also already ensured that we have a valid number
            row = std::strtoul(row_part.data(), nullptr, 10) - 1ul;
            return std::make_pair(column, row);
        }

        static std::string_view inputToFriendly(std::string_view input)
        {
            constexpr const std::string_view emptyString = "empty";

            auto base = !input.empty() && input.back() == '\n'
                ? input.substr(0ul, input.length() - 1ul)
                : input;

            return base.empty() ? emptyString : base;
        }

        static void displayBoard()
        {
            auto line_seperator = computeLineSeperator();

            for(std::size_t i{0ul}; i < Configuration::boardWidth; ++i)
                std::print(" {}  ", static_cast<char>('A' + i));

            std::fputc('\n', stdout);

            for(std::size_t i{0ul}; i < Configuration::boardWidth; ++i)
                std::print(" {}  ", 'v');
            
            std::fputc('\n', stdout);

            for(std::size_t j{0ul}; j < Configuration::boardHeight; ++j)
            {
                for(std::size_t i{0ul}; i < Configuration::boardWidth; ++i)
                {
                    auto cellStateCharacter = getCellStateCharacter(i, j);
                    std::print(" {} ", cellStateCharacter);

                    if(i != Configuration::boardWidth - 1ul)
                        std::fputc('|', stdout);
                }

                std::print(" < {}", j + 1ul);

                if(j != Configuration::boardHeight - 1ul)
                    std::println("\n{}", line_seperator);
            }

            std::fputc('\n', stdout);
        }

        static char getCellStateCharacter(std::size_t position_x, std::size_t position_y)
        {
            auto cell = Board::getCell(position_x, position_y);

            if(!cell.has_value())
                return '_';
            else if(*cell == BoardState::FirstPlayer)
                return Configuration::firstPlayerSymbol;
            else
                return Configuration::secondPlayerSymbol;
        }

        static std::string computeLineSeperator()
        {
            std::string line_seperator;
            line_seperator.reserve(Configuration::boardWidth * 4ul - 1ul);

            for(std::size_t i{0ul}; i < Configuration::boardWidth * 4ul - 1ul; ++i)
                line_seperator += '-';

            return line_seperator;
        }

        static bool getVictoryState(std::size_t move_x, std::size_t move_y)
        {
            auto move = Board::getCell(move_x, move_y);

            if(!move.has_value())
                return false;

            auto board_width = Configuration::boardWidth;
            auto board_height = Configuration::boardHeight;

            for(const auto& [offset_x, offset_y] : s_moveAxes)
            {
                std::size_t pile_size{1ul};

                // Negative direction
                std::size_t start_x{move_x - offset_x}, start_y{move_y - offset_y};
                while(true)
                {
                    if(start_x >= board_width || start_y >= board_height)
                        break;
                    else if(Board::getCell(start_x, start_y) != move)
                        break;

                    ++pile_size;
                    start_x -= offset_x;
                    start_y -= offset_y;
                }

                std::size_t end_x{move_x + offset_x}, end_y{move_y + offset_y};
                while(true)
                {
                    if(end_x >= board_width || end_y >= board_height)
                        break;
                    else if(Board::getCell(end_x, end_y) != move)
                        break;

                    ++pile_size;
                    end_x += offset_x;
                    end_y += offset_y;
                }

                // std::println("got pile size of {} for direction [{}, {}] for player {}", pile_size, offset_x, offset_y, *move == BoardState::FirstPlayer? "X": "O");
                if(pile_size >= Configuration::winPileSize)
                    return true;
            }

            return false;
        }
    };
};

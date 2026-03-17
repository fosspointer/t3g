#include <cstdio>
#include "arguments.hpp"
#include "configuration.hpp"
#include "game.hpp"

int main(int argument_count, const char** arguments)
{
    static constexpr char option_flag_column_count = 'c';
    static constexpr char option_flag_row_count = 'r';
    static constexpr char option_flag_first_player = 'f';
    static constexpr char option_flag_second_player = 's';

    using namespace t3g;
    std::unordered_map<char, ArgumentDefinition> argument_definitions{
        // {option_flag_color, ArgumentDefinition{
        //     .description = "Enable color support",
        //     .longForm = "color",
        //     .isFlag = true
        // }},
        {option_flag_column_count, ArgumentDefinition{
            .description = "Set the row count of the tic-tac-toe board.",
            .longForm = "column-count",
            .isFlag = false
        }},
        {option_flag_row_count, ArgumentDefinition{
            .description = "Set the column count of the tic-tac-toe board.",
            .longForm = "row-count",
            .isFlag = false
        }},
        {option_flag_first_player, ArgumentDefinition{
            .description = "Set the symbol which represents player #1.",
            .longForm = "first-player",
            .isFlag = false
        }},
        {option_flag_second_player, ArgumentDefinition{
            .description = "Set the symbol which represents player #2.",
            .longForm = "second-player",
            .isFlag = false
        }},
    };

    auto argument_count_size_t = static_cast<std::size_t>(argument_count);
    auto argument_handler_result = Arguments::init(argument_definitions, argument_count_size_t, arguments);

    if(!argument_handler_result.has_value())
    {
        std::print("{}", argument_handler_result.error());
        return -1;
    }

    auto argument_handler = *argument_handler_result;
    // Configuration::enableColors = argument_handler.getFlag(option_flag_color);

    if(auto board_width = argument_handler.getValue(option_flag_column_count); board_width.has_value())
    {
        Configuration::boardWidth = std::strtoul(board_width->data(), nullptr, 10);
    }

    if(auto board_height = argument_handler.getValue(option_flag_row_count); board_height.has_value())
    {
        Configuration::boardHeight = std::strtoul(board_height->data(), nullptr, 10);
    }

    if(auto first_player = argument_handler.getValue(option_flag_first_player); first_player.has_value())
    {
        Configuration::firstPlayerSymbol = first_player->data()[0ul];
    }

    if(auto second_player = argument_handler.getValue(option_flag_second_player); second_player.has_value())
    {
        Configuration::secondPlayerSymbol = second_player->data()[0ul];
    }

    if(argument_handler.isHelp())
    {
        std::print("{}{}", arguments[0ul], argument_handler.help(" - Usage:"));
        return 0;
    }

    Game::start();

    return 0;
}

#include <cstdio>
#include "arguments.hpp"
#include "configuration.hpp"
#include "game.hpp"

int main(int argument_count, const char** arguments)
{
    static constexpr char option_flag_color = 'c';

    using namespace t3g;
    std::unordered_map<char, ArgumentDefinition> argument_definitions{
        {option_flag_color, ArgumentDefinition{
            .description = "Enable color support",
            .longForm = "color",
            .isFlag = true
        }}
    };

    auto argument_count_size_t = static_cast<std::size_t>(argument_count);
    auto argument_handler_result = Arguments::init(argument_definitions, argument_count_size_t, arguments);

    if(!argument_handler_result.has_value())
    {
        std::println("{}", argument_handler_result.error());
        return -1;
    }

    auto argument_handler = *argument_handler_result;
    Configuration::enableColors = argument_handler.getFlag(option_flag_color);

    Game::start();

    return 0;
}

#include "arguments.hpp"

namespace t3g
{
    auto Arguments::init(const std::unordered_map<char, ArgumentDefinition>& definitions, std::size_t argument_count, const char** arguments) -> std::expected<Arguments, std::string>
    {
        std::unordered_map<char, std::string_view> value_arguments{};
        std::unordered_set<char> flag_arguments{};
        bool is_help{false};

        // Start at index 1, so as to ignore the 'executable-to-run' part of our command line arguments.
        for(std::size_t argument_index{1ul}; argument_index < argument_count; ++argument_index)
        {
            const auto& argument = std::string_view{arguments[argument_index]};

            if(argument.length() == 0ul) // Not sure if this is possible, but just to be sure
                continue;

            if(argument[0] != '-')
                return std::unexpected(error(definitions, argument_count, arguments, std::format("Invalid argument `{}`. All valid options must start with a `-` or `--`.", argument)));

            if(argument.length() < 2ul)
                return std::unexpected(error(definitions, argument_count, arguments, "Cannot have argument with empty name."));

            for(std::size_t i{1ul}; i < argument.length() && std::isalnum(argument[i]); ++i)
            {
                if(argument[i] == s_helpCommand)
                {
                    is_help = true;
                    continue;
                }

                auto find = definitions.find(argument[i]);

                if(find == definitions.end())
                    return std::unexpected(error(definitions, argument_count, arguments, std::format("Invalid short-format option `-{}`.", argument[i])));

                else if(!find->second.isFlag)
                    return std::unexpected(error(definitions, argument_count, arguments, std::format("Short-format option `-{}` specified in option group is not a flag-style option.", argument[i])));

                flag_arguments.insert(find->first);
            }
        }

        return Arguments(M{
            .definitions = definitions,
            .valueArguments = value_arguments,
            .flagArguments = flag_arguments
        });
    }

    const ArgumentDefinition Arguments::s_helpDefinition{
        .description = "Displays help about tic-tac-toe.",
        .longForm = "--help",
        .isFlag = true
    };
}



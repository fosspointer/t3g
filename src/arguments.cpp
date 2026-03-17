#include "arguments.hpp"
#include <algorithm>

namespace t3g
{
    auto Arguments::init(const std::unordered_map<char, ArgumentDefinition>& definitions, std::size_t argument_count, const char** arguments) -> std::expected<Arguments, std::string>
    {
        std::unordered_map<char, std::string_view> value_arguments{};
        std::unordered_set<char> flag_arguments{};
        bool is_help{false}, has_short_form{false};

        // Start at index 1, so as to ignore the 'executable-to-run' part of our command line arguments.
        for(std::size_t argument_index{1ul}; argument_index < argument_count; ++argument_index)
        {
            const auto& argument = std::string_view{arguments[argument_index]};

            if(argument.length() == 0ul) // Not sure if this is possible, but just to be sure
                continue;

            if(argument[0] != '-')
                return std::unexpected(error(definitions, std::format("Invalid argument `{}`. All valid options must start with a `-` or `--`.", argument)));

            if(argument.length() < 2ul)
                return std::unexpected(error(definitions, "Cannot have argument with empty name."));

            if(argument[1] != '-')
            {
                auto [got_help, error_message] = evaluateShortFormArgument(definitions, value_arguments, flag_arguments, arguments, argument_index);

                if(got_help)
                    is_help = true;
                else if(error_message.has_value())
                    return std::unexpected(error(definitions, *error_message));
                continue;
            }

            if(argument.length() < 3ul)
                return std::unexpected(error(definitions, "Cannot have long-form argument with empty name."));
            auto [got_help, error_message] = evaluateLongFormArgument(definitions, value_arguments, flag_arguments, arguments, argument_index);

            if(got_help)
                is_help = true;
            else if(error_message.has_value())
                return std::unexpected(error(definitions, *error_message));
        }

        return Arguments(M{
            .definitions = definitions,
            .valueArguments = value_arguments,
            .flagArguments = flag_arguments,
            .isHelp = is_help
        });
    }

    std::pair<bool, std::optional<std::string>> Arguments::evaluateShortFormArgument(
        const std::unordered_map<char, ArgumentDefinition>& definitions,
        std::unordered_map<char, std::string_view>& value_arguments,
        std::unordered_set<char> flag_arguments,
        const char** arguments,
        std::size_t& argument_index)
    {
        auto argument = std::string_view{arguments[argument_index]};
        std::size_t sub_argument_index{1ul};

        auto find = definitions.end();
        for(; sub_argument_index < argument.length() && std::isalpha(argument[sub_argument_index]); ++sub_argument_index)
        {
            auto letter = argument[sub_argument_index];

            if(letter == s_helpCommand)
                return std::make_pair(true, std::nullopt);

            find = definitions.find(letter);

            if(find == definitions.end())
                return std::make_pair(false, std::format("Invalid short-form option `-{}`.", letter));

            // Non-flags can't be grouped together unless there's a singular one at the end
            else if(!find->second.isFlag)
                break;

            // Insert flag
            flag_arguments.insert(find->first);
        }

        if(sub_argument_index != argument.length() - 1ul)
            return std::make_pair(false, std::format("Short-form `-{}` takes a value, so it must proceed all other options in its option group.", find->first));

        auto argument_value = std::string_view{arguments[++argument_index]};

        if(argument_value.empty() || argument_value[0ul] == '-')
            return std::make_pair(false, std::format("Value of option `-{}` must be a non-empty string which does not begin with hyphens.", find->first));

        value_arguments[find->first] = argument_value;

        return std::make_pair(false, std::nullopt);
    }

    std::pair<bool, std::optional<std::string>> Arguments::evaluateLongFormArgument(
        const std::unordered_map<char, ArgumentDefinition>& definitions,
        std::unordered_map<char, std::string_view>& value_arguments,
        std::unordered_set<char> flag_arguments,
        const char** arguments,
        std::size_t& argument_index)
    {
        auto argument = std::string_view{&arguments[argument_index][2ul]};
        auto is_valid = std::all_of(argument.begin(), argument.end(), [](char character)
        {
            return std::isalnum(static_cast<unsigned char>(character))
                || character == '_' 
                || character == '-';
        });

        if(!is_valid)
            return std::make_pair(false, std::format("Invalid option name: `--{}`. Option names can only contain alphanumeric characters, as well as hyphens and underscores.", argument));

        if(argument == s_helpDefinition.longForm)
            return std::make_pair(true, std::nullopt);

        auto find = findFromLongForm(definitions, argument);

        if(find == definitions.end())
            return std::make_pair(false, std::format("Invalid option `--{}`.", argument));

        if(find->second.isFlag)
            return std::make_pair(false, std::nullopt);

        auto argument_value = std::string_view{arguments[++argument_index]};

        if(argument_value.empty() || argument_value[0ul] == '-')
            return std::make_pair(false, std::format("Value of option `--{}` must be a non-empty string which does not begin with hyphens.", argument));

        value_arguments[find->first] = argument_value;
        return std::make_pair(false, std::nullopt);
    }
}



#pragma once
#include <cctype>
#include <cstddef>
#include <vector>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <format>
#include <expected>
#include <optional>

namespace t3g
{
    struct ArgumentDefinition final
    {
        std::string_view description;
        std::string_view longForm;
        bool isFlag;
    };

    class Arguments final
    {
    public:
        static std::expected<Arguments, std::string> init(const std::unordered_map<char, ArgumentDefinition>& definitions, std::size_t argument_count, const char** arguments);

        static std::string error(const std::unordered_map<char, ArgumentDefinition>& definitions, const std::string& error_message)
        {
            std::string result = "Error: ";
            result += help(definitions, error_message);
            return result;
        }

        static std::string help(const std::unordered_map<char, ArgumentDefinition>& definitions, std::string_view message)
        {
            std::string result{message};
            result += '\n';
            result += std::format(" -{} (--{}): {}\n", s_helpCommand, s_helpDefinition.longForm, s_helpDefinition.description);

            for(const auto& argument : definitions)
                result += std::format(" -{} (--{}): {}\n", argument.first, argument.second.longForm, argument.second.description);
            return result;
        }

        inline std::string help(std::string_view message)
        {
            return help(m.definitions, message);
        }

        bool getFlag(char name)
        {
            return m.flagArguments.find(name) != m.flagArguments.end();
        }

        std::optional<std::string_view> getValue(char name)
        {
            auto find = m.valueArguments.find(name);

            if(find == m.valueArguments.end())
                return std::nullopt;

            return std::make_optional(find->second);
        }

        inline bool isHelp() const { return m.isHelp; }
    private:
        static inline std::unordered_map<char, ArgumentDefinition>::const_iterator findFromLongForm(
            const std::unordered_map<char, ArgumentDefinition>& definitions,
            std::string_view long_form)
        {
            for(auto iterator = definitions.begin(); iterator != definitions.end(); ++iterator)
            {
                if(iterator->second.longForm == long_form)
                    return iterator;
            }

            return definitions.end();
        }

        static std::pair<bool, std::optional<std::string>> evaluateShortFormArgument(
            const std::unordered_map<char, ArgumentDefinition>& definitions,
            std::unordered_map<char, std::string_view>& value_arguments,
            std::unordered_set<char> flag_arguments,
            const char** arguments,
            std::size_t& argument_index);

        static std::pair<bool, std::optional<std::string>> evaluateLongFormArgument(
            const std::unordered_map<char, ArgumentDefinition>& definitions,
            std::unordered_map<char, std::string_view>& value_arguments,
            std::unordered_set<char> flag_arguments,
            const char** arguments,
            std::size_t& argument_index);

        struct M final
        {
            std::unordered_map<char, ArgumentDefinition> definitions;
            std::unordered_map<char, std::string_view> valueArguments;
            std::unordered_set<char> flagArguments;
            bool isHelp;
        } m;
        
        static constexpr char s_helpCommand = 'h';
        static constexpr ArgumentDefinition s_helpDefinition{
            .description = "Displays help about tic-tac-toe.",
            .longForm = "help",
            .isFlag = true
        };

        Arguments(const M& m) : m(m) {}
    };
}

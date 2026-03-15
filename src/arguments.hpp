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

        static std::string error(std::unordered_map<char, ArgumentDefinition> definitions, std::size_t argument_count, const char** arguments, const std::string& error_message)
        {
            std::string result;
            result += std::format("Error: {}\n", error_message);
            result += std::format("Usage: {}\n", arguments[0]);

            for(const auto& argument : definitions)
                result += std::format(" -{} (--{}): {}", argument.first, argument.second.longForm, argument.second.description);

            return result;
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
    private:
        struct M final
        {
            std::unordered_map<char, ArgumentDefinition> definitions;
            std::unordered_map<char, std::string_view> valueArguments;
            std::unordered_set<char> flagArguments;
        } m;
        
        static const ArgumentDefinition s_helpDefinition;
        static constexpr char s_helpCommand = 'h';
        Arguments(const M& m) : m(m) {}
    };
}

#pragma once

#include <string>
#include <functional>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <tuple>
#include <utility>
#include <unordered_map>

// TODO: replace bool return type by void and use exceptions instead

namespace parser {

    template <typename T>
    T parse_value(const std::string& in);

    // Some template helpers
    namespace detail {

        template <int n, typename First, typename ... Rest>
        struct nth {
            typedef typename nth<n - 1, Rest...>::type type;
        };

        template <typename First, typename ... Rest>
        struct nth<0, First, Rest...> {
            typedef First type;
        };

        static_assert(std::is_same<nth<0, std::string, int32_t>::type, std::string>::value,
                "Indexing wrong");
        static_assert(std::is_same<nth<1, std::string, int32_t>::type, int32_t>::value,
                "Indexing wrong");

        template <typename ... Types>
        struct apply_parser_impl {
            private:
                template <std::size_t ... Seq>
                static std::tuple<Types...> _apply(
                        const std::vector<std::string>& values,
                        std::index_sequence<Seq...>)
                {
                    std::tuple<Types...> parsed_values(
                        parser::parse_value<typename nth<Seq, Types...>::type>(values[Seq+1])...
                    );
                    return parsed_values;
                }
            public:
                static std::tuple<Types...> apply(
                        const std::vector<std::string>& values)
                {
                    return _apply(values, std::index_sequence_for<Types...>{});
                }
        };

        template <typename ... Types>
        std::tuple<Types...> parse_values(const std::vector<std::string>& values)
        {
            return apply_parser_impl<Types...>::apply(values);
        }

        template <typename ... Types, std::size_t ... Seq>
        void call_on_tuple_impl(std::function<void(Types...)> func,
                std::tuple<Types...> tup, std::index_sequence<Seq...>)
        {
            func(std::get<Seq>(tup)...);
        }

        template <typename ... Types>
        void call_on_tuple(std::function<void(Types...)> func,
                std::tuple<Types...> tup)
        {
            call_on_tuple_impl(func, tup, std::index_sequence_for<Types...>{});
        }

    } // namespace detail

    class parse_error : public std::runtime_error
    {
        public:
            parse_error(const std::string& what)
                : std::runtime_error(what)
            {}
    };

    class CommandParser
    {
        public:
        private:

            class CommandBase {
                public:
                    virtual ~CommandBase() {}
                    virtual void parse(const std::vector<std::string>& values) = 0;
            };

            template <typename ... Types>
            class Command : public CommandBase {
                private:
                    const std::string name;
                    std::function<void(Types...)> action;

                public:
                    Command(const std::string name, std::function<void(Types...)>& action)
                        : name(name), action(action)
                    {}
                    void parse(const std::vector<std::string>& values) override;
            };

            std::unordered_map<std::string, CommandBase*> commands;
        public:
            ~CommandParser();

            template <typename ... Types>
            void add_command(
                    const std::string name,
                    std::function<void(Types...)> action);

            void parse(const std::string& contents);
    };

    template <typename ... Types>
    void CommandParser::Command<Types...>::parse(const std::vector<std::string>& values)
    {
        if(values.size() - 1 != sizeof...(Types))
        {
            std::ostringstream sstream;
            sstream << "Command " << name << " expects " << sizeof...(Types) <<
                " arguments but " << (values.size() - 1) << " given";
            throw std::invalid_argument(sstream.str());
        }
        if constexpr(sizeof...(Types) == 0)
        {
            action();
        }
        else
        {
            std::tuple<Types...> parsed_values =
                detail::parse_values<Types...>(values);
            detail::call_on_tuple(action, parsed_values);
        }
    }

    template <typename ... Types>
    void CommandParser::add_command(const std::string name,
            std::function<void(Types...)> action)
    {
        if(name == "")
            throw std::invalid_argument("Cannot have an unnamed command");
        if(commands.find(name) != commands.end())
        {
            std::ostringstream sstream;
            sstream << "Command " << name << " already exists";
            throw std::invalid_argument(sstream.str());
        }
        commands[name] = new Command<Types...>(name, action);
    }

} // namespace parser

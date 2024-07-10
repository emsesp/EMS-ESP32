/*
 * uuid-console - Microcontroller console shell
 * Copyright 2019,2022  Simon Arlott
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <uuid/console.h>

#include <Arduino.h>

#include <algorithm>
#include <memory>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#ifndef __cpp_lib_make_unique
namespace std {

template <typename _Tp, typename... _Args>
inline unique_ptr<_Tp> make_unique(_Args &&... __args) {
    return unique_ptr<_Tp>(new _Tp(std::forward<_Args>(__args)...));
}

} // namespace std
#endif

namespace uuid {

namespace console {

void Commands::add_command(const string_vector & name, command_function function) {
    add_command(0, 0, 0, name, string_vector{}, function, nullptr);
}

void Commands::add_command(const string_vector & name, const string_vector & arguments, command_function function) {
    add_command(0, 0, 0, name, arguments, function, nullptr);
}

void Commands::add_command(const string_vector & name, const string_vector & arguments, command_function function, argument_completion_function arg_function) {
    add_command(0, 0, 0, name, arguments, function, arg_function);
}

void Commands::add_command(unsigned int context, const string_vector & name, command_function function) {
    add_command(context, 0, 0, name, string_vector{}, function, nullptr);
}

void Commands::add_command(unsigned int context, const string_vector & name, const string_vector & arguments, command_function function) {
    add_command(context, 0, 0, name, arguments, function, nullptr);
}

void Commands::add_command(unsigned int                 context,
                           const string_vector &        name,
                           const string_vector &        arguments,
                           command_function             function,
                           argument_completion_function arg_function) {
    add_command(context, 0, 0, name, arguments, function, arg_function);
}

void Commands::add_command(unsigned int context, unsigned int flags, const string_vector & name, command_function function) {
    add_command(context, flags, 0, name, string_vector{}, function, nullptr);
}

void Commands::add_command(unsigned int context, unsigned int flags, const string_vector & name, const string_vector & arguments, command_function function) {
    add_command(context, flags, 0, name, arguments, function, nullptr);
}

void Commands::add_command(unsigned int                 context,
                           unsigned int                 flags,
                           const string_vector &        name,
                           const string_vector &        arguments,
                           command_function             function,
                           argument_completion_function arg_function) {
    add_command(context, flags, 0, name, arguments, function, arg_function);
}

void Commands::add_command(unsigned int context, unsigned int flags, unsigned int not_flags, const string_vector & name, command_function function) {
    add_command(context, flags, not_flags, name, string_vector{}, function, nullptr);
}

void Commands::add_command(unsigned int          context,
                           unsigned int          flags,
                           unsigned int          not_flags,
                           const string_vector & name,
                           const string_vector & arguments,
                           command_function      function) {
    add_command(context, flags, not_flags, name, arguments, function, nullptr);
}

void Commands::add_command(unsigned int                 context,
                           unsigned int                 flags,
                           unsigned int                 not_flags,
                           const string_vector &        name,
                           const string_vector &        arguments,
                           command_function             function,
                           argument_completion_function arg_function) {
    commands_.emplace(std::piecewise_construct, std::forward_as_tuple(context), std::forward_as_tuple(flags, not_flags, name, arguments, function, arg_function));
}

Commands::Execution Commands::execute_command(Shell & shell, CommandLine && command_line) {
    auto      commands = find_command(shell, command_line);
    auto      longest  = commands.exact.crbegin();
    Execution result;

    result.error = nullptr;

    if (commands.exact.empty()) {
        result.error = "Command not found. Try 'help' for a list of commands.";
    } else if (commands.exact.count(longest->first) == 1) {
        auto &                   command = longest->second;
        std::vector<std::string> arguments;

        for (auto it = std::next(command_line->cbegin(), command->name_.size()); it != command_line->cend(); it++) {
            arguments.push_back(std::move(*it));
        }
        command_line.reset();

        if (commands.partial.upper_bound(longest->first) != commands.partial.end() && !arguments.empty()) {
            result.error = "Command not found. Try 'help' for a list of commands.";
        } else if (arguments.size() < command->minimum_arguments()) {
            result.error = "Not enough arguments for command";
        } else if (arguments.size() > command->maximum_arguments()) {
            result.error = "Too many arguments for command";
        } else {
            command->function_(shell, arguments);
        }
    } else {
        result.error = "Fatal error (multiple commands found)";
    }

    return result;
}

bool Commands::find_longest_common_prefix(const std::multimap<size_t, const Command *> & commands, std::vector<std::string> & longest_name) {
    size_t component_prefix = 0;
    size_t shortest_match   = commands.begin()->first;

    longest_name.reserve(shortest_match);

    {
        // Check if any of the commands have a common prefix of components
        auto & first     = commands.begin()->second->name_;
        bool   all_match = true;

        for (size_t length = 0; all_match && length < shortest_match; length++) {
            for (auto command_it = std::next(commands.begin()); command_it != commands.end(); command_it++) {
                if (*std::next(first.begin(), length) != *std::next(command_it->second->name_.begin(), length)) {
                    all_match = false;
                    break;
                }
            }

            if (all_match) {
                component_prefix = length + 1;
            }
        }

        auto name_it = first.begin();
        for (size_t i = 0; i < component_prefix; i++) {
            longest_name.push_back(std::move(*name_it));
            name_it++;
        }
    }

    if (component_prefix < shortest_match) {
        // Check if the next component has a common substring
        auto   first        = *std::next(commands.begin()->second->name_.begin(), component_prefix);
        bool   all_match    = true;
        size_t chars_prefix = 0;

        for (size_t length = 0; all_match; length++) {
            for (auto command_it = std::next(commands.begin()); command_it != commands.end(); command_it++) {
                // This relies on the null terminator character limiting the
                // length before it becomes longer than any of the strings

                if (pgm_read_byte(first + length) != pgm_read_byte((*std::next(command_it->second->name_.begin(), component_prefix)) + length)) {
                    all_match = false;
                    break;
                }

                // if (pgm_read_byte(reinterpret_cast<PGM_P>(first) + length)
                //     != pgm_read_byte(reinterpret_cast<PGM_P>(*std::next(command_it->second->name_.begin(), component_prefix)) + length)) {
                //     all_match = false;
                //     break;
                // }
            }

            if (all_match) {
                chars_prefix = length + 1;
            }
        }

        if (chars_prefix > 0) {
            longest_name.push_back(std::string(first).substr(0, chars_prefix));
            return false;
        }
    }

    return true;
}

std::string Commands::find_longest_common_prefix(const std::vector<std::string> & arguments) {
    auto & first        = *arguments.begin();
    bool   all_match    = true;
    size_t chars_prefix = 0;

    for (size_t length = 0; all_match; length++) {
        for (auto argument_it = std::next(arguments.begin()); argument_it != arguments.end(); argument_it++) {
            // This relies on the null terminator character limiting the
            // length before it becomes longer than any of the strings
            if (first[length] != (*argument_it)[length]) {
                all_match = false;
                break;
            }
        }

        if (all_match) {
            chars_prefix = length + 1;
        }
    }

    return arguments.begin()->substr(0, chars_prefix);
}

Commands::Completion Commands::complete_command(Shell & shell, const CommandLine & command_line) {
    auto       commands = find_command(shell, command_line);
    Completion result;

    auto   match = commands.partial.begin();
    size_t count;
    bool   multiple_matches;
    if (match != commands.partial.end()) {
        count = commands.partial.count(match->first);
        multiple_matches =
            count > 1 || commands.partial.size() > count || (!commands.exact.empty() && commands.exact.rbegin()->first >= command_line.total_size());
    } else if (!commands.exact.empty()) {
        // Use prev() because both iterators must be forwards
        match            = std::prev(commands.exact.end());
        count            = commands.exact.count(match->first);
        multiple_matches = false;
    } else {
        return result;
    }

    std::unique_ptr<Command>                         temp_command;
    std::vector<std::string>                         temp_command_name;
    std::multimap<size_t, const Command *>::iterator temp_command_it;

    if (multiple_matches && (commands.exact.empty() || command_line.total_size() > commands.exact.begin()->second->name_.size())) {
        // There are multiple matching commands, find the longest common prefix
        bool whole_components = find_longest_common_prefix(commands.partial, temp_command_name);

        // Construct a temporary command with the longest common prefix to use as the replacement
        if (!temp_command_name.empty() && command_line.total_size() <= temp_command_name.size()) {
            temp_command                      = std::make_unique<Command>(0, 0, string_vector{}, string_vector{}, nullptr, nullptr);
            count                             = 1;
            match                             = commands.partial.end();
            result.replacement.trailing_space = whole_components;

            for (auto & name : temp_command_name) {
                result.replacement->emplace_back(name);
            }
        }
    }

    if (count == 1 && !multiple_matches) {
        // Construct a replacement string for a single matching command
        auto & matching_command = match->second;

        for (auto & name : matching_command->name_) {
            result.replacement->push_back(name);
        }

        if (command_line.total_size() > result.replacement->size()
            && command_line.total_size() <= matching_command->name_.size() + matching_command->maximum_arguments()) {
            // Try to auto-complete arguments
            std::vector<std::string> arguments{std::next(command_line->cbegin(), result.replacement->size()), command_line->cend()};

            result.replacement->insert(result.replacement->end(), arguments.cbegin(), arguments.cend());
            result.replacement.trailing_space = command_line.trailing_space;

            auto        current_args_count = arguments.size();
            std::string last_argument;

            if (!command_line.trailing_space) {
                // Remove the last argument so that it can be auto-completed
                last_argument = std::move(result.replacement->back());
                result.replacement->pop_back();
                if (!arguments.empty()) {
                    arguments.pop_back();
                    current_args_count--;
                }
            }

            auto potential_arguments =
                matching_command->arg_function_ ? matching_command->arg_function_(shell, arguments, last_argument) : std::vector<std::string>{};

            // Remove arguments that can't match
            if (!command_line.trailing_space) {
                for (auto it = potential_arguments.begin(); it != potential_arguments.end();) {
                    if (it->rfind(last_argument, 0) == std::string::npos) {
                        it = potential_arguments.erase(it);
                    } else {
                        it++;
                    }
                }
            }

            // Auto-complete if there's something present in the last argument
            // or the only potential argument is an empty string.
            if (!command_line.trailing_space) {
                if (potential_arguments.size() == 1) {
                    if (last_argument == *potential_arguments.begin()) {
                        if (result.replacement->size() + 1 < matching_command->name_.size() + matching_command->maximum_arguments()) {
                            // Add a space because this argument is complete and there are more arguments for this command
                            result.replacement.trailing_space = true;
                        }
                    }

                    last_argument = *potential_arguments.begin();
                    potential_arguments.clear();

                    // Remaining help should skip the replaced argument
                    current_args_count++;
                } else if (potential_arguments.size() > 1) {
                    last_argument = find_longest_common_prefix(potential_arguments);
                }
            }

            // Put the last argument back
            if (!command_line.trailing_space) {
                result.replacement->push_back(std::move(last_argument));
            }

            CommandLine remaining_help;

            if (!potential_arguments.empty()) {
                // Remaining help should skip the suggested argument
                current_args_count++;
            }

            if (current_args_count < matching_command->maximum_arguments()) {
                remaining_help.escape_initial_parameters();

                for (auto it = std::next(matching_command->arguments_.cbegin(), current_args_count); it != matching_command->arguments_.cend(); it++) {
                    remaining_help->push_back(std::move(*it));
                }
            }

            if (potential_arguments.empty()) {
                if (!remaining_help->empty()) {
                    result.help.push_back(std::move(remaining_help));
                }
            } else {
                for (auto potential_argument : potential_arguments) {
                    CommandLine help;

                    help->emplace_back(potential_argument);

                    if (!remaining_help->empty()) {
                        help.escape_initial_parameters();
                        help->insert(help->end(), remaining_help->begin(), remaining_help->end());
                    }

                    result.help.push_back(std::move(help));
                }
            }
        } else if (result.replacement->size() < matching_command->name_.size() + matching_command->maximum_arguments()) {
            // Add a space because there are more arguments for this command
            result.replacement.trailing_space = true;
        }
    } else if (count != 0) {
        // Provide help for all of the potential commands
        for (auto command_it = commands.all.begin(); command_it != commands.all.end(); command_it++) {
            CommandLine help;

            auto line_it       = command_line->cbegin();
            auto flash_name_it = (*command_it)->name_.cbegin();

            if (temp_command) {
                // Skip parts of the command name/line when the longest common prefix was used
                size_t skip = temp_command_name.size();
                if (!result.replacement.trailing_space) {
                    skip--;
                }

                // Exact match that is shorter than the replacement
                if (flash_name_it + skip > (*command_it)->name_.cend()) {
                    continue;
                }

                flash_name_it += skip;
                while (line_it != command_line->cend()) {
                    line_it++;
                }
            }

            for (; flash_name_it != (*command_it)->name_.cend(); flash_name_it++) {
                std::string name = *flash_name_it;

                // Skip parts of the command name that match the command line
                if (line_it != command_line->cend()) {
                    if (name == *line_it++) {
                        continue;
                    } else {
                        line_it = command_line->cend();
                    }
                }

                help->emplace_back(name);
            }

            help.escape_initial_parameters();

            for (auto argument : (*command_it)->arguments_) {
                // Skip parts of the command arguments that exist in the command line
                if (line_it != command_line->cend()) {
                    line_it++;
                    continue;
                }

                help->push_back(argument);
            }

            result.help.push_back(std::move(help));
        }
    }

    if (multiple_matches && !commands.exact.empty() && result.replacement.total_size() == 0) {
        // Try to add a space to exact matches if there's no other partial match replacement
        auto longest = commands.exact.crbegin();

        if (commands.exact.count(longest->first) == 1) {
            for (auto & name : longest->second->name_) {
                result.replacement->push_back(name);
            }

            // Add a space because there are sub-commands for a command that has matched exactly
            result.replacement.trailing_space = true;
        }
    }

    // Don't try to shorten the command line or offer an identical replacement
    if (command_line.total_size() > result.replacement.total_size() || result.replacement == command_line) {
        result.replacement.reset();
    }

    return result;
}

Commands::Match Commands::find_command(Shell & shell, const CommandLine & command_line) {
    Match commands;
    auto  context_commands = commands_.equal_range(shell.context());

    for (auto it = context_commands.first; it != context_commands.second; it++) {
        auto & command = it->second;
        bool   match   = true;
        bool   exact   = true;

        if (!shell.has_flags(command.flags_, command.not_flags_)) {
            continue;
        }

        auto name_it = command.name_.cbegin();
        auto line_it = command_line->cbegin();

        for (; name_it != command.name_.cend() && line_it != command_line->cend(); name_it++, line_it++) {
            std::string name  = *name_it;
            size_t      found = name.rfind(*line_it, 0);

            if (found == std::string::npos) {
                match = false;
                break;
            } else if (line_it->length() != name.length()) {
                for (auto line_check_it = std::next(line_it); line_check_it != command_line->cend(); line_check_it++) {
                    if (!line_check_it->empty()) {
                        // If there's more in the command line then this can't match
                        match = false;
                    }
                }

                if (command_line.trailing_space) {
                    // If there's a trailing space in the command line then this can't be a partial match
                    match = false;
                }

                // Don't check the rest of the command if this is only a partial match
                break;
            }
        }

        if (name_it != command.name_.cend()) {
            exact = false;
        }

        if (match) {
            if (exact) {
                commands.exact.emplace(command.name_.size(), &command);
            } else {
                commands.partial.emplace(command.name_.size(), &command);
            }
            commands.all.push_back(&command);
        }
    }

    return commands;
}

Commands::Command::Command(unsigned int                 flags,
                           unsigned int                 not_flags,
                           const string_vector          name,
                           const string_vector          arguments,
                           command_function             function,
                           argument_completion_function arg_function)
    : flags_(flags)
    , not_flags_(not_flags)
    , name_(name)
    , arguments_(arguments)
    , function_(function)
    , arg_function_(arg_function) {
}

Commands::Command::~Command() {
}

size_t Commands::Command::minimum_arguments() const {
    return std::count_if(arguments_.cbegin(), arguments_.cend(), [](const char * argument) { return pgm_read_byte(argument) == '<'; });
}

} // namespace console

} // namespace uuid

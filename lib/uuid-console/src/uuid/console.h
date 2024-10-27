/*
 * uuid-console - Microcontroller console shell
 * Copyright 2019,2021-2022  Simon Arlott
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

#ifndef UUID_CONSOLE_H_
#define UUID_CONSOLE_H_

#include <Arduino.h>

#include <cstdarg>
#include <cstdint>
#include <deque>
#include <functional>
#include <iterator>
#include <limits>
#include <list>
#include <memory>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <uuid/common.h>
#include <uuid/log.h>

#ifndef UUID_LOG_THREAD_SAFE
#define UUID_LOG_THREAD_SAFE 0
#endif

#ifndef UUID_COMMON_STD_MUTEX_AVAILABLE
#define UUID_COMMON_STD_MUTEX_AVAILABLE 0
#endif

#if UUID_COMMON_STD_MUTEX_AVAILABLE
#define UUID_CONSOLE_THREAD_SAFE 1
#else
#define UUID_CONSOLE_THREAD_SAFE 0
#endif

#if UUID_CONSOLE_THREAD_SAFE
#include <mutex>
#endif

enum CommandFlags : uint8_t { USER = 0, ADMIN = (1 << 0), LOCAL = (1 << 1) };

namespace uuid {

/**
 * Console shell.
 *
 * - <a href="https://github.com/nomis/mcu-uuid-console/">Git Repository</a>
 * - <a href="https://mcu-uuid-console.readthedocs.io/">Documentation</a>
 */
namespace console {

/**
 * Thread-safe status of the library.
 *
 * @since 1.0.0
 */
#if UUID_COMMON_THREAD_SAFE && UUID_LOG_THREAD_SAFE && UUID_CONSOLE_THREAD_SAFE
static constexpr bool thread_safe = true;
#else
static constexpr bool thread_safe = false;
#endif

class Commands;
class CommandLine;
class Shell;

/**
 * Container of commands for use by a Shell.
 *
 * These should normally be stored in a std::shared_ptr and reused.
 *
 * @since 0.1.0
 */
class Commands {
  private:
    class Command;

  public:
    struct Completion;

    /**
	 * Result of a command execution operation.
	 *
	 * @since 0.1.0
	 */
    struct Execution {
        const char * error; /*!< Error message if the command could not be executed. @since 0.1.0 */
    };

    /**
	 * Function to handle a command.
	 *
	 * @param[in] shell Shell instance that is executing the command.
	 * @param[in] arguments Command line arguments.
	 * @since 0.1.0
	 */
    using command_function = std::function<void(Shell & shell, std::vector<std::string> & arguments)>;
    /**
	 * Function to obtain completions for a command line.
	 *
	 * This is a vector instead of set so that a custom sort order can
	 * be used. It should normally be sorted lexicographically so that
	 * the list of options is not confusing.
	 *
	 * @param[in] shell Shell instance that has a command line matching
	 *                  this command.
	 * @param[in] current_arguments Command line arguments prior to (but
	 *                              excluding) the argument being
	 *                              completed.
	 * @param[in] next_argument Next argument (the one being completed).
	 * @return Possible values for the next argument on the command
	 *         line.
	 * @since 2.0.0
	 */
    using argument_completion_function =
        std::function<const std::vector<std::string>(Shell & shell, const std::vector<std::string> & current_arguments, const std::string & next_argument)>;

    /**
	 * Available command for execution on a Shell.
	 *
	 * @since 0.9.0
	 */
    class AvailableCommand {
      public:
        /**
		 * Construct a new command that is available for execution on a Shell.
		 *
		 * @since 0.9.0
		 */
        AvailableCommand(const Command & command);

        /**
		 * Get the name of the command.
		 *
		 * @return Name of the command as a std::vector of strings.
		 * @since 0.9.0
		 */
        inline const std::vector<std::string> & name() const {
            return name_;
        }

        /**
		 * Get the help text of the command's arguments.
		 *
		 * @return Help text for arguments that the command accepts as a std::vector of strings.
		 * @since 0.9.0
		 */
        inline const std::vector<std::string> & arguments() const {
            return arguments_;
        }

        /**
		 * Get the function to be used when the command is executed.
		 *
		 * @return Function that can be used to execute the command.
		 * @since 0.9.0
		 */
        inline const command_function & function() const {
            return command_.function_;
        };

        /**
		 * Get the function to be used to perform argument completions for the command.
		 *
		 * @return Function that can be used to perform argument completions for the command.
		 * @since 0.9.0
		 */
        inline const argument_completion_function & arg_function() const {
            return command_.arg_function_;
        };

        /**
		 * Get the shell flags that must be set for this command to be available.
		 *
		 * @return Shell flags.
		 * @since 0.9.0
		 */
        inline int flags() const {
            return command_.flags_;
        }

        /**
		 * Get the shell flags that must not be set for this command to be available.
		 *
		 * @return Shell flags.
		 * @since 0.9.0
		 */
        inline int not_flags() const {
            return command_.not_flags_;
        }

      private:
        const Commands::Command & command_;   /*!< Command that is available. @since 0.9.0 */
        std::vector<std::string>  name_;      /*!< Name of the command. @since 0.9.0 */
        std::vector<std::string>  arguments_; /*!< Help text for arguments that the command accepts. @since 0.9.0 */
    };

    /**
	 * Available commands in the shell's context with the current flags.
	 *
	 * Iterators are invalidated if the commands, context or flags for
	 * the shell change.
	 *
	 * @since 0.9.0
	 */
    class AvailableCommands {
      public:
        using command_iterator = std::multimap<unsigned int, Command>::const_iterator; /*!< Type of the underlying command iterator. @since 0.9.0 */

        /**
		 * Iterator of available commands for execution on a Shell.
		 *
		 * @since 0.9.0
		 */
        class const_iterator {
          public:
            using iterator_category = std::bidirectional_iterator_tag; /*!< Category of this iterator (bidirectional).  @since 0.9.0 */
            using difference_type   = std::size_t;                     /*!< Type used by std::distance(). @since 0.9.0 */
            using value_type        = AvailableCommand;                /*!< Type of this iterator's values. @since 0.9.0 */
            using pointer           = const value_type *;              /*!< Pointer type of this iterator. @since 0.9.0 */
            using reference         = const value_type &;              /*!< Reference type of this iterator. @since 0.9.0 */

            /**
			 * Create an iterator describing an available command in a
			 * shell with the current flags.
			 *
			 * Iterators are invalidated if the commands, context or flags for
			 * the shell change.
			 *
			 * @param[in] shell Shell that is accessing commands.
			 * @param[in] begin Beginning of command iterators.
			 * @param[in] command Initial command iterator.
			 * @param[in] end End of command iterators.
			 * @since 0.9.0
			 */
            const_iterator(const Shell & shell, const command_iterator & begin, const command_iterator & command, const command_iterator & end);

            /**
			 * Dereference the current available command.
			 *
			 * @returns A pointer to the current available command,
			 *          that remains valid only while the iterator at
			 *          this position exists.
			 * @since 0.9.0
			 */
            inline reference operator*() const {
                return *available_command_;
            }
            /**
			 * Access the current available command.
			 *
			 * @returns A reference to the current available command,
			 *          that remains valid only while the iterator at
			 *          this position exists.
			 * @since 0.9.0
			 */
            inline pointer operator->() const {
                return available_command_.get();
            }

            /**
			 * Pre-increment the current iterator to the next
			 * available command.
			 *
			 * @returns The iterator to the next available command.
			 * @since 0.9.0
			 */
            const_iterator & operator++();
            /**
			 * Post-increment the current iterator to the next
			 * available command.
			 *
			 * @returns The current iterator.
			 * @since 0.9.0
			 */
            inline const_iterator operator++(int) {
                auto tmp = *this;
                ++(*this);
                return tmp;
            }

            /**
			 * Pre-decrement the current iterator to the previous
			 * available command.
			 *
			 * @returns The iterator to the previous available command.
			 * @since 0.9.0
			 */
            const_iterator & operator--();
            /**
			 * Post-decrement the current iterator to the previous
			 * available command.
			 *
			 * @returns The current iterator.
			 * @since 0.9.0
			 */
            inline const_iterator operator--(int) {
                auto tmp = *this;
                --(*this);
                return tmp;
            }

            /**
			 * Compare an available commands iterator to another available
			 * commands iterator for equality.
			 *
			 * @param[in] lhs Left-hand side available commands iterator.
			 * @param[in] rhs Right-hand side available commands iterator.
			 * @return True if the available commands iterators are equal, otherwise false.
			 * @since 0.9.0
			 */
            friend inline bool operator==(const const_iterator & lhs, const const_iterator & rhs) {
                return lhs.command_ == rhs.command_;
            }
            /**
			 * Compare an available commands iterator to another available
			 * commands iterator for inequality.
			 *
			 * @param[in] lhs Left-hand side available commands iterator.
			 * @param[in] rhs Right-hand side available commands iterator.
			 * @return True if the available commands iterators are not equal, otherwise false.
			 * @since 0.9.0
			 */
            friend inline bool operator!=(const const_iterator & lhs, const const_iterator & rhs) {
                return lhs.command_ != rhs.command_;
            }

          private:
            /**
			 * Update the available command from the current command iterator.
			 *
			 * @since 0.9.0
			 */
            void update();

            const Shell &                     shell_;             /*!< Shell that is accessing commands. @since 0.9.0 */
            const command_iterator            begin_;             /*!< Beginning of command iterators. @since 0.9.0 */
            command_iterator                  command_;           /*!< Current command iterator. @since 0.9.0 */
            const command_iterator            end_;               /*!< End of command iterators. @since 0.9.0 */
            std::shared_ptr<AvailableCommand> available_command_; /*!< Current available command. @since 0.9.0 */
        };

        /**
		 * Create an iterable object describing available commands in a
		 * shell with the current flags.
		 *
		 * Iterators are invalidated if the commands, context or flags for
		 * the shell change.
		 *
		 * @param[in] shell Shell that is accessing commands.
		 * @param[in] begin Beginning of command iterators.
		 * @param[in] end End of command iterators.
		 * @since 0.9.0
		 */
        AvailableCommands(const Shell & shell, const command_iterator & begin, const command_iterator & end);

        /**
		 * Returns an iterator to the first command.
		 *
		 * @since 0.9.0
		 */
        inline const_iterator begin() const {
            return cbegin();
        }
        /**
		 * Returns an iterator to the first command.
		 *
		 * @since 0.9.0
		 */
        const_iterator cbegin() const;
        /**
		 * Returns an iterator to the element following the last command.
		 *
		 * @since 0.9.0
		 */
        inline const_iterator end() const {
            return cend();
        }
        /**
		 * Returns an iterator to the element following the last command.
		 *
		 * @since 0.9.0
		 */
        const_iterator cend() const;

      private:
        const Shell &    shell_; /*!< Shell that is accessing commands. @since 0.9.0 */
        command_iterator begin_; /*!< Beginning of command iterators. @since 0.9.0 */
        command_iterator end_;   /*!< End of command iterators. @since 0.9.0 */
    };

    /**
	 * Construct a new container of commands for use by a Shell.
	 *
	 * This should normally be stored in a std::shared_ptr and reused.
	 *
	 * @since 0.1.0
	 */
    Commands()  = default;
    ~Commands() = default;

    /**
	 * Add a command with no arguments to the list of commands in this
	 * container.
	 *
	 * The shell context for the command will default to 0 and not
	 * require any flags for it to be available.
	 *
	 * @param[in] name Name of the command as a std::vector of strings.
	 * @param[in] function Function to be used when the command is
	 *                     executed.
	 * @since 0.2.0
	 */
    void add_command(const string_vector & name, command_function function);
    /**
	 * Add a command with arguments to the list of commands in this
	 * container.
	 *
	 * The shell context for the command will default to 0 and not
	 * require any flags for it to be available.
	 *
	 * @param[in] name Name of the command as a std::vector of strings.
	 * @param[in] arguments Help text for arguments that the command
	 *                      accepts as a std::vector of strings
	 *                      (use "<" to indicate a required argument).
	 * @param[in] function Function to be used when the command is
	 *                     executed.
	 * @since 0.2.0
	 */
    void add_command(const string_vector & name, const string_vector & arguments, command_function function);
    /**
	 * Add a command with arguments and automatic argument completion
	 * to the list of commands in this container.
	 *
	 * The shell context for the command will default to 0 and not
	 * require any flags for it to be available.
	 *
	 * @param[in] name Name of the command as a std::vector of strings.
	 * @param[in] arguments Help text for arguments that the command
	 *                      accepts as a std::vector of strings
	 *                      (use "<" to indicate a required argument).
	 * @param[in] function Function to be used when the command is
	 *                     executed.
	 * @param[in] arg_function Function to be used to perform argument
	 *                         completions for this command.
	 * @since 0.2.0
	 */
    void add_command(const string_vector & name, const string_vector & arguments, command_function function, argument_completion_function arg_function);

    /**
	 * Add a command with no arguments to the list of commands in this
	 * container.
	 *
	 * The command will not require any flags for it to be available.
	 *
	 * @param[in] context Shell context in which this command is
	 *                    available.
	 * @param[in] name Name of the command as a std::vector of 
	 *                 strings.
	 * @param[in] function Function to be used when the command is
	 *                     executed.
	 * @since 0.8.0
	 */
    void add_command(unsigned int context, const string_vector & name, command_function function);
    /**
	 * Add a command with arguments to the list of commands in this
	 * container.
	 *
	 * The command will not require any flags for it to be available.
	 *
	 * @param[in] context Shell context in which this command is
	 *                    available.
	 * @param[in] name Name of the command as a std::vector of 
	 *                 strings.
	 * @param[in] arguments Help text for arguments that the command
	 *                      accepts as a std::vector of  strings
	 *                      (use "<" to indicate a required argument).
	 * @param[in] function Function to be used when the command is
	 *                     executed.
	 * @since 0.8.0
	 */
    void add_command(unsigned int context, const string_vector & name, const string_vector & arguments, command_function function);
    /**
	 * Add a command with arguments and automatic argument completion
	 * to the list of commands in this container.
	 *
	 * The command will not require any flags for it to be available.
	 *
	 * @param[in] context Shell context in which this command is
	 *                    available.
	 * @param[in] name Name of the command as a std::vector of 
	 *                 strings.
	 * @param[in] arguments Help text for arguments that the command
	 *                      accepts as a std::vector of strings
	 *                      (use "<" to indicate a required argument).
	 * @param[in] function Function to be used when the command is
	 *                     executed.
	 * @param[in] arg_function Function to be used to perform argument
	 *                         completions for this command.
	 * @since 0.8.0
	 */
    void add_command(unsigned int                 context,
                     const string_vector &        name,
                     const string_vector &        arguments,
                     command_function             function,
                     argument_completion_function arg_function);
    /**
	 * Add a command with no arguments to the list of commands in this
	 * container.
	 *
	 * @param[in] context Shell context in which this command is
	 *                    available.
	 * @param[in] flags Shell flags that must be set for this command
	 *                  to be available.
	 * @param[in] name Name of the command as a std::vector of
	 *                 strings.
	 * @param[in] function Function to be used when the command is
	 *                     executed.
	 * @since 0.1.0
	 */
    void add_command(unsigned int context, unsigned int flags, const string_vector & name, command_function function);
    /**
	 * Add a command with arguments to the list of commands in this
	 * container.
	 *
	 * @param[in] context Shell context in which this command is
	 *                    available.
	 * @param[in] flags Shell flags that must be set for this command
	 *                  to be available.
	 * @param[in] name Name of the command as a std::vector of 
	 *                 strings.
	 * @param[in] arguments Help text for arguments that the command
	 *                      accepts as a std::vector of strings
	 *                      (use "<" to indicate a required argument).
	 * @param[in] function Function to be used when the command is
	 *                     executed.
	 * @since 0.1.0
	 */
    void add_command(unsigned int context, unsigned int flags, const string_vector & name, const string_vector & arguments, command_function function);
    /**
	 * Add a command with arguments and automatic argument completion
	 * to the list of commands in this container.
	 *
	 * @param[in] context Shell context in which this command is
	 *                    available.
	 * @param[in] flags Shell flags that must be set for this command
	 *                  to be available.
	 * @param[in] name Name of the command as a std::vector of 
	 *                 strings.
	 * @param[in] arguments Help text for arguments that the command
	 *                      accepts as a std::vector of strings
	 *                      (use "<" to indicate a required argument).
	 * @param[in] function Function to be used when the command is
	 *                     executed.
	 * @param[in] arg_function Function to be used to perform argument
	 *                         completions for this command.
	 * @since 0.1.0
	 */
    void add_command(unsigned int                 context,
                     unsigned int                 flags,
                     const string_vector &        name,
                     const string_vector &        arguments,
                     command_function             function,
                     argument_completion_function arg_function);
    /**
	 * Add a command with no arguments to the list of commands in this
	 * container.
	 *
	 * @param[in] context Shell context in which this command is
	 *                    available.
	 * @param[in] flags Shell flags that must be set for this command
	 *                  to be available.
	 * @param[in] not_flags Shell flags that must not be set for this command
	 *                      to be available.
	 * @param[in] name Name of the command as a std::vector of 
	 *                 strings.
	 * @param[in] function Function to be used when the command is
	 *                     executed.
	 * @since 0.8.0
	 */
    void add_command(unsigned int context, unsigned int flags, unsigned int not_flags, const string_vector & name, command_function function);
    /**
	 * Add a command with arguments to the list of commands in this
	 * container.
	 *
	 * @param[in] context Shell context in which this command is
	 *                    available.
	 * @param[in] flags Shell flags that must be set for this command
	 *                  to be available.
	 * @param[in] not_flags Shell flags that must not be set for this command
	 *                      to be available.
	 * @param[in] name Name of the command as a std::vector of 
	 *                 strings.
	 * @param[in] arguments Help text for arguments that the command
	 *                      accepts as a std::vector of strings
	 *                      (use "<" to indicate a required argument).
	 * @param[in] function Function to be used when the command is
	 *                     executed.
	 * @since 0.8.0
	 */
    void add_command(unsigned int          context,
                     unsigned int          flags,
                     unsigned int          not_flags,
                     const string_vector & name,
                     const string_vector & arguments,
                     command_function      function);
    /**
	 * Add a command with arguments and automatic argument completion
	 * to the list of commands in this container.
	 *
	 * @param[in] context Shell context in which this command is
	 *                    available.
	 * @param[in] flags Shell flags that must be set for this command
	 *                  to be available.
	 * @param[in] not_flags Shell flags that must not be set for this command
	 *                      to be available.
	 * @param[in] name Name of the command as a std::vector of 
	 *                 strings.
	 * @param[in] arguments Help text for arguments that the command
	 *                      accepts as a std::vector of strings
	 *                      (use "<" to indicate a required argument).
	 * @param[in] function Function to be used when the command is
	 *                     executed.
	 * @param[in] arg_function Function to be used to perform argument
	 *                         completions for this command.
	 * @since 0.8.0
	 */
    void add_command(unsigned int                 context,
                     unsigned int                 flags,
                     unsigned int                 not_flags,
                     const string_vector &        name,
                     const string_vector &        arguments,
                     command_function             function,
                     argument_completion_function arg_function);

    /**
	 * Execute a command for a Shell if it exists in the current
	 * context and with the current flags.
	 *
	 * @param[in] shell Shell that is executing the command.
	 * @param[in] command_line Command line parameters.
	 * @return An object describing the result of the command execution
	 *         operation.
	 * @since 0.1.0
	 */
    Execution execute_command(Shell & shell, CommandLine && command_line);

    /**
	 * Complete a partial command for a Shell if it exists in the
	 * current context and with the current flags.
	 *
	 * @param[in] shell Shell that is completing the command.
	 * @param[in] command_line Command line parameters.
	 * @return An object describing the result of the command
	 *         completion operation.
	 * @since 0.1.0
	 */
    Completion complete_command(Shell & shell, const CommandLine & command_line);

    /**
	 * Get the available commands in the current context and with the
	 * current flags.
	 *
	 * This iterable object and its iterators are invalidated if the
	 * commands, context or flags for the shell change.
	 *
	 * @param[in] shell Shell that is accessing commands.
	 * @return An iterable object describing the available commands.
	 * @since 0.9.0
	 */
    AvailableCommands available_commands(const Shell & shell) const;

  private:
    /**
	 * Command for execution on a Shell.
	 * @since 0.1.0
	 */
    class Command {
      public:
        /**
		 * Create a command for execution on a Shell.
		 *
		 * @param[in] flags Shell flags that must be set for this command
		 *                  to be available.
		 * @param[in] not_flags Shell flags that must not be set for this
		 *                      command to be available.
		 * @param[in] name Name of the command as a std::vector of 
		 *                 strings.
		 * @param[in] arguments Help text for arguments that the command
		 *                      accepts as a std::vector of strings
		 *                      (use "<" to indicate a required argument).
		 * @param[in] function Function to be used when the command is
		 *                     executed.
		 * @param[in] arg_function Function to be used to perform argument
		 *                         completions for this command.
		 * @since 0.8.0
		 */
        Command(unsigned int                 flags,
                unsigned int                 not_flags,
                const string_vector          name,
                const string_vector          arguments,
                command_function             function,
                argument_completion_function arg_function);
        ~Command();

        /**
		 * Determine the minimum number of arguments for this command
		 * based on the help text for the arguments that begin with the
		 * "<" character.
		 *
		 * @return The minimum number of arguments for this command.
		 * @since 0.1.0
		 */
        size_t minimum_arguments() const;
        /**
		 * Determine the maximum number of arguments for this command
		 * based on the length of help text for the arguments.
		 *
		 * @return The maximum number of arguments for this command.
		 * @since 0.1.0
		 */
        inline size_t maximum_arguments() const {
            return arguments_.size();
        }

        unsigned int                 flags_;        /*!< Shell flags that must be set for this command to be available. @since 0.1.0 */
        unsigned int                 not_flags_;    /*!< Shell flags that must not be set for this command to be available. @since 0.8.0 */
        const string_vector          name_;         /*!< Name of the command as a std::vector of strings. @since 0.1.0 */
        const string_vector          arguments_;    /*!< Help text for arguments that the command accepts as a std::vector of strings. @since 0.1.0 */
        command_function             function_;     /*!< Function to be used when the command is executed. @since 0.1.0 */
        argument_completion_function arg_function_; /*!< Function to be used to perform argument completions for this command. @since 0.1.0 */

      private:
        Command(const Command &)             = delete;
        Command & operator=(const Command &) = delete;
    };

    /**
	 * Result of a command find operation.
	 *
	 * Each matching command is returned in a map grouped by size.
	 * @since 0.1.0
	 */
    struct Match {
        std::multimap<size_t, const Command *> exact; /*!< Commands that match the command line exactly, grouped by the size of the command names. @since 0.1.0 */
        std::multimap<size_t, const Command *> partial; /*!< Commands that the command line partially matches, grouped by the size of the command names. @since 0.1.0 */
        std::vector<const Command *> all;               /*!< Commands that match the command line, in defined order. @since 0.7.6 */
    };

    /**
	 * Find commands by matching them against the command line.
	 *
	 * @param[in] shell Shell that is accessing commands.
	 * @param[in] command_line Command line parameters.
	 * @return An object describing the result of the command find
	 *         operation.
	 * @since 0.1.0
	 */
    Match find_command(Shell & shell, const CommandLine & command_line);

    /**
	 * Find the longest common prefix from a shortest match of commands.
	 *
	 * @param[in] commands All commands that matched (at least 2).
	 * @param[out] longest_name The longest common prefix as a list of
	 *                          strings.
	 * @return True if the longest common prefix is made up of whole
	 *          components, false if the last part is constructed from
	 *          a partial component.
	 * @since 0.1.0
	 */
    static bool find_longest_common_prefix(const std::multimap<size_t, const Command *> & commands, std::vector<std::string> & longest_name);

    /**
	 * Find the longest common prefix from a list of potential arguments.
	 *
	 * @param[in] arguments All potential arguments (at least 2).
	 * @return The longest common prefix, which could be empty.
	 * @since 0.1.0
	 */
    static std::string find_longest_common_prefix(const std::vector<std::string> & arguments);

    std::multimap<unsigned int, Command> commands_; /*!< Commands stored in this container, separated by context. @since 0.1.0 */
};

/**
 * Base class for a command shell.
 *
 * Must be constructed within a std::shared_ptr.
 *
 * @since 0.1.0
 */
class Shell : public std::enable_shared_from_this<Shell>, public uuid::log::Handler, public ::Stream {
  public:
    static constexpr size_t MAX_COMMAND_LINE_LENGTH = 80; /*!< Maximum length of a command line. @since 0.1.0 */
    static constexpr size_t MAX_LOG_MESSAGES        = 20; /*!< Maximum number of log messages to buffer before they are output. @since 0.1.0 */

    // added for EMS-ESP
    static constexpr uint8_t MAX_LINES = 5; /*!< Maximum lines in buffer */

    /**
	 * Function to handle the response to a password entry prompt.
	 *
	 * @param[in] shell Shell instance where the password entry prompt
	 *                  was requested.
	 * @param[in] completed Password entry at the prompt was either
	 *                      completed (true) or aborted (false).
	 * @param[in] password Password entered at the prompt (may be
	 *                     empty).
	 * @since 0.1.0
	 */
    using password_function = std::function<void(Shell & shell, bool completed, const std::string & password)>;
    /**
	 * Function to handle the end of an execution delay.
	 *
	 * @param[in] shell Shell instance where execution was delayed.
	 * @since 0.1.0
	 */
    using delay_function = std::function<void(Shell & shell)>;
    /**
	 * Function to handle a blocking operation.
	 *
	 * @param[in] shell Shell instance where execution is blocked.
	 * @param[in] stop Request to return so that the shell can stop
	 *                 (true) or continue (false).
	 * @return True if the blocking operation is finished, otherwise
	 *         false.
	 * @since 0.2.0
	 */
    using blocking_function = std::function<bool(Shell & shell, bool stop)>;

    /**
	 * Create a new Shell operating on a Stream with the given commands,
	 * default context and initial flags.
	 *
	 * The default context is put on the stack and cannot be removed.
	 *
	 * The Stream must remain valid until the Shell has been destroyed.
	 * Monitor the Shell using a std::weak_ptr and destroy the Stream
	 * only after it has expired.
	 *
	 * @param[in] stream Stream used for the input/output of this shell.
	 * @param[in] commands Commands available for execution in this shell.
	 * @param[in] context Default context for the shell.
	 * @param[in] flags Initial flags for the shell.
	 * @since 3.0.0
	 */
    Shell(Stream & stream, std::shared_ptr<Commands> commands, unsigned int context = 0, unsigned int flags = 0);

    ~Shell() = default;

    /**
	 * Loop through all registered shell objects.
	 *
	 * Call loop_one() on every Shell (if it has not been stopped).
	 * Any Shell that is stopped is then unregistered.
	 *
	 * @since 0.1.0
	 */
    static void loop_all();

    /**
	 * Perform startup process for this shell.
	 *
	 * Register as a uuid::log::Handler at the uuid::log::Level::NOTICE
	 * log level, output the banner and	register this Shell with the
	 * loop_all() set.
	 *
	 * The started() function will be called after startup is complete.
	 *
	 * Do not call this function more than once.
	 *
	 * @since 0.1.0
	 */
    void start();
    /**
	 * Perform one execution step of this shell.
	 *
	 * Depending on the current mode, either read input characters and
	 * process them or check if an execution delay has passed.
	 *
	 * @since 0.1.0
	 */
    void loop_one();
    /**
	 * Determine if this shell is still running.
	 *
	 * @return Running status of this Shell, false if the Shell has
	 *         been stopped using stop().
	 * @since 0.1.0
	 */
    bool running() const;
    /**
	 * Stop this shell from running.
	 *
	 * If the shell is currently executing a blocking function, that
	 * must complete before the shell will stop.
	 *
	 * It is not possible to restart the Shell, which must be destroyed
	 * after it has been stopped.
	 *
	 * @since 0.1.0
	 */
    void stop();

    /**
	 * Get the built-in uuid::log::Logger instance for shells.
	 *
	 * @return Logger instance.
	 * @since 0.1.0
	 */
    static const uuid::log::Logger & logger();
    /**
	 * Add a new log message.
	 *
	 * This will be put in a queue for output at the next loop_one()
	 * process. The queue has a maximum size of maximum_log_messages()
	 * and will discard the oldest message first.
	 *
	 * @param[in] message New log message, shared by all handlers.
	 * @since 0.1.0
	 */
    virtual void operator<<(std::shared_ptr<uuid::log::Message> message) override;
    /**
	 * Get the current log level.
	 *
	 * This only affects newly received log messages, not messages that
	 * have already been queued.
	 *
	 * @return The current log level.
	 * @since 0.6.0
	 */
    uuid::log::Level log_level() const;
    /**
	 * Set the current log level.
	 *
	 * This only affects newly received log messages, not messages that
	 * have already been queued.
	 *
	 * @param[in] level Minimum log level that the shell will receive
	 *                  messages for.
	 * @since 0.6.0
	 */
    void log_level(uuid::log::Level level);

    /**
	 * Get the maximum length of a command line.
	 *
	 * @return The maximum length of a command line in bytes.
	 * @since 0.6.0
	 */
    size_t maximum_command_line_length() const;
    /**
	 * Set the maximum length of a command line.
	 *
	 * Defaults to Shell::MAX_COMMAND_LINE_LENGTH.
	 *
	 * @param[in] length The maximum length of a command line in bytes.
	 * @since 0.6.0
	 */
    void maximum_command_line_length(size_t length);
    /**
	 * Get the maximum number of queued log messages.
	 *
	 * @return The maximum number of queued log messages.
	 * @since 0.6.0
	 */
    size_t maximum_log_messages() const;
    /**
	 * Set the maximum number of queued log messages.
	 *
	 * Defaults to Shell::MAX_LOG_MESSAGES.
	 *
	 * @param[in] count The maximum number of queued log messages.
	 * @since 0.6.0
	 */
    void maximum_log_messages(size_t count);
    /**
	 * Get the idle timeout.
	 *
	 * @return The idle timeout in seconds.
	 * @since 0.7.0
	 */
    unsigned long idle_timeout() const;
    /**
	 * Set the idle timeout.
	 *
	 * Defaults to 0 (no idle timeout).
	 *
	 * @param[in] timeout Idle timeout in seconds.
	 * @since 0.7.0
	 */
    void idle_timeout(unsigned long timeout);

    /**
	 * Get the context at the top of the stack.
	 *
	 * The current context affects which commands are available.
	 *
	 * @return Current shell context.
	 * @since 0.1.0
	 */
    inline unsigned int context() const {
        if (!context_.empty()) {
            return context_.back();
        } else {
            return 0;
        }
    }
    /**
	 * Push a new context onto the stack.
	 *
	 * The current context affects which commands are available.
	 *
	 * @param[in] context New context.
	 * @since 0.1.0
	 */
    inline void enter_context(unsigned int context) {
        context_.emplace_back(context);
    }
    /**
	 * Pop a context off the stack.
	 *
	 * The current context affects which commands are available.
	 *
	 * @return True if there was a context to be exited, false if there
	 *         are no more contexts to exit.
	 * @since 0.1.0
	 */
    virtual bool exit_context();

    /**
	 * Add one or more flags to the current flags.
	 *
	 * Flags are not affected by execution context. The current flags
	 * affect which commands are available (for access control).
	 *
	 * @param[in] flags Flag bits to add.
	 * @since 0.1.0
	 */
    inline void add_flags(unsigned int flags) {
        flags_ |= flags;
    }
    /**
	 * Check if the current flags include all of the specified flags
	 * and none of the specified not_flags.
	 *
	 * Flags are not affected by execution context. The current flags
	 * affect which commands are available (for access control).
	 *
	 * @param[in] flags Flag bits to check for presence of.
	 * @param[in] not_flags Flag bits to check for absence of.
	 * @return True if the current flags includes all of the specified
	 *         flags and none of the specified not_flags, otherwise false.
	 * @since 0.8.0
	 */
    inline bool has_flags(unsigned int flags, unsigned int not_flags = 0) const {
        return has_all_flags(flags) && !has_any_flags(not_flags);
    }
    /**
	 * Check if the current flags include all of the specified flags.
	 *
	 * Flags are not affected by execution context. The current flags
	 * affect which commands are available (for access control).
	 *
	 * @param[in] flags Flag bits to check for.
	 * @return True if the current flags includes all of the specified
	 *         flags, otherwise false.
	 * @since 0.8.0
	 */
    inline bool has_all_flags(unsigned int flags) const {
        return (flags_ & flags) == flags;
    }
    /**
	 * Check if the current flags include any of the specified flags.
	 *
	 * Flags are not affected by execution context. The current flags
	 * affect which commands are available (for access control).
	 *
	 * @param[in] flags Flag bits to check for.
	 * @return True if the current flags includes any of the specified
	 *         flags, otherwise false.
	 * @since 0.8.0
	 */
    inline bool has_any_flags(unsigned int flags) const {
        return (flags_ & flags) != 0;
    }
    /**
	 * Remove one or more flags from the current flags.
	 *
	 * Flags are not affected by execution context. The current flags
	 * affect which commands are available (for access control).
	 *
	 * @param[in] flags Flag bits to remove.
	 * @since 0.1.0
	 */
    inline void remove_flags(unsigned int flags) {
        flags_ &= ~flags;
    }

    /**
	 * Prompt for a password to be entered on this shell.
	 *
	 * Password entry is not visible and can be interrupted by the
	 * user.
	 *
	 * The shell must not be currently executing a blocking function.
	 *
	 * @param[in] prompt Message to display prompting for password
	 *                   input.
	 * @param[in] function Function to be executed after the password
	 *                     has been entered prior to resuming normal
	 *                     execution.
	 * @since 0.1.0
	 */
    void enter_password(const char * prompt, password_function function);

    /**
	 * Stop executing anything on this shell for a period of time.
	 *
	 * There is an assumption that 2^64 milliseconds uptime will always
	 * be enough time for this delay process.
	 *
	 * The shell must not be currently executing a blocking function.
	 *
	 * @param[in] ms Time in milliseconds to delay execution for.
	 * @param[in] function Function to be executed at a future time,
	 *                     prior to resuming normal execution.
	 * @since 0.1.0
	 */
    void delay_for(unsigned long ms, delay_function function);

    /**
	 * Stop executing anything on this shell until a future time is
	 * reached.
	 *
	 * There is an assumption that 2^64 milliseconds uptime will always
	 * be enough time for this delay process.
	 *
	 * The reference time is uuid::get_uptime_ms().
	 *
	 * The shell must not be currently executing a blocking function.
	 *
	 * @param[in] ms Uptime in the future (in milliseconds) when the
	 *               function should be executed.
	 * @param[in] function Function to be executed at a future time,
	 *                     prior to resuming normal execution.
	 * @since 0.1.0
	 */
    void delay_until(uint64_t ms, delay_function function);

    /**
	 * Execute a blocking function on this shell until it returns true.
	 *
	 * The function will be executed every time loop_one() is called,
	 * which allows commands that do not complete immediately to be run
	 * asynchronously.
	 *
	 * The shell must not be currently executing a blocking function.
	 *
	 * @param[in] function Function to be executed on every loop_one()
	 *                     until normal execution resumes.
	 */
    void block_with(blocking_function function);

    /**
	 * Check for available input.
	 *
	 * The shell must be currently executing a blocking function
	 * otherwise it will always return 0.
	 *
	 * @return The number of bytes available to read.
	 * @since 0.2.0
	 */
    int available() final override;
    /**
	 * Read one byte from the available input.
	 *
	 * The shell must be currently executing a blocking function
	 * otherwise it will always return -1.
	 *
	 * @return An unsigned char if input is available, otherwise -1.
	 * @since 0.2.0
	 */
    int read() final override;
    /**
	 * Read one byte from the available input without advancing to the
	 * next one.
	 *
	 * The shell must be currently executing a blocking function
	 * otherwise it will always return -1.
	 *
	 * @return An unsigned char if input is available, otherwise -1.
	 * @since 0.2.0
	 */
    int peek() final override;

    /**
	 * Write one byte to the output stream.
	 *
	 * @param[in] data Data to be output.
	 * @return The number of bytes that were output.
	 * @since 3.0.0
	 */
    size_t write(uint8_t data) final override;
    /**
	 * Write an array of bytes to the output stream.
	 *
	 * @param[in] buffer Buffer to be output.
	 * @param[in] size Length of the buffer.
	 * @return The number of bytes that were output.
	 * @since 3.0.0
	 */
    size_t write(const uint8_t * buffer, size_t size) final override;
    /**
	 * Does nothing.
	 *
	 * This is a pure virtual function in Arduino's Stream class, which
	 * makes no sense because that class is for input and this is an
	 * output function. Later versions move it to Print as an empty
	 * virtual function so this is here for backward compatibility.
	 *
	 * @since 3.0.0
	 */
    void flush() final override;

    using ::Print::print; /*!< Include standard Arduino print() functions. */
    /**
	 * Output a string.
	 *
	 * @param[in] data String to be output.
	 * @return The number of bytes that were output.
	 * @since 0.1.0
	 */
    size_t print(const std::string & data);
    using ::Print::println; /*!< Include standard Arduino println() functions. */
    /**
	 * Output a string followed by CRLF end of line characters.
	 *
	 * @param[in] data String to be output.
	 * @return The number of bytes that were output, including CRLF.
	 * @since 0.1.0
	 */
    size_t println(const std::string & data);
    /**
	 * Output a message.
	 *
	 * @param[in] format Format string.
	 * @param[in] ... Format string arguments.
	 * @return The number of bytes that were output.
	 * @since 0.1.0
	 */
    size_t printf(const char * format, ...) /* __attribute__((format(printf, 2, 3))) */;

    /**
	 * Output a message followed by CRLF end of line characters.
	 *
	 * @param[in] format Format string.
	 * @param[in] ... Format string arguments.
	 * @return The number of bytes that were output, including CRLF.
	 * @since 0.1.0
	 */
    size_t printfln(const char * format, ...) /* __attribute__((format (printf, 2, 3))) */;

    /**
	 * Get the available commands in the current context and with the
	 * current flags.
	 *
	 * The iterable object and its iterators are invalidated if the
	 * commands, context or flags for the shell change.
	 *
	 * @return An iterable object describing the available commands.
	 * @since 0.9.0
	 */
    Commands::AvailableCommands available_commands() const;
    /**
	 * Output a list of all available commands with their arguments.
	 *
	 * @since 0.4.0
	 */
    void print_all_available_commands();

  protected:
    /**
	 * Output ANSI escape sequence to erase the current line.
	 *
	 * @since 0.1.0
	 */
    virtual void erase_current_line();
    /**
	 * Output ANSI escape sequence to erase characters.
	 *
	 * @param[in] count The number of characters to erase.
	 * @since 0.1.0
	 */
    virtual void erase_characters(size_t count);

    /**
	 * Startup complete event.
	 *
	 * The shell is ready to start executing commands.
	 * @since 0.1.0
	 */
    virtual void started();
    /**
	 * Output the startup banner.
	 *
	 * There is no default banner.
	 * @since 0.1.0
	 */
    virtual void display_banner();
    /**
	 * Get the hostname to be included in the command prompt.
	 *
	 * Defaults to "".
	 *
	 * @return The current hostname of the device, or an empty string
	 *         for no hostname.
	 * @since 0.1.0
	 */
    virtual std::string hostname_text();
    /**
	 * Get the text indicating the current context, to be included in
	 * the command prompt.
	 *
	 * Defaults to "".
	 *
	 * @return Text indicating the current context, or an empty string
	 *         for no context description.
	 * @since 0.1.0
	 */
    virtual std::string context_text();
    /**
	 * Get the prefix to be used at the beginning of the command
	 * prompt.
	 *
	 * Defaults to "".
	 *
	 * @return Text for the beginning of the command prompt, or an
	 *         empty string.
	 * @since 0.1.0
	 */
    virtual std::string prompt_prefix();
    /**
	 * Get the prefix to be used at the end of the command prompt.
	 *
	 * Defaults to "$".
	 *
	 * @return Text for the end of the command prompt, or an empty
	 *         string.
	 * @since 0.1.0
	 */
    virtual std::string prompt_suffix();
    /**
	 * The end of transmission character has been received.
	 *
	 * A command can be invoked using invoke_command(). All other
	 * actions may result in an inconsistent display of the command
	 * prompt. If the shell is stopped without invoking a command then
	 * a blank line should usually be printed first.
	 *
	 * If an idle timeout is set then the default action (since 0.7.2)
	 * is to stop the shell, otherwise there is no default action.
	 * @since 0.1.0
	 */
    virtual void end_of_transmission();
    /**
	 * Stop request event.
	 *
	 * The shell is going to stop executing.
	 * @since 0.1.0
	 */
    virtual void stopped();

    // change for EMS-ESP to make public so can be used in test.cpp
  public:
    /**
	 * Invoke a command on the shell.
	 *
	 * This will output a prompt with the provided command line and
	 * then try to execute it.
	 *
	 * Intended for use from end_of_transmission() to execute an "exit"
	 * or "logout" command.
	 *
	 * @param[in] line The command line to be executed.
	 * @since 0.1.0
	 */
    void invoke_command(const std::string & line);

  private:
    /**
	 * Current mode of the shell.
	 *
	 * @since 0.1.0
	 */
    enum class Mode : uint8_t {
        NORMAL,   /*!< Normal command execution. @since 0.1.0 */
        PASSWORD, /*!< Password entry prompt. @since 0.1.0 */
        DELAY,    /*!< Delay execution until a future time. @since 0.1.0 */
        BLOCKING, /*!< Block execution by calling a function repeatedly. @since 0.2.0 */
    };

    /**
	 * Base class of data for a shell mode.
	 *
	 * @since 0.1.0
	 */
    class ModeData {
      public:
        virtual ~ModeData() = default;

      protected:
        ModeData() = default;
    };

    /**
	 * Data for the Mode::PASSWORD shell mode.
	 *
	 * @since 0.1.0
	 */
    class PasswordData : public ModeData {
      public:
        /**
		 * Create Mode::PASSWORD shell mode data.
		 *
		 * @param[in] password_prompt Message to display prompting for
		 *                            password input.
		 * @param[in] password_function Function to be executed after
		 *                              the password has been entered
		 *                              prior to resuming normal
		 *                              execution.
		 * @since 0.1.0
		 */
        PasswordData(const char * password_prompt, password_function && password_function);
        ~PasswordData() override = default;

        const char *      password_prompt_;   /*!< Prompt requesting password input. @since 0.1.0 */
        password_function password_function_; /*!< Function to execute after password entry. @since 0.1.0 */
    };

    /**
	 * Data for the Mode::DELAY shell mode.
	 *
	 * @since 0.1.0
	 */
    class DelayData : public ModeData {
      public:
        /**
		 * Create Mode::DELAY shell mode data.
		 *
		 * @param[in] delay_time Uptime in the future (in milliseconds)
		 *                       when the function should be executed.
		 * @param[in] delay_function Function to be executed at a
		 *                           future time, prior to resuming
		 *                           normal execution.
		 * @since 0.1.0
		 */
        DelayData(uint64_t delay_time, delay_function && delay_function);
        ~DelayData() override = default;

        uint64_t       delay_time_;     /*!< Future uptime to resume execution (in milliseconds). @since 0.1.0 */
        delay_function delay_function_; /*!< Function execute after delay. @since 0.1.0 */
    };

    /**
	 * Data for the Mode::BLOCKING shell mode.
	 *
	 * @since 0.2.0
	 */
    class BlockingData : public ModeData {
      public:
        /**
		 * Create Mode::DELAY shell mode data.
		 *
		 * @param[in] blocking_function Function to be executed on
		 *                              every loop_one() until it
		 *                              returns true.
		 * @since 0.2.0
		 */
        explicit BlockingData(blocking_function && blocking_function);
        ~BlockingData() override = default;

        blocking_function blocking_function_;         /*!< Function executed on every loop_one(). @since 0.2.0 */
        bool              consume_line_feed_ = true;  /*!< Stream input should try to consume the first line feed following a carriage return. @since 0.2.0 */
        bool              stop_              = false; /*!< There is a stop pending for the shell. @since 0.2.0 */
    };

    /**
	 * Log message that has been queued.
	 *
	 * Contains an identifier sequence to indicate when log messages
	 * could not be output because the queue discarded one or more
	 * messages.
	 *
	 * @since 0.1.0
	 */
    class QueuedLogMessage {
      public:
        /**
		 * Create a queued log message.
		 *
		 * @param[in] id Identifier to use for the log message on the queue.
		 * @param[in] content Log message content.
		 * @since 0.1.0
		 */
        QueuedLogMessage(unsigned long id, std::shared_ptr<uuid::log::Message> && content);
        ~QueuedLogMessage() = default;

        unsigned long                             id_;      /*!< Sequential identifier for this log message. @since 0.1.0 */
        std::shared_ptr<const uuid::log::Message> content_; /*!< Log message content. @since 0.1.0 */
    };

    Shell(const Shell &)             = delete;
    Shell & operator=(const Shell &) = delete;

    /**
	 * Get registered running shells to be executed.
	 *
	 * @return Registered running shells to be executed.
	 * @since 0.7.4
	 */
    static std::set<std::shared_ptr<Shell>> & registered_shells();

    /**
	 * Perform one execution step in Mode::NORMAL mode.
	 *
	 * Read characters and execute commands or invoke tab completion.
	 *
	 * @since 0.1.0
	 */
    void loop_normal();
    /**
	 * Perform one execution step in Mode::PASSWORD mode.
	 *
	 * Read characters until interrupted or password entry is complete.
	 *
	 * @since 0.1.0
	 */
    void loop_password();
    /**
	 * Perform one execution step in Mode::DELAY mode.
	 *
	 * Wait until the delay uptime is reached.
	 *
	 * @since 0.1.0
	 */
    void loop_delay();
    /**
	 * Perform one execution step in Mode::BLOCKING mode.
	 *
	 * Wait until the function returns true.
	 *
	 * @since 0.2.0
	 */
    void loop_blocking();

    /**
	 * Output a prompt on the shell.
	 *
	 * Based on the current mode this will output the appropriate text,
	 * e.g. the command prompt with the current command line (for
	 * Mode::NORMAL mode).
	 *
	 * @since 0.1.0
	 */
    void display_prompt();
    /**
	 * Output queued log messages for this shell.
	 *
	 * @since 0.1.0
	 */
    void output_logs();
    /**
	 * Try to execute a command with the current command line.
	 *
	 * @since 0.1.0
	 */
    void process_command();
    /**
	 * Try to complete a command from the current command line.
	 *
	 * @since 0.1.0
	 */
    void process_completion();
    /**
	 * Finish password entry.
	 *
	 * The entered password is in the command line buffer.
	 *
	 * @param[in] completed Password entry at the prompt was either
	 *                      completed (true) or aborted (false).
	 * @since 0.1.0
	 */
    void process_password(bool completed);

    /**
	 * Check idle timeout expiry.
	 *
	 * @since 0.7.2
	 */
    void check_idle_timeout();

    /**
	 * Delete a word from the command line buffer.
	 *
	 * @param[in] display True if output to erased characters is
	 *                    required, false for no output.
	 * @since 0.1.0
	 */
    void delete_buffer_word(bool display);

    /**
	 * Output a message.
	 *
	 * @param[in] format Format string.
	 * @param[in] ap Variable arguments pointer for format string.
	 * @return The number of bytes that were output.
	 * @since 0.1.0
	 */
    size_t vprintf(const char * format, va_list ap);

    // added for EMS-ESP
    void        set_command_str(const char * str);
    std::string line_old_[MAX_LINES]; /*!< old Command line buffer.*/
    uint8_t     line_no_ = 0;
    uint8_t     cursor_  = 0; /*!< cursor position from end of line */
    uint8_t     esc_     = 0; /*!< esc sequence running */

    Stream &                  stream_;    /*!< Stream used for the input/output of this shell. @since 3.0.0 */
    std::shared_ptr<Commands> commands_;  /*!< Commands available for execution in this shell. @since 0.1.0 */
    std::deque<unsigned int>  context_;   /*!< Context stack for this shell. Affects which commands are available. Should never be empty. @since 0.1.0 */
    unsigned int              flags_ = 0; /*!< Current flags for this shell. Affects which commands are available. @since 0.1.0 */
#if UUID_CONSOLE_THREAD_SAFE
    mutable std::mutex mutex_; /*!< Mutex for queued log messages. @since 1.0.0 */
#endif
    unsigned long               log_message_id_ = 0;                      /*!< The next identifier to use for queued log messages. @since 0.1.0 */
    std::list<QueuedLogMessage> log_messages_;                            /*!< Queued log messages, in the order they were received. @since 0.1.0 */
    size_t                      maximum_log_messages_ = MAX_LOG_MESSAGES; /*!< Maximum command line length in bytes. @since 0.6.0 */
    std::string                 line_buffer_; /*!< Command line buffer. Limited to maximum_command_line_length() bytes. @since 0.1.0 */
    size_t                      maximum_command_line_length_ = MAX_COMMAND_LINE_LENGTH; /*!< Maximum command line length in bytes. @since 0.6.0 */
    unsigned char               previous_  = 0; /*!< Previous character that was entered on the command line. Used to detect CRLF line endings. @since 0.1.0 */
    Mode                        mode_      = Mode::NORMAL; /*!< Current execution mode. @since 0.1.0 */
    std::unique_ptr<ModeData>   mode_data_ = nullptr;      /*!< Data associated with the current execution mode. @since 0.1.0 */
    bool                        stopped_   = false;        /*!< Indicates that the shell has been stopped. @since 0.1.0 */
    bool prompt_displayed_ = false; /*!< Indicates that a command prompt has been displayed, so that the output of invoke_command() is correct. @since 0.1.0 */
    uint64_t idle_time_    = 0;     /*!< Time the shell became idle. @since 0.7.0 */
    uint64_t idle_timeout_ = 0;     /*!< Idle timeout (in milliseconds). @since 0.7.0 */
};

/**
 * Representation of a command line, with parameters separated by
 * spaces and an optional trailing space.
 *
 * @since 0.4.0
 */
class CommandLine {
  public:
    /**
	 * Create an empty command line.
	 *
	 * @since 0.4.0
	 */
    CommandLine() = default;

    /**
	 * Parse a command line into separate parameters using built-in
	 * escaping rules.
	 *
	 * @param[in] line Command line to parse.
	 * @since 0.4.0
	 */
    explicit CommandLine(const std::string & line);

    /**
	 * Create a command line from one or more vectors of parameters.
	 *
	 * @param[in] arguments Vectors of parameters to add to the command
	 *                      line.
	 * @since 0.4.0
	 */
    explicit CommandLine(std::initializer_list<const std::vector<std::string>> arguments);

    ~CommandLine() = default;

#ifdef UNIT_TEST
    CommandLine(CommandLine &&)                                              = default;
    CommandLine & operator=(CommandLine &&)                                  = default;
    CommandLine(const CommandLine &) __attribute__((deprecated))             = default;
    CommandLine & operator=(const CommandLine &) __attribute__((deprecated)) = default;
#endif

    /**
	 * Format a command line from separate parameters using built-in
	 * escaping rules.
	 *
	 * @param[in] reserve String buffer size to preallocate.
	 * @return A command line, with escaping of characters sufficient
	 *         to reproduce the same command line parameters when
	 *         parsed.
	 * @since 0.4.0
	 */
    std::string to_string(size_t reserve = Shell::MAX_COMMAND_LINE_LENGTH) const;

    /**
	 * Get the total size of the command line parameters, taking into
	 * account the trailing space.
	 *
	 * @return The size of the command line parameters, plus 1 for
	 *         a trailing space.
	 * @since 0.4.0
	 */
    inline size_t total_size() const {
        return parameters_.size() + (trailing_space ? 1 : 0);
    }
    /**
	 * Reset this command line's data.
	 *
	 * Clears the parameter list and removes any trailing space.
	 *
	 * @since 0.4.0
	 */
    void reset();

    /**
	 * Escape all parameters when formatting the command line for
	 * output.
	 *
	 * This is the default.
	 *
	 * @since 0.5.0
	 */
    inline void escape_all_parameters() {
        escape_parameters_ = std::numeric_limits<size_t>::max();
    }
    /**
	 * Only escape the number of parameters that currently exist when
	 * formatting the command line for output.
	 *
	 * Use this before appending argument help that should not be
	 * escaped.
	 *
	 * Empty parameters will always be escaped.
	 *
	 * @since 0.5.0
	 */
    inline void escape_initial_parameters() {
        escape_parameters_ = parameters_.size();
    }
    /**
	 * Escape the first count parameters when formatting the command
	 * line for output.
	 *
	 * Use this to prevent argument help from being escaped.
	 *
	 * Empty parameters will always be escaped.
	 *
	 * @param[in] count Number of parameters to escape.
	 * @since 0.5.0
	 */
    inline void escape_initial_parameters(size_t count) {
        escape_parameters_ = count;
    }

    /**
	 * Obtain the parameters for this command line.
	 *
	 * @return A reference to the parameters.
	 * @since 0.6.0
	 */
    inline std::vector<std::string> & operator*() {
        return parameters_;
    }
    /**
	 * Obtain the parameters for this command line.
	 *
	 * @return A reference to the parameters.
	 * @since 0.6.0
	 */
    inline const std::vector<std::string> & operator*() const {
        return parameters_;
    }
    /**
	 * Obtain the parameters for this command line.
	 *
	 * @return A pointer to the parameters.
	 * @since 0.4.0
	 */
    inline std::vector<std::string> * operator->() {
        return &parameters_;
    }
    /**
	 * Obtain the parameters for this command line.
	 *
	 * @return A pointer to the parameters.
	 * @since 0.4.0
	 */
    inline const std::vector<std::string> * operator->() const {
        return &parameters_;
    }

    /**
	 * Compare a command line to another command line for equality.
	 *
	 * @param[in] lhs Left-hand side command line.
	 * @param[in] rhs Right-hand side command line.
	 * @return True if the command lines are equal, otherwise false.
	 * @since 0.4.0
	 */
    friend inline bool operator==(const CommandLine & lhs, const CommandLine & rhs) {
        return (lhs.trailing_space == rhs.trailing_space) && (lhs.parameters_ == rhs.parameters_);
    }
    /**
	 * Compare this command line to another command line for inequality.
	 *
	 * @param[in] lhs Left-hand side command line.
	 * @param[in] rhs Right-hand side command line.
	 * @return True if the command lines are not equal, otherwise false.
	 * @since 0.4.0
	 */
    friend inline bool operator!=(const CommandLine & lhs, const CommandLine & rhs) {
        return !(lhs == rhs);
    }

    bool trailing_space = false; /*!< Command line has a trailing space. @since 0.4.0 */

  private:
    std::vector<std::string> parameters_;                                             /*!< Separate command line parameters. @since 0.4.0 */
    size_t                   escape_parameters_ = std::numeric_limits<size_t>::max(); /*!< Number of initial arguments to escape in output. @since 0.5.0 */
};

/**
 * Result of a command completion operation.
 *
 * Each space-delimited parameter is a separate string.
 *
 * @since 0.1.0
 */
struct Commands::Completion {
    std::list<CommandLine> help;        /*!< Suggestions for matching commands. @since 0.1.0 */
    CommandLine            replacement; /*!< Replacement matching full or partial command line. @since 0.1.0 */
};

} // namespace console

} // namespace uuid

#endif
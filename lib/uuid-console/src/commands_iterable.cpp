/*
 * uuid-console - Microcontroller console shell
 * Copyright 2022  Simon Arlott
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

#include <memory>
#include <utility>

namespace uuid {

namespace console {

Commands::AvailableCommands Commands::available_commands(const Shell & shell) const {
    auto range = commands_.equal_range(shell.context());
    return AvailableCommands(shell, range.first, range.second);
}

Commands::AvailableCommand::AvailableCommand(const Command & command)
    : command_(command) {
    name_.reserve(command_.name_.size());
    for (auto name : command_.name_) {
        name_.push_back(name);
    }

    arguments_.reserve(command_.arguments_.size());
    for (auto argument : command_.arguments_) {
        arguments_.push_back(argument);
    }
}

Commands::AvailableCommands::AvailableCommands(const Shell & shell, const command_iterator & begin, const command_iterator & end)
    : shell_(shell)
    , begin_(begin)
    , end_(end) {
    // Skip over unavailable commands at the beginning
    while (begin_ != end_ && !shell_.has_flags(begin_->second.flags_, begin_->second.not_flags_)) {
        ++begin_;
    }
}

Commands::AvailableCommands::const_iterator Commands::AvailableCommands::cbegin() const {
    return AvailableCommands::const_iterator(shell_, begin_, begin_, end_);
}

Commands::AvailableCommands::const_iterator Commands::AvailableCommands::cend() const {
    return AvailableCommands::const_iterator(shell_, begin_, end_, end_);
}

Commands::AvailableCommands::const_iterator::const_iterator(const Shell &            shell,
                                                            const command_iterator & begin,
                                                            const command_iterator & command,
                                                            const command_iterator & end)
    : shell_(std::move(shell))
    , begin_(begin)
    , command_(command)
    , end_(end) {
    update();
}

void Commands::AvailableCommands::const_iterator::update() {
    if (command_ != end_) {
        available_command_ = std::make_shared<AvailableCommand>(command_->second);
    } else {
        available_command_.reset();
    }
}

Commands::AvailableCommands::const_iterator & Commands::AvailableCommands::const_iterator::operator++() {
    do {
        ++command_;
    } while (command_ != end_ && !shell_.has_flags(command_->second.flags_, command_->second.not_flags_));

    update();
    return *this;
}

Commands::AvailableCommands::const_iterator & Commands::AvailableCommands::const_iterator::operator--() {
    do {
        --command_;
    } while (command_ != begin_ && !shell_.has_flags(command_->second.flags_, command_->second.not_flags_));

    update();
    return *this;
}

} // namespace console

} // namespace uuid
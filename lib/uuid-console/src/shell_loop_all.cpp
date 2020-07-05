/*
 * uuid-console - Microcontroller console shell
 * Copyright 2019  Simon Arlott
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
#include <set>

namespace uuid {

namespace console {

std::set<std::shared_ptr<Shell>> Shell::shells_;

void Shell::loop_all() {
    for (auto shell = shells_.begin(); shell != shells_.end();) {
        shell->get()->loop_one();

        // This avoids copying the shared_ptr every time loop_one() is called
        if (!shell->get()->running()) {
            shell = shells_.erase(shell);
        } else {
            shell++;
        }
    }
}

} // namespace console

} // namespace uuid

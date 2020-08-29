/*
 * uuid-telnet - Telnet service
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

#include "uuid/telnet.h"

#include <Arduino.h>

#include <algorithm>
#include <string>
#include <vector>

namespace uuid {

namespace telnet {

TelnetStream::TelnetStream(WiFiClient &client)
		: client_(client) {
	output_buffer_.reserve(BUFFER_SIZE);
}

void TelnetStream::start() {
	raw_write({
		IAC, WILL, OPT_ECHO,
		IAC, WILL, OPT_BINARY,
		IAC, WILL, OPT_SGA,
		IAC, DONT, OPT_ECHO,
		IAC, DO, OPT_BINARY,
		IAC, DO, OPT_SGA
	});
}

int TelnetStream::available() {
	if (peek() == -1) {
		return 0;
	} else {
		return 1;
	}
}

int TelnetStream::read() {
	if (peek_ != -1) {
		int data = peek_;
		peek_ = -1;
		return data;
	}

	buffer_flush();

restart:
	int data = raw_read();

	if (data == -1) {
		return -1;
	}

	unsigned char c = data;

	if (sub_negotiation_) {
		if (previous_raw_in_ == IAC) {
			switch (c) {
			case SE:
				sub_negotiation_ = false;
				previous_raw_in_ = 0;
				goto restart;

			case IAC:
				previous_raw_in_ = 0;
				goto restart;
			}
		} else {
			switch (c) {
				case IAC:
					previous_raw_in_ = c;
					goto restart;

				default:
					previous_raw_in_ = 0;
					goto restart;
			}
		}
	} else {
		if (previous_raw_in_ == IAC) {
			switch (c) {
			case IP:
				// Interrupt (^C)
				previous_raw_in_ = 0;
				c = '\x03';
				break;

			case EC:
				// Backspace (^H)
				previous_raw_in_ = 0;
				c = '\x08';
				break;

			case EL:
				// Delete line (^U)
				previous_raw_in_ = 0;
				c = '\x15';
				break;

			case IAC:
				previous_raw_in_ = 0;
				break;

			case SB:
			case WILL:
			case WONT:
			case DO:
			case DONT:
				previous_raw_in_ = c;
				goto restart;

			case SE:
			case DM:
			case BRK:
			case AO:
			case AYT:
			case GA:
			case NOP:
			default:
				previous_raw_in_ = 0;
				goto restart;
			}
		} else if (previous_raw_in_ == SB) {
			sub_negotiation_ = true;
			previous_raw_in_ = 0;
			goto restart;
		} else if (previous_raw_in_ == WILL || previous_raw_in_ == WONT) {
			switch (c) {
			case OPT_ECHO:
				// Don't do these
				raw_write({IAC, DONT, c});
				break;

			case OPT_BINARY:
			case OPT_SGA:
				// Do these
				raw_write({IAC, DO, c});
				break;

			default:
				// Don't do anything else
				raw_write({IAC, DONT, c});
				break;
			}

			previous_raw_in_ = 0;
			goto restart;
		} else if (previous_raw_in_ == DO) {
			switch (c) {
			case OPT_ECHO:
			case OPT_BINARY:
			case OPT_SGA:
				// These are always enabled
				break;

			default:
				// Refuse to do anything else
				raw_write({IAC, WONT, c});
				break;
			}

			previous_raw_in_ = 0;
			goto restart;
		} else if (previous_raw_in_ == DONT) {
			switch (c) {
			case OPT_ECHO:
			case OPT_BINARY:
			case OPT_SGA:
				// Insist that we do these
				raw_write({IAC, WILL, c});
				break;

			default:
				// Everything else is always disabled
				break;
			}

			previous_raw_in_ = 0;
			goto restart;
		} else {
			switch (c) {
			case IAC:
				previous_raw_in_ = c;
				goto restart;

			default:
				previous_raw_in_ = 0;
				break;
			}
		}
	}

	if (previous_in_ == CR) {
		if (c == NUL) {
			previous_in_ = 0;
			goto restart;
		}
	}

	previous_in_ = c;
	return c;
}

int TelnetStream::peek() {
	buffer_flush();

	// It's too complicated to implement this by calling peek()
	// on the original stream, especially if the original stream
	// doesn't actually support peeking.
	if (peek_ == -1) {
		peek_ = read();
	}

	return peek_;
}

size_t TelnetStream::write(uint8_t data) {
	if (previous_out_ == CR && data != LF) {
		previous_out_ = data;

		if (raw_write({NUL, data}) != 2) {
			return 0;
		}
	} else {
		previous_out_ = data;
	}

	if (data == IAC) {
		if (raw_write({IAC, IAC}) != 2) {
			return 0;
		}
	} else {
		if (raw_write(data) != 1) {
			return 0;
		}
	}

	return 1;
}

size_t TelnetStream::write(const uint8_t *buffer, size_t size) {
	std::vector<unsigned char> data;
	data.reserve(size);

	while (size-- > 0) {
		unsigned char c = *buffer++;

		if (previous_out_ == CR && c != LF) {
			data.push_back((unsigned char)NUL);
		}

		if (c == IAC) {
			data.push_back((unsigned char)IAC);
		}

		previous_out_ = c;
		data.push_back(c);
	}

	size_t len = raw_write(data);
	if (len < size) {
		len = 0;
	}
	return len;
}

void TelnetStream::flush() {
	// This is a pure virtual function in Arduino's Stream class, which
	// makes no sense because that class is for input and this is an
	// output function. Later versions move it to Print as an empty
	// virtual function so this is here for backward compatibility.
}

int TelnetStream::raw_available() {
	return client_.available();
}

int TelnetStream::raw_read() {
	return client_.read();
}

void TelnetStream::buffer_flush() {
	if (!output_buffer_.empty()) {
		size_t len = client_.write(reinterpret_cast<const unsigned char*>(output_buffer_.data()), output_buffer_.size());
		if (len != output_buffer_.size()) {
			client_.stop();
		}
		output_buffer_.clear();
		output_buffer_.shrink_to_fit();
	}
}

size_t TelnetStream::raw_write(unsigned char data) {
	output_buffer_.push_back(data);

	if (output_buffer_.size() >= BUFFER_SIZE) {
		buffer_flush();
	}

	return 1;
}

size_t TelnetStream::raw_write(const std::vector<unsigned char> &data) {
	return raw_write(reinterpret_cast<const unsigned char*>(data.data()), data.size());
}

size_t TelnetStream::raw_write(const uint8_t *buffer, size_t size) {
	size_t offset = 0;
	size_t remaining = size;

	if (!output_buffer_.empty()) {
		// Fill the rest of the buffer
		size_t block = std::min(remaining, BUFFER_SIZE - output_buffer_.size());

		output_buffer_.insert(output_buffer_.end(), buffer, buffer + block);
		offset += block;
		remaining -= block;

		if (output_buffer_.size() >= BUFFER_SIZE) {
			buffer_flush();
		}
	}

	if (remaining >= BUFFER_SIZE) {
		// Output directly if it won't fit in the buffer
		size_t len = client_.write(buffer + offset, remaining);
		if (len != remaining) {
			client_.stop();
			return offset + len;
		}
	} else if (remaining > 0) {
		// Put the rest in the buffer
		output_buffer_.insert(output_buffer_.end(), buffer + offset, buffer + offset + remaining);
	}

	return size;
}

} // namespace telnet

} // namespace uuid

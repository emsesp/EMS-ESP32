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

#ifndef UUID_TELNET_H_
#define UUID_TELNET_H_

#include <Arduino.h>
#ifdef ARDUINO_ARCH_ESP8266
# include <ESP8266WiFi.h>
#else
# include <WiFi.h>
#endif
#include <WiFiUdp.h>

#include <functional>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include <uuid/console.h>

namespace uuid {

/**
 * Telnet service.
 *
 * - <a href="https://github.com/nomis/mcu-uuid-telnet/">Git Repository</a>
 * - <a href="https://mcu-uuid-telnet.readthedocs.io/">Documentation</a>
 */
namespace telnet {

/**
 * Stream wrapper that performs telnet protocol handling, option
 * negotiation and output buffering.
 *
 * @since 0.1.0
 */
class TelnetStream: public ::Stream {
public:
	/**
	 * Create a new telnet stream wrapper.
	 *
	 * @param[in] client Client connection.
	 * @since 0.1.0
	 */
	explicit TelnetStream(WiFiClient &client);
	virtual ~TelnetStream() = default;

	/**
	 * Perform initial negotiation.
	 *
	 * @since 0.1.0
	 */
	void start();

	/**
	 * Check for available input.
	 *
	 * @return The number of bytes available to read.
	 * @since 0.1.0
	 */
	int available() override;
	/**
	 * Read one byte from the available input.
	 *
	 * @return An unsigned char if input is available, otherwise -1.
	 * @since 0.1.0
	 */
	int read() override;
	/**
	 * Read one byte from the available input without advancing to the
	 * next one.
	 *
	 * @return An unsigned char if input is available, otherwise -1.
	 * @since 0.1.0
	 */
	int peek() override;
	/**
	 * Write one byte to the output stream.
	 *
	 * Disconnect the client if the socket buffer is full.
	 *
	 * @param[in] data Data to be output.
	 * @return The number of bytes that were output.
	 * @since 0.1.0
	 */
	size_t write(uint8_t data) override;
	/**
	 * Write an array of bytes to the output stream.
	 *
	 * Disconnect the client if the socket buffer is full.
	 *
	 * @param[in] buffer Buffer to be output.
	 * @param[in] size Length of the buffer.
	 * @return The number of bytes that were output.
	 * @since 0.1.0
	 */
	size_t write(const uint8_t *buffer, size_t size) override;
	/**
	 * Does nothing.
	 *
	 * This is a pure virtual function in Arduino's Stream class, which
	 * makes no sense because that class is for input and this is an
	 * output function. Later versions move it to Print as an empty
	 * virtual function so this is here for backward compatibility.
	 *
	 * @since 0.1.0
	 */
	void flush() override;

private:
	static constexpr const unsigned char NUL = 0; /*!< No operation. @since 0.1.0 */
	static constexpr const unsigned char BEL = 7; /*!< Produces an audible or visible signal. @since 0.1.0 */
	static constexpr const unsigned char BS = 8; /*!< Moves the print head one character position towards the left margin. @since 0.1.0 */
	static constexpr const unsigned char HT = 9; /*!< Moves the printer to the next horizontal tab stop. @since 0.1.0 */
	static constexpr const unsigned char LF = 10; /*!< Line Feed. @since 0.1.0 */
	static constexpr const unsigned char VT = 11; /*!< Moves the printer to the next vertical tab stop. @since 0.1.0 */
	static constexpr const unsigned char FF = 12; /*!< Moves the printer to the top of the next page, keeping the same horizontal position. @since 0.1.0 */
	static constexpr const unsigned char CR = 13; /*!< Carriage Return. @since 0.1.0 */
	static constexpr const unsigned char SE = 240; /*!< End of sub-negotiation parameters. @since 0.1.0 */
	static constexpr const unsigned char NOP = 241; /*!< No operation. @since 0.1.0 */
	static constexpr const unsigned char DM = 242; /*!< The data stream portion of a Synch. @since 0.1.0 */
	static constexpr const unsigned char BRK = 243; /*!< NVT character BRK. @since 0.1.0 */
	static constexpr const unsigned char IP = 244; /*!< Interrupt Process function. @since 0.1.0 */
	static constexpr const unsigned char AO = 245; /*!< Abort Output function. @since 0.1.0 */
	static constexpr const unsigned char AYT = 246; /*!< Are You There function. @since 0.1.0 */
	static constexpr const unsigned char EC = 247; /*!< Erase Character function. @since 0.1.0 */
	static constexpr const unsigned char EL = 248; /*!< Erase Line function. @since 0.1.0 */
	static constexpr const unsigned char GA = 249; /*!< Go Ahead signal. @since 0.1.0 */
	static constexpr const unsigned char SB = 250; /*!< Sub-negotiation of the indicated option. @since 0.1.0 */
	static constexpr const unsigned char WILL = 251; /*!< Indicates the desire to begin performing, or confirmation that you are now performing, the indicated option. @since 0.1.0 */
	static constexpr const unsigned char WONT = 252; /*!< Indicates the refusal to perform, or continue performing, the indicated option. @since 0.1.0 */
	static constexpr const unsigned char DO = 253; /*!< Indicates the request that the other party perform, or confirmation that you are expecting the other party to perform, the indicated option. @since 0.1.0 */
	static constexpr const unsigned char DONT = 254; /*!< Indicates the demand that the other party stop performing, or confirmation that you are no longer expecting the other party to perform, the indicated option. @since 0.1.0 */
	static constexpr const unsigned char IAC = 255; /*!< Interpret As Command escape character. @since 0.1.0 */

	static constexpr const unsigned char OPT_BINARY = 0; /*!< Binary (8-bit) transmission mode. (RFC 856). @since 0.1.0 */
	static constexpr const unsigned char OPT_ECHO = 1; /*!< Remote Echo (RFC 857). @since 0.1.0 */
	static constexpr const unsigned char OPT_SGA = 3; /*!< Suppress Go Ahead (RFC 858). @since 0.1.0 */

	static constexpr const size_t BUFFER_SIZE = 536; /*!< Output buffer size. @since 0.1.0 */

	TelnetStream(const TelnetStream&) = delete;
	TelnetStream& operator=(const TelnetStream&) = delete;

	/**
	 * Directly check for available input.
	 *
	 * @return The number of bytes available to read.
	 * @since 0.1.0
	 */
	int raw_available();
	/**
	 * Read one byte directly from the available input.
	 *
	 * @return An unsigned char if input is available, otherwise -1.
	 * @since 0.1.0
	 */
	int raw_read();
	/**
	 * Flush output stream buffer.
	 *
	 * Disconnect the client if the socket buffer is full.
	 *
	 * @since 0.1.0
	 */
	void buffer_flush();
	/**
	 * Write one byte directly to the output stream.
	 *
	 * Disconnect the client if the socket buffer is full.
	 *
	 * @param[in] data Data to be output.
	 * @return The number of bytes that were output.
	 * @since 0.1.0
	 */
	size_t raw_write(unsigned char data);
	/**
	 * Write a vector of bytes directly to the output stream.
	 *
	 * Disconnect the client if the socket buffer is full.
	 *
	 * @param[in] data Data to be output.
	 * @return The number of bytes that were output.
	 * @since 0.1.0
	 */
	size_t raw_write(const std::vector<unsigned char> &data);
	/**
	 * Write an array of bytes directly to the output stream.
	 *
	 * Disconnect the client if the socket buffer is full.
	 *
	 * @param[in] buffer Buffer to be output.
	 * @param[in] size Length of the buffer.
	 * @return The number of bytes that were output.
	 * @since 0.1.0
	 */
	size_t raw_write(const uint8_t *buffer, size_t size);

	WiFiClient &client_; /*!< Client connection. @since 0.1.0 */
	unsigned char previous_raw_in_ = 0; /*!< Previous raw character that was received. Used to detect commands. @since 0.1.0 */
	bool sub_negotiation_ = false; /*!< Sub-negotiation mode. @since 0.1.0 */
	unsigned char previous_in_ = 0; /*!< Previous character that was received. Used to detect CR NUL. @since 0.1.0 */
	unsigned char previous_out_ = 0; /*!< Previous character that was sent. Used to insert NUL after CR without LF. @since 0.1.0 */
	int peek_ = -1; /*!< Previously read data cached by peek(). @since 0.1.0 */
	std::vector<char> output_buffer_; /*!< Buffer data to be output until a read function is called. @since 0.1.0 */
};

/**
 * Provides access to a console shell as a telnet server.
 *
 * @since 0.1.0
 */
class TelnetService {
public:
	static constexpr size_t MAX_CONNECTIONS = 3; /*!< Maximum number of concurrent open connections. @since 0.1.0 */
	static constexpr uint16_t DEFAULT_PORT = 23; /*!< Default TCP port to listen on. @since 0.1.0 */
	static constexpr unsigned long DEFAULT_IDLE_TIMEOUT = 600; /*!< Default initial idle timeout (in seconds). @since 0.1.0 */
	static constexpr unsigned long DEFAULT_WRITE_TIMEOUT = 0; /*!< Default write timeout (in milliseconds). @ since 0.1.0 */

	/**
	 * Function to handle the creation of a shell.
	 *
	 * @param[in] stream Stream for the telnet connection.
	 * @param[in] addr Remote IP address.
	 * @param[in] port Remote port.
	 * @since 0.1.0
	 */
	using shell_factory_function = std::function<std::shared_ptr<uuid::console::Shell>(Stream &stream, const IPAddress &addr, uint16_t port)>;

	/**
	 * Create a new telnet service listening on the default port.
	 *
	 * @param[in] commands Commands available for execution in shells.
	 * @param[in] context Default context for shells.
	 * @param[in] flags Initial flags for shells.
	 * @since 0.1.0
	 */
	TelnetService(std::shared_ptr<uuid::console::Commands> commands, unsigned int context = 0, unsigned int flags = 0);

	/**
	 * Create a new telnet service listening on a specific port.
	 *
	 * @param[in] port TCP listening port.
	 * @param[in] commands Commands available for execution in shells.
	 * @param[in] context Default context for shells.
	 * @param[in] flags Initial flags for shells.
	 * @since 0.1.0
	 */
	TelnetService(uint16_t port, std::shared_ptr<uuid::console::Commands> commands, unsigned int context = 0, unsigned int flags = 0);

	/**
	 * Create a new telnet service listening on the default port.
	 *
	 * @param[in] shell_factory Function to create a shell for new connections.
	 * @since 0.1.0
	 */
	explicit TelnetService(shell_factory_function shell_factory);

	/**
	 * Create a new telnet service listening on a specific port.
	 *
	 * @param[in] port TCP listening port.
	 * @param[in] shell_factory Function to create a shell for new connections.
	 * @since 0.1.0
	 */
	TelnetService(uint16_t port, shell_factory_function shell_factory);

	~TelnetService() = default;

	/**
	 * Start listening for connections on the configured port.
	 *
	 * @since 0.1.0
	 */
	void start();
	/**
	 * Close all connections.
	 *
	 * The listening status is not affected.
	 *
	 * @since 0.1.0
	 */
	void close_all();
	/**
	 * Stop listening for connections.
	 *
	 * Existing connections are not affected.
	 *
	 * @since 0.1.0
	 */
	void stop();

	/**
	 * Get the maximum number of concurrent open connections.
	 *
	 * @return The maximum number of concurrent open connections.
	 * @since 0.1.0
	 */
	size_t maximum_connections() const;
	/**
	 * Set the maximum number of concurrent open connections.
	 *
	 * Defaults to TelnetService::MAX_CONNECTIONS.
	 *
	 * @since 0.1.0
	 */
	void maximum_connections(size_t count);

	/**
	 * Get the initial idle timeout for new connections.
	 *
	 * @return The initial idle timeout in seconds (or 0 for disabled).
	 * @since 0.1.0
	 */
	unsigned long initial_idle_timeout() const;
	/**
	 * Set the initial idle timeout for new connections.
	 *
	 * Defaults to TelnetService::DEFAULT_IDLE_TIMEOUT.
	 *
	 * @param[in] timeout Idle timeout in seconds (or 0 to disable).
	 * @since 0.1.0
	 */
	void initial_idle_timeout(unsigned long timeout);

	/**
	 * Get the default socket write timeout for new connections.
	 *
	 * @return The default socket write timeout in seconds (or 0 for
	 *         platform default).
	 * @since 0.1.0
	 */
	unsigned long default_write_timeout() const;
	/**
	 * Set the default socket write timeout for new connections.
	 *
	 * Defaults to TelnetService::DEFAULT_WRITE_TIMEOUT (platform
	 * default).
	 *
	 * @param[in] timeout Socket write timeout in seconds (or 0 for
	 *                    platform default).
	 * @since 0.1.0
	 */
	void default_write_timeout(unsigned long timeout);

	/**
	 * Accept new connections.
	 *
	 * @since 0.1.0
	 */
	void loop();

private:
	/**
	 * Telnet connection.
	 *
	 * Holds the client and stream instance for the lifetime of the shell.
	 *
	 * @since 0.1.0
	 */
	class Connection {
	public:
		/**
		 * Create a telnet connection shell.
		 *
		 * @param[in] shell_factory Function to create a shell for new connections.
		 * @param[in] client Client connection.
		 * @param[in] idle_timeout Idle timeout in seconds.
		 * @param[in] write_timeout Idle timeout in milliseconds.
		 * @since 0.1.0
		 */
		Connection(shell_factory_function &shell_factory, WiFiClient &&client, unsigned long idle_timeout, unsigned long write_timeout);
		~Connection() = default;

		/**
		 * Check if the shell is still active.
		 *
		 * @return Active status of the shell.
		 * @since 0.1.0
		 */
		bool active();
		/**
		 * Stop the shell if the client is not connected.
		 *
		 * @return Active status of the shell.
		 * @since 0.1.0
		 */
		bool loop();
		/**
		 * Stop the shell.
		 *
		 * @since 0.1.0
		 */
		void stop();

	private:
		Connection(const Connection&) = delete;
		Connection& operator=(const Connection&) = delete;

		WiFiClient client_; /*!< Client connection. @since 0.1.0 */
		TelnetStream stream_; /*!< Telnet stream for the connection. @since 0.1.0 */
		std::shared_ptr<uuid::console::Shell> shell_; /*!< Shell for connection. @since 0.1.0 */
		IPAddress addr_; /*!< Remote address of connection. @since 0.1.0 */
		uint16_t port_; /*!< Remote port of connection. @since 0.1.0 */
	};

	TelnetService(const TelnetService&) = delete;
	TelnetService& operator=(const TelnetService&) = delete;

	static uuid::log::Logger logger_; /*!< uuid::log::Logger instance for telnet services. @since 0.1.0 */

	WiFiServer server_; /*!< TCP server. @since 0.1.0 */
	size_t maximum_connections_ = MAX_CONNECTIONS; /*!< Maximum number of concurrent open connections. @since 0.1.0 */
	std::list<Connection> connections_; /*!< Open connections. @since 0.1.0 */
	shell_factory_function shell_factory_; /*!< Function to create a shell. @since 0.1.0 */
	unsigned long initial_idle_timeout_ = DEFAULT_IDLE_TIMEOUT; /*!< Initial idle timeout (in seconds). @since 0.1.0 */
	unsigned long write_timeout_ = DEFAULT_WRITE_TIMEOUT; /*!< Write timeout (in milliseconds). @since 0.1.0 */
};

} // namespace telnet

} // namespace uuid

#endif

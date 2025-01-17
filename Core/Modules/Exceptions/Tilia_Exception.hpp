/**************************************************************************************************
 * @file   Tilia_Exception.hpp
 * 
 * @brief  Holds a custom exception class deriving from the <stdexcept> exception class.
 * 
 * @author Gustav Fagerlind
 * @date   10/07/2022
 *************************************************************************************************/

#ifndef TILIA_TILIA_EXCEPTION_HPP
#define TILIA_TILIA_EXCEPTION_HPP

// Standard
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>

// Tilia
#include "Core/Values/Directories.hpp"
#include TILIA_CONSTANTS_INCLUDE

/**
 * @brief Small macro which can be used in the construcor of the Tilia_Exception class
 */
#define TILIA_LOCATION __FILE__, static_cast<std::size_t>(__LINE__)

namespace tilia {

	namespace utils {

		/**
		 * @brief Helper class for Tilia_Exception. Holds data for exceptions.
		 */
		struct Exception_Data
		{

			/**
			 * @brief Creates the exception data with just the location.
			 */
			Exception_Data(const std::string& file, std::size_t line)
				: m_file{ file }, m_line{ line } { }

			/**
			 * @brief Creates the exception data with the location and a message.
			 */
			template<typename... T>
			Exception_Data(const std::string& file, std::size_t line, T... message)
				: Exception_Data(file, line)
			{
				Set_Message(message...);
			}

			/**
			 * @brief Sets the stored message to the given one.
			 */
			template<typename... T>
			void Set_Message(T... message)
			{
				m_message = {};
				Add_Message(message...);
			}
			/**
			 * @brief Add given message to previous the one.
			 */
			template<typename... T>
			void Add_Message(T... message)
			{
				std::stringbuf str_buf{};
				std::ostream buffer{ &str_buf };
				((buffer << std::forward<T>(message)), ...);
				m_message += std::move(str_buf.str());
			}

			/**
			 * @brief Gets the exception message.
			 */
			auto Get_Message() const { return m_message; }

			/**
			 * @brief Sets the file and line at which the exception message was created.
			 */
			void Set_Location(const std::string& file, std::size_t line)
			{
				m_file = file; m_line = line;
			}
			/**
			 * @brief Gets the file and line of which the exception message was created at.
			 */
			auto Get_Location() const { return std::make_pair(m_file, m_line); }

		private:
			// The message stream
			std::string m_message{};
			// The file of which the exception message was created
			std::string m_file{};
			// The line at which the exception message was created
			std::size_t m_line{};

		public:

			friend class Tilia_Exception;

			Exception_Data(const Exception_Data& other)
				: m_message{ other.m_message }, m_file{ other.m_file },
				m_line{ other.m_line } { }

			Exception_Data(Exception_Data&& other) noexcept
				: m_message{ std::move(other.m_message) }, m_file{ std::move(other.m_file) },
				m_line{ other.m_line } { }

			Exception_Data& operator=(const Exception_Data& other)
			{
				if (this == &other)
					return *this;
				m_message = other.m_message;
				m_file = other.m_file;
				m_line = other.m_line;
				return *this;
			}

			Exception_Data& operator=(Exception_Data&& other) noexcept
			{
				if (this == &other)
					return *this;
				m_message = std::move(other.m_message);
				m_file = std::move(other.m_file);
				m_line = other.m_line;
				return *this;
			}

			friend bool operator==(const Exception_Data& lhs, const Exception_Data& rhs)
			{
				if (&lhs == &rhs)
					return true;
				if (lhs.m_message != lhs.m_message || lhs.m_file != rhs.m_file
					|| lhs.m_line != rhs.m_line)
					return false;
				return true;
			}
			friend bool operator!=(const Exception_Data& lhs, const Exception_Data& rhs)
			{
				if (&lhs == &rhs)
					return false;
				return !(lhs == rhs);
			}

		}; // Exception_Data

		/**
		 * @brief Custom exception class deriving from the STL exception class. Holds a
		 * vector of messages which can be added to while the exception is being passed along.
		 */
		class Tilia_Exception : public std::exception
		{
		public:
			
			explicit Tilia_Exception(const Exception_Data& message)
				: m_messages{ message } { }

			explicit Tilia_Exception(Exception_Data&& message) noexcept
				: m_messages{ std::move(message) } { }

			/**
			 * @brief Gets all of the messages concatenated into one string.
			 */
			virtual const char* what() const noexcept override {
				std::stringstream ret{};
				static std::string ret_str{};
				auto potential_message{ [](const auto& message)
				{
					if (message.size() > 0)
						return message.c_str();
					return "Message is empty";
				} };
				const auto message_count{ m_messages.size() };
				for (std::size_t i{ 0 }; i < message_count; ++i)
				{
					const auto& message{ m_messages[i] };
					ret << "Message #" << (i + 1) << ":\n" << "File: " << message.m_file << 
						" : Line: " << message.m_line << "\nMessage:\n" << 
						potential_message(message.m_message) << "\n\n";
				}
				ret_str = std::move(ret.str());
				return ret_str.c_str();
			}

			/**
			 * @brief Adds a message to the exception.
			 */
			auto& Add_Message(const Exception_Data& message)
			{
				m_messages.push_back(message); return *this;
			}
			/**
			 * @brief Adds a message to the exception.
			 */
			auto& Add_Message(Exception_Data&& message)
			{ 
				m_messages.push_back(std::move(message)); return *this;
			}

			/**
			 * @brief Gets the message of the exception at the index.
			 */
			auto Get_Message(std::size_t index) const { return m_messages[index]; }
			/**
			 * @brief The number of messages in the exception.
			 */
			auto Get_Count() const { return m_messages.size(); }

#if TILIA_UNIT_TESTS == 1

			/**
			 * @brief Unit test for Tilia_Exception.
			 */
			static void Test();

#endif // TILIA_UNIT_TESTS == 1

		private:

			// A list of all of the messages added to the exception
			std::vector<Exception_Data> m_messages{};

		public:

			Tilia_Exception(const Tilia_Exception& other)
				: m_messages{ other.m_messages } { }

			Tilia_Exception(Tilia_Exception&& other) noexcept
				: m_messages{ std::move(other.m_messages) } { }

			friend bool operator==(const Tilia_Exception& lhs, const Tilia_Exception& rhs)
			{
				if (&lhs == &rhs)
					return true;
				if (lhs.m_messages != rhs.m_messages)
					return false;
				return true;
			}
			friend bool operator!=(const Tilia_Exception& lhs, const Tilia_Exception& rhs)
			{
				if (&lhs == &rhs)
					return false;
				return !(lhs == rhs);
			}

		}; // Tilia_Exception

	} // utils

} // tilia

#endif // TILIA_TILIA_EXCEPTION_HPP
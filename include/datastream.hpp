#pragma once

#include <exception>
#include <functional>
#include <istream>

namespace DS {

using BytePredicate = std::function<bool(unsigned char)>;

using Codepoint = uint_least32_t;
constexpr Codepoint Failure = -1;

class Exception : public std::exception {
protected:
	Exception() = default;
};

class MessageException : public virtual Exception {
public:
	MessageException() = delete;
	virtual const char *what() const noexcept override;
protected:
	explicit MessageException(std::string&& message);
private:
	std::string Message;
};

class EndOfStream : public virtual Exception {
public:
	explicit EndOfStream() = default;
	virtual const char *what() const noexcept override;
};

class InvalidEncoding : public virtual Exception {
public:
	explicit InvalidEncoding() = default;
};

class InvalidFirstByte final : public virtual InvalidEncoding, public virtual MessageException {
public:
	InvalidFirstByte() = delete;
	explicit InvalidFirstByte(unsigned char byte);
};

class InvalidContinuationByte final : public virtual InvalidEncoding, public virtual MessageException {
public:
	InvalidContinuationByte() = delete;
	explicit InvalidContinuationByte(unsigned char byte);
};

class UTF8IStream : private std::istream {
public:
	explicit UTF8IStream(std::streambuf *sb);

	// Extracts a single character
	[[nodiscard]]
	Codepoint	Get();
private:
	[[nodiscard]]
	unsigned char	GetByte();
	[[nodiscard]]
	unsigned char	GetByte(BytePredicate p, Exception e);

	pos_type Character{}, Row{}, Column{};
};

}

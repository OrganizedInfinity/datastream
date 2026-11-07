#pragma once

#include <exception>
#include <istream>

namespace DS {

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

class ExpectedContinuationByte final : public virtual InvalidEncoding {
public:
	explicit ExpectedContinuationByte() = default;
	virtual const char *what() const noexcept override;
};

using Codepoint = uint_least32_t;
constexpr Codepoint Failure = -1;

using Position = unsigned long;
struct Location {
	Position Character, Row, Column;
};

enum class LineEnding : Codepoint {
	CR = '\r',
	// use for CRLF line endings
	LF = '\n',
	Never = Failure
};

class IStream : protected std::istream {
public:
	// The location of the last read character
	// Newlines are at column 0
	[[nodiscard]]
	Location	GetLocation();
	// Extracts a single character
	[[nodiscard]]
	Codepoint	Get();
protected:
	IStream() = delete;
	explicit IStream(std::streambuf *sb, LineEnding le);
	[[nodiscard]]
	unsigned char GetByte();
	[[nodiscard]]
	virtual Codepoint GetCodepoint() = 0;
private:
	Position Character = 0, Row = 1, Column = 0;
	const LineEnding Ending;
};

class UTF8IStream final : public IStream {
public:
	explicit UTF8IStream(std::streambuf *sb, LineEnding le = LineEnding::LF);

private:
	[[nodiscard]]
	virtual Codepoint GetCodepoint() override;
};

}

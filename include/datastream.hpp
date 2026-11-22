#pragma once

#include <functional>
#include <istream>

namespace DS {

using Codepoint = char32_t;
constexpr Codepoint Failure = -1;

using String = std::basic_string<Codepoint>;

using Position = unsigned long;
struct Location {
	bool operator==(const Location&) const = default;

	Position Character, Row, Column;
};

using Predicate = std::function<bool(Codepoint)>;

class NamedPredicate final : public Predicate {
public:
	NamedPredicate() = delete;
	// name should describe the class of characters permitted by this predicate
	// e.g. "alphanumeric character" for aA-zZ, 0-9
	NamedPredicate(const char *name, Predicate&& predicate);

	const char *const Name;
};

enum class LineEnding : Codepoint {
	CR = '\r',
	// use for CRLF line endings
	LF = '\n',
	Never = Failure
};

enum class Inversion : bool {
	Inverse = true,
	Noninverse = false
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
	// Throws exception when extracted character does not match the predicate (or if it does when inverse is true)
	template<Inversion Inverse = Inversion::Noninverse>
	Codepoint	Get(const Codepoint& predicate);
	// Throws a generic exception when predicate returns false
	Codepoint	Get(const Predicate& predicate);
	// Throws an exception including the predicate name, if the predicate returns false
	Codepoint	Get(const NamedPredicate& predicate);

	// Gets all characters matching the predicate
	template<Inversion Inverse = Inversion::Noninverse>
	String		GetWhile(const Codepoint& predicate);
	String		GetWhile(const Predicate& predicate);
	String		GetWhile(const NamedPredicate& predicate);

	// Returns the next character, but does not advance the position,
	// if an exception occurs, will return failure
	[[nodiscard]]
	Codepoint	Peek() noexcept;
protected:
	IStream() = delete;
	explicit IStream(std::streambuf *sb, LineEnding le);
	[[nodiscard]]
	unsigned char GetByte();
	[[nodiscard]]
	virtual Codepoint GetCodepoint() = 0;
private:
	[[nodiscard]]
	Codepoint GetNext();
	[[nodiscard]]
	Codepoint Pop();
	Codepoint Put(Codepoint cp);

	Position Character = 0, Row = 1, Column = 0;
	const LineEnding Ending;
	std::vector<Codepoint> Putback;
};

class UTF8IStream final : public IStream {
public:
	explicit UTF8IStream(std::streambuf *sb, LineEnding le = LineEnding::LF);

private:
	[[nodiscard]]
	virtual Codepoint GetCodepoint() override;
};

class UTF16IStream : public virtual IStream {
protected:
	[[nodiscard]]
	virtual uint_least16_t GetWord() = 0;
private:
	[[nodiscard]]
	virtual Codepoint GetCodepoint() final override;
};

class UTF16LEIStream final : public UTF16IStream {
public:
	explicit UTF16LEIStream(std::streambuf *sb, LineEnding le = LineEnding::LF);
private:
	[[nodiscard]]
	virtual uint_least16_t GetWord() override;
};

class UTF16BEIStream final : public UTF16IStream {
public:
	explicit UTF16BEIStream(std::streambuf *sb, LineEnding le = LineEnding::LF);
private:
	[[nodiscard]]
	virtual uint_least16_t GetWord() override;
};

}

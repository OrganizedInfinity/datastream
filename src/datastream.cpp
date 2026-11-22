#include "datastream.hpp"
#include "exceptions.hpp"

#include <cstdint>
#include <istream>
#include <utility>

namespace DS {
using namespace Exceptions;

static std::string UTF8Encode(Codepoint cp) noexcept {
	// ascii characters
	if (cp < 0x80)
		return std::string{static_cast<char>(cp)};

	std::string s;

	auto bytes = 2;
	if (cp > 0x7ff)
		bytes++;
	if (cp > 0xffff)
		bytes++;
	s += static_cast<char>(
		~0 << (8 - bytes) |
		cp >> (bytes - 1) * 6
	);
	for ( bytes--; bytes > 0; bytes-- )
		s += static_cast<char>(
			0x80 |
			( cp >> (bytes - 1) * 6 & 0x3f )
		);

	return s;
}

// includes printable names for the ascii non-printable characters
static std::string GetCodepointName(Codepoint cp) noexcept {
	switch (cp) {
	case '\x0':
		return "null character";
	case '\x1':
		return "start of heading character";
	case '\x2':
		return "start of text character";
	case '\x3':
		return "end of text character";
	case '\x4':
		return "end of transmission character";
	case '\x5':
		return "enquiry character";
	case '\x6':
		return "acknowledge character";
	case '\x7':
		return "bell character";
	case '\x8':
		return "backspace character";
	case '\x9':
		return "horizontal tab character";
	case '\xA':
		return "new line character";
	case '\xB':
		return "vertical tab character";
	case '\xC':
		return "form feed character";
	case '\xD':
		return "carriage return character";
	case '\xE':
		return "shift out character";
	case '\xF':
		return "shift in character";
	case '\x10':
		return "data link escape character";
	case '\x11':
		return "device control character 1";
	case '\x12':
		return "device control character 2";
	case '\x13':
		return "device control character 3";
	case '\x14':
		return "device control character 4";
	case '\x15':
		return "negative acknowledge character";
	case '\x16':
		return "synchronous idle character";
	case '\x17':
		return "end of transmission block character";
	case '\x18':
		return "cancel character";
	case '\x19':
		return "end of medium character";
	case '\x1A':
		return "substitute character";
	case '\x1B':
		return "escape character";
	case '\x1C':
		return "file separator character";
	case '\x1D':
		return "group separator character";
	case '\x1E':
		return "record separator character";
	case '\x1F':
		return "unit separator character";
	case '\x20':
		return "space";
	case '\x7F':
		return "delete character";
	default:
		return "'" + UTF8Encode(cp) + "'";
	}
}

MessageException::MessageException(std::string&& message) : Message(message) {}

const char *MessageException::what() const noexcept {
	return Message.c_str();
}

const char *EndOfStream::what() const noexcept {
	return "End of stream";
}

InvalidFirstByte::InvalidFirstByte(unsigned char byte) :
	MessageException(std::format("Invalid first byte of character: 0x{:x}", byte)) {}

InvalidContinuationByte::InvalidContinuationByte(unsigned char byte) :
	MessageException(std::format("Invalid continuation byte of character: 0x{:x}", byte)) {}

const char *ExpectedContinuationByte::what() const noexcept {
	return "Expected continuation byte, got end of stream";
}

const char *InvalidSurrogateOrder::what() const noexcept {
	return "Invalid surrogate pair, got high surrogate before low surrogate";
}

const char *ExpectedLowSurrogate::what() const noexcept {
	return "Expected low surrogate, got end of stream";
}

InvalidSurrogate::InvalidSurrogate(uint_least32_t surrogate) :
	MessageException(std::format("Invalid high surrogate byte of character: 0x{:x}", surrogate)) {}

const char *ExpectedEvenByte::what() const noexcept {
	return "Expected second byte in UTF-16 byte pair";
}

CharacterMismatch::CharacterMismatch(Codepoint predicate, Codepoint mismatch) :
	MessageException(std::format("Expected {}, got {}", GetCodepointName(predicate), GetCodepointName(mismatch))) {}

UnexpectedCharacter::UnexpectedCharacter(Codepoint predicate) :
	MessageException(std::format("Did not expect {} at this point", GetCodepointName(predicate))) {}

PredicateViolation::PredicateViolation(Codepoint cp) :
	MessageException(std::format("Predicate does not permit {} here", GetCodepointName(cp))) {}

NamedPredicateViolation::NamedPredicateViolation(Codepoint cp, const char *predicateName) :
	MessageException(std::format("Expected {}, got {}", predicateName, GetCodepointName(cp))) {}

//
// NamedPredicate
//
NamedPredicate::NamedPredicate(const char *name, Predicate&& predicate) : Predicate(predicate), Name(name) {}

//
// IStream
//
IStream::IStream(std::streambuf *sb, LineEnding le) : std::istream(sb), Ending(le) {}

Location IStream::GetLocation() {
	return {
		.Character = Character,
		.Row = Row,
		.Column = Column
	};
}

Codepoint IStream::Get() {
	auto cp = GetNext();
	Column++;
	Character++;
	if ( cp == std::to_underlying(Ending) ) {
		Row++;
		Column = 0;
	}
	return cp;
}

template<Inversion Inverse>
Codepoint IStream::Get(const Codepoint& predicate) {
	auto cp = Get();
	if ( (cp == predicate) != std::to_underlying(Inverse) )
		return cp;
	
	if constexpr (std::to_underlying(Inverse))
		throw UnexpectedCharacter(predicate);
	else
		throw CharacterMismatch(predicate, cp);
}
template Codepoint IStream::Get<Inversion::Inverse>(const Codepoint& predicate);
template Codepoint IStream::Get<Inversion::Noninverse>(const Codepoint& predicate);

Codepoint IStream::Get(const Predicate& predicate) {
	auto cp = Get();
	if (!predicate(cp))
		throw PredicateViolation(cp);
	return cp;
}

Codepoint IStream::Get(const NamedPredicate& predicate) {
	auto cp = Get();
	if (!predicate(cp))
		throw NamedPredicateViolation(cp, predicate.Name);
	return cp;
}

Codepoint IStream::Peek() noexcept {
	try {
		return Put(GetNext());
	} catch (Exception&) {
		return Failure;
	};
}

template<Inversion Inverse>
String IStream::GetWhile(const Codepoint& predicate) {
	String s;
	while ( (Peek() == predicate) != std::to_underlying(Inverse) )
		s += Get();
	return s;
}
template String IStream::GetWhile<Inversion::Inverse>(const Codepoint& predicate);
template String IStream::GetWhile<Inversion::Noninverse>(const Codepoint& predicate);

String IStream::GetWhile(const Predicate& predicate) {
	String s;
	while (predicate(Peek()))
		s += Get();
	return s;
}

String IStream::GetWhile(const NamedPredicate& predicate) {
	String s;
	while (predicate(Peek()))
		s += Get();
	return s;
}

unsigned char IStream::GetByte() {
	auto c = get();
	if ( c == traits_type::eof() ) {
		clear();
		throw EndOfStream();
	}
	return c;
}

Codepoint IStream::GetNext() {
	return Putback.empty() ? GetCodepoint() : Pop();
}

Codepoint IStream::Pop() {
	auto cp = Putback.back();
	Putback.pop_back();
	return cp;
}

Codepoint IStream::Put(Codepoint cp) {
	Putback.push_back(cp);
	return cp;
}

//
// UTF8IStream
//
UTF8IStream::UTF8IStream(std::streambuf *sb, LineEnding le) : IStream(sb, le) {}

Codepoint UTF8IStream::GetCodepoint() {
	Codepoint cp = GetByte();
	if ( (cp & 0x80) == 0 )
		return cp;

	for ( auto bytes = 2; bytes <= 4; bytes++ ) {
		if ( (cp & ~0 << (7-bytes)) != static_cast<unsigned char>(~0 << (8-bytes)) )
			continue;
		
		cp &= static_cast<unsigned char>(~0) >> (bytes+1);

		for ( unsigned char byte; bytes > 1; bytes-- ) {
			try {
				byte = GetByte();
			} catch (EndOfStream&) {
				throw ExpectedContinuationByte();
			}
			if ((byte & 0xc0) != 0x80)
				throw InvalidContinuationByte(byte);
			cp <<= 6;
			cp += byte & 0x3f;
		}

		return cp;
	}
	throw InvalidFirstByte(cp);
}

//
// UTF16IStream
//
Codepoint UTF16IStream::GetCodepoint() {
	Codepoint cp = GetWord();
	if ( (cp & 0xf800) != 0xd800 )
		return cp;
	
	if ( (cp & 0x0400) != 0 )
		throw InvalidSurrogateOrder();

	cp &= 0x03ff;
	cp <<= 10;

	uint_least32_t low;
	try {
		low = GetWord();
	} catch (EndOfStream&) {
		throw ExpectedLowSurrogate();
	}

	if ( (low & 0xfc00) != 0xdc00 )
		throw InvalidSurrogate(low);

	low &= 0x03ff;
	cp += low;

	return cp + 0x10000;
}

//
// UTF16LEIStream
//
UTF16LEIStream::UTF16LEIStream(std::streambuf *sb, LineEnding le) : IStream(sb, le) {}

uint_least16_t UTF16LEIStream::GetWord() {
	uint_least16_t word = GetByte();

	try {
		word += GetByte() << 8;
	} catch (EndOfStream&) {
		throw ExpectedEvenByte();
	}

	return word;
}

//
// UTF16BEIStream
//
UTF16BEIStream::UTF16BEIStream(std::streambuf *sb, LineEnding le) : IStream(sb, le) {}

uint_least16_t UTF16BEIStream::GetWord() {
	uint_least16_t word = GetByte() << 8;

	try {
		word += GetByte();
	} catch (EndOfStream&) {
		throw ExpectedEvenByte();
	}

	return word;
}

} // namespace DS

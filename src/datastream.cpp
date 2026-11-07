#include "datastream.hpp"

#include <cstdint>
#include <iostream>
#include <utility>

namespace DS {

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
	auto cp = GetCodepoint();
	Column++;
	Character++;
	if (cp == std::to_underlying(Ending)) {
		Row++;
		Column = 0;
	}
	return cp;
}

unsigned char IStream::GetByte() {
	auto c = get();
	if ( c == traits_type::eof() ) {
		clear();
		throw EndOfStream();
	}
	return c;
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

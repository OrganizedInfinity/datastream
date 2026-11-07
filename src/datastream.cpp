#include "datastream.hpp"

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
	return "Expected continuation byte, got end of stream!";
}

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

UTF8IStream::UTF8IStream(std::streambuf *sb, LineEnding le) : IStream(sb, le) {}

unsigned char IStream::GetByte() {
	auto c = get();
	if ( c == traits_type::eof() ) {
		clear();
		throw EndOfStream();
	}
	return c;
}

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

} // namespace DS

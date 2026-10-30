#include "datastream.hpp"

#include <iostream>

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

UTF8IStream::UTF8IStream(std::streambuf *sb) : std::istream(sb) {}

Codepoint UTF8IStream::Get() {
	Codepoint cp = GetByte();
	if ( (cp & 0x80) == 0 )
		return cp;

	for ( auto bytes = 2; bytes <= 4; bytes++ ) {
		if ( (cp & ~0 << (7-bytes)) != static_cast<unsigned char>(~0 << (8-bytes)) )
			continue;
		
		cp &= static_cast<unsigned char>(~0) >> (bytes+1);

		for ( ; bytes > 1; bytes-- ) {
			auto byte = GetByte();
			if ((byte & 0xc0) != 0x80)
				throw InvalidContinuationByte(byte);
			cp <<= 6;
			cp += byte & 0x3f;
		}

		return cp;
	}
	throw InvalidFirstByte(cp);
}

unsigned char UTF8IStream::GetByte() {
	auto c = get();
	if ( c == traits_type::eof() ) {
		clear();
		throw EndOfStream();
	}
	return c;
}

unsigned char UTF8IStream::GetByte(BytePredicate p, Exception e) {
	auto c = get();
	if ( c == traits_type::eof() ) {
		clear();
		throw EndOfStream();
	}

	if (!p(c))
		throw e;

	return c;
}

} // namespace DS

void Test() {
	DS::UTF8IStream st{std::cin.rdbuf()};
}

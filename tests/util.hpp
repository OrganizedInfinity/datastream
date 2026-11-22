#pragma once

#include <iostream>

#include "include/datastream.hpp"
#include "include/exceptions.hpp"

class TestStream : public DS::IStream {
public:
	TestStream(DS::LineEnding le = DS::LineEnding::Never) : DS::IStream(std::cout.rdbuf(), le) {};
	void SetCP(DS::Codepoint cp) {
		Codepoint = cp;
	}
private:
	DS::Codepoint Codepoint = DS::Failure;

	[[nodiscard]]
	virtual DS::Codepoint GetCodepoint() override {
		if (Codepoint == DS::Failure)
			throw DS::Exceptions::EndOfStream();
		return Codepoint;
	}
};

template<typename ...values>
void Discard(values...) {}

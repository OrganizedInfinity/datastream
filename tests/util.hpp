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
	void SetLambda(std::function<DS::Codepoint()> lambda) {
		Lambda = lambda;
	}
private:
	DS::Codepoint Codepoint = DS::Failure;
	std::function<DS::Codepoint()> Lambda;

	[[nodiscard]]
	virtual DS::Codepoint GetCodepoint() override {
		if (Lambda)
			return Lambda();
		if (Codepoint == DS::Failure)
			throw DS::Exceptions::EndOfStream();
		return Codepoint;
	}
};

template<typename ...values>
void Discard(values...) {}

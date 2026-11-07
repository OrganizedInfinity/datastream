//
// Tests the position tracking
//

#include <dumbtest/util.hpp>
#include <iostream>
#include "include/datastream.hpp"

#define SuiteName Location
DefineSuite();

class TestStream : public DS::IStream {
public:
	TestStream(DS::LineEnding le) : DS::IStream(std::cout.rdbuf(), le) {};
	void SetCP(DS::Codepoint cp) {
		Codepoint = cp;
	}
private:
	DS::Codepoint Codepoint = DS::Failure;

	[[nodiscard]]
	virtual DS::Codepoint GetCodepoint() override {
		if (Codepoint == DS::Failure)
			throw DS::EndOfStream();
		return Codepoint;
	}
};

template<typename ...values>
void Discard(values...) {}

ImplementTest(Character) {
	TestStream s{DS::LineEnding::Never};

	for ( unsigned char i = 1; i < static_cast<unsigned char>(~0); i++ ) {
		s.SetCP(i);
		Discard(s.Get());
		EQ(i, s.GetLocation().Character);
	}
}

ImplementTest(LineFeed) {
	TestStream s{DS::LineEnding::LF};

	for ( auto row = 1; row < 10; row++ ) {
		EQ(row, s.GetLocation().Row);
		for ( auto col = 1; col < 50; col++ ) {
			s.SetCP('A');
			Discard(s.Get());
			EQ(col, s.GetLocation().Column);
		}
		s.SetCP('\n');
		Discard(s.Get());
	}
}

ImplementTest(CarriageReturn) {
	TestStream s{DS::LineEnding::CR};

	for ( auto row = 1; row < 10; row++ ) {
		EQ(row, s.GetLocation().Row);
		for ( auto col = 1; col < 50; col++ ) {
			s.SetCP('A');
			Discard(s.Get());
			EQ(col, s.GetLocation().Column);
		}
		s.SetCP('\r');
		Discard(s.Get());
	}
}

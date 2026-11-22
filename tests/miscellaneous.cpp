//
// Miscellaneus tests
//

#include <dumbtest/util.hpp>
#include "util.hpp"

#define SuiteName Miscellaneus
DefineSuite();

ImplementTest(Peek) {
	TestStream s;
	s.SetCP('A');
	EQ(s.Peek(), 'A');

	s.SetCP('B');
	s.Get('A');

	auto location = s.GetLocation();
	EQ(s.Peek(), 'B');
	EQ(s.Peek(), 'B');
	EQ(s.Peek(), 'B');
	EQ(location, s.GetLocation());

	s.SetCP(DS::Failure);
	s.Get('B');

	EQ(s.Peek(), DS::Failure);
}

//
// Tests the predicated overloads of IStream::Get
//

#include <dumbtest/util.hpp>
#include "util.hpp"

#define SuiteName Predicate
DefineSuite();

ImplementTest(Character) {
	TestStream s;
	s.SetCP('A');

	Discard(s.Get('A'));
	EE(DS::Exceptions::CharacterMismatch&, Discard(s.Get('B')));

	Discard(s.Get<true>('B'));
	EE(DS::Exceptions::UnexpectedCharacter&, Discard(s.Get<true>('A')));
}

ImplementTest(Anonymous) {
	TestStream s;
	DS::Predicate predicate = [](auto cp){ return cp == 'A'; };

	s.SetCP('A');
	Discard(s.Get(predicate));

	s.SetCP('B');
	EE(DS::Exceptions::PredicateViolation&, Discard(s.Get(predicate)));
}

ImplementTest(Named) {
	TestStream s;
	DS::NamedPredicate predicate("test character", [](auto cp){ return cp == 'A'; });

	s.SetCP('A');
	Discard(s.Get(predicate));

	s.SetCP('B');
	EE(DS::Exceptions::NamedPredicateViolation&, Discard(s.Get(predicate)));
}

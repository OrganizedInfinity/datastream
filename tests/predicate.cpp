//
// Tests the predicated overloads of IStream::Get
//

#include <dumbtest/util.hpp>
#include "util.hpp"

#define SuiteName Predicate
DefineSuite();

using enum DS::Inversion;

ImplementTest(Character) {
	TestStream s;
	s.SetCP('A');

	Discard(s.Get('A'));
	EE(DS::Exceptions::CharacterMismatch&, Discard(s.Get('B')));

	Discard(s.Get<Inverse>('B'));
	EE(DS::Exceptions::UnexpectedCharacter&, Discard(s.Get<Inverse>('A')));
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

ImplementTest(WhileCharacter) {
	TestStream s;
	auto reps = 0;

	s.SetLambda([&]() {
		return reps--, reps >= 0 ? 'a' : 'b';
	});

	EQ(s.GetWhile('a'), DS::String{});
	s.Get('b');

	reps = 3;
	EQ(s.GetWhile('a'), DS::String(U"aaa"));
	s.Get('b');

	reps = 1;
	EQ(s.GetWhile<Inverse>('a'), DS::String{});
	s.Get('a');

	reps = 3;
	EQ(s.GetWhile<Inverse>('b'), DS::String{U"aaa"});
	s.Get('b');
}

ImplementTest(WhileAnonymous) {
	TestStream s;
	auto reps = 0;

	s.SetLambda([&]() {
		return reps--, reps >= 0 ? 'a' : 'b';
	});

	DS::Predicate predicate = [](auto cp) { return cp == 'a'; };

	EQ(s.GetWhile(predicate), DS::String{});
	s.Get('b');

	reps = 3;
	EQ(s.GetWhile(predicate), DS::String(U"aaa"));
	s.Get('b');
}

ImplementTest(WhileNamed) {
	TestStream s;
	auto reps = 0;

	s.SetLambda([&]() {
		return reps--, reps >= 0 ? 'a' : 'b';
	});

	DS::NamedPredicate predicate("test character", [](auto cp){ return cp == 'a'; });

	EQ(s.GetWhile(predicate), DS::String{});
	s.Get('b');

	reps = 3;
	EQ(s.GetWhile(predicate), DS::String(U"aaa"));
	s.Get('b');
}

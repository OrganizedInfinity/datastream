//
// Tests if the testing program itself functions
//
#include "../util.hpp"

#define SuiteName Selftest
DefineSuite();

using enum dumbtest::TestResult;
ImplementTest(Simple) {}

ImplementTest(Assertion) {
	AT(true);
}
ImplementTest(AssertionFailure, Failure) {
	AT(false);
}

ImplementTest(Equality) {
	EQ(12345, 12345);
}
ImplementTest(EqualityFailure, Failure) {
	EQ(12345, 54321);
}

ImplementTest(Inequality) {
	NE(12345, 54321);
}
ImplementTest(InequalityFailure, Failure) {
	NE(12345, 12345);
}

ImplementTest(GreaterThan) {
	GT(1, 0);
}
ImplementTest(GreaterThanFailureLT, Failure) {
	GT(0, 1);
}
ImplementTest(GreaterThanFailureEQ, Failure) {
	GT(0, 0);
}

ImplementTest(LessThan) {
	LT(0, 1);
}
ImplementTest(LessThanFailureGT, Failure) {
	LT(1, 0);
}
ImplementTest(LessThanFailureEQ, Failure) {
	LT(0, 0);
}

ImplementTest(GreaterOrEqual) {
	GE(0, 0);
	GE(1, 0);
}
ImplementTest(GreaterOrEqualFailure, Failure) {
	GE(0, 1);
}

ImplementTest(LessOrEqual) {
	LE(0, 0);
	LE(0, 1);
}
ImplementTest(LessOrEqualFailure, Failure) {
	LE(1, 0);
}

class DummyException {};
ImplementTest(ExpectedException) {
	EE(DummyException&, throw DummyException());
}

static void DummyFunction() {};
ImplementTest(ExpectedExceptionFailure, Failure) {
	EE(DummyException&, DummyFunction());
}

ImplementTest(Complex) {
	AT(true);
	EQ(12345, 12345);
	NE(12345, 54321);
	GT(1, 0);
	LT(0, 1);
	GE(0, 0);
	GE(1, 0);
	LE(0, 0);
	LE(0, 1);
	EE(DummyException&, throw DummyException());
}

#include "suite.hpp"
#include <cstring>

namespace dumbtest {

//
// Failure
//
const char* Failure::what() const noexcept {
	return "Uncaught failure!";
}
std::ostream& operator<<(std::ostream& stream, Failure& fail) {
	fail >> stream;
	return stream;
}

//
// Test
//
Test::Test(Suite& suite, const char *name, std::function<void()> implementation, TestResult expectedResult)
	: Name(name), Implementation(implementation), ExpectedResult(expectedResult) {
	suite.RegisterTest(this);
}

bool Test::FailExpected() const {
	return ExpectedResult == TestResult::Failure;
}

void Test::operator()() const {
	Implementation();
}

//
// Suite
//
void Suite::RegisterTest(Test *test) {
	Tests.push_back(test);
	TestNameWidth = std::max(TestNameWidth, std::strlen(test->Name));
}

Suite::Suite(const char *name) : Name(name) {
	TestCollection.RegisterSuite(this);
}

const std::vector<Test*>& Suite::GetTests() {
	return Tests;
}

unsigned long Suite::GetTestNameWidth() const {
	return TestNameWidth;
}

//
// Collection
//
Collection *Collection::Instance = nullptr;

void Collection::RegisterSuite(Suite *suite) {
	Suites.push_back(suite);
}

Collection& Collection::GetInstance() {
	if (Instance == nullptr)
		Instance = new Collection();
	return *Instance;
}

const std::vector<Suite*>& Collection::GetSuites() {
	return Suites;
}

}

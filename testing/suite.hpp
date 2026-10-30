#pragma once

#include <functional>
#include <ostream>
#include <vector>

namespace dumbtest {

class Failure : public std::exception {
protected:
	Failure() = default;
public:
	const char* what() const noexcept override;
	virtual void operator>>(std::ostream & stream) const = 0;
};

std::ostream& operator<<(std::ostream& stream, Failure& fail);

struct Test;

class Suite final {
	unsigned long TestNameWidth;
	std::vector<Test*> Tests;
	
	void RegisterTest(Test *test);
	friend struct Test;
public:
	const char *const Name;

	Suite() = delete;
	explicit Suite(const char *name);
	const std::vector<Test*>& GetTests();
	unsigned long GetTestNameWidth() const;
};
#define DefineSuiteInstance(Name) dumbtest::Suite TestSuite_##Name{#Name}
#define DefineSuiteValue(Value) DefineSuiteInstance(Value)
// to use DefineSuite, first define a "SuiteName" macro with an unquoted value
#define DefineSuite() DefineSuiteValue(SuiteName)

enum class TestResult {
	Success,
	Failure
};

struct Test final {
	const char *const Name;
	const std::function<void()> Implementation;
	const TestResult ExpectedResult;

	Test() = delete;
	explicit Test(Suite& suite, const char *name, std::function<void()> implementation, TestResult expectedResult = TestResult::Success);

	bool FailExpected() const;
	void operator()() const;
};
#define ImplementTestInstance(Name, SuiteName, ...) \
	void Test_##SuiteName##_##Name##_impl(); \
	dumbtest::Test Test_##SuiteName##_##Name{TestSuite_##SuiteName, #Name, Test_##SuiteName##_##Name##_impl, __VA_ARGS__}; \
	void Test_##SuiteName##_##Name##_impl()
#define ImplementTestValue(Name, Value, ...) ImplementTestInstance(Name, Value, __VA_ARGS__)
#define ImplementTest(Name, ...) ImplementTestValue(Name, SuiteName, __VA_ARGS__)

// has to be a singleton to avoid the static initialization order fiasco
class Collection final {
	static Collection *Instance;
	std::vector<Suite*> Suites;

	void RegisterSuite(Suite *suite);
	friend class Suite;
public:
	Collection() = default;
	static Collection& GetInstance();
	const std::vector<Suite*>& GetSuites();
};
#define TestCollection (dumbtest::Collection::GetInstance())

}

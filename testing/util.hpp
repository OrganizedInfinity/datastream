#include "suite.hpp"

namespace dumbtest {

#define DefineFailure(Name) class Name##Failure : public Failure
#define Fail(FailureName, ...) throw dumbtest::FailureName##Failure(__VA_ARGS__) 

DefineFailure(Assertion) {
	const char *const Expression;
public:
	AssertionFailure() = delete;
	AssertionFailure(const char *expression);
	void operator>>(std::ostream& stream) const override;
};

#define Assert(expr) \
	if (static_cast<bool>(expr) == false) \
		Fail(Assertion, #expr)
#define AT Assert

DefineFailure(Equality) {
	const char *const LeftExpression, *const RightExpression;
public:
	EqualityFailure() = delete;
	EqualityFailure(const char *leftExpression, const char *rightExpression);
	void operator>>(std::ostream& stream) const override;
};

#define Equal(left, right) \
	if (!((left) == (right))) \
		Fail(Equality, #left, #right)
#define EQ Equal

DefineFailure(Inequality) {
	const char *const LeftExpression, *const RightExpression;
public:
	InequalityFailure() = delete;
	InequalityFailure(const char *leftExpression, const char *rightExpression);
	void operator>>(std::ostream& stream) const override;
};

#define Inequal(left, right) \
	if (!((left) != (right))) \
		Fail(Inequality, #left, #right)
#define NE Inequal

DefineFailure(GreaterThan) {
	const char *const LeftExpression, *const RightExpression;
public:
	GreaterThanFailure() = delete;
	GreaterThanFailure(const char *leftExpression, const char *rightExpression);
	void operator>>(std::ostream& stream) const override;
};

#define Greater(left, right) \
	if (!((left) > (right))) \
		Fail(GreaterThan, #left, #right)
#define GT Greater

DefineFailure(LessThan) {
	const char *const LeftExpression, *const RightExpression;
public:
	LessThanFailure() = delete;
	LessThanFailure(const char *leftExpression, const char *rightExpression);
	void operator>>(std::ostream& stream) const override;
};

#define Lesser(left, right) \
	if (!((left) < (right))) \
		Fail(LessThan, #left, #right)
#define LT Lesser

DefineFailure(GreaterOrEqual) {
	const char *const LeftExpression, *const RightExpression;
public:
	GreaterOrEqualFailure() = delete;
	GreaterOrEqualFailure(const char *leftExpression, const char *rightExpression);
	void operator>>(std::ostream& stream) const override;
};

#define GreaterOrEqual(left, right) \
	if (!((left) >= (right))) \
		Fail(GreaterOrEqual, #left, #right)
#define GE GreaterOrEqual

DefineFailure(LessOrEqual) {
	const char *const LeftExpression, *const RightExpression;
public:
	LessOrEqualFailure() = delete;
	LessOrEqualFailure(const char *leftExpression, const char *rightExpression);
	void operator>>(std::ostream& stream) const override;
};

#define LesserOrEqual(left, right) \
	if (!((left) <= (right))) \
		Fail(LessOrEqual, #left, #right)
#define LE LesserOrEqual

DefineFailure(ExpectedException) {
	const char *const ClassName, *const Expression;
public:
	ExpectedExceptionFailure() = delete;
	ExpectedExceptionFailure(const char *className, const char *expression);
	void operator>>(std::ostream& stream) const override;
};

#define ExpectException(className, expression) \
	try { expression; Fail(ExpectedException, #className, #expression); } \
	catch (className) {}

#define EE ExpectException

}

#include "util.hpp"

namespace dumbtest {

AssertionFailure::AssertionFailure(const char *expression) : Expression(expression) {}

void AssertionFailure::operator>>(std::ostream& stream) const {
	stream << "Assertion failed! (" << Expression << " == false)";
}

EqualityFailure::EqualityFailure(const char *leftExpression, const char *rightExpression)
	: LeftExpression(leftExpression), RightExpression(rightExpression) {}

void EqualityFailure::operator>>(std::ostream& stream) const {
	stream << "Equality assertion failed! (" << LeftExpression << " != " << RightExpression << ')';
}

InequalityFailure::InequalityFailure(const char *leftExpression, const char *rightExpression)
	: LeftExpression(leftExpression), RightExpression(rightExpression) {}

void InequalityFailure::operator>>(std::ostream& stream) const {
	stream << "Inequality assertion failed! (" << LeftExpression << " == " << RightExpression << ')';
}

GreaterThanFailure::GreaterThanFailure(const char *leftExpression, const char *rightExpression)
	: LeftExpression(leftExpression), RightExpression(rightExpression) {}

void GreaterThanFailure::operator>>(std::ostream& stream) const {
	stream << "Greater than assertion failed! (" << LeftExpression << " <= " << RightExpression << ')';
}

LessThanFailure::LessThanFailure(const char *leftExpression, const char *rightExpression)
	: LeftExpression(leftExpression), RightExpression(rightExpression) {}

void LessThanFailure::operator>>(std::ostream& stream) const {
	stream << "Less than assertion failed! (" << LeftExpression << " >= " << RightExpression << ')';
}

GreaterOrEqualFailure::GreaterOrEqualFailure(const char *leftExpression, const char *rightExpression)
	: LeftExpression(leftExpression), RightExpression(rightExpression) {}

void GreaterOrEqualFailure::operator>>(std::ostream& stream) const {
	stream << "Greater than or equal to assertion failed! (" << LeftExpression << " < " << RightExpression << ')';
}

LessOrEqualFailure::LessOrEqualFailure(const char *leftExpression, const char *rightExpression)
	: LeftExpression(leftExpression), RightExpression(rightExpression) {}

void LessOrEqualFailure::operator>>(std::ostream& stream) const {
	stream << "Less than or equal to assertion failed! (" << LeftExpression << " > " << RightExpression << ')';
}

ExpectedExceptionFailure::ExpectedExceptionFailure(const char *className, const char *expression) :
	ClassName(className), Expression(expression) {}

void ExpectedExceptionFailure::operator>>(std::ostream& stream) const {
	stream << "Expected exception of class " << ClassName << "! (" << Expression << " did not throw)";
}

}

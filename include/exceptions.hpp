#pragma once

#include <cstdint>
#include <exception>
#include <string>

#include "datastream.hpp"

namespace DS::Exceptions {

class Exception : public std::exception {
protected:
	Exception() = default;
};

class MessageException : public virtual Exception {
public:
	MessageException() = delete;
	virtual const char *what() const noexcept override;
protected:
	explicit MessageException(std::string&& message);
private:
	std::string Message;
};

class EndOfStream final : public virtual Exception {
public:
	explicit EndOfStream() = default;
	virtual const char *what() const noexcept override;
};

class InvalidEncoding : public virtual Exception {
public:
	explicit InvalidEncoding() = default;
};

class InvalidFirstByte final : public virtual InvalidEncoding, public virtual MessageException {
public:
	explicit InvalidFirstByte(unsigned char byte);
};

class InvalidContinuationByte final : public virtual InvalidEncoding, public virtual MessageException {
public:
	explicit InvalidContinuationByte(unsigned char byte);
};

class ExpectedContinuationByte final : public virtual InvalidEncoding {
public:
	explicit ExpectedContinuationByte() = default;
	virtual const char *what() const noexcept override;
};

class InvalidSurrogateOrder final : public virtual InvalidEncoding {
public:
	explicit InvalidSurrogateOrder() = default;
	virtual const char *what() const noexcept override;
};

class ExpectedLowSurrogate final : public virtual InvalidEncoding {
public:
	explicit ExpectedLowSurrogate() = default;
	virtual const char *what() const noexcept override;
};

class InvalidSurrogate final : public virtual InvalidEncoding, public virtual MessageException {
public:
	explicit InvalidSurrogate(uint_least32_t surrogate);
};

class ExpectedEvenByte final : public virtual InvalidEncoding {
public:
	explicit ExpectedEvenByte() = default;
	virtual const char *what() const noexcept override;
};

class PredicateException : public virtual Exception {
public:
	explicit PredicateException() = default;
};

class CharacterPredicateException : public virtual PredicateException {
public:
	explicit CharacterPredicateException() = default;
};

class CharacterMismatch final : public virtual CharacterPredicateException, public virtual MessageException {
public:
	explicit CharacterMismatch(Codepoint predicate, Codepoint mismatch);
};

class UnexpectedCharacter final : public virtual CharacterPredicateException, public virtual MessageException {
public:
	explicit UnexpectedCharacter(Codepoint predicate);
};

class PredicateViolation final : public virtual CharacterPredicateException, public virtual MessageException {
public:
	explicit PredicateViolation(Codepoint cp);
};

class NamedPredicateViolation final : public virtual CharacterPredicateException, public virtual MessageException {
public:
	explicit NamedPredicateViolation(Codepoint cp, const char *predicateName);
};

}

#include "suite.hpp"

#include <cstring>
#include <iostream>

int main() {
	if (TestCollection.GetSuites().empty()) {
		std::cout << "No testing suites available!" << std::endl;
		return 255;
	}
	unsigned long successfulTests = 0;
	unsigned long unsuccessfulTests = 0;
	for (auto suite : TestCollection.GetSuites()) {
		std::cout << "Running tests in suite \"" << suite->Name << '\"' << std::endl;
		for (auto test : suite->GetTests()) {
			std::cout << '\t' << test->Name << ':' << std::string(suite->GetTestNameWidth() + 1 - std::strlen(test->Name), ' ');
			try {
				test->Implementation();
			} catch (dumbtest::Failure& fail) {
				if (test->FailExpected()) {
					std::cout << "\033[32mEXPECTED fail\033[37m - " << fail << std::endl;
					successfulTests++;
					continue;
				} else {
					std::cout << "\033[31mFAIL\033[37m - " << fail << std::endl;
					unsuccessfulTests++;
					continue;
				}
			} catch (std::exception& except) {
				auto padding = std::string((suite->GetTestNameWidth() + 1 + 8) / 8 + 1, '\t');
				std::cout << "\033[31mEXCEPTION\033[3m\n" << padding;
				for (auto c : std::string{except.what()}) {
					std::cout << c;
					if (c == '\n')
						std::cout << padding;
				}
				std::cout << "\033[37m\033[23m" << std::endl;
				unsuccessfulTests++;
				continue;
			}
			if (test->FailExpected()) {
				std::cout << "\033[31mUNEXPECTED pass\033[37m" << std::endl;
				unsuccessfulTests++;
				continue;
			}
			std::cout << "\033[32mPASS\033[37m" << std::endl;
			successfulTests++;
		}
	}

	auto totalTests = successfulTests + unsuccessfulTests;
	std::cout << "\n\033[1m" << totalTests << "\033[22m " << (totalTests == 1 ? "test" : "tests") << " ran:";
	if (successfulTests > 0)
		std::cout << "\n\t\033[1;2;32m" << successfulTests << " \033[22m" << (successfulTests == 1 ? "test" : "tests") << " completed successfully\033[37m";
	if (unsuccessfulTests > 0)
		std::cout << "\n\t\033[1;2;31m" << unsuccessfulTests << " \033[22m" << (unsuccessfulTests == 1 ? "test" : "tests") << " completed unsuccessfully\033[37m";
	std::cout << std::endl;
	return unsuccessfulTests > 0;
}

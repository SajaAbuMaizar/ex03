#pragma once

class InvalidPath :public std::exception
{
public:
	InvalidPath() {};
	~InvalidPath() {};
	const char* what() const noexcept {
		return "\nInvalid path: path file doesn't exist, try again!\n";
	}
};

class FileError :public std::exception
{
public:
	FileError() {};
	~FileError() {};
	const char* what() const noexcept {
		return "\nError at line: ";
	}
};


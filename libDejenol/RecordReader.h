#pragma once

#include<fstream>
#include<cassert>

template <size_t RECORD_LENGTH> class RecordReader {
	uint8_t buffer[RECORD_LENGTH];
	std::ifstream file;
	size_t cursor;

public:
	RecordReader(const std::string& filename) : buffer{ 0 } {
		file.open(filename, std::ios::binary);
		if (!file)
			throw "Unable to open file";
	}
	~RecordReader() {
		file.close();
	}

	void checklength(int length) {
		if (RECORD_LENGTH == 0) {
			read(length);
		}
		else {
			assert(cursor + length <= RECORD_LENGTH);
		}
	}

	void read(int length = RECORD_LENGTH) {
		cursor = 0;
		if (length == 0) {
			return;
		}
		file.read((char*)buffer, length);
		if (file.fail() && !file.eof())
			throw "File Read Error";
	}

	//seek cursor to the n'th record
	void seek(size_t recnum) {
		if (RECORD_LENGTH == 0) {
			file.seekg(recnum);
		}
		else {
			file.seekg((std::ifstream::pos_type)RECORD_LENGTH * (recnum - 1));
		}
	}

	//The 4 basic raw reading functions
	uint8_t& get(uint8_t & var) {
		checklength(1);
		var = buffer[cursor];
		cursor += 1;
		return var;
	}
	uint16_t& get(uint16_t & var) {
		checklength(2);
		var = buffer[cursor] + (buffer[cursor + 1] << 8);
		cursor += 2;
		return var;
	}
	uint32_t & get(uint32_t & var) {
		checklength(4);
		var = 0;
		for (size_t i = 0; i < 4; i++) {
			var = (var << 8) + buffer[cursor + 3 - i];
		}
		cursor += 4;
		return var;
	}
	uint64_t& get(uint64_t & var) {
		checklength(8);
		var = 0;
		for (size_t i = 0; i < 8; i++) {
			var = (var << 8) + buffer[cursor + 7 - i];
		}
		cursor += 8;
		return var;
	}

	//Basic raw read functions for more types
	int8_t& get(int8_t & var) {
		get(*reinterpret_cast<uint8_t*>(&var));
		return var;
	}
	int16_t& get(int16_t & var) {
		get(*reinterpret_cast<uint16_t*>(&var));
		return var;
	}
	int32_t& get(int32_t & var) {
		get(*reinterpret_cast<uint32_t*>(&var));
		return var;
	}
	int64_t& get(int64_t & var) {
		get(*reinterpret_cast<uint64_t*>(&var));
		return var;
	}
	float& get(float& var) {
		get(*reinterpret_cast<uint32_t*>(&var));
		return var;
	}

	//Read string. When length is 0, 16 bit length will be read from the stream first.
	std::string& get(std::string & str, uint16_t length = 0) {
		if (!length) {
			length = getWord();
		}
		checklength(length);
		str = std::string(reinterpret_cast<char*>(buffer + cursor), reinterpret_cast<char*>(buffer + cursor + length));
		cursor = cursor + length;
		return str;
	}

	template<typename T>
	void getArray(T * var, size_t len) {
		for (size_t i = 0; i < len; i++) {
			get(var[i]);
		}
	}

	//
	// Named Get functions that return the values
	//

	std::string getString(uint16_t len = 0) {
		std::string var;
		return get(var, len);
	}

	uint8_t getByte() { //VB3 'String * 1'
		uint8_t var;
		return get(var);
	}
	int16_t getWord() { //VB3 'Integer'
		int16_t var;
		return get(var);
	}
	int32_t getDword() { //VB3 'Long'
		int32_t var;
		return get(var);
	}
	int64_t getIntCurrency() {//VB3 Currency rounded to 64-bit integer
		int64_t var;
		return get(var) / 10000;
	}
	float getFloat() { //VB3 Float
		float f;
		return get(f);
	}
	int64_t getCurrency() {//VB3 Currency is 64-bit integer 100th of a cent.
		int64_t var;
		get(var);
		return var;
	}


};

#pragma once

template<typename T>
struct Uniform
{
   private:

	Uniform();
	Uniform(const Uniform&);
	Uniform& operator=(const Uniform&);
	~Uniform();

   public:

	static void Set(int slot, const T& value);
	static void Set(int slot, const T* inputArray, unsigned int arrayLength);
	static void Set(int slot, const std::vector<T>& inputArray);
};

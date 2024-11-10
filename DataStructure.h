#pragma once
#include<iostream>

//This class is used to store the position and direction 
class Vec2 {
public:
	float x, y;

	Vec2(float _x, float _y) : x(_x), y(_y) {}
	Vec2() : x(0), y(0) {};

	Vec2 operator+(const Vec2& other);
	Vec2 operator-(const Vec2& other);
	Vec2 operator*(const Vec2& other);
	Vec2 operator*(float& other);
	Vec2 operator/(const Vec2& other);

	//Get the magnitude of the vector
	float magnitude() const;
	//get x^2+y^2
	float length2() const;

	//Normalize the vector
	void normalize();

	//make x and y to 0
	void clear();
};

//This class is used to store the enemies in Enemypool
template<class T>
class myVector {
public:
	T** Data;
	size_t size;
	size_t capacity;

	//Because the size of the vector depends on the number of enemies, the default capacity is 64
	myVector(size_t _capacity = 64) :Data(nullptr), size(0), capacity(_capacity) {
		Data = new T * [capacity];
	}
	~myVector() {
		for (size_t i = 0; i < size; i++) {
			delete Data[i];
		}
		delete[] Data;
	}
	//resize the vector if the size is larger than the capacity
	void resize(size_t NewCapacity) {
		if (NewCapacity > capacity) {
			T** temp = new T * [NewCapacity];
			for (size_t i = 0; i < size; i++) {
				temp[i] = Data[i];
			}
			delete[] Data;
			Data = temp;
			capacity = NewCapacity;
		}
	}
	//Fill Data with [capacity] default T elements
	void Initialize() {
		for (size_t i = 0; i < capacity; i++) {
			T* temp = new T();
			push_back(temp);
		}
	}
	//add an element to the end of the vector
	void push_back(T* Element) {
		if (size == capacity) {
			resize(capacity * 2);
		}
		Data[size++] = Element;
	}

	//get the pointer at the index and delete it in the vector
	T* erase(size_t index) {
		if (index < size) {
			if (index == size - 1) return Data[--size];
			else {
				T* temp = Data[index];
				Data[index] = Data[--size];
				return temp;
			}

		}
		else throw std::runtime_error("Out of range");
	}
	//get the last pointer in the vector.if the vector is empty, return new pointer
	T* acquire() {
		if (size == 0) {
			T* temp = new T();
			return temp;
		}
		else {
			return erase(size - 1);
		}
	}

	T*& operator[](size_t index) {
		if (index >= size) throw std::runtime_error("Out of range");
		return Data[index];
	}

	//find the element in the vector and return the index


	size_t find(T* Element) {
		//std::cout << "Element:" << Element << std::endl;
		for (size_t i = 0; i < size; i++) {
			//std::cout << Data[i] << std::endl;
			if (Data[i] == Element) return i;
		}
		throw std::runtime_error("Not found");
	}

};

#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>

using namespace std;

template <class T> 
struct Animation
{
	vector<T> m_vec;	// wartosci
	float m_time;		// czas pojedynczej animacji

	Animation();
	Animation(size_t cnt, T defVal, float time = 1.f); // ilosc wartosci, domyslna wartosc , czas pojedynczej animacji
	T getVal(float t); // t - czas
	T getVal(size_t index, float t); // t - czas od m_vec[index] do nastepnego, od punktu
};

#endif
#include "Animation.h"
#include <glm/glm.hpp>

template <class T>
Animation<T>::Animation()
{
	;
}

template <class T>
Animation<T>::Animation(size_t cnt, T defVal, float time)
{
	m_vec.assign(cnt, defVal);
	m_time = time;
}

template <class T>
T Animation<T>::getVal(float t) 
{
	t /= m_time; // 5 / 2 = 2.5 - index klatki , pol klatki drugiej i pol klatki trzeciej
	size_t index = (int)t % m_vec.size(); // 10s calosc / 4 klatki animacji -> to % = 2 // index klatki od ktorej odchodzi
	t -= (float)((int)t);
	return m_vec[index] * (1.f - t) + m_vec[(index + 1) % m_vec.size()] * t;
}

template <class T>
T Animation<T>::getVal(size_t index, float t) // t - od 0 do 1
{
	return m_vec[index] * (1.f - t) + m_vec[(index + 1) % m_vec.size()] * t; // index to klatka od ktorej sie rozpoczyna, index + 1 to kolejna
}

template class Animation<float>;		// musi byc bo inaczaj c++ sie obra¿a bo tamplate
template class Animation<glm::vec3>;	// musi byc bo inaczaj c++ sie obra¿a bo tamplate 
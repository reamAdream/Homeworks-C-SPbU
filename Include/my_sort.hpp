#pragma once

#include <vector>
#include <iterator>
#include <functional>


//Функция слияния двух отсортированных частей с помощью компараторов
template <typename It, typename Compare>
void mergeRange(It first, It middle, It last, Compare comp) {
	auto i = first;
	auto j = middle;
	//Определяем тип элементов через тип итератора
	using T = typename std::iterator_traits<It>::value_type;
	std::vector<T> temp;
	//Сравниваем текущие элементы обеих отсортированных половин
	while (i < middle && j < last) {
		if (comp(*i, *j)) {
			temp.push_back(*i);
			++i;
		}
		else {
			temp.push_back(*j);
			++j;
		}
	}
	while (i < middle) {
		temp.push_back(*i);
		++i;
	}
	while (j < last) {
		temp.push_back(*j);
		++j;
	}
	for (int k = 0; k < temp.size(); ++k) {
		*(first + k) = temp[k];
	}
}

//рекурсивная сортировка слиянием с помощью компараторов
template <typename It, typename Compare = std::less<typename std::iterator_traits<It>::value_type>>
void mySort(It first, It last, Compare comp = Compare{}) {
	if (last - first <= 1) { return; }
	auto middle = first + (last - first) / 2;
	mySort(first, middle, comp);
	mySort(middle, last, comp);
	mergeRange(first, middle, last, comp);
}

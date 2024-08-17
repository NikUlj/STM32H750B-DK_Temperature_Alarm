/*
 * conversion_fun.c
 *
 *  Created on: Aug 9, 2024
 *      Author: Nik
 */

#include "temp_conversion.h"

TempPair Binary_search(double res) {
	int left = 0;
	int right = LUTSize - 1;

	while (left <= right) {
		int mid = left + (right - left) / 2;

		if (LUT[mid].res >= res) {
			if (LUT[mid+1].res <= res) {
				return Get_temps(mid);
			} else {
				left = mid + 1;
			}
		} else {
			if (LUT[mid-1].res >= res) {
				return Get_temps(mid-1);
			} else {
				right = mid - 1;
			}
		}
	}

	TempPair error;
	error.res1 = -1;
	return error;
}

TempPair Get_temps(int mid) {
	TempPair result;
	result.temp1 = LUT[mid].temp;
	result.res1 = LUT[mid].res;
	result.temp2 = LUT[mid+1].temp;
	result.res2 = LUT[mid+1].res;
	return result;
}

float Linear_interpolation(TempPair temps, double res) {
	float weight = (res - temps.res1) / (temps.res2 - temps.res1);

	return temps.temp1 + weight;
}



/*
 * temp_conversion.h
 *
 *  Created on: Aug 8, 2024
 *      Author: Nik
 */

#ifndef INC_TEMP_CONVERSION_H_
#define INC_TEMP_CONVERSION_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int res;
    int temp;
} LUTEntry;

typedef struct {
	int temp1;
	int res1;
	int temp2;
	int res2;
} TempPair;

extern const LUTEntry LUT[];
extern const int LUTSize;

TempPair Binary_search(double res);
TempPair Get_temps(int mid);
float Linear_interpolation(TempPair temps, double res);

#ifdef __cplusplus
}
#endif

#endif /* INC_TEMP_CONVERSION_H_ */

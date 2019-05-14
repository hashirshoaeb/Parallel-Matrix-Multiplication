#include<iostream>
#include<Windows.h>
#include<omp.h>
//#include <chrono>

const int size = 3;
void clean(int **temp);
int **single_Core_Multiplication(int **a, int **b, int **c);
int **single_Core_Multiplication(int **a, int **b);
int **multi_Core_Multiplication(int **a, int **b, int **c);
void display(int **mult);
bool Lock[size][size];
int main()
{
	int **a = new int*[size]/*sizeY*/, **b = new int*[size], **c = new int*[size], **mult;
	for (int i = 0; i < size/*sizeY*/; ++i)
	{
		a[i] = new int[size];//sizeX
		b[i] = new int[size];//sizeX
		c[i] = new int[size];//sizeX
	}
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
		{
			a[i][j] = b[i][j] = c[i][j] = (j + 1) + (i * size);
			//std::cin >> a[i][j];
		}
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
		{
			//a[i][j] = b[i][j] = c[i][j] = (j + 1) + (i * size);
			//std::cin >> b[i][j];
		}
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
		{
			//a[i][j] = b[i][j] = c[i][j] = (j + 1) + (i * size);
			//std::cin >> c[i][j];
		}
	int **temp;
	double start, end;
//--------------------------------------SINGLE CORE turn by turn----------------------------------------//
	start = omp_get_wtime();
	mult = single_Core_Multiplication(temp = single_Core_Multiplication(a, b), c);
	//Displaying the multiplication of two matrix. // SINGLE CORE
	display(mult);
	end = omp_get_wtime();
	std::cout << "It took turn by turn multiplication " << end - start << " seconds.";
	std::cout << std::endl;
	clean(temp); clean(mult);
//--------------------------------------SINGLE CORE ---------------------------------------------------//
	std::cout << std::endl;
	start = omp_get_wtime();
	mult = single_Core_Multiplication(a, b, c);
	//display(mult);
	end = omp_get_wtime();
	std::cout << "It took single_Core_Multiplication " << end - start << " seconds.";
	clean(mult);
//---------------------------------------MULTI CORE--------------------------------------------------//
	std::cout << std::endl;
	start = omp_get_wtime();
	mult = multi_Core_Multiplication(a, b, c);
	display(mult);
	end = omp_get_wtime();
	std::cout << "It took multi_Core_Multiplication " << end - start << " seconds.";
	clean(mult);
	std::cout << std::endl;
	system("PAUSE");
	return 0;
}

void display(int **mult)
{
	std::cout << std::endl << "Output Matrix: " << std::endl;
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
		{
			std::cout << " " << mult[i][j];
			if (j == size - 1)
				std::cout << std::endl;
		}
}
void clean(int **temp)
{
	for (int i = 0; i < size; ++i)
		delete[] temp[i];
	delete[] temp;
}
int **single_Core_Multiplication(int **a, int **b)
{
	int **mult = new int*[size];
	for (int i = 0; i < size/*sizeY*/; ++i)
		mult[i] = new int[size];//sizeX
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			mult[i][j] = 0;
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
		{
			for (int k = 0; k < size; k++)
			{
				mult[i][j] += a[i][k] * b[k][j];
			}
			
		}
	return mult;
}
int **single_Core_Multiplication(int **a, int **b, int **c)
{
	int **mult1 = new int*[size], **mult2 = new int*[size];
	for (int i = 0; i < size/*sizeY*/; ++i)
	{
		mult1[i] = new int[size];//sizeX
		mult2[i] = new int[size];//sizeX
	}
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
		{
			mult1[i][j] = 0;
			mult2[i][j] = 0;
		}
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			for (int t = 0; t < size; t++)
			{
				mult1[i][j] += a[i][t] * b[t][j];
				Lock[i][j] = true;
			}
		}
		// after one iteration row 1 of mult is filled	// so now we will multiply it with third matrics
		for (int j = 0; j < size; j++)
		{
			for (int k = 0; k < size; k++)
			{
				while (!Lock[i][j]); //wait
				mult2[i][j] += mult1[i][k] * c[k][j];
			}
		}
	}
	clean(mult1);
	return mult2;
}
int **multi_Core_Multiplication(int **a, int **b, int **c)
{
	int **mult1 = new int*[size], **mult2 = new int*[size];
	for (int i = 0; i < size/*sizeY*/; ++i)
	{
		mult1[i] = new int[size];//sizeX
		mult2[i] = new int[size];//sizeX
	}
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			mult1[i][j] = 0;
			mult2[i][j] = 0;
			Lock[i][j] = 0;
		}
	}
	// project demand start............................................ //
#pragma omp parallel for //collapse(2)
	for (int i = 0; i < size; i++)
	{
#pragma omp parallel for
		for (int j = 0; j < size; j++)
		{
			for (int t = 0; t < size; t++)
			{
				mult1[i][j] += a[i][t] * b[t][j];
			}
			//Lock[i][j] = true;
		}
			// after one iteration row 1 of mult is filled
			// so now we will multiply it with third matrics
		for (int j = 0; j < size; j++)
		{
			//while (!Lock[i][j]);
			for (int k = 0; k < size; k++)
			{
				mult2[i][j] += mult1[i][k] * c[k][j];
			}
		}
	}
	// project demand end..............................................//
	clean(mult1);
	return mult2;
}
#ifndef DATA_H
#define DATA_H

#include <ctime>
#include <cstring>
#include <algorithm>
#include <QVector>

using namespace std;

class Data {
public:
  Data();
  void setPuzzle(const QVector<int>& puzzle);
  void setSolution(const QVector<int>& solution);
  void newPuzzle(int blank);
  int getPuzzle(int r, int c);
  void setUserAns(int r, int c, int n);
  int getUserAns(int r, int c);
  bool checkUserAns(void);
  int getAns(int r, int c);
  bool solveSudoku();

private:
  bool sameNumberExists(int(*a)[10], int r, int c, int n);
  bool fillNumber(int(*a)[10], int r, int c);
  void randomRow(int(*a)[10], int r);
  void randomSwapPuzzle(int(*a)[10]);
  void swapColumn(int(*a)[10], int m, int n);
  void fillArrayWithVector(const QVector<int> data, int array[10][10]);

  int a[10][10];//puzzle data without blanks
  int b[10][10];//puzzle data with blanks
  int c[10][10];//user given input


};

#endif // DATA_H

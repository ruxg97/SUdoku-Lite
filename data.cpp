#include "data.h"
#include "exactcovermatrix.h"

Data::Data() {
    srand(static_cast<unsigned int>(time(0)));
    memset(c, 0, sizeof(c));
}

void Data::setPuzzle(const QVector<int>& puzzle)
{
    fillArrayWithVector(puzzle, b);
}

void Data::setSolution(const QVector<int>& solution)
{
    fillArrayWithVector(solution, a);
}


bool Data::sameNumberExists(int(*a)[10], int r, int c, int n) {
    bool e = 0;
    for (int i = 1; i < c; ++i) {
        if (a[r][i] == n) {
            e = 1;
            break;
        }
    }
    if (e == 0) {
        for (int i = 1; i < r; ++i) {
            if (a[i][c] == n) {
                e = 1;
                break;
            }
        }
    }
    if (e == 0) {
        int gmr = (r - 1) / 3 * 3 + 1;//going minimum row
        int gmc = (c - 1) / 3 * 3 + 1;//going minimum column
        for (int i = gmr; i < gmr + 3; ++i) {
            for (int j = gmc; j < gmc + 3; ++j) {
                if (!(i==r&&j==c)&&a[i][j] == n) {
                    e = 1;
                }
            }
        }
    }
    return e;
}

bool Data::fillNumber(int(*a)[10], int r, int c) {
    if (r > 9 || c > 9) {
        return 1;
    }
    for (int i = 1; i <= 9; ++i) {
        int e = sameNumberExists(a, r, c, i);
        if (e) {
            continue;
        } else {
            a[r][c] = i;
            if (c == 9) {
                if (fillNumber(a, r + 1, 1)) {
                    return 1;
                } else {
                    a[r][c] = 0;
                    continue;
                }
            } else {
                if (fillNumber(a, r, c + 1)) {
                    return 1;
                } else {
                    a[r][c] = 0;
                    continue;
                }
            }
        }
    }
    return 0;
}

void Data::randomRow(int(*a)[10], int r) {
    for (int i = 1; i <= 9; ++i) {
        a[r][i] = i;
    }
    random_shuffle(&a[r][1], &a[r][10]);
}

void Data::newPuzzle(int blank) {
    memset(a, 0, sizeof(int) * 100);
    randomRow(a, 1);
    fillNumber(a, 2, 1);
    randomSwapPuzzle(a);
    memcpy(b, a, sizeof(int) * 100);
    for (int i = 0; i < blank; i++) {
        b[rand() % 9 + 1][rand() % 9 + 1] = 0;
    }
}

int Data::getPuzzle(int r, int c) {
    return b[r][c];
}

void Data::setUserAns(int _r, int _c, int n) {
    c[_r][_c] = n;
}

int Data::getUserAns(int r, int c)
{
    return Data::c[r][c];
}

bool Data::checkUserAns(void) {
    //switch()
    bool ok = true;
    for (int i = 1; i <= 9; ++i) {
        for (int j = 1; j <= 9; ++j) {
            if (sameNumberExists(c, i, j, c[i][j]) == 1) return false;
            if (c[i][j] == 0) return false;
        }
    }
    return ok;
}

int Data::getAns(int r, int c) {
    return a[r][c];
}

void Data::randomSwapPuzzle(int(*a)[10]) {
    const int swapColumnTimes = 10;
    //swap column
    for (int c = 0; c < swapColumnTimes; ++c) {
        int p = rand() % 3 * 3 + 1;
        int m = p + rand() % 3;
        int n = p + rand() % 3;
        swapColumn(a, m, n);
    }
    //swap blocks
    int p = rand() % 3 * 3 + 1;
    if (p == 1 || p == 4) {
        swapColumn(a, p, p + 3);
        swapColumn(a, p + 1, p + 4);
        swapColumn(a, p + 2, p + 5);
    } else {
        swapColumn(a, p, p -6);
        swapColumn(a, p + 1, p -5);
        swapColumn(a, p + 2, p -4);
    }
}

void Data::swapColumn(int(*a)[10], int m, int n) {
    int t[10];
    for (int i = 1; i <= 9; ++i) {
        t[i] = a[i][m];
    }
    for (int i = 1; i <= 9; ++i) {
        a[i][m] = a[i][n];
    }
    for (int i = 1; i <= 9; ++i) {
        a[i][n] = t[i];
    }

}

void Data::fillArrayWithVector(const QVector<int> data, int array[10][10])
{
    int i = 1, j = 1;
    for(int n : data){
        array[i][j] = n;
        if(j == 9){
            j = 1;
            ++i;
        }
        else ++j;
    }
}

using namespace std;

const int N_Cell = 3;
const int N = 9;
const int OFFSET = 9*9;
const int ROWS = N*N*N;
const int COLS = N*N*4;

bool Data::solveSudoku()
{
    int grid[N][N];
    for(int i = 0;i < N;++i){
        for(int j = 0;j < N;++j){
            grid[i][j] = c[i + 1][j + 1];
        }
    }

    //tranform a sudoku to a exact cover problem
    int** sudoku_matrix = new int*[ROWS];
    for( int i=0; i<ROWS; i++ )
        sudoku_matrix[i] = new int[5];
    for( int i=0; i<N; i++ )
        for( int j=0; j<N; j++ )
        {
            int b = (i/N_Cell)*N_Cell + (j/N_Cell);
            int r = i*N*N + j*N;
            for( int k=1; k<=N; k++ )
            {
                sudoku_matrix[r][0]=i*N+j+1;
                sudoku_matrix[r][1]=OFFSET+(i*N+k);
                sudoku_matrix[r][2]=OFFSET*2+(j*N+k);
                sudoku_matrix[r][3]=OFFSET*3+(b*N+k);
                sudoku_matrix[r][4]=-1;
                r++;
            }
        }
    ExactCoverMatrix sudoku( ROWS, COLS, sudoku_matrix );
    //cover those have been filled
    for( int i=0; i<N; i++ )
        for( int j=0; j<N; j++ )
        {
            int k;
            if((k = grid[i][j]))
            {
                int b = (i/N_Cell)*N_Cell + j/N_Cell;
                sudoku.cover( i*N+j+1 );
                sudoku.cover( OFFSET+( i*N+k ) );
                sudoku.cover( OFFSET*2+(j*N+k) );
                sudoku.cover( OFFSET*3+(b*N+k) );
            }
        }
    int solved = 0;
    if( (solved = sudoku.search())>0 )
    {
        int c, r, k;
        for( int i=0; i<solved; i++ )
        {
            r = sudoku.disjointSubet[i]/(N*N);
            c = (sudoku.disjointSubet[i]/N)%N;
            k = sudoku.disjointSubet[i]%N;
            grid[r][c] = k+1;
            Data::c[r + 1][c + 1] = k + 1;
        }
        return true;
    }
    else{
        return false;
    }
}

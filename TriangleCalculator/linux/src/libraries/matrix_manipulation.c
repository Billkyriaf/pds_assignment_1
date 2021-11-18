#include "matrix_manipulation.h"

/**
 * Calculates the number of triangles of a CSR matrix C where C = A .* (A * A) with A being the initial sparse matrix of
 * the graph. The steps for finding the triangles from the C matrix are C
 *          1. Multiply the C matrix with a column matrix e with all the elements equal to 1
 *          2. The result will be a column matrix with every elements' value denoting how many time the ith node takes
 *             part in a triangle.
 *             e.g
 *                  if A[5] = 6 the 6th node (0-based system) is found in 6 triangles
 *
 *          3. The next step is to divide this column matrix by 2. The reason for this is that we have calculated every
 *             triangle 2 times.
 *             e.g.
 *                  if we have nodes A B and C completing a triangle we have counted the ABC triangle and the ACB
 *                  triangle which is essentially the same triangle.
 *
 *          4. By adding all the elements of the resulting matrix together and dividing by 3 we finally get the number of
 *             triangles.
 *
 * This long and resource consuming process can be simplified by adding all the elements of the A matrix and dividing
 * by 6 (sum * 1/2 * 1/3 = sum * 1/6)
 *
 * @param matrix the matrix to count the triangle of
 * @return The number of triangles
 */
int measureTriangles(CSR *matrix) {
    int triangles = 0;

    for (int i = 0; i < matrix->nonzero; ++i) {
        triangles += matrix->A[i];
    }

    return triangles / 6;
}

/**
 * Compares the a row and a column and finds the common elements between them. The elements of the column and the row
 * are the indexes of the nonzero elements so if two elements match in the ordinary multiplication this would mean that
 * the values in those positions would be multiplied. Since all the values are 1 in the sparse matrix we simply find the
 * count of those pairs. Both the column and the row are derived from the same vector because the initial matrix is
 * symmetric and hence the CSC and CSR representations are identical.
 *
 * @param elements  The JA array of the input matrix
 * @param colStart  The starting index of the column to compare
 * @param colEnd    The finishing index of the column to compare
 * @param rowStart  The starting index of the column to compare
 * @param rowEnd    The finishing index of the column to compare
 *
 * @return          The count of the common elements found in the row and the column. This count represents the dot
 *                  product of the column with the row of the matrix since all the nonzero elements are 1
 */
int colRowProduct(const int *elements, int colStart, int colEnd, int rowStart, int rowEnd) {
    int res = 0;
    int i = colStart, j = rowStart;

    // Both parts of the elements array are sorted in an increasing order so this is a good way to check for common elements
    while (colEnd >= i && rowEnd >= j){
        if (elements[i] < elements[j]){
            i++;

        } else if (elements[i] > elements[j]){
            j++;

        } else {
            res++;
            i++;
            j++;
        }
    }

    return res;
}


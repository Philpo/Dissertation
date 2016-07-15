#include "ImplicitEuler.h"
#include "Utils.h"

void ImplicitEuler::preCalculateHessian(int columns, int n, float structuralStiffness, float shearStiffness, float flexionStiffness) {
  this->n = n;
  hessian = new float*[n];
  for (int i = 0; i < n; i++) {
    hessian[i] = new float[n];

    for (int j = 0; j < n; j++) {
      switch (connections[i][j]) {
        case 1:
          hessian[i][j] = structuralStiffness;
          break;
        case 2:
          hessian[i][j] = shearStiffness;
          break;
        case 3:
          hessian[i][j] = flexionStiffness;
          break;
        default:
          hessian[i][j] = 0.0f;
      }
    }
  }

  for (int i = 0; i < n; i++) {
    float sum = 0.0f;
    for (int j = 0; j < n; j++) {
      sum += hessian[i][j];
    }
    hessian[i][i] = -sum;
    //// corner particles first
    //if (i == 0) {
    //  hessian[i][1] = structuralStiffness;
    //  hessian[i][i + columns] = structuralStiffness;

    //  hessian[i][i + columns + 1] = shearStiffness;

    //  hessian[i][2] = flexionStiffness;
    //  hessian[i][i + (columns * 2)] = flexionStiffness;

    //  hessian[i][i] = -((structuralStiffness * 2) + shearStiffness + (flexionStiffness * 2));

    //  continue;
    //}
    //else if (i == n - 1) {
    //  hessian[i][i - 1] = structuralStiffness;
    //  hessian[i][i - columns] = structuralStiffness;

    //  hessian[i][i - columns - 1] = shearStiffness;

    //  hessian[i][i - 2] = flexionStiffness;
    //  hessian[i][i - (columns * 2)] = flexionStiffness;

    //  hessian[i][i] = -((structuralStiffness * 2) + shearStiffness + (flexionStiffness * 2));
    //  
    //  continue;
    //}
    //else if (i == columns - 1) {
    //  hessian[i][i - 1] = structuralStiffness;
    //  hessian[i][i + columns] = structuralStiffness;

    //  hessian[i][i + columns - 1] = shearStiffness;

    //  hessian[i][i - 2] = flexionStiffness;
    //  hessian[i][i + (columns * 2)] = flexionStiffness;

    //  hessian[i][i] = -((structuralStiffness * 2) + shearStiffness + (flexionStiffness * 2));

    //  continue;
    //}
    //else if (i == n - columns) {
    //  hessian[i][i + 1] = structuralStiffness;
    //  hessian[i][i - columns] = structuralStiffness;

    //  hessian[i][i - columns + 1] = shearStiffness;

    //  hessian[i][i + 2] = flexionStiffness;
    //  hessian[i][i - (columns * 2)] = flexionStiffness;

    //  hessian[i][i] = -((structuralStiffness * 2) + shearStiffness + (flexionStiffness * 2));

    //  continue;
    //}

    //// edge particles next
    //if (i < columns - 2) {
    //  hessian[i][i + 1] = structuralStiffness;
    //  hessian[i][i - 1] = structuralStiffness;
    //  hessian[i][i + columns] = structuralStiffness;

    //  hessian[i][i + columns + 1] = shearStiffness;
    //  hessian[i][i + columns - 1] = shearStiffness;

    //  hessian[i][i + 2] = flexionStiffness;
    //  hessian[i][i + (columns * 2)] = flexionStiffness;

    //  hessian[i][i] = -((structuralStiffness * 3) + (shearStiffness * 2) + (flexionStiffness * 2));

    //  continue;
    //}
    //else if (i >= columns - 2 && i < columns - 1) {
    //  hessian[i][i + 1] = structuralStiffness;
    //  hessian[i][i - 1] = structuralStiffness;
    //  hessian[i][i + columns] = structuralStiffness;

    //  hessian[i][i + columns + 1] = shearStiffness;
    //  hessian[i][i + columns - 1] = shearStiffness;

    //  hessian[i][i - 2] = flexionStiffness;
    //  hessian[i][i + (columns * 2)] = flexionStiffness;

    //  hessian[i][i] = -((structuralStiffness * 3) + (shearStiffness * 2) + (flexionStiffness * 2));

    //  continue;
    //}
    //else if (i > n - columns && i < n - 2) {
    //  hessian[i][i + 1] = structuralStiffness;
    //  hessian[i][i - 1] = structuralStiffness;
    //  hessian[i][i - columns] = structuralStiffness;

    //  hessian[i][i - columns + 1] = shearStiffness;
    //  hessian[i][i - columns - 1] = shearStiffness;

    //  hessian[i][i + 2] = flexionStiffness;
    //  hessian[i][i - (columns * 2)] = flexionStiffness;

    //  hessian[i][i] = -((structuralStiffness * 3) + (shearStiffness * 2) + (flexionStiffness * 2));

    //  continue;
    //}
    //else if (i >= n - 2 && i < n - 1) {
    //  hessian[i][i + 1] = structuralStiffness;
    //  hessian[i][i - 1] = structuralStiffness;
    //  hessian[i][i - columns] = structuralStiffness;

    //  hessian[i][i - columns + 1] = shearStiffness;
    //  hessian[i][i - columns - 1] = shearStiffness;

    //  hessian[i][i - 2] = flexionStiffness;
    //  hessian[i][i - (columns * 2)] = flexionStiffness;

    //  hessian[i][i] = -((structuralStiffness * 3) + (shearStiffness * 2) + (flexionStiffness * 2));

    //  continue;
    //}
    //else if (i % columns == 0 && i / columns < ) {
    //  hessian[i][i + 1] = structuralStiffness;
    //  hessian[i][i + columns ] = structuralStiffness;
    //  hessian[i][i - columns] = structuralStiffness;

    //  hessian[i][i + columns + 1] = shearStiffness;
    //  hessian[i][i - columns - 1] = shearStiffness;

    //  hessian[i][i + 2] = flexionStiffness;
    //  hessian[i][i - (columns * 2)] = flexionStiffness;

    //  hessian[i][i] = -((structuralStiffness * 3) + (shearStiffness * 2) + (flexionStiffness * 2));

    //  continue;
    //}

    // horizontal structural springs
    //if (j == 0) {
    //  hessian[i][j + 1] = structuralStiffness;
    //}
    //else if (j == n - 1) {
    //  hessian[i][j - 1] = structuralStiffness;
    //}
    //else if (j > i + 1 || j < i - 1) {
    //  hessian[i][j] = 0.0f;
    //}
    //else {
    //  hessian[i][j + 1] = structuralStiffness;
    //  hessian[i][j - 1] = structuralStiffness;
    //}

    // vertical structural springs
    //if (i == 0) {
    //  hessian[i + 1][j] = structuralStiffness;
    //}
    //else if (i == n - 1) {
    //  hessian[i - 1][j] = structuralStiffness;
    //}
    //else if (i > i + 1 || i < i - 1) {
    //  hessian[i][j] = 0.0f;
    //}
    //else {
    //  hessian[i + 1][j] = structuralStiffness;
    //  hessian[i - 1][j] = structuralStiffness;
    //}

    //// shear springs
    //if (i == 0) {
    //  if (j == 0) {
    //    hessian[i + 1][j + 1] = shearStiffness;
    //  }
    //  else if (j == n - 1) {
    //    hessian[i + 1][j - 1] = shearStiffness;
    //  }
    //  else {
    //    hessian[i + 1][j + 1] = shearStiffness;
    //    hessian[i + 1][j - 1] = shearStiffness;
    //  }
    //}
    //else if (i == n - 1) {
    //  if (j == 0) {
    //    hessian[i - 1][j + 1] = shearStiffness;
    //  }
    //  else if (j == n - 1) {
    //    hessian[i - 1][j - 1] = shearStiffness;
    //  }
    //  else {
    //    hessian[i - 1][j + 1] = shearStiffness;
    //    hessian[i - 1][j - 1] = shearStiffness;
    //  }
    //}
    //else {
    //  hessian[i + 1][j + 1] = shearStiffness;
    //  hessian[i + 1][j - 1] = shearStiffness;
    //  hessian[i - 1][j + 1] = shearStiffness;
    //  hessian[i - 1][j - 1] = shearStiffness;
    //}

    //// horizontal flexion springs
    //if (j < n - 2) {
    //  hessian[i][j + 2] = flexionStiffness;
    //}
    //else if (j >= n - 2) {
    //  hessian[i][j - 2] = flexionStiffness;
    //}
    //else {
    //  hessian[i][j + 2] = flexionStiffness;
    //  hessian[i][j - 2] = flexionStiffness;
    //}

    //// vertical flexion springs
    //if (i < n - 2) {
    //  hessian[i + 2][j] = flexionStiffness;
    //}
    //else if (i >= n - 2) {
    //  hessian[i - 2][j] = flexionStiffness;
    //}
    //else {
    //  hessian[i + 2][j] = flexionStiffness;
    //  hessian[i - 2][j] = flexionStiffness;
    //}
  }
}

void ImplicitEuler::printHessian() {
  ofstream file("hessian.txt");

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (j == 0) {
        file << "[";
      }
      file << hessian[i][j];
      if (j == n - 1) {
        file << "]" << endl;
      }
      else {
        file << " ";
      }
    }
  }

  file.close();
}
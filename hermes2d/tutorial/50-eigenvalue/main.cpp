#define HERMES_REPORT_ALL
#include "hermes2d.h"
#include <stdio.h>

//  This example solves the eigenproblem for the Laplace operator in 
//  a square with zero boundary conditions. Python and Pysparse must
//  be installed. 
//
//  PDE: -Laplace u = lambda_k u,
//  where lambda_0, lambda_1, ... are the eigenvalues.
//
//  Domain: Square (0, pi)^2.
//
//  BC:  Homogeneous Dirichlet.
//
//  The following parameters can be changed:

const int NUMBER_OF_EIGENVALUES = 50;             // Desired number of eigenvalues.
const int P_INIT = 4;                             // Uniform polynomial degree of mesh elements.
const int INIT_REF_NUM = 3;                       // Number of initial mesh refinements.
const double TARGET_VALUE = 2.0;                  // PySparse parameter: Eigenvalues in the vicinity of this number will be computed. 
const double TOL = 1e-10;                         // Pysparse parameter: Error tolerance.
const int MAX_ITER = 1000;                        // PySparse parameter: Maximum number of iterations.
MatrixSolverType matrix_solver = SOLVER_UMFPACK;  // Possibilities: SOLVER_AMESOS, SOLVER_AZTECOO, SOLVER_MUMPS,
                                                  // SOLVER_PARDISO, SOLVER_PETSC, SOLVER_SUPERLU, SOLVER_UMFPACK.

// Boundary markers.
const int BDY_BOTTOM = 1, BDY_RIGHT = 2, BDY_TOP = 3, BDY_LEFT = 4;

// Weak forms.
#include "forms.cpp"

// Write the matrix in Matrix Market format.
void write_matrix_mm(const char* filename, Matrix* mat) 
{
  // Get matrix size.
  int ndof = mat->get_size();
  FILE *out = fopen(filename, "w" );
  if (out == NULL) error("failed to open file for writing.");

  // Calculate the number of nonzeros.
  int nz = 0;
  for (int i = 0; i < ndof; i++) {
    for (int j = 0; j <= i; j++) { 
      double tmp = mat->get(i, j);
      if (fabs(tmp) > 1e-15) nz++;
    }
  } 

  // Write the matrix in MatrixMarket format
  fprintf(out,"%%%%MatrixMarket matrix coordinate real symmetric\n");
  fprintf(out,"%d %d %d\n", ndof, ndof, nz);
  for (int i = 0; i < ndof; i++) {
    for (int j = 0; j <= i; j++) { 
      double tmp = mat->get(i, j);
      if (fabs(tmp) > 1e-15) fprintf(out, "%d %d %24.15e\n", i + 1, j + 1, tmp);
    }
  } 

  fclose(out);
}

int main(int argc, char* argv[])
{
  info("Desired number of eigenvalues: %d.", NUMBER_OF_EIGENVALUES);

  // Load the mesh.
  Mesh mesh;
  H2DReader mloader;
  mloader.load("domain.mesh", &mesh);

  // Perform initial mesh refinements (optional).
  for (int i = 0; i < INIT_REF_NUM; i++) mesh.refine_all_elements();

  // Enter boundary markers. 
  BCTypes bc_types;
  bc_types.add_bc_dirichlet(Hermes::vector<int>(BDY_BOTTOM, BDY_RIGHT, BDY_TOP, BDY_LEFT));

  // Enter Dirichlet boundary values.
  BCValues bc_values;
  bc_values.add_zero(Hermes::vector<int>(BDY_BOTTOM, BDY_RIGHT, BDY_TOP, BDY_LEFT));

  // Create an H1 space with default shapeset.
  H1Space space(&mesh, &bc_types, &bc_values, P_INIT);
  int ndof = Space::get_num_dofs(&space);
  info("ndof: %d.", ndof);

  // Initialize the weak formulation for the left hand side, i.e., H.
  WeakForm wf_left, wf_right;
  wf_left.add_matrix_form(callback(bilinear_form_left));
  wf_right.add_matrix_form(callback(bilinear_form_right));

  // Initialize matrices.
  SparseMatrix* matrix_left = create_matrix(matrix_solver);
  SparseMatrix* matrix_right = create_matrix(matrix_solver);

  // Assemble the matrices.
  bool is_linear = true;
  DiscreteProblem dp_left(&wf_left, &space, is_linear);
  dp_left.assemble(matrix_left);
  DiscreteProblem dp_right(&wf_right, &space, is_linear);
  dp_right.assemble(matrix_right);

  // Write matrix_left in MatrixMarket format.
  write_matrix_mm("mat_left.mtx", matrix_left);

  // Write matrix_left in MatrixMarket format.
  write_matrix_mm("mat_right.mtx", matrix_right);

  // Calling Python eigensolver. Solution will be written to "eivecs.dat".
  info("Calling Pysparse...");
  char call_cmd[255];
  sprintf(call_cmd, "python solveGenEigenFromMtx.py mat_left.mtx mat_right.mtx %g %d %g %d", 
	  TARGET_VALUE, NUMBER_OF_EIGENVALUES, TOL, MAX_ITER);
  system(call_cmd);
  info("Pysparse finished.");

  // Initializing solution vector, solution and ScalarView.
  double* coeff_vec = new double[ndof];
  Solution sln;
  ScalarView view("Solution", new WinGeom(0, 0, 440, 350));

  // Reading solution vectors from file and visualizing.
  double* eigenval = new double[NUMBER_OF_EIGENVALUES];
  FILE *file = fopen("eivecs.dat", "r");
  char line [64];                  // Maximum line size.
  fgets(line, sizeof line, file);  // ndof
  int n = atoi(line);            
  if (n != ndof) error("Mismatched ndof in the eigensolver output file.");  
  fgets(line, sizeof line, file);  // Number of eigenvectors in the file.
  int neig = atoi(line);
  if (neig != NUMBER_OF_EIGENVALUES) error("Mismatched number of eigenvectors in the eigensolver output file.");  
  for (int ieig = 0; ieig < neig; ieig++) {
    // Get next eigenvalue from the file
    fgets(line, sizeof line, file);
    eigenval[ieig] = atof(line);            
    // Get the corresponding eigenvector.
    for (int i = 0; i < ndof; i++) {  
      fgets(line, sizeof line, file);
      coeff_vec[i] = atof(line);
    }

    // Convert coefficient vector into a Solution.
    Solution::vector_to_solution(coeff_vec, &space, &sln);

    // Visualize the solution.
    char title[100];
    sprintf(title, "Solution %d, val = %g", ieig, eigenval[ieig]);
    view.set_title(title);
    view.show(&sln);

    // Wait for keypress.
    View::wait(HERMES_WAIT_KEYPRESS);
  }  
  fclose(file);

  delete [] coeff_vec;

  return 0; 
};


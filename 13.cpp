#include "mpi.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <iomanip>

using namespace std;

int main(int argc, char** argv) {
    int myid, n_proc;
    double start_time, end_time, send_time_start_1, send_time_start_2, send_time_start_3, send_time_end_1, send_time_end_2, parallel_time;
    int N = 0, M = 0; // Размеры двумерного массива
    vector<vector<int>> matrix;


    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &n_proc);

    // Процесс 0 инициализирует массив
    if (myid == 0) {
        if (argc < 2) {
            cerr << "Usage: mpiexec -n <num_processes> <program> <input_file>" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        ifstream input_file(argv[1]);
        if (!input_file) {
            cerr << "Error: Unable to open file " << argv[1] << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        input_file >> N >> M;
        cout << "N = " << N << " , M = " << M << endl;
        matrix.resize(N, vector<int>(M));
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < M; ++j) {
                input_file >> matrix[i][j];
            }
        }
        input_file.close();
        /*cout << "Matrix :" << endl;
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < M; ++j) {
                cout << matrix[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl*/;
    }
    start_time = MPI_Wtime();
    send_time_start_1 = MPI_Wtime();
    // Передача размеров массива всем процессам
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&M, 1, MPI_INT, 0, MPI_COMM_WORLD);
    send_time_end_1 = MPI_Wtime();

    int rows_per_proc = (N + n_proc - 1) / n_proc; // округление вверх
    int local_row_count = min(rows_per_proc, max(0, N - myid * rows_per_proc));

    vector<int> local_matrix(local_row_count * M);
    vector<int> flat_matrix(N * M);
    // Преобразование матрици в массив
    if (myid == 0) {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < M; ++j) {
                flat_matrix[i * M + j] = matrix[i][j];
            }
        }
        /*cout << "Flat matrix to Scatter:" << endl;
        for (int i = 0; i < N * M; ++i) {
            cout << flat_matrix[i] << " ";
        }
        cout << endl << endl;*/
    }

    send_time_start_2 = MPI_Wtime();
    MPI_Scatter(flat_matrix.data(), rows_per_proc * M, MPI_INT, local_matrix.data(), rows_per_proc * M, MPI_INT, 0, MPI_COMM_WORLD);
    send_time_end_2 = MPI_Wtime();

    vector<int> local_max_values(local_row_count);
    for (int i = 0; i < local_row_count; ++i) {
        int local_max_value = local_matrix[i * M];
        for (int j = 1; j < M; ++j) {
            if (local_matrix[i * M + j] > local_max_value) {
                local_max_value = local_matrix[i * M + j];
            }
        }
        local_max_values[i] = local_max_value;
    }
    parallel_time = MPI_Wtime() - send_time_end_2;

    /*cout << "Flat in work at "<< myid << " process :" << endl;
    for (int i = 0; i < local_matrix.size(); i++) {
        cout << local_matrix[i] << " ";
    }
    cout << endl;*/

    // Сбор всех локальных максимумов на процесс 0
    vector<int> global_max_values(N);
    send_time_start_3 = MPI_Wtime();
    MPI_Gather(local_max_values.data(), local_row_count, MPI_INT, global_max_values.data(), rows_per_proc, MPI_INT, 0, MPI_COMM_WORLD);
    end_time = MPI_Wtime();

    // Процесс 0 вычисляет произведение всех максимумов
    if (myid == 0) {
        long long product = 1;
        /*cout << endl;
        cout << "Maximum:" << endl;*/
        for (int i = 0; i < N; ++i) {
            //cout << global_max_values[i] << " * ";
            product *= global_max_values[i];
        }
        cout << endl;
        cout << "Product of maximum values in each row: " << product << endl;
        cout << "Total processing time: " << setprecision(6) << end_time - start_time << " seconds" << endl;
        cout << "Data sending time MPI_Bcast: " << setprecision(6) << (send_time_end_1 - send_time_start_1) << " seconds" << endl;
        cout << "Data sending time MPI_Scatter: " << setprecision(6) << (send_time_end_2 - send_time_start_2) << " seconds" << endl;
        cout << "Data sending time MPI_Gather: " << setprecision(6) << (end_time - send_time_start_3) << " seconds" << endl;
        cout << "Processing time for process 0: " << parallel_time << " seconds" << endl;
    }

    MPI_Finalize();
    return 0;
}

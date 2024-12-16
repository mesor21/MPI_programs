#include "mpi.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

// Проверка, является ли слово "иностранным"
bool is_foreign_word(const string& word) {
    for (char c : word) {
        if (!isalpha(static_cast<unsigned char>(c)))
            return false;
    }
    return true;
}

int main(int argc, char** argv) {
    int myid, n_proc;
    double start_time, end_time, send_time_start_1, send_time_start_2, send_time_start_3, send_time_end_1, send_time_end_2, parallel_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    if (argc < 2) {
        if (myid == 0) {
            cerr << "Usage: mpiexec -n <num_processes> <program> <filename>" << endl;
        }
        MPI_Finalize();
        return 1;
    }

    string filename = argv[1];
    string text;

    // Процесс 0 читает файл в строку
    if (myid == 0) {
        ifstream input_file(filename);
        if (!input_file) {
            cerr << "Failed to open file: " << filename << endl;
            MPI_Finalize();
            return 1;
        }

        stringstream buffer;
        buffer << input_file.rdbuf();
        text = buffer.str();  // Читаем весь файл как одну строку
    }

    start_time = MPI_Wtime(); // Общее время начала работы программы
    int total_chars = text.size();
    send_time_start_1 = MPI_Wtime();
    MPI_Bcast(&total_chars, 1, MPI_INT, 0, MPI_COMM_WORLD);  // Передача общего числа символов всем процессам
    send_time_end_1 = MPI_Wtime();
    int chars_per_proc = (total_chars + n_proc - 1) / n_proc;  // Разделить текст на процессы
    vector<char> local_text(chars_per_proc);

    // Время отправки данных
    send_time_start_2 = MPI_Wtime();
    MPI_Scatter(text.data(), chars_per_proc, MPI_CHAR, local_text.data(), chars_per_proc, MPI_CHAR, 0, MPI_COMM_WORLD);
    send_time_end_2 = MPI_Wtime();

    parallel_time = MPI_Wtime();

    // Преобразуем локальный текст в строки (слова)
    string local_text_str(local_text.begin(), local_text.end());
    stringstream ss(local_text_str);
    string word;
    int local_count = 0;
    while (ss >> word) {
        if (is_foreign_word(word)) {
            ++local_count;
        }
    }

    // Время обработки данных
    parallel_time = MPI_Wtime() - parallel_time;

    // Сбор данных о количестве иностранных слов с каждого процесса
    vector<int> counts(n_proc, 0);
    send_time_start_3 = MPI_Wtime();
    MPI_Gather(&local_count, 1, MPI_INT, counts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    end_time = MPI_Wtime();

    vector<char> locales_test(total_chars + 4);
    MPI_Gather(local_text.data(), chars_per_proc, MPI_CHAR, locales_test.data(), chars_per_proc, MPI_CHAR, 0, MPI_COMM_WORLD);

    // Процесс 0 выводит результаты
    if (myid == 0) {
        string filename = "output.txt";
        ofstream file(filename);
        for (int i = 0; i < n_proc; ++i) {
            int start_idx = i * chars_per_proc;
            int end_idx = min((i + 1) * chars_per_proc, total_chars);
            string process_text = string(locales_test.begin() + start_idx, locales_test.begin() + end_idx);

            file << process_text << endl;
        }
        file.close();

        int global_count = 0;
        for (int i = 0; i < n_proc; ++i) {
            cout << "Process " << i << " found " << counts[i] << " foreign words." << endl;
            global_count += counts[i];
        }

        cout << "Total foreign word count: " << global_count << endl;
        cout << "Total processing time: " << setprecision(6) << end_time - start_time << " seconds" << endl;
        cout << "Data sending time MPI_Bcast: " << setprecision(6) << send_time_end_1 - send_time_start_1 << " seconds" << endl;
        cout << "Data sending time MPI_Scatter: " << setprecision(6) << send_time_end_2 - send_time_start_2 << " seconds" << endl;
        cout << "Data sending time MPI_Gather: " << setprecision(6) << end_time - send_time_start_3 << " seconds" << endl;
        cout << "Processing time for process 0: " << parallel_time << " seconds" << endl;
    }

    MPI_Finalize();
    return 0;
}

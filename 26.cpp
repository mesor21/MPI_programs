#include "mpi.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <chrono>
#include <sstream>

bool is_foreign_word(const std::string& word) {
    for (char c : word) {
        if (c < 0 || !std::isalpha(static_cast<unsigned char>(c)))
            return true;
    }
    return false;
}

int count_foreign_words_in_text(const std::string& text) {
    std::istringstream stream(text);
    std::string word;
    int foreign_word_count = 0;

    while (stream >> word) {
        if (is_foreign_word(word)) {
            ++foreign_word_count;
        }
    }
    return foreign_word_count;
}

int main(int argc, char** argv) {
    int myid, n_proc;
    double start_time, end_time, parallel_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    if (argc < 2) {
        if (myid == 0) {
            std::cerr << "Usage: mpirun -np <num_processes> <program> <filename>" << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream input_file;
    std::vector<std::string> lines;

    if (myid == 0) {
        input_file.open(filename);
        if (!input_file) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            MPI_Finalize();
            return 1;
        }

        std::string line;
        while (std::getline(input_file, line)) {
            lines.push_back(line);
        }
    }

    start_time = MPI_Wtime();

    int total_lines = lines.size();
    MPI_Bcast(&total_lines, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int lines_per_proc = total_lines / n_proc;
    int remainder = total_lines % n_proc;
    int start_line = myid * lines_per_proc + std::min(myid, remainder);
    int end_line = start_line + lines_per_proc + (myid < remainder ? 1 : 0);

    std::string local_text;
    if (myid == 0) {
        for (int i = 1; i < n_proc; ++i) {
            int i_start = i * lines_per_proc + std::min(i, remainder);
            int i_end = i_start + lines_per_proc + (i < remainder ? 1 : 0);

            for (int j = i_start; j < i_end; ++j) {
                int line_len = lines[j].size();
                MPI_Send(&line_len, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(lines[j].c_str(), line_len, MPI_CHAR, i, 0, MPI_COMM_WORLD);
            }
        }

        for (int i = start_line; i < end_line; ++i) {
            local_text += lines[i] + "\n";
        }
    }
    else {
        for (int i = start_line; i < end_line; ++i) {
            int line_len;
            MPI_Recv(&line_len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::vector<char> line_buffer(line_len + 1);
            MPI_Recv(line_buffer.data(), line_len, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            line_buffer[line_len] = '\0';
            local_text += std::string(line_buffer.data()) + "\n";
        }
    }

    parallel_time = MPI_Wtime();
    int local_count = count_foreign_words_in_text(local_text);
    parallel_time = MPI_Wtime() - parallel_time;

    int global_count = 0;
    MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (myid == 0) {
        end_time = MPI_Wtime();
        std::cout << "Total foreign words: " << global_count << std::endl;
        std::cout << "Total time: " << (end_time - start_time) << " seconds" << std::endl;
        std::cout << "Parallel computation time: " << parallel_time << " seconds" << std::endl;
    }

    MPI_Finalize();
    return 0;
}

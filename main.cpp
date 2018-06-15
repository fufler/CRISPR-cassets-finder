//
// Created by Евгений Кегелес on 02.04.2018.
//

#include "graph_assembly.h"
#include "visualise.h"
#include "graph_analysis.h"



//using namespace std;

char *getCmdOption(char **begin, char **end, const std::string &option) {
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char **begin, char **end, const std::string &option) {
    return std::find(begin, end, option) != end;
}


int main(int argc, char *argv[]) {

    // boost::program_options умеют всё это
    if (cmdOptionExists(argv, argv + argc, "-h")) {
        cout << "Parametres:" << endl;
        cout << "-h" << " " << "[ ]" << "-- help" << endl;
        cout << "-f [path_to_fasta_file] -- find CRISPR arrays from assembled sequence" << endl;
        cout << "-fq [path_to_fastaq_file] -- find CRISPR arrays from NGS data" << endl;
        cout << "-k [int] -- length of k-mer" << endl;
        cout << "-r [int] -- max repeat length in k-mers" << endl;
        cout << "-s [int] -- max spacer length in k-mers (with 2 k-mers from repeat)" << endl;
        cout << "-t [int] -- threshold difference between in_order and out_order" << endl;
        return 0;
    }

    bool fasta = true;
    char *filename;
    if (cmdOptionExists(argv, argv + argc, "-f")) {
        filename = getCmdOption(argv, argv + argc, "-f");
    } else if (cmdOptionExists(argv, argv + argc, "-fq")) {
        fasta = false;
        filename = getCmdOption(argv, argv + argc, "-fq");
    }
    if (!filename) {
        cout << "No input files!" << endl;
        return 0;
    }

    int k;
    char *k_char;
    k_char = getCmdOption(argv, argv + argc, "-k");
    if (k_char) {
        k = atoi(k_char);
    } else {
        cout << "No k!" << endl;
        return 0;
    }

    int max_path_length;
    char *r_char;
    r_char = getCmdOption(argv, argv + argc, "-r");
    if (r_char) {
        max_path_length = atoi(r_char);
    } else {
        cout << "No max repeat length!" << endl;
        return 0;
    }

    int max_spacer_length;
    char *s_char;
    s_char = getCmdOption(argv, argv + argc, "-s");
    if (s_char) {
        max_spacer_length = atoi(s_char);
    } else {
        cout << "No max spacer length!" << endl;
        return 0;
    }

    int vertexes_treshold;
    char *t_char;
    t_char = getCmdOption(argv, argv + argc, "-t");
    if (t_char) {
        vertexes_treshold = atoi(t_char);
    } else {
        cout << "No threshold !" << endl;
        return 0;
    }

    // --------------------------------
    // Вот примерно до этого места нужно всё выкидывать и заменять на boost

//    int k = 20;
//
//    int max_path_length = 50;
//
//    int max_spacer_length = 130;
//
//    int vertexes_treshold = 2;


    unordered_map<string, triple> vertexes;
    unordered_map<string, int> edges;

//    std::pair<int, int> size = read_from_fasta("/Users/evgenijkegeles/CLionProjects/CRISPR-cassets-finder/for_checking.txt", k, vertexes, edges);


    std::pair<int, int> size;
    if (fasta) {
        size = read_from_fasta(filename, k, vertexes, edges);
    } else {
        size = read_from_fastq(filename, k, vertexes, edges);

    }


    cout << size.first << " " << size.second << endl;


    vector<int> max_out_vertexes = find_max_out_vertexes(vertexes, vertexes_treshold);

    vector<int> max_in_vertexes = find_max_in_vertexes(vertexes, vertexes_treshold);



    vector<int> int_edges(size.second, 0);
    vector<int> weights(size.second, 0);
    vector<int> offset(size.first + 1, 0);

    vector<string> int_vertexes = make_int_graph(vertexes, edges, size, int_edges, weights, offset);

    vertexes.clear();
    edges.clear();

    vector<path> possible_ways = find_possible_pairs_parallel(int_edges, weights, offset, max_in_vertexes,
                                                              max_out_vertexes,
                                                              max_path_length);


    vector<path> possible_spacers = find_possible_spacers_parallel(int_edges, weights, offset, possible_ways,
                                                                   max_spacer_length);


    unordered_map<string, triple> final_vertexes;
    unordered_map<string, int> final_edges;
    unordered_map<string, unordered_set<string> > connections;

    pair<int, int> final_size = final_graph(possible_ways, possible_spacers, final_vertexes, final_edges, connections, int_vertexes);



    final_visualise(final_vertexes,final_edges,connections, k);

    graph_visualise(final_vertexes, final_edges);

    make_output(max_in_vertexes, max_out_vertexes, possible_ways, possible_spacers, int_vertexes, k);

    return 0;
}
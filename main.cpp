//
// Created by Евгений Кегелес on 02.04.2018.
//

#include "graph_assembly.h"
#include "visualise.h"
#include "graph_analysis.h"



//using namespace std;

int main() {

    int k = 15;

    int max_path_length = 50;

    int max_spacer_length = 70;

    int vertexes_treshold = 2;


    ifstream ifs("/Users/evgenijkegeles/CLionProjects/CRISPR-cassets-finder/sequence_1");
    string seq((std::istreambuf_iterator<char>(ifs)),
               (std::istreambuf_iterator<char>()));

    unordered_map<string, triple> vertexes;
    unordered_map<string, int> edges;

    std::pair<int, int> size = make_graph(k, seq, vertexes, edges);


    vector<int> max_out_vertexes = find_max_out_vertexes(vertexes, vertexes_treshold);

    vector<int> max_in_vertexes = find_max_in_vertexes(vertexes, vertexes_treshold);

    graph_visualise(vertexes, edges);

    vector<int> int_edges(size.second, 0);
    vector<int> weights(size.second, 0);
    vector<int> offset(size.first + 1, 0);

    vector<string> int_vertexes = make_int_graph(vertexes, edges, size, int_edges, weights, offset);


    vertexes.clear();
    edges.clear();

/*
    std::cout << max_out_vertexes.size() << endl;
    for (const auto &pair : max_out_vertexes) {
        std::cout << pair << " : ";
    }

    std::cout << endl << max_in_vertexes.size() << endl;

    for (const auto &pair : max_in_vertexes) {
        std::cout << pair << " : ";
    }

    cout << endl;
*/

    vector<path> possible_ways = find_possible_pairs(int_edges, weights, offset, max_in_vertexes, max_out_vertexes,
                                                     max_path_length);


    vector<path> possible_spacers = find_possible_spacers(int_edges, weights, offset, possible_ways, max_spacer_length);


    make_output(max_in_vertexes, max_out_vertexes, possible_ways, possible_spacers, int_vertexes, k);


/*
    for (const auto &pair : possible_pairs) {
        std::cout << pair.first << ' ' << pair.second << ' ' << pair.third << " ; ";
    }

    cout << endl;
    for (const auto &pair : weights) {
        std::cout << pair << " : ";
    }

    std::cout << endl;

    for (const auto &pair : offset) {
        std::cout << pair << " : ";
    }

    cout << endl;
*/
/*
    for (const auto &pair : edges) {
        std::cout << pair.second << " : " << pair.first << endl << endl;
    }

    for (const auto &pair : vertexes) {
        std::cout << pair.second.first << " : " << pair.second.second << ":" << pair.second.third << " :: "
                  << pair.first << endl << endl;
    }

    for (const auto &pair : int_edges) {
        std::cout << pair << " : ";
    }


    for (int i = 0; i < int_vertexes.size(); i++)
    {
        for (int j = 0; j < int_vertexes.size(); j++)
        {
            cout << int_graph[i][j] << " ";
        }
        cout << endl;

    };

*/


    return 0;
}
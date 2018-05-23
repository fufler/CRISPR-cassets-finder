//
// Created by Евгений Кегелес on 02.04.2018.
//

#ifndef CRISPR_CASSES_FINDER_GRAPH_ASSEMBLY_H
#define CRISPR_CASSES_FINDER_GRAPH_ASSEMBLY_H

#include <cassert>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <fstream>
#include <unordered_set>
#include <deque>
#include <algorithm>
#include <functional>
#include <set>
#include <pthread.h>
#include <thread>

using namespace std;

struct triple {
    int first, second, third;
};

struct path {
    int start, end, length, weight;
    vector<int> way;
    set<int> visited;
};

typedef std::function<bool(pair<string, triple>, pair<string, triple>)> Comparator;

Comparator compFunctor =
        [](pair<string, triple> elem1, pair<string, triple> elem2) {
            return elem1.second.third < elem2.second.third;
        };


std::pair<int, int> make_graph(const int k, const string &sequence, unordered_map<string, triple> &vertexes,
                               unordered_map<string, int> &edges, int &counter_vertexes, int &counter_edges) {

    string kmer12, kmer1, kmer2;
    for (int i = 0; i < sequence.length() - k - 1; i++) {
        kmer12 = sequence.substr(i, k + 1);
        transform(kmer12.begin(), kmer12.end(), kmer12.begin(), ::tolower);
        kmer1 = kmer12.substr(0, k);
        kmer2 = kmer12.substr(1, k);


        if (edges.count(kmer12) > 0) {
            ++edges[kmer12];
        } else {
            ++edges[kmer12];
            counter_edges += 1;
            if (vertexes.count(kmer1) == 0) {
                vertexes[kmer1] = {0, 1, counter_vertexes};
                counter_vertexes += 1;
            } else {
                vertexes.at(kmer1).second += 1;
            }
            if (vertexes.count(kmer2) == 0) {
                vertexes[kmer2] = {1, 0, counter_vertexes};
                counter_vertexes += 1;
            } else {
                vertexes.at(kmer2).first += 1;
            }
        }
    }
    return {counter_vertexes, counter_edges};
}

vector<string>
make_int_graph(const unordered_map<string, triple> &vertexes, const unordered_map<string, int> &edges,
               const std::pair<int, int> &size, vector<int> &int_edges, vector<int> &weights, vector<int> &offset) {

    vector<string> result_vertexes;
    set<pair<string, triple>, Comparator> ordered_vertexes(
            vertexes.begin(), vertexes.end(), compFunctor);
    offset[0] = 0;
    auto it = ordered_vertexes.begin();
    for (int i = 0; i < size.first; i++) {
        offset[i + 1] = offset[i] + it->second.second;

        for (int j = 0; j < it->second.second; j++) {
            if (edges.count(it->first + "a") > 0) {
                int_edges[offset[i] + j] = vertexes.at(it->first.substr(1, it->first.length() - 1) + "a").third;
                weights[offset[i] + j] = edges.at(it->first + "a");
                j++;
            }
            if (edges.count(it->first + "t") > 0) {
                int_edges[offset[i] + j] = vertexes.at(it->first.substr(1, it->first.length() - 1) + "t").third;
                weights[offset[i] + j] = edges.at(it->first + "t");
                j++;
            }
            if (edges.count(it->first + "g") > 0) {
                int_edges[offset[i] + j] = vertexes.at(it->first.substr(1, it->first.length() - 1) + "g").third;
                weights[offset[i] + j] = edges.at(it->first + "g");
                j++;
            }
            if (edges.count(it->first + "c") > 0) {
                int_edges[offset[i] + j] = vertexes.at(it->first.substr(1, it->first.length() - 1) + "c").third;
                weights[offset[i] + j] = edges.at(it->first + "c");
                j++;
            }
        }
        result_vertexes.push_back(it->first);
        it++;
    }
    return result_vertexes;
}

pair<int, int> read_from_fasta(const string &file_path, int k, unordered_map<string, triple> &vertexes,
                               unordered_map<string, int> &edges) {
    string s;
    ifstream in(file_path);
    string p;
    pair<int, int> to_return;
    int vertexes_count = 0;
    int edges_count = 0;
    while (getline(in, s)) {
        if (s[0] == '>' || s[0] == '\n') {
            if (p.length() > k) {
                pair<int, int> tmp = make_graph(k, p, vertexes, edges, vertexes_count, edges_count);
                to_return.first += tmp.first;
                to_return.second += tmp.second;
                p.clear();
            }
            continue;
        }
        p += s;
    }
    pair<int, int> tmp = make_graph(k, p, vertexes, edges, vertexes_count, edges_count);
    to_return.first += tmp.first;
    to_return.second += tmp.second;
    in.close();
    return to_return;
}

pair<int, int> read_from_fastq(const string &file_path, int k, unordered_map<string, triple> &vertexes,
                               unordered_map<string, int> &edges) {
    string s;
    ifstream in(file_path);
    string p;
    int vertexes_count = 0;
    int edges_count = 0;
    pair<int, int> tmp;
    while (getline(in, s)) {
        if (s[0] == '@') {
            getline(in, p);
            if (p[0] != '@' && p.length() > k) {
                tmp = make_graph(k, p, vertexes, edges, vertexes_count, edges_count);
            }
        }

    }
    in.close();

    cout << vertexes_count << "==" << vertexes.size() << endl;
    return tmp;
}

pair<int, int> final_graph(const vector<path> &possible_repeats, const vector<path> &possible_spacers,
                           unordered_map<string, triple> &final_vertexes,
                           unordered_map<string, int> &final_edges,
                           unordered_map<string, unordered_set<string> > &connections,
                           const vector<string> &int_vertexes) {
    int counter_vertexes = 0;
    int counter_edges = 0;
    for (auto &y : possible_repeats) {
        for (int i = 0; i < y.way.size() - 1; i++) {
            string kmer12, kmer1, kmer2;
            kmer1 = int_vertexes[y.way[i]];
            kmer2 = int_vertexes[y.way[i + 1]];
            kmer12 = int_vertexes[y.way[i]] + kmer2[kmer2.length() - 1];

            if (final_edges.count(kmer12) > 0) {
                ++final_edges[kmer12];
            } else {
                ++final_edges[kmer12];
                counter_edges += 1;
                if (final_vertexes.count(kmer1) == 0) {
                    final_vertexes[kmer1] = {0, 1, counter_vertexes};
                    connections[kmer1] = {kmer2};
                    counter_vertexes += 1;
                } else {
                    final_vertexes.at(kmer1).second += 1;
                    connections.at(kmer1).insert(kmer2);
                }
                if (final_vertexes.count(kmer2) == 0) {
                    final_vertexes[kmer2] = {1, 0, counter_vertexes};
                    connections[kmer2] = {};
                    counter_vertexes += 1;
                } else {
                    final_vertexes.at(kmer2).first += 1;
                }
            }
        }
    }
    for (auto &y : possible_spacers) {
        for (int i = 0; i < y.way.size() - 1; i++) {
            string kmer12, kmer1, kmer2;
            kmer1 = int_vertexes[y.way[i]];
            kmer2 = int_vertexes[y.way[i + 1]];
            kmer12 = int_vertexes[y.way[i]] + kmer2[kmer2.length() - 1];

            if (final_edges.count(kmer12) > 0) {
                ++final_edges[kmer12];
            } else {
                ++final_edges[kmer12];
                counter_edges += 1;
                if (final_vertexes.count(kmer1) == 0) {
                    final_vertexes[kmer1] = {0, 1, counter_vertexes};
                    connections[kmer1] = {kmer2};
                    counter_vertexes += 1;
                } else {
                    final_vertexes.at(kmer1).second += 1;
                    connections.at(kmer1).insert(kmer2);

                }
                if (final_vertexes.count(kmer2) == 0) {
                    final_vertexes[kmer2] = {1, 0, counter_vertexes};
                    connections[kmer2] = {};
                    counter_vertexes += 1;
                } else {
                    final_vertexes.at(kmer2).first += 1;
                }
            }
        }
    }

    return {counter_vertexes, counter_edges};
};



#endif //CRISPR_CASSES_FINDER_GRAPH_ASSEMBLY_H

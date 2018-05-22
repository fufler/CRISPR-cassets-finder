//
// Created by Евгений Кегелес on 16.04.2018.
//

#ifndef CRISPR_CASSES_FINDER_VISUALISE_H


#define CRISPR_CASSES_FINDER_VISUALISE_H

#include <cassert>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>

using namespace std;

void
graph_visualise_debug(const unordered_map<string, triple> &vertexes, const unordered_map<string, int> &edges) {
    int k = vertexes.begin()->first.length();
    int fd = open("/Users/evgenijkegeles/CLionProjects/CRISPR-cassets-finder/graph_visualise.txt",
                  O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int BUF_SIZE = 1000;
    char buf[BUF_SIZE];
    sprintf(buf, "digraph {\n node [shape=box]\n");
    write(fd, buf, strlen(buf));

    for (auto it = edges.begin(); it != edges.end(); ++it) {
        string vertex1 = it->first.substr(0, k);
        string vertex2 = it->first.substr(1, k);
        sprintf(buf, "%d%s_%d_%d -> %d%s_%d_%d [label=\"%s:%d\" penwidth=%d.0]\n", vertexes.at(vertex1).third,
                vertex1.c_str(), vertexes.at(vertex1).first,
                vertexes.at(vertex1).second, vertexes.at(vertex2).third, vertex2.c_str(), vertexes.at(vertex2).first,
                vertexes.at(vertex2).second,
                it->first.c_str(), it->second, it->second);
        write(fd, buf, strlen(buf));

    }
    sprintf(buf, "}\n");
    write(fd, buf, strlen(buf));
    close(fd);
}

void
graph_visualise(const unordered_map<string, triple> &vertexes, const unordered_map<string, int> &edges) {
    int k = vertexes.begin()->first.length();
    int fd = open("/Users/evgenijkegeles/CLionProjects/CRISPR-cassets-finder/graph_visualise.txt",
                  O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int BUF_SIZE = 1000;
    char buf[BUF_SIZE];
    sprintf(buf, "digraph {\n node [shape=box]\n");
    write(fd, buf, strlen(buf));

    for (auto it = edges.begin(); it != edges.end(); ++it) {
        string vertex1 = it->first.substr(0, k);
        string vertex2 = it->first.substr(1, k);
        sprintf(buf, "%s_%d_%d -> %s_%d_%d [label=\"%s:%d\" penwidth=%d.0]\n",
                vertex1.c_str(), vertexes.at(vertex1).first,
                vertexes.at(vertex1).second, vertex2.c_str(), vertexes.at(vertex2).first,
                vertexes.at(vertex2).second,
                it->first.c_str(), it->second, it->second);
        write(fd, buf, strlen(buf));

    }
    sprintf(buf, "}\n");
    write(fd, buf, strlen(buf));
    close(fd);
}

string get_string_path(const path &condensing_path, const vector<string> &int_vertexes) {

    string way = int_vertexes[condensing_path.way[0]];

    for (int i = 1 ; i < condensing_path.way.size(); i++) {
        way += int_vertexes[condensing_path.way[i]].back();
    }
    return way;
}

void
make_output(const vector<int> &max_in_vertexes, const vector<int> &max_out_vertexes,
            const vector<path> &possible_paths, const vector<path> &possible_spacers,
            const vector<string> &int_vertexes, const int k) {
    int fd = open("/Users/evgenijkegeles/CLionProjects/CRISPR-cassets-finder/OUTPUT.txt",
                  O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int BUF_SIZE = 1000;
    char buf[BUF_SIZE];

    sprintf(buf, "k-mer length (%d)\n", k);
    write(fd, buf, strlen(buf));

    sprintf(buf, "Left flanking pretendents (%d)\n", (int) max_in_vertexes.size());
    write(fd, buf, strlen(buf));
    for (auto &x : max_in_vertexes) {
        sprintf(buf, "%d ", (int) x);
        write(fd, buf, strlen(buf));
    }
    sprintf(buf, "\n\n");
    write(fd, buf, strlen(buf));

    sprintf(buf, "Right flanking pretendents (%d)\n", (int) max_out_vertexes.size());
    write(fd, buf, strlen(buf));
    for (auto &x : max_out_vertexes) {
        sprintf(buf, "%d ", (int) x);
        write(fd, buf, strlen(buf));
    }
    sprintf(buf, "\n\n");
    write(fd, buf, strlen(buf));

    sprintf(buf, "Possible repeats (%d)\n", (int) possible_paths.size());
    write(fd, buf, strlen(buf));
    for (auto &x : possible_paths) {
        sprintf(buf, "Start: %d, Finish: %d, Length: %d, Weight: %d, Path: %s \n", x.start, x.end, x.length, x.weight,
                get_string_path(x, int_vertexes).c_str());
        write(fd, buf, strlen(buf));
        for (auto &y : x.way) {
            sprintf(buf, "%d ", y);
            write(fd, buf, strlen(buf));
        }
        sprintf(buf, "\n\n");
        write(fd, buf, strlen(buf));
    }
    sprintf(buf, "\n\n");
    write(fd, buf, strlen(buf));

    sprintf(buf, "Possible spacers (%d)\n", (int) possible_spacers.size());
    write(fd, buf, strlen(buf));
    for (auto &x : possible_spacers) {
        sprintf(buf, "Start: %d, Finish: %d, Length: %d, Weight: %d, Path: %s \n", x.start, x.end, x.length, x.weight,
                get_string_path(x, int_vertexes).c_str());
        write(fd, buf, strlen(buf));
        for (auto &y : x.way) {
            sprintf(buf, "%d ", y);
            write(fd, buf, strlen(buf));
        }
        sprintf(buf, "\n\n");
        write(fd, buf, strlen(buf));
    }
    sprintf(buf, "\n\n");
    write(fd, buf, strlen(buf));


    close(fd);
}




#endif //CRISPR_CASSES_FINDER_VISUALISE_H

//
// Created by Евгений Кегелес on 30.04.2018.
//

#ifndef CRISPR_CASSES_FINDER_GRAPH_ANALYSIS_H
#define CRISPR_CASSES_FINDER_GRAPH_ANALYSIS_H


bool sortbysecdesc(const pair<int, int> &a,
                   const pair<int, int> &b) {
    return a.first > b.first;
}


vector<int> find_max_out_vertexes(const unordered_map<string, triple> &vertexes, int max_out_treshold) {
    vector<int> max_out_vertexes;

    for (auto it = vertexes.begin(); it != vertexes.end(); ++it) {
        if (it->second.second - it->second.first >= max_out_treshold) {
            max_out_vertexes.push_back(vertexes.at(it->first).third);
        }
    }


    return max_out_vertexes;
}

vector<int> find_max_in_vertexes(const unordered_map<string, triple> &vertexes, int max_in_treshold) {
    vector<int> max_in_vertexes;

    for (auto it = vertexes.begin(); it != vertexes.end(); ++it) {
        if (it->second.first - it->second.second >= max_in_treshold) {
            max_in_vertexes.push_back(vertexes.at(it->first).third);
        }
    }

    return max_in_vertexes;
}

vector<path> find_possible_path_bfs(const vector<int> &int_edges, const vector<int> &weights, const vector<int> &offset,
                                    int start, vector<int> finish, int max_path_length, vector<path> &paths) {

    deque<path> Q;
//    unordered_set<int> used;
    path current_path = {start, start, 0, 0, {start}};
    Q.push_back(current_path);
//    used.insert(start);

    while (!Q.empty()) {
        current_path = Q.front();
        Q.pop_front();

        for (int j = 0; j < finish.size(); j++) {
            if (current_path.end == finish[j]) {
                cout << int_edges[offset[current_path.end]] << " == " << finish[j] << endl;
                paths.push_back(current_path);
            }
        }
        vector<pair<int, int> > order_to_add = {};
        for (int i = 0; i < offset[current_path.end + 1] - offset[current_path.end]; i++) {
            order_to_add.clear();
            order_to_add.push_back({weights[offset[current_path.end] + i], i});
            sort(order_to_add.begin(), order_to_add.end(), sortbysecdesc);
        }
        for (auto &i : order_to_add)
            if (current_path.length < max_path_length) {
                if (find(current_path.way.begin(), current_path.way.end(),
                         int_edges[offset[current_path.end] + i.second]) == current_path.way.end()) {

                    vector<int> to_add = current_path.way;
                    to_add.push_back(int_edges[offset[current_path.end] + i.second]);

                    Q.push_back({current_path.start, int_edges[offset[current_path.end] + i.second],
                                 current_path.length + 1,
                                 current_path.weight + weights[offset[current_path.end] + i.second], to_add});
//                    used.insert(int_edges[offset[current_path.end] + i.second]);


                }
            } else {
                return paths;
            }

    }

    return paths;
}


vector<path> find_possible_pairs(const vector<int> &int_edges, const vector<int> &weights, const vector<int> &offset,
                                 const vector<int> &out_vertexes, const vector<int> &in_vertexes,
                                 int max_path_length) {
    vector<path> possible_pairs;
    for (auto &start: out_vertexes) {
        find_possible_path_bfs(int_edges, weights, offset, start, in_vertexes, max_path_length, possible_pairs);
    }
    return possible_pairs;
}


vector<path> find_possible_spacers(const vector<int> &int_edges, const vector<int> &weights, const vector<int> &offset,
                                   const vector<path> &possible_ways, int max_path_length) {

    unordered_set<int> starts;
    unordered_set<int> finishes;
    for (auto &x : possible_ways) {
        starts.insert(x.end);
        finishes.insert(x.start);
    }
    vector<path> possible_pairs;
    vector<int> aims;
    for (auto &y : finishes){
        aims.push_back(y);
    }
    for (auto &start: starts) {
        find_possible_path_bfs(int_edges, weights, offset, start, aims, max_path_length, possible_pairs);
    }
    return possible_pairs;

}

#endif //CRISPR_CASSES_FINDER_GRAPH_ANALYSIS_H

//
// Created by Евгений Кегелес on 30.04.2018.
//

#ifndef CRISPR_CASSES_FINDER_GRAPH_ANALYSIS_H
#define CRISPR_CASSES_FINDER_GRAPH_ANALYSIS_H

#include <pthread.h>
#include <thread>
#include <unistd.h>
#include <fcntl.h>

bool sortbyfirstdesc(const pair<int, int> &a,
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
                paths.push_back(current_path);
            }
        }
        vector<pair<int, int> > order_to_add = {};
        for (int i = 0; i < offset[current_path.end + 1] - offset[current_path.end]; i++) {
            order_to_add.push_back({weights[offset[current_path.end] + i], i});
        }
        sort(order_to_add.begin(), order_to_add.end(), sortbyfirstdesc);
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

typedef struct {
    const vector<int> &int_edges;
    const vector<int> &weights;
    const vector<int> &offset;
    const unordered_set<int> &start;
    const unordered_set<int> &finishes;
    int max_path_length;
    vector<path> &paths;
    pthread_mutex_t *mtx;
} thread_arg_repeats;

void *thread_body_repeats(void *arg) {
    thread_arg_repeats *argument = (thread_arg_repeats *) arg;

    const vector<int> &int_edges = argument->int_edges;
    const vector<int> &weights = argument->weights;
    const vector<int> &offset = argument->offset;
    const unordered_set<int> &starts = argument->start;
    const unordered_set<int> &finish = argument->finishes;
    int max_path_length = argument->max_path_length;
    vector<path> &paths = argument->paths;
    pthread_mutex_t *mutex = argument->mtx;

    for (auto &start : starts) {

        deque<path> Q;
        path current_path = {start, start, 0, 0, {start}, {start}};
        Q.push_back(current_path);
        bool to_break = false;
        while (!Q.empty()) {
            current_path = Q.front();
            Q.pop_front();

            if(finish.count(current_path.end) > 0){
                pthread_mutex_lock(mutex);
                paths.push_back(current_path);
                pthread_mutex_unlock(mutex);
            }
//
//            for (int j = 0; j < finish.size(); j++) {
//                if (current_path.end == finish[j]) {
//                    pthread_mutex_lock(mutex);
//                    paths.push_back(current_path);
//                    pthread_mutex_unlock(mutex);
//                }
//            }
            vector<pair<int, int> > order_to_add = {};
            for (int i = 0; i < offset[current_path.end + 1] - offset[current_path.end]; i++) {
                order_to_add.push_back({weights[offset[current_path.end] + i], i});
            }
            sort(order_to_add.begin(), order_to_add.end(), sortbyfirstdesc);

            for (auto &i : order_to_add) {
                if (current_path.length < max_path_length) {
                    if (current_path.visited.count(int_edges[offset[current_path.end] + i.second]) == 0) {

                        vector<int> to_add = current_path.way;
                        to_add.push_back(int_edges[offset[current_path.end] + i.second]);
                        set<int> adding = current_path.visited;
                        adding.insert(int_edges[offset[current_path.end] + i.second]);

                        Q.push_back({current_path.start, int_edges[offset[current_path.end] + i.second],
                                     current_path.length + 1,
                                     current_path.weight + weights[offset[current_path.end] + i.second], to_add, adding});
                    }
                } else {
                    to_break = true;
                    break;
                }
            }
            if (to_break) {
                break;
            }
        }
    }
    return nullptr;
}

vector<path>
find_possible_pairs_parallel(const vector<int> &int_edges, const vector<int> &weights, const vector<int> &offset,
                             const vector<int> &out_vertexes, const vector<int> &in_vertexes,
                             int max_path_length) {

    unsigned concurentThreadsSupported = thread::hardware_concurrency();
//    unsigned concurentThreadsSupported = 1;
    vector<pair<pthread_t, unordered_set<int> > > threads;
    int count = 0;
    vector<path> possible_pairs;
    unordered_set<int> finishes(in_vertexes.begin(), in_vertexes.end());
    unordered_set<int> starts(out_vertexes.begin(), out_vertexes.end());
//    for (auto &start: out_vertexes)
//    {
//        if (starts.count(start) == 0){
//            starts.insert(start);
//        }
//    }
//    for (auto &finish: in_vertexes)
//    {
//        if (finishes.count(finish) == 0){
//            finishes.insert(finish);
//        }
//    }

    for (auto &start: starts) {
        count++;
        if (threads.size() < concurentThreadsSupported) {
            pthread_t t;
            pair<pthread_t, unordered_set<int> > to_add = {t, {start}};
            threads.push_back(to_add);
        } else {
            threads[count % concurentThreadsSupported].second.insert(start);
        }
    }
    pthread_mutex_t mtx;
    pthread_mutex_init(&mtx, nullptr);
    vector<thread_arg_repeats> args;
    args.reserve(threads.size());

    for (auto &thread : threads) {

        thread_arg_repeats arg1 = {int_edges, weights, offset, thread.second, finishes, max_path_length,
                                   possible_pairs, &mtx};
        args.push_back(arg1);

        pthread_create(&(thread.first), nullptr, thread_body_repeats, &args[args.size() - 1]);
    }
    for (auto &thread : threads) {
        pthread_join(thread.first, nullptr);
    }
    return possible_pairs;
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
    for (auto &y : finishes) {
        aims.push_back(y);
    }
    for (auto &start: starts) {
        find_possible_path_bfs(int_edges, weights, offset, start, aims, max_path_length, possible_pairs);
    }
    return possible_pairs;

}

vector<path> find_possible_spacers_parallel(const vector<int> &int_edges, const vector<int> &weights, const vector<int> &offset,
                                   const vector<path> &possible_ways, int max_path_length) {

    unordered_set<int> starts;
    unordered_set<int> finishes;
    for (auto &x : possible_ways) {
        starts.insert(x.end);
        finishes.insert(x.start);
    }
    vector<path> possible_pairs;

    unsigned concurentThreadsSupported = thread::hardware_concurrency();
    vector<pair<pthread_t, unordered_set<int> > > threads;
    int count = 0;


    for (auto &start: starts) {
        count++;
        if (threads.size() < concurentThreadsSupported) {
            pthread_t t;
            pair<pthread_t, unordered_set<int> > to_add = {t, {start}};
            threads.push_back(to_add);
        } else {
            threads[count % concurentThreadsSupported].second.insert(start);
        }
    }
    pthread_mutex_t mtx;
    pthread_mutex_init(&mtx, nullptr);
    vector<thread_arg_repeats> args;
    args.reserve(threads.size());

    for (auto &thread : threads) {

        thread_arg_repeats arg1 = {int_edges, weights, offset, thread.second, finishes, max_path_length,
                                   possible_pairs, &mtx};
        args.push_back(arg1);

        pthread_create(&(thread.first), nullptr, thread_body_repeats, &args[args.size() - 1]);
    }
    for (auto &thread : threads) {
        pthread_join(thread.first, nullptr);
    }
    return possible_pairs;
}

#endif //CRISPR_CASSES_FINDER_GRAPH_ANALYSIS_H

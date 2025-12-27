#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/property_map/property_map.hpp>

#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>

using StringVect = std::vector<std::string>;
using Input = std::vector<StringVect>;

// Bundled vertex properties
struct VProps
{
    int32_t numPaths = -1;  // memoized: paths from this vertex to 'dest'; -1 = unknown
};

using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VProps>;
using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

namespace {

std::string dot_escape(std::string s)
{
    // Escape quotes and backslashes for DOT quoted strings
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if (c == '\\' || c == '"')
            out.push_back('\\');
        out.push_back(c);
    }
    return out;
}

std::string dot_id(const std::string& name)
{
    // Use a quoted DOT ID
    return "\"" + dot_escape(name) + "\"";
}

[[maybe_unused]]
void dump_graph_to_dot(const Graph& g, const std::map<Vertex, std::string>& vertex2node, const std::string& filename)
{
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Failed to open " << filename << " for writing\n";
        return;
    }

    const std::set<std::string> highlight = {"you", "out", "svr", "dac", "fft"};

    auto vertex_name = [&](Vertex v) -> std::string
    {
        auto it = vertex2node.find(v);
        if (it != vertex2node.end())
            return it->second;

        // Fallback to stable name if missing
        std::ostringstream oss;
        oss << "v" << static_cast<std::size_t>(v);
        return oss.str();
    };

    out << "digraph G {\n";
    out << "  rankdir=LR;\n";
    out << "  node [shape=circle];\n\n";

    // Write all vertices with labels (and optional highlighting)
    auto vr = boost::vertices(g);
    for (auto it = vr.first; it != vr.second; ++it) {
        Vertex v = *it;
        std::string name = vertex_name(v);

        out << "  " << dot_id(name) << " [label=" << dot_id(name);

        if (highlight.count(name)) {
            out << ", style=filled, fillcolor=\"gold\", penwidth=2";  // highlighted
            // (No explicit color set, you can add: fillcolor="gold" etc. if you want)
        }

        out << "];\n";
    }

    out << "\n";

    // Write all edges using names
    auto er = boost::edges(g);
    for (auto it = er.first; it != er.second; ++it) {
        Vertex src = boost::source(*it, g);
        Vertex tgt = boost::target(*it, g);

        std::string sname = vertex_name(src);
        std::string tname = vertex_name(tgt);

        out << "  " << dot_id(sname) << " -> " << dot_id(tname) << ";\n";
    }

    out << "}\n";

    std::cout << "Graph saved to " << filename << "\n";
    std::cout << "To visualize, run: dot -Tpng " << filename << " -o graph.png\n";
    std::cout << "Or for SVG: dot -Tsvg " << filename << " -o graph.svg\n\n";
}

}  // namespace

enum class State : uint8_t {
    Unknown = 0,
    InProgress = 1,
    Done = 2
};

// state: 0=unvisited, 1=visiting, 2=done
int dfs_count_paths_to_dest(Graph& g, Vertex const u, Vertex const dest, std::vector<State>& state)
{
    if (u == dest) {
        g[u].numPaths = 1;  // exactly one path: dest -> dest (empty path)
        return 1;
    }

    if (state[u] == State::InProgress) {
        throw std::runtime_error("Cycle detected on a reachable path: number of paths may be unbounded");
    }
    if (state[u] == State::Done) {
        return g[u].numPaths;  // READ memo
    }
    if (g[u].numPaths != -1) {
        return g[u].numPaths;  // extra safety
    }

    state[u] = State::InProgress;

    int32_t total{0};  // use wider temp to reduce overflow risk
    auto const& [eb, ee] = boost::out_edges(u, g);
    for (auto it{eb}; it != ee; ++it) {
        const Vertex v = boost::target(*it, g);
        total += dfs_count_paths_to_dest(g, v, dest, state);
    }

    g[u].numPaths = total;  // WRITE memo
    state[u] = State::Done;
    return total;
}


int32_t count_paths(Graph& g, Vertex start, Vertex dest)
{
    // reset bundled memo field for a fresh query
    for (std::size_t i = 0; i < boost::num_vertices(g); ++i) {
        g[i].numPaths = -1;
    }

    std::vector<State> state(boost::num_vertices(g), State::Unknown);
    return dfs_count_paths_to_dest(g, start, dest, state);
}


Graph make_graph(
        Input const& input,
        std::unordered_map<std::string, Vertex>& node2vertex,
        std::map<Vertex, std::string>& vertex2node)
{
    Graph g;

    for (auto const& v : input) {
        for (auto const& s : v) {
            if (node2vertex.contains(s))
                continue;
            Vertex vx{boost::add_vertex(g)};
            node2vertex[s] = vx;
            vertex2node[vx] = s;
        }

        Vertex const& from{node2vertex.at(v.at(0))};
        for (auto const& s : v | std::views::drop(1)) {
            boost::add_edge(from, node2vertex.at(s), g);
        }
    }

    return g;
}


void process(Input const& input)
{
    std::unordered_map<std::string, Vertex> node2vertex;
    std::map<Vertex, std::string> vertex2node;

    Graph g{make_graph(input, node2vertex, vertex2node)};

    //  dump_graph_to_dot(g, vertex2node, "d11.dot");

    fmt::print("1: {}\n", count_paths(g, node2vertex.at("you"), node2vertex.at("out")));

    auto const leg1{count_paths(g, node2vertex.at("svr"), node2vertex.at("fft"))};
    auto const leg2{count_paths(g, node2vertex.at("fft"), node2vertex.at("dac"))};
    auto const leg3{count_paths(g, node2vertex.at("dac"), node2vertex.at("out"))};

    fmt::print("2: {}\n", (1ull * leg1 * leg2 * leg3));
}


int main()
{
    Input input;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }

        StringVect v;
        boost::algorithm::split(v, line, boost::is_any_of(": "), boost::algorithm::token_compress_on);
        input.push_back(std::move(v));
    }

    process(input);

    return 0;
}

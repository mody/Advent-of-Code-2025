#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/property_map/property_map.hpp>

#include <cassert>
#include <deque>
#include <iostream>
#include <fstream>
#include <ranges>
#include <string>

using StringVect = std::vector<std::string>;
using Input = std::vector<StringVect>;

using EdgeWeight = boost::property<boost::edge_weight_t, unsigned>;
using ColorMap = boost::property<boost::vertex_color_t, boost::default_color_type>;
using VertexDistance = boost::property<boost::vertex_distance_t, unsigned>;

using Graph = boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::directedS,
        ColorMap,
        EdgeWeight>;

using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

using Path = std::deque<Vertex>;


class PathEnumerator : public boost::default_dfs_visitor
{
public:
    using ColorMapType = boost::iterator_property_map<
            std::vector<boost::default_color_type>::iterator,
            boost::property_map<Graph, boost::vertex_index_t>::const_type>;

    std::vector<Path>& paths;
    ColorMapType& color_map;
    Path current_path;
    Vertex const target;


    PathEnumerator(std::vector<Path>& p, ColorMapType& c, Vertex t)
        : paths{p}
        , color_map{c}
        , target{t}
    {
    }

    void discover_vertex(Vertex v, const Graph& g)
    {
        current_path.push_back(v);
        if (v == target) {
            paths.push_back(current_path);
        }
    }

    void finish_vertex(Vertex v, const Graph& g)
    {
        boost::put(color_map, v, boost::white_color);

        current_path.pop_back();
    }
};


// Function to dump graph to DOT format
void dump_graph_to_dot(const Graph& g, const std::string& filename)
{
    std::ofstream out(filename);

    out << "digraph G {\n";
    out << "  rankdir=LR;\n";  // Left to right layout
    out << "  node [shape=circle];\n\n";

    // Write all vertices
    for (size_t i = 0; i < boost::num_vertices(g); ++i) {
        out << "  " << i << ";\n";
    }

    out << "\n";

    // Write all edges
    auto edge_range = boost::edges(g);
    for (auto it = edge_range.first; it != edge_range.second; ++it) {
        Vertex src = boost::source(*it, g);
        Vertex tgt = boost::target(*it, g);
        out << "  " << src << " -> " << tgt << ";\n";
    }

    out << "}\n";
    out.close();

    std::cout << "Graph saved to " << filename << "\n";
    std::cout << "To visualize, run: dot -Tpng " << filename << " -o graph.png\n";
    std::cout << "Or for SVG: dot -Tsvg " << filename << " -o graph.svg\n\n";
}


void part1(Input input)
{
    Graph g;

    std::unordered_map<std::string, Vertex> node2vertex;
    std::map<Vertex, std::string> vertex2node;

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
            boost::add_edge(from, node2vertex.at(s), EdgeWeight{1}, g);
        }
    }

    //  dump_graph_to_dot(g, "d11.dot");

    std::vector<Path> paths;
    std::vector<boost::default_color_type> color_map(boost::num_vertices(g));
    auto color_pmap = boost::make_iterator_property_map(
            color_map.begin(),
            boost::get(boost::vertex_index, g));

    boost::depth_first_visit(
            g,
            node2vertex.at("you"),
            PathEnumerator{paths, color_pmap, node2vertex.at("out")},
            color_pmap);


    //  for (size_t i = 0; i < paths.size(); ++i) {
    //      std::cout << "Path " << (i + 1) << ": ";
    //      for (size_t j = 0; j < paths[i].size(); ++j) {
    //          std::cout << vertex2node.at(paths[i][j]);
    //          if (j < paths[i].size() - 1)
    //              std::cout << " -> ";
    //      }
    //      std::cout << "\n";
    //  }


    fmt::print("1: {}\n", paths.size());
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

    part1(input);

    return 0;
}

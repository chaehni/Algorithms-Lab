// STL includes
#include <iostream>
#include <cstdlib>
// BGL includes
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>

// BGL Graph definitions
// =====================
// Graph Type with nested interior edge properties for Cost Flow Algorithms
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> Traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
                              boost::property<boost::edge_capacity_t, long,
                                              boost::property<boost::edge_residual_capacity_t, long,
                                                              boost::property<boost::edge_reverse_t, Traits::edge_descriptor,
                                                                              boost::property<boost::edge_weight_t, long>>>>>
    Graph;
// Interior Property Maps
typedef boost::property_map<Graph, boost::edge_capacity_t>::type EdgeCapacityMap;
typedef boost::property_map<Graph, boost::edge_weight_t>::type EdgeWeightMap;
typedef boost::property_map<Graph, boost::edge_residual_capacity_t>::type ResidualCapacityMap;
typedef boost::property_map<Graph, boost::edge_reverse_t>::type ReverseEdgeMap;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
typedef boost::graph_traits<Graph>::out_edge_iterator OutEdgeIt;

// Custom Edge Adder Class, that holds the references
// to the graph, capacity map, weight map and reverse edge map
// ===============================================================
class EdgeAdder
{
    Graph &G;
    EdgeCapacityMap &capacitymap;
    EdgeWeightMap &weightmap;
    ReverseEdgeMap &revedgemap;

  public:
    EdgeAdder(Graph &G, EdgeCapacityMap &capacitymap, EdgeWeightMap &weightmap, ReverseEdgeMap &revedgemap)
        : G(G), capacitymap(capacitymap), weightmap(weightmap), revedgemap(revedgemap) {}

    void addEdge(int u, int v, long c, long w)
    {
        Edge e, rev_e;
        boost::tie(e, boost::tuples::ignore) = boost::add_edge(u, v, G);
        boost::tie(rev_e, boost::tuples::ignore) = boost::add_edge(v, u, G);
        capacitymap[e] = c;
        weightmap[e] = w;
        capacitymap[rev_e] = 0;
        weightmap[rev_e] = -w;
        revedgemap[e] = rev_e;
        revedgemap[rev_e] = e;
    }
};

using namespace std;

void run()
{
    int n, m, s;
    cin >> n >> m >> s;

    // Create Graph and Maps
    Graph G(n + m + s + 2);
    Vertex source = n + m + s;
    Vertex sink = n + m + s + 1;
    EdgeCapacityMap capacitymap = boost::get(boost::edge_capacity, G);
    EdgeWeightMap weightmap = boost::get(boost::edge_weight, G);
    ReverseEdgeMap revedgemap = boost::get(boost::edge_reverse, G);
    ResidualCapacityMap rescapacitymap = boost::get(boost::edge_residual_capacity, G);
    EdgeAdder eag(G, capacitymap, weightmap, revedgemap);

    // add edges between source and bidders
    for (int i = 0; i < n; i++)
        eag.addEdge(source, i, 1, 0); // every buyer is interested in at most one piece of land

    // add edges between states and sink
    for (int i = 0; i < s; i++)
    {
        int max;
        cin >> max;
        eag.addEdge(n + m + i, sink, max, 0);
    }

    // add edges between sites and states
    for (int i = 0; i < m; i++)
    {
        int state;
        cin >> state;
        eag.addEdge(n + i, n + m + state - 1, 1, 0);
    }

    // add edges between bidders and sites
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            int bid;
            cin >> bid;
            //eag.addEdge(i, n + j, 1, -bid);  negative weights hit timelimit for largest case
            eag.addEdge(i, n + j, 1, 100 - bid); // 100 is the maximum a bidder pays -> invert numbers
        }
    }

    // Min Cost Max Flow with cycle_canceling (supports negative weight) but is too slow (80pts)
    int flow = boost::push_relabel_max_flow(G, source, sink);
    //boost::cycle_canceling(G); too slow, 80 points only
    boost::successive_shortest_path_nonnegative_weights(G, source, sink);
    int cost = boost::find_flow_cost(G);
    cout << flow << " " << flow * 100 - cost << "\n";
}

int main()
{
    ios_base::sync_with_stdio(false);
    int n;
    cin >> n;
    while (n--)
    {
        run();
    }
}

#include <iostream>
#include <vector>
#include <set>

// BGL includes
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/edmonds_karp_max_flow.hpp>

// BGL Graph definitions
// =====================
// Graph Type with nested interior edge properties for Flow Algorithms
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> Traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
                              boost::property<boost::edge_capacity_t, long,
                                              boost::property<boost::edge_residual_capacity_t, long,
                                                              boost::property<boost::edge_reverse_t, Traits::edge_descriptor>>>>
    Graph;
// Interior Property Maps
typedef boost::property_map<Graph, boost::edge_capacity_t>::type EdgeCapacityMap;
typedef boost::property_map<Graph, boost::edge_residual_capacity_t>::type ResidualCapacityMap;
typedef boost::property_map<Graph, boost::edge_reverse_t>::type ReverseEdgeMap;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
typedef boost::graph_traits<Graph>::edge_iterator EdgeIt;

// Custom Edge Adder Class, that holds the references
// to the graph, capacity map and reverse edge map
// ===================================================
class EdgeAdder
{
    Graph &G;
    EdgeCapacityMap &capacitymap;
    ReverseEdgeMap &revedgemap;

  public:
    // to initialize the Object
    EdgeAdder(Graph &G, EdgeCapacityMap &capacitymap, ReverseEdgeMap &revedgemap) : G(G), capacitymap(capacitymap), revedgemap(revedgemap) {}

    // to use the Function (add an edge)
    void addEdge(int from, int to, long capacity)
    {
        Edge e, rev_e;
        bool success;
        boost::tie(e, success) = boost::add_edge(from, to, G);
        boost::tie(rev_e, success) = boost::add_edge(to, from, G);
        capacitymap[e] = capacity;
        capacitymap[rev_e] = 0; // reverse edge has no capacity!
        revedgemap[e] = rev_e;
        revedgemap[rev_e] = e;
    }
};

using namespace std;

void run()
{
    int z, j;
    cin >> z >> j;

    Graph G(z + j + 2);
    Vertex src = z + j;
    Vertex sink = z + j + 1;
    EdgeCapacityMap capacitymap = boost::get(boost::edge_capacity, G);
    ReverseEdgeMap revedgemap = boost::get(boost::edge_reverse, G);
    ResidualCapacityMap rescapacitymap = boost::get(boost::edge_residual_capacity, G);
    EdgeAdder eag(G, capacitymap, revedgemap);

    // add edges between source and zones with zone cost as weight
    vector<int> costs(z);
    for (int i = 0; i < z; i++)
    {
        int c;
        cin >> c;
        costs[i] = c;
        eag.addEdge(src, i, c);
    }

    // add edges between jobs and sink with job reward as weight
    int reward = 0;
    for (int i = 0; i < j; i++)
    {
        int r;
        cin >> r;
        reward += r;
        eag.addEdge(z + i, sink, r);
    }

    // add edges between zones and jobs with zone cost as weight
    for (int i = 0; i < j; i++)
    {
        int zones;
        cin >> zones;
        for (int k = 0; k < zones; k++)
        {
            int zone;
            cin >> zone;
            eag.addEdge(zone, z + i, costs[zone]);
        }
    }

    long flow = boost::push_relabel_max_flow(G, src, sink);

    cout << reward - flow << "\n";
}

int main()
{
    ios_base::sync_with_stdio(false);
    int n;
    cin >> n;
    while (n--)
        run();
}
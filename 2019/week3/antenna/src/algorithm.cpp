#include <iostream>
#include <vector>

#include <CGAL/Exact_predicates_exact_constructions_kernel_with_sqrt.h>
#include <CGAL/Min_circle_2.h>
#include <CGAL/Min_circle_2_traits_2.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel_with_sqrt K;
typedef CGAL::Min_circle_2_traits_2<K> Traits;
typedef CGAL::Min_circle_2<Traits> Min_circle;
typedef K::Point_2 P;

using namespace std;

double ceil_to_double(const K::FT &x)
{
    double a = std::floor(CGAL::to_double(x));
    while (a < x)
        a += 1;
    while (a - 1 >= x)
        a -= 1;
    return a;
}

void run(int n)
{
    vector<P> points(n);
    for (int i = 0; i < n; i++)
    {
        long x, y;
        cin >> x >> y;
        points[i] = P(x, y);
    }

    Min_circle min(points.begin(), points.end(), true);

    cout << ceil_to_double(CGAL::sqrt(min.circle().squared_radius())) << endl;
}

int main(int argc, char const *argv[])
{
    ios_base::sync_with_stdio(false);
    cout << fixed << setprecision(0);
    while (true)
    {
        int n;
        cin >> n;
        if (n == 0)
            break;
        run(n);
    }
}

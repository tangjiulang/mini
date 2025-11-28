#include <unordered_map>

#include "polyCorrect_SL.hxx"
using namespace std;

// horizontal segment on scanline y: [x0, x1] with x0 < x1
struct Segment {
    VECTOR2D a; // left
    VECTOR2D b; // right
};

static const double EPS = 1e-9;
// snap multiplier for hashing endpoints (avoid precision issues)
static const double SNAP = 1e6;

// compute signed area (positive = CCW, negative = CW)
double signed_area(const vector<VECTOR2D>& poly) {
    double A = 0;
    int n = (int)poly.size();
    if (n < 3) return 0.0;
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        A += poly[i].x * poly[j].y - poly[j].x * poly[i].y;
    }
    return A * 0.5;
}

// robust intersection test: does horizontal scanline y intersect edge p1-p2?
// returns true and outX if intersection within edge (excluding horizontal edges).
bool edge_intersect_scanline(const VECTOR2D& p1, const VECTOR2D& p2, double y, double& outX) {
    double y1 = p1.y, y2 = p2.y;
    double x1 = p1.x, x2 = p2.x;

    // If the scanline is outside edge y-range -> no intersect.
    if (y < min(y1, y2) - EPS || y > max(y1, y2) + EPS) return false;

    // ignore horizontal edges to avoid double counting
    if (fabs(y2 - y1) < EPS) return false;

    // compute intersection parameter t in [0,1]
    double t = (y - y1) / (y2 - y1);
    // guard numeric issues
    if (t < -EPS || t > 1.0 + EPS) return false;

    outX = x1 + t * (x2 - x1);
    return true;
}

// produce geometric scanline segments by stepping y from miny to maxy with step dy
vector<Segment> scanline_segments_geometric(const vector<VECTOR2D>& poly, double dy) {
    double miny = numeric_limits<double>::infinity();
    double maxy = -numeric_limits<double>::infinity();
    for (auto& p : poly) {
        miny = min(miny, p.y);
        maxy = max(maxy, p.y);
    }
    if (miny > maxy) return {};

    vector<Segment> segs;
    // choose starting y slightly above miny to handle vertex cases robustly
    double start_y = miny + dy * 0.5;
    // iterate y in (miny, maxy) with step dy
    for (double y = start_y; y <= maxy - dy * 0.5; y += dy) {
        vector<double> xs;
        int n = (int)poly.size();
        for (int i = 0; i < n; ++i) {
            VECTOR2D p1 = poly[i];
            VECTOR2D p2 = poly[(i + 1) % n];
            double ix;
            if (edge_intersect_scanline(p1, p2, y, ix)) {
                xs.push_back(ix);
            }
        }
        if (xs.size() < 2) continue;
        sort(xs.begin(), xs.end());
        // pair them as [x0,x1], [x2,x3], ...
        // if odd count due to numerical issues, ignore last
        for (size_t k = 0; k + 1 < xs.size(); k += 2) {
            double x0 = xs[k];
            double x1 = xs[k + 1];
            if (x1 - x0 > EPS) {
                segs.push_back(Segment{ VECTOR2D{x0, y}, VECTOR2D{x1, y} });
            }
        }
    }
    return segs;
}

// helper to key a point to integer pair (for robust map lookup)
pair<long long, long long> key_of(const VECTOR2D& p) {
    long long xi = llround(p.x * SNAP);
    long long yi = llround(p.y * SNAP);
    return { xi, yi };
}

// connect horizontal segments into closed polygons
vector<vector<VECTOR2D>> connect_segments_to_polygons(vector<Segment>& segs) {
    vector<vector<VECTOR2D>> polygons;
    if (segs.empty()) return polygons;

    // Map endpoint -> indices of segments touching that endpoint
    unordered_map<long long, vector<int>> mapIdx;
    auto pack = [&](const pair<long long, long long>& k)->long long {
        // pack into 128-bit like using XOR+shift but here simple combine (safe with SNAP)
        // Use 64-bit key = (xi << 32) ^ yi but need cast
        long long xi = k.first;
        long long yi = k.second;
        // mix to single 64-bit: (xi << 32) ^ (yi & 0xffffffff)
        long long key = (xi << 32) ^ (yi & 0xffffffffLL);
        return key;
        };

    for (int i = 0; i < (int)segs.size(); ++i) {
        auto k1 = key_of(segs[i].a);
        auto k2 = key_of(segs[i].b);
        mapIdx[pack(k1)].push_back(i);
        mapIdx[pack(k2)].push_back(i);
    }

    vector<char> used(segs.size(), 0);

    for (int i = 0; i < (int)segs.size(); ++i) {
        if (used[i]) continue;

        // Try to build a polygon starting from seg i in both orientations (a->b or b->a)
        // We'll attempt forward chaining; if fails, mark used minimal parts and continue.
        for (int attempt = 0; attempt < 2; ++attempt) {
            vector<VECTOR2D> poly;
            int cur_idx = i;
            VECTOR2D cur_end = (attempt == 0) ? segs[i].b : segs[i].a;
            VECTOR2D cur_start = (attempt == 0) ? segs[i].a : segs[i].b;
            // seed
            poly.push_back(cur_start);
            poly.push_back(cur_end);
            used[cur_idx] = 1;

            bool closed = false;
            const int MAX_STEPS = (int)segs.size() * 4;
            int steps = 0;
            while (steps++ < MAX_STEPS) {
                // find next segment sharing cur_end that is unused
                auto k = key_of(cur_end);
                long long pk = pack(k);
                int next_idx = -1;
                if (mapIdx.count(pk)) {
                    for (int idx : mapIdx[pk]) {
                        if (idx == cur_idx) continue;
                        if (used[idx]) continue;
                        next_idx = idx;
                        break;
                    }
                }
                if (next_idx == -1) {
                    // no unused segment from this endpoint -> stop
                    break;
                }
                // choose the next endpoint (the opposite endpoint of this segment)
                VECTOR2D s0 = segs[next_idx].a;
                VECTOR2D s1 = segs[next_idx].b;
                VECTOR2D next_pt;
                if (fabs(s0.x - cur_end.x) < 1e-8 && fabs(s0.y - cur_end.y) < 1e-8) next_pt = s1;
                else next_pt = s0;

                poly.push_back(next_pt);
                used[next_idx] = 1;
                cur_idx = next_idx;
                cur_end = next_pt;

                // if we've closed to start
                if (fabs(cur_end.x - poly.front().x) < 1e-8 && fabs(cur_end.y - poly.front().y) < 1e-8) {
                    closed = true;
                    break;
                }
            }

            if (closed) {
                // remove possible consecutive duplicates and ensure at least 3 distinct pts
                vector<VECTOR2D> cleaned;
                for (auto& p : poly) {
                    if (cleaned.empty() || !(fabs(cleaned.back().x - p.x) < 1e-9 && fabs(cleaned.back().y - p.y) < 1e-9))
                        cleaned.push_back(p);
                }
                // ensure closed: last == first
                if (!(fabs(cleaned.front().x - cleaned.back().x) < 1e-9 && fabs(cleaned.front().y - cleaned.back().y) < 1e-9))
                    cleaned.push_back(cleaned.front());
                // valid polygon must have >= 4 points (including repeated first at end)
                if (cleaned.size() >= 4) polygons.push_back(cleaned);
                break; // break attempt loop
            }
            else {
                // failed attempt, revert used marks from this attempt
                // we already marked used[cur_idx]=1 for seed and others; to be safe we just continue and
                // leave used marks as is (to avoid infinite loops). This may leave some segments unused;
                // they'll be processed in other tries.
                break;
            }
        }
    }

    return polygons;
}

ContourResult classify_orientation(vector<vector<VECTOR2D>>& polys) {
    ContourResult res;
    for (auto& p : polys) {
        double a = signed_area(p);
        if (a < 0) {
            // CW -> outer (already CW)
            res.outer.push_back(p);
        }
        else {
            // CCW -> hole (already CCW)
            res.holes.push_back(p);
        }
    }
    return res;
}

// optional: simplify polygon with RDP (Douglas-Peucker)
double point_line_distance(const VECTOR2D& p, const VECTOR2D& a, const VECTOR2D& b) {
    double vx = b.x - a.x, vy = b.y - a.y;
    double wx = p.x - a.x, wy = p.y - a.y;
    double c2 = vx * vx + vy * vy;
    if (c2 < EPS) return hypot(wx, wy);
    double t = (vx * wx + vy * wy) / c2;
    t = max(0.0, min(1.0, t));
    double px = a.x + t * vx, py = a.y + t * vy;
    return hypot(px - p.x, py - p.y);
}
void rdp_rec(const vector<VECTOR2D>& src, int L, int R, vector<char>& keep, double eps) {
    if (R <= L + 1) return;
    double maxd = -1; int idx = -1;
    for (int i = L + 1; i < R; ++i) {
        double d = point_line_distance(src[i], src[L], src[R]);
        if (d > maxd) { maxd = d; idx = i; }
    }
    if (maxd > eps) {
        keep[idx] = 1;
        rdp_rec(src, L, idx, keep, eps);
        rdp_rec(src, idx, R, keep, eps);
    }
}
vector<VECTOR2D> simplify_rdp(const vector<VECTOR2D>& poly, double eps) {
    int n = poly.size();
    if (n < 4) return poly;
    vector<char> keep(n, 0);
    keep[0] = keep[n - 1] = 1;
    rdp_rec(poly, 0, n - 1, keep, eps);
    vector<VECTOR2D> out;
    for (int i = 0; i < n; ++i) if (keep[i]) out.push_back(poly[i]);
    if (!(out.front() == out.back())) out.push_back(out.front());
    return out;
}

// driver wrapper: scanline (dy) -> connect -> simplify -> classify
ContourResult scanline_to_polygons(const vector<VECTOR2D>& poly, double dy, double simplify_eps) {
    auto segs = scanline_segments_geometric(poly, dy);
    if (segs.empty()) return {};
    auto polygons = connect_segments_to_polygons(segs);
    // optionally simplify polygons
    vector<vector<VECTOR2D>> polys_simp;
    for (auto& p : polygons) {
        // choose eps relative to dy
        double eps = simplify_eps;
        auto s = simplify_rdp(p, eps);
        if (s.size() >= 4) polys_simp.push_back(s);
    }
    auto res = classify_orientation(polys_simp);
    return res;
}


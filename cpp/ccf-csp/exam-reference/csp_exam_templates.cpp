#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using i128 = __int128_t;
using pii = pair<int, int>;
using pll = pair<ll, ll>;

const ll INF64 = (1LL << 62);

#ifdef LOCAL
#define debug(x) cerr << #x << " = " << (x) << '\n'
#else
#define debug(x) ((void)0)
#endif

// ==================== 安全数值工具 ====================

// 仅适用于 a >= 0, b > 0
ll ceil_div_nonneg(ll a, ll b) {
    assert(a >= 0 && b > 0);
    return a / b + (a % b != 0);
}

ll norm_mod(ll x, ll mod) {
    x %= mod;
    if (x < 0) x += mod;
    return x;
}

string to_string_i128(i128 x) {
    if (x == 0) return "0";
    bool neg = x < 0;
    if (neg) x = -x;
    string s;
    while (x > 0) {
        s.push_back(char('0' + x % 10));
        x /= 10;
    }
    if (neg) s.push_back('-');
    reverse(s.begin(), s.end());
    return s;
}

// ==================== 二分答案 ====================

// check(x): true true ... false，返回最大的 true。
template <class Check>
ll binary_search_last_true(ll low, ll high, Check check) {
    while (low < high) {
        ll mid = low + (high - low + 1) / 2;
        if (check(mid)) low = mid;
        else high = mid - 1;
    }
    return low;
}

// check(x): false false ... true，返回最小的 true。
template <class Check>
ll binary_search_first_true(ll low, ll high, Check check) {
    while (low < high) {
        ll mid = low + (high - low) / 2;
        if (check(mid)) high = mid;
        else low = mid + 1;
    }
    return low;
}

// ==================== 并查集 ====================

struct DSU {
    vector<int> parent, size;

    explicit DSU(int n = 0) { init(n); }

    void init(int n) {
        parent.resize(n + 1);
        size.assign(n + 1, 1);
        iota(parent.begin(), parent.end(), 0);
    }

    int find(int x) {
        while (x != parent[x]) {
            parent[x] = parent[parent[x]];
            x = parent[x];
        }
        return x;
    }

    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (size[a] < size[b]) swap(a, b);
        parent[b] = a;
        size[a] += size[b];
        return true;
    }
};

// ==================== 树状数组：单点加、前缀和 ====================

struct Fenwick {
    int n = 0;
    vector<ll> bit;

    explicit Fenwick(int n_ = 0) { init(n_); }

    void init(int n_) {
        n = n_;
        bit.assign(n + 1, 0);
    }

    void add(int index, ll delta) {
        for (int i = index; i <= n; i += i & -i) bit[i] += delta;
    }

    ll prefix_sum(int index) const {
        ll result = 0;
        for (int i = index; i > 0; i -= i & -i) result += bit[i];
        return result;
    }

    ll range_sum(int left, int right) const {
        if (left > right) return 0;
        return prefix_sum(right) - prefix_sum(left - 1);
    }
};

// ==================== 线段树：区间加、区间和 ====================

struct LazySegmentTree {
    int n = 0;
    vector<ll> sum, lazy;

    explicit LazySegmentTree(int n_ = 0) { init(n_); }

    void init(int n_) {
        n = n_;
        sum.assign(4 * n + 4, 0);
        lazy.assign(4 * n + 4, 0);
    }

    void build(int node, int left, int right, const vector<ll>& a) {
        if (left == right) {
            sum[node] = a[left];
            return;
        }
        int mid = (left + right) / 2;
        build(node * 2, left, mid, a);
        build(node * 2 + 1, mid + 1, right, a);
        pull(node);
    }

    void pull(int node) {
        sum[node] = sum[node * 2] + sum[node * 2 + 1];
    }

    void apply(int node, int left, int right, ll value) {
        sum[node] += value * (right - left + 1);
        lazy[node] += value;
    }

    void push(int node, int left, int right) {
        if (lazy[node] == 0 || left == right) return;
        int mid = (left + right) / 2;
        apply(node * 2, left, mid, lazy[node]);
        apply(node * 2 + 1, mid + 1, right, lazy[node]);
        lazy[node] = 0;
    }

    void range_add(int node, int left, int right,
                   int ql, int qr, ll value) {
        if (ql <= left && right <= qr) {
            apply(node, left, right, value);
            return;
        }
        push(node, left, right);
        int mid = (left + right) / 2;
        if (ql <= mid) range_add(node * 2, left, mid, ql, qr, value);
        if (qr > mid) range_add(node * 2 + 1, mid + 1, right, ql, qr, value);
        pull(node);
    }

    ll range_query(int node, int left, int right, int ql, int qr) {
        if (ql <= left && right <= qr) return sum[node];
        push(node, left, right);
        int mid = (left + right) / 2;
        ll result = 0;
        if (ql <= mid) result += range_query(node * 2, left, mid, ql, qr);
        if (qr > mid) result += range_query(node * 2 + 1, mid + 1, right, ql, qr);
        return result;
    }

    void range_add(int left, int right, ll value) {
        if (left <= right) range_add(1, 1, n, left, right, value);
    }

    ll range_query(int left, int right) {
        return left <= right ? range_query(1, 1, n, left, right) : 0;
    }
};

// ==================== BFS：网格最短路 ====================

vector<vector<int>> bfs_grid(const vector<string>& grid, int sx, int sy) {
    const int n = (int)grid.size();
    const int m = n ? (int)grid[0].size() : 0;
    vector<vector<int>> dist(n, vector<int>(m, -1));
    if (sx < 0 || sx >= n || sy < 0 || sy >= m || grid[sx][sy] == '#') {
        return dist;
    }

    static const int dx[4] = {-1, 1, 0, 0};
    static const int dy[4] = {0, 0, -1, 1};
    queue<pii> q;
    q.push({sx, sy});
    dist[sx][sy] = 0;  // 入队即标记

    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();
        for (int dir = 0; dir < 4; ++dir) {
            int nx = x + dx[dir];
            int ny = y + dy[dir];
            if (nx < 0 || nx >= n || ny < 0 || ny >= m) continue;
            if (grid[nx][ny] == '#' || dist[nx][ny] != -1) continue;
            dist[nx][ny] = dist[x][y] + 1;
            q.push({nx, ny});
        }
    }
    return dist;
}

// ==================== Dijkstra：非负边权 ====================

struct Edge {
    int to;
    ll weight;
};

vector<ll> dijkstra(const vector<vector<Edge>>& graph, int source) {
    int n = (int)graph.size() - 1;
    vector<ll> dist(n + 1, INF64);
    priority_queue<pll, vector<pll>, greater<pll>> pq;
    dist[source] = 0;
    pq.push({0, source});

    while (!pq.empty()) {
        auto [distance, u] = pq.top();
        pq.pop();
        if (distance != dist[u]) continue;  // 跳过旧记录

        for (const Edge& edge : graph[u]) {
            if (distance > INF64 - edge.weight) continue;
            ll candidate = distance + edge.weight;
            if (candidate < dist[edge.to]) {
                dist[edge.to] = candidate;
                pq.push({candidate, edge.to});
            }
        }
    }
    return dist;
}

// ==================== 拓扑排序 ====================

vector<int> topological_sort(const vector<vector<int>>& graph) {
    int n = (int)graph.size() - 1;
    vector<int> indegree(n + 1, 0);
    for (int u = 1; u <= n; ++u)
        for (int v : graph[u])
            ++indegree[v];

    queue<int> q;
    for (int i = 1; i <= n; ++i)
        if (indegree[i] == 0) q.push(i);

    vector<int> order;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        order.push_back(u);
        for (int v : graph[u])
            if (--indegree[v] == 0) q.push(v);
    }
    // order.size() < n 表示有环
    return order;
}

// ==================== 背包 ====================

ll zero_one_knapsack(int capacity, const vector<pair<int, ll>>& items) {
    vector<ll> dp(capacity + 1, 0);
    for (auto [weight, value] : items) {
        for (int c = capacity; c >= weight; --c) {
            dp[c] = max(dp[c], dp[c - weight] + value);
        }
    }
    return *max_element(dp.begin(), dp.end());
}

ll complete_knapsack(int capacity, const vector<pair<int, ll>>& items) {
    vector<ll> dp(capacity + 1, 0);
    for (auto [weight, value] : items) {
        for (int c = weight; c <= capacity; ++c) {
            dp[c] = max(dp[c], dp[c - weight] + value);
        }
    }
    return *max_element(dp.begin(), dp.end());
}

// ==================== LCA：二进制倍增 ====================

struct LCA {
    int n = 0, log = 0;
    vector<int> depth;
    vector<vector<int>> up;
    vector<vector<int>> graph;

    explicit LCA(int n_ = 0) { init(n_); }

    void init(int n_) {
        n = n_;
        log = 1;
        while ((1 << log) <= max(1, n)) ++log;
        depth.assign(n + 1, 0);
        up.assign(log, vector<int>(n + 1, 0));
        graph.assign(n + 1, {});
    }

    void add_edge(int u, int v) {
        graph[u].push_back(v);
        graph[v].push_back(u);
    }

    void build(int root = 1) {
        vector<int> parent(n + 1, 0);
        stack<int> st;
        st.push(root);
        parent[root] = root;
        depth[root] = 0;

        while (!st.empty()) {
            int u = st.top();
            st.pop();
            for (int v : graph[u]) {
                if (v == parent[u]) continue;
                parent[v] = u;
                depth[v] = depth[u] + 1;
                st.push(v);
            }
        }

        for (int v = 1; v <= n; ++v) up[0][v] = parent[v];
        for (int k = 1; k < log; ++k)
            for (int v = 1; v <= n; ++v)
                up[k][v] = up[k - 1][up[k - 1][v]];
    }

    int lift(int u, int distance) const {
        for (int k = 0; k < log; ++k)
            if (distance >> k & 1)
                u = up[k][u];
        return u;
    }

    int query(int u, int v) const {
        if (depth[u] < depth[v]) swap(u, v);
        u = lift(u, depth[u] - depth[v]);
        if (u == v) return u;
        for (int k = log - 1; k >= 0; --k) {
            if (up[k][u] != up[k][v]) {
                u = up[k][u];
                v = up[k][v];
            }
        }
        return up[0][u];
    }
};

// ==================== 大模拟：事件与版本懒删除骨架 ====================

struct Event {
    ll time;
    int priority;
    int id;
    int version;

    bool operator>(const Event& other) const {
        return tie(time, priority, id, version) >
               tie(other.time, other.priority, other.id, other.version);
    }
};

// 使用示例：
// priority_queue<Event, vector<Event>, greater<Event>> events;
// vector<int> current_version(n + 1);
// while (!events.empty()) {
//     Event event = events.top();
//     events.pop();
//     if (event.version != current_version[event.id]) continue;
//     // 处理有效事件
// }

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // 1. 读入
    // 2. 先实现正确朴素版
    // 3. 用数据范围替换瓶颈
    // 4. 提交前删除 LOCAL 调试

    return 0;
}

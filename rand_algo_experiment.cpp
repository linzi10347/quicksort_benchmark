#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <chrono>

#define DATASIZE 1e6
#define DATAFILE "unsorted_ints.txt"
#define STATFILE "D:/files/codes/statistics.csv"

using namespace std;

vector<int> unsorted(DATASIZE);
vector<int> sorted(DATASIZE);

random_device rd;  // 创建随机数引擎，用于生成随机种子
mt19937 gen(rd()); // Mersenne Twister 生成器
uniform_int_distribution<long long> dis(0, DATASIZE);

void generate_data();
void read_data();
void init_random_vector(int min, int max);
int find_median(vector<int> &nums, const int pos1, const int pos2, const int pos3);
int partition(vector<int> &nums, const int pivot_pos, const int low, const int high);
void cert_QSort(vector<int> &nums, const int low, const int high);
void mid_QSort(vector<int> &nums, const int low, const int high);
void rand_QSort(vector<int> &nums, const int low, const int high);
void experiment();
void statistic(const vector<int64_t> &mss);

int main() {
    init_random_vector(0, DATASIZE);
    // generate_data();
    // read_data();

    cout << "data has been generated\n";

    sorted = unsorted;
    sort(sorted.begin(), sorted.end());

    experiment();

    printf("press any key to exit.");
    getchar();
    return 0;
}

void generate_data() {
    init_random_vector(-DATASIZE, DATASIZE);

    ofstream ofs(DATAFILE);

    if(!ofs) {
        cerr << "文件打开失败！";
        return;
    }

    for(int i = 0; i < unsorted.size() - 1; i++) {
        ofs << unsorted[i] << "\n";
    }
    ofs << unsorted.back();

    ofs.flush();
    ofs.close();
}

void read_data() {
    ifstream ifs(DATAFILE);

    if(!ifs) {
        cerr << "文件打开失败！";
        return;
    }

    int cnt = 0;
    while(cnt < DATASIZE && ifs >> unsorted[cnt]) {
        ++cnt;
        if(!(cnt % int(1e7))) cout << cnt << " nums has been read.\n";
    }

    ifs.close();
}

void init_random_vector(int min, int max) {
    random_device rd; // 随机数种子
    mt19937 gen(rd()); // Mersenne Twister 生成器
    uniform_int_distribution<> dist(min, max); // 均匀分布范围

    for(int &num : unsorted) {
        num = dist(gen);
    }
}

int find_median_pos(vector<int> &nums, const int pos1, const int pos2, const int pos3) {
    if((nums[pos1] > nums[pos2]) != (nums[pos1] > nums[pos3])) return pos1;
    else if((nums[pos2] > nums[pos1]) != (nums[pos2] > nums[pos3])) return pos2;
    else return pos3;
}

int partition(vector<int> &nums, const int pivot_pos, const int low, const int high) {
    int left, right, pivot;
    pivot = nums[pivot_pos];
    swap(nums[low], nums[pivot_pos]);
    left = low;
    right = high;

    while(left < right) {
        while(left < right && nums[right] >= pivot) --right;
        if(left < right) nums[left] = nums[right];

        while(left < right && nums[left] <= pivot) ++left;
        if(left < right) nums[right] = nums[left];
    }

    nums[left] = pivot;
    return left;
}

void cert_QSort(vector<int> &nums, const int low, const int high) {
    if(low >= high) return;

    int mid = partition(nums, high, low, high);
    cert_QSort(nums, low, mid - 1);
    cert_QSort(nums, mid + 1, high);
}

void mid_QSort(vector<int> &nums, const int low, const int high) {
    if(low >= high) return;

    int pivot_pos = find_median_pos(nums, low, low + (high - low) / 2, high);
    int mid = partition(nums, pivot_pos, low, high);
    mid_QSort(nums, low, mid - 1);
    mid_QSort(nums, mid + 1, high);
}

void rand_QSort(vector<int> &nums, const int low, const int high) {
    if(low >= high) return;

    // uniform_int_distribution<long long> dis(0, high - low); // 0 到 10^9 之间的均匀分布

    int pivot_pos = low + (dis(gen) % (high - low));
    int mid = partition(nums, pivot_pos, low, high);
    rand_QSort(nums, low, mid - 1);
    rand_QSort(nums, mid + 1, high);
}

int64_t run_timing(void (*func)(vector<int>&, int, int), vector<int>& nums, int low, int high) {
    // 获取开始时间点
    auto start = std::chrono::high_resolution_clock::now();
    // 调用传入的函数
    func(nums, low, high);
    // 获取结束时间点
    auto end = std::chrono::high_resolution_clock::now();
    // 计算执行时长（单位：微秒）
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    return duration.count();
}

void experiment() {
    int n, point, step;
    n = 100;
    point = 0;
    step = 225;

    vector<int> temp_unsorted, temp_sorted;

    while(n <= DATASIZE) {
        temp_unsorted = unsorted;
        temp_sorted = unsorted;
        int64_t cert_unsorted = run_timing(cert_QSort, temp_unsorted, 0, n - 1);
        int64_t cert_sorted = run_timing(cert_QSort, temp_sorted, 0, n - 1);

        temp_unsorted = unsorted;
        temp_sorted = unsorted;
        int64_t mid_unsorted = run_timing(mid_QSort, temp_unsorted, 0, n - 1);
        int64_t mid_sorted = run_timing(mid_QSort, temp_sorted, 0, n - 1);

        temp_unsorted = unsorted;
        temp_sorted = unsorted;
        int64_t rand_unsorted = run_timing(rand_QSort, temp_unsorted, 0, n - 1);
        int64_t rand_sorted = run_timing(rand_QSort, temp_sorted, 0, n - 1);

        // printf("n=%d:\n", n);
        // printf("cert_QSort(unsorted, %d)=%lldms.\n", n, cert_unsorted);
        // printf("cert_QSort(sorted, %d)=%lldms.\n", n, cert_sorted);
        // printf("mid_QSort(unsorted, %d)=%lldms.\n", n, mid_unsorted);
        // printf("mid_QSort(sorted, %d)=%lldms.\n", n, mid_sorted);
        // printf("rand_QSort(unsorted, %d)=%lldms.\n", n, rand_unsorted);
        // printf("rand_QSort(sorted, %d)=%lldms.\n", n, rand_sorted);

        statistic({n, cert_unsorted, cert_sorted, mid_unsorted, mid_sorted, rand_unsorted, rand_sorted});

        n += step;
        if(!(++point % 4)) step *= 10;
    }
}

void statistic(const vector<int64_t> &mss) {
    ofstream ofs(STATFILE, ios::app);

    if(!ofs) {
        cerr << "文件打开失败！";
        return;
    }

    for(int i = 0; i < mss.size() - 1; i++) {
        ofs << mss[i] << ",";
    }
    ofs << mss.back() << "\n";

    ofs.flush();
    ofs.close();
}
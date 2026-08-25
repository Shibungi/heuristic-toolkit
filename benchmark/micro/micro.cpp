#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <functional>
#include <iostream>
#include <numeric>
#include <queue>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>
#include "library/random/rng.hpp"

using Clock = std::chrono::steady_clock;
volatile std::uint64_t sink;

template <class F> void bench(const std::string& name, std::uint64_t operations, F f) {
  auto begin = Clock::now(); auto value = f(); auto end = Clock::now(); sink = static_cast<std::uint64_t>(value);
  double ns = std::chrono::duration<double, std::nano>(end - begin).count();
  std::cout << name << ',' << operations << ',' << ns << ',' << ns / operations << std::endl;
}

struct State { int x[64]; };
struct Base { virtual ~Base() = default; virtual std::uint64_t f(std::uint64_t x) const = 0; };
struct Derived : Base { std::uint64_t f(std::uint64_t x) const override { return x + 1; } };
struct Derived2 : Base { std::uint64_t f(std::uint64_t x) const override { return x * 3 + 1; } };

int main(int argc, char**) {
  constexpr int N = 2'000'000;
  std::cout << "name,operations,total_ns,ns_per_op\n";
  ht::Xoshiro256StarStar input_rng(123); std::vector<std::uint64_t> input(N);
  for (auto& x : input) x = input_rng();
  bench("xoshiro_u64", N, [] { ht::Xoshiro256StarStar r(1); std::uint64_t s=0; for(int i=0;i<N;++i)s+=r(); return s; });
  bench("mt19937_64", N, [] { std::mt19937_64 r(1); std::uint64_t s=0; for(int i=0;i<N;++i)s+=r(); return s; });
  bench("uniform01", N, [] { ht::Xoshiro256StarStar r(1); double s=0; for(int i=0;i<N;++i)s+=r.uniform01(); return static_cast<std::uint64_t>(s); });
  bench("exp", N, [] { double s=0; for(int i=1;i<=N;++i)s+=std::exp(-double(i%1000)/100); return static_cast<std::uint64_t>(s); });
  bench("steady_clock", N/10, [] { std::uint64_t s=0; for(int i=0;i<N/10;++i)s+=Clock::now().time_since_epoch().count(); return s; });
  bench("state_copy", N, [] { State a{},b{}; for(int i=0;i<N;++i){b=a;b.x[i&63]++;a=b;} return a.x[0]; });
  bench("memcpy_state", N, [] { State a{},b{}; for(int i=0;i<N;++i){std::memcpy(&b,&a,sizeof(State));b.x[i&63]++;std::memcpy(&a,&b,sizeof(State));} return a.x[0]; });
  bench("pointer_state", N, [] { State a{},b{}; State* p=&a; for(int i=0;i<N;++i){p=(p==&a?&b:&a);p->x[i&63]++;} return a.x[0]+b.x[0]; });
  bench("rollback_assign", N, [&] { std::uint64_t x[64]{}; std::uint64_t s=0; for(int i=0;i<N;++i){int k=i&63;auto old=x[k];x[k]=input[i];s+=x[k];x[k]=old;} return s; });
  bench("lambda_call", N, [&] { auto f=[](std::uint64_t x){return x+1;}; std::uint64_t s=0; for(int i=0;i<N;++i)s+=f(input[i]); return s; });
  bench("std_function", N, [&] { std::function<std::uint64_t(std::uint64_t)> f=[](std::uint64_t x){return x+1;}; std::uint64_t s=0; for(int i=0;i<N;++i)s+=f(input[i]); return s; });
  bench("virtual_call", N, [&] { Derived d; Derived2 d2; const Base* p=argc>1?static_cast<const Base*>(&d):static_cast<const Base*>(&d2); std::uint64_t s=0; for(int i=0;i<N;++i)s+=p->f(input[i]); return s; });
  bench("flat_array_lookup", N, [] { std::vector<int> a(1<<16); std::iota(a.begin(),a.end(),0); std::uint64_t s=0; for(int i=0;i<N;++i)s+=a[i&65535]; return s; });
  bench("unordered_map_lookup", N, [] { std::unordered_map<int,int> m; m.reserve(1<<16); for(int i=0;i<(1<<16);++i)m.emplace(i,i); std::uint64_t s=0; for(int i=0;i<N;++i)s+=m.find(i&65535)->second; return s; });
  bench("priority_queue", N/10, [] { std::priority_queue<int> q; for(int i=0;i<N/10;++i)q.push(static_cast<int>((static_cast<std::uint64_t>(i)*48271)&0xfffff)); std::uint64_t s=0; while(!q.empty()){s+=q.top();q.pop();} return s; });
  ht::Xoshiro256StarStar rng(2); std::vector<int> source(4096); for(auto&x:source)x=rng();
  bench("sort_4096", 300, [&] { std::uint64_t s=0; for(int k=0;k<300;++k){auto v=source;std::sort(v.begin(),v.end());s+=v[100];} return s; });
  bench("nth_element_4096", 300, [&] { std::uint64_t s=0; for(int k=0;k<300;++k){auto v=source;std::nth_element(v.begin(),v.begin()+100,v.end());s+=v[100];} return s; });
}

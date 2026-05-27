#pragma once
#include <complex>
#include <valarray>
namespace framework {

inline auto dft(std::span<double> val) -> std::valarray<std::complex<double>> {
  std::valarray<std::complex<double>> X;
  std::valarray<std::complex<double>> result;
  const int TOTAL = val.size();
  X.resize(TOTAL);
  result.resize(TOTAL);
  for (int k = 0; k < TOTAL; k++) {
    auto &Xk = X[k];
    for (int m = 0; m < TOTAL; m++) {
      // Xk += val[m] * dvec2::cexp(Xk, 1.0, -2.0 * PI * (double) m * (double)k /(double)N );
      Xk += val[m] * std::polar(1.0, -2.0 * glm::pi<double>() * (double)m * (double)k / (double)TOTAL);
    }
    // result[k] = Xk.dot();
    result[k] = Xk; //.real()*Xk.real() + Xk.imag()*Xk.imag();
  }
  return result;
}

inline auto recursive_fft(std::valarray<std::complex<double>> &X) -> void {
  std::valarray<std::complex<double>> result;
  if(X.size() == 1) return;
  std::valarray<std::complex<double>> even  = X[std::slice(0, X.size() / 2, 2)];
  std::valarray<std::complex<double>> odd = X[std::slice(1, X.size() / 2, 2)];

  recursive_fft(even);
  recursive_fft(odd);

  const size_t N = X.size();

  for (int k = 0; k < N/2; k++) {
    auto p = even[k];
    auto q = odd[k] * std::polar(1.0, -2.0 * glm::pi<double>() * (double)k / (double)N);
    X[k] = p + q;
    X[k+N/2] = p - q;
  }
}

inline void fft(std::valarray<std::complex<double>> &x) {
  // DFT
  unsigned int N = x.size(), k = N, n;
  double thetaT = glm::pi<double>() / N;
  std::complex<double> phiT = std::complex<double>(cos(thetaT), -sin(thetaT)), T;
  while (k > 1) {
    n = k;
    k >>= 1;
    phiT = phiT * phiT;
    T = 1.0;
    for (unsigned int l = 0; l < k; l++) {
      for (unsigned int a = l; a < N; a += n) {
        unsigned int b = a + k;
        if (a >= N || b >= N)
          continue;
        std::complex<double> t = x[a] - x[b];
        x[a] += x[b];
        x[b] = t * T;
      }
      T *= phiT;
    }
  }
  // Decimate
  unsigned int m = (unsigned int)log2(N);
  for (unsigned int a = 0; a < N; a++) {
    unsigned int b = a;
    // Reverse bits
    b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
    b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
    b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
    b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
    b = ((b >> 16) | (b << 16)) >> (32 - m);
    if (b > a) {
      std::complex<double> t = x[a];
      x[a] = x[b];
      x[b] = t;
    }
  }
  //// Normalize (This section make it not working correctly)
   //std::complex<double> f = 1.0 / sqrt(N);
   //for (unsigned int i = 0; i < N; i++)
  	//x[i] *= f;
}
} // namespace framework
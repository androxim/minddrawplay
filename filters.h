/* header file for filters class -
   responsible for signal filtering (Butterworth zero-phase filter, Low-pass / High-pass filter
   based on translation of Matlab codes
   https://stackoverflow.com/questions/10373184/bandpass-butterworth-filter-implementation-in-c
   https://stackoverflow.com/questions/17675053/matlabs-filtfilt-algorithm
*/

#ifndef FILTERS_H
#define FILTERS_H

#include "plotwindow.h"

using namespace Eigen;

typedef std::vector<int> vectori;
typedef std::vector<double> vectord;

class filters
{
public:

    filters(int ord, int lcut, int hcut, int srate);
    double* DenC;
    double* NumC;
    vectord acoeffs, bcoeffs, ac, bc, b_f, a_f, b_s, a_s;
    int butterord, lcutoff, hcutoff, samprate;
    double* ComputeLP(int FilterOrder );
    double* ComputeHP(int FilterOrder );
    double* TrinomialMultiply(int FilterOrder, double *b, double *c );
    double* ComputeNumCoeffs(int FilterOrder, double Lcutoff, double Ucutoff, double *DenC);
    double* ComputeDenCoeffs(int FilterOrder, double Lcutoff, double Ucutoff);
    void filter(int ord, double *a, double *b, int np, double *x, double *y);
    void butterfiltcoefs(int lcut, int hcut, int order, int sampr);
    void filtfilt(vectord B, vectord A, const vectord &X, vectord &Y);
    void filter(vectord B, vectord A, const vectord &X, vectord &Y, vectord &Zi);
    vectord subvector_reverse(const vectord &vec, int idx_end, int idx_start);
    void append_vector(vectord &vec, const vectord &tail);
    void add_index_const(vectori &indices, int value, size_t numel);
    void add_index_range(vectori &indices, int beg, int end);
    void zerophaseinit(int lcut, int hcut, int order, int sampr);
    void zerophasefilt(int pos, int length, QVector<double> &inp);
    void getcoeffs(vectord& acc, vectord& bcc, int ord);
};

#endif // FILTERS_H

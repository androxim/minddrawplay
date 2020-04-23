/* source file for filters class -
   responsible for signal filtering (Butterworth zero-phase filter, Low-pass / High-pass filter
   based on translation of Matlab codes
   https://stackoverflow.com/questions/10373184/bandpass-butterworth-filter-implementation-in-c
   https://stackoverflow.com/questions/17675053/matlabs-filtfilt-algorithm
*/

#include "filters.h"

filters::filters(int ord, int lcut, int hcut, int srate)
{
    butterord = ord;
    lcutoff = lcut;
    hcutoff = hcut;
    NumC = new double [20];
    DenC = new double [20];
    samprate = srate;

    butterfiltcoefs(lcutoff, hcutoff, butterord, samprate);
    acoeffs = vector<double>(butterord*2+1);
    for(int k = 0; k<butterord*2+1; k++)
        acoeffs[k]=DenC[k];
    bcoeffs = vector<double>(butterord*2+1);
    for(int k = 0; k<butterord*2+1; k++)
        bcoeffs[k]=NumC[k];
}

// ==== butterfiltcoeffs implementation ==== //
double* filters::ComputeLP( int FilterOrder )
{
    double *NumCoeffs;
    int m;
    int i;

    NumCoeffs = (double *)calloc( FilterOrder+1, sizeof(double) );
    if( NumCoeffs == NULL ) return( NULL );

    NumCoeffs[0] = 1;
    NumCoeffs[1] = FilterOrder;
    m = FilterOrder/2;
    for( i=2; i <= m; ++i)
    {
        NumCoeffs[i] =(double) (FilterOrder-i+1)*NumCoeffs[i-1]/i;
        NumCoeffs[FilterOrder-i]= NumCoeffs[i];
    }
    NumCoeffs[FilterOrder-1] = FilterOrder;
    NumCoeffs[FilterOrder] = 1;

    return NumCoeffs;
}

double* filters::ComputeHP( int FilterOrder )
{
    double *NumCoeffs;
    int i;

    NumCoeffs = ComputeLP(FilterOrder);
    if(NumCoeffs == NULL ) return( NULL );

    for( i = 0; i <= FilterOrder; ++i)
        if( i % 2 ) NumCoeffs[i] = -NumCoeffs[i];

    return NumCoeffs;
}

double* filters::TrinomialMultiply( int FilterOrder, double *b, double *c )
{
    int i, j;
    double *RetVal;

    RetVal = (double *)calloc( 4 * FilterOrder, sizeof(double) );
    if( RetVal == NULL ) return( NULL );

    RetVal[2] = c[0];
    RetVal[3] = c[1];
    RetVal[0] = b[0];
    RetVal[1] = b[1];

    for( i = 1; i < FilterOrder; ++i )
    {
        RetVal[2*(2*i+1)]   += c[2*i] * RetVal[2*(2*i-1)]   - c[2*i+1] * RetVal[2*(2*i-1)+1];
        RetVal[2*(2*i+1)+1] += c[2*i] * RetVal[2*(2*i-1)+1] + c[2*i+1] * RetVal[2*(2*i-1)];

        for( j = 2*i; j > 1; --j )
        {
            RetVal[2*j]   += b[2*i] * RetVal[2*(j-1)]   - b[2*i+1] * RetVal[2*(j-1)+1] +
                c[2*i] * RetVal[2*(j-2)]   - c[2*i+1] * RetVal[2*(j-2)+1];
            RetVal[2*j+1] += b[2*i] * RetVal[2*(j-1)+1] + b[2*i+1] * RetVal[2*(j-1)] +
                c[2*i] * RetVal[2*(j-2)+1] + c[2*i+1] * RetVal[2*(j-2)];
        }

        RetVal[2] += b[2*i] * RetVal[0] - b[2*i+1] * RetVal[1] + c[2*i];
        RetVal[3] += b[2*i] * RetVal[1] + b[2*i+1] * RetVal[0] + c[2*i+1];
        RetVal[0] += b[2*i];
        RetVal[1] += b[2*i+1];
    }

    return RetVal;
}

double* filters::ComputeNumCoeffs(int FilterOrder, double Lcutoff, double Ucutoff, double *DenC)
{
    double *TCoeffs;
    double *NumCoeffs;
    std::complex<double> *NormalizedKernel;
    double Numbers[17]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    int i;

    NumCoeffs = (double *)calloc( 2*FilterOrder+1, sizeof(double) );
    if( NumCoeffs == NULL ) return( NULL );

    TCoeffs = ComputeHP(FilterOrder);
    if( TCoeffs == NULL ) return( NULL );

    NormalizedKernel = (std::complex<double> *)calloc( 2*FilterOrder+1, sizeof(std::complex<double>) );
    if( NormalizedKernel == NULL ) return( NULL );

    for( i = 0; i < FilterOrder; ++i)
    {
        NumCoeffs[2*i] = TCoeffs[i];
        NumCoeffs[2*i+1] = 0.0;
    }
    NumCoeffs[2*FilterOrder] = TCoeffs[FilterOrder];
    double cp[2];
    double Wn;
    cp[0] = 2*2.0*tan(M_PI * Lcutoff / 2.0);
    cp[1] = 2*2.0*tan(M_PI * Ucutoff / 2.0);
    //double Bw;
   // Bw = cp[1] - cp[0];
    //center frequency
    Wn = sqrt(cp[0]*cp[1]);
    Wn = 2*atan2(Wn,4);
    const std::complex<double> result = std::complex<double>(-1,0);

    for(int k = 0; k<FilterOrder*2+1; k++)
    {
        NormalizedKernel[k] = std::exp(-sqrt(result)*Wn*Numbers[k]);
    }

    double b=0;
    double den=0;
    for(int d = 0; d<FilterOrder*2+1; d++)
    {
        b+=real(NormalizedKernel[d]*NumCoeffs[d]);
        den+=real(NormalizedKernel[d]*DenC[d]);
    }
    for(int c = 0; c<FilterOrder*2+1; c++)
    {
        NumCoeffs[c]=(NumCoeffs[c]*den)/b;
    }

    free(TCoeffs);
    return NumCoeffs;
}

double* filters::ComputeDenCoeffs( int FilterOrder, double Lcutoff, double Ucutoff )
{
    int k;            // loop variables
    double theta;     // PI * (Ucutoff - Lcutoff) / 2.0
    double cp;        // cosine of phi
    double st;        // sine of theta
    double ct;        // cosine of theta
    double s2t;       // sine of 2*theta
    double c2t;       // cosine 0f 2*theta
    double *RCoeffs;     // z^-2 coefficients
    double *TCoeffs;     // z^-1 coefficients
    double *DenomCoeffs;     // dk coefficients
    double PoleAngle;      // pole angle
    double SinPoleAngle;     // sine of pole angle
    double CosPoleAngle;     // cosine of pole angle
    double a;         // workspace variables

    cp = cos(M_PI * (Ucutoff + Lcutoff) / 2.0);
    theta = M_PI * (Ucutoff - Lcutoff) / 2.0;
    st = sin(theta);
    ct = cos(theta);
    s2t = 2.0*st*ct;        // sine of 2*theta
    c2t = 2.0*ct*ct - 1.0;  // cosine of 2*theta

    RCoeffs = (double *)calloc( 2 * FilterOrder, sizeof(double) );
    TCoeffs = (double *)calloc( 2 * FilterOrder, sizeof(double) );

    for( k = 0; k < FilterOrder; ++k )
    {
        PoleAngle = M_PI * (double)(2*k+1)/(double)(2*FilterOrder);
        SinPoleAngle = sin(PoleAngle);
        CosPoleAngle = cos(PoleAngle);
        a = 1.0 + s2t*SinPoleAngle;
        RCoeffs[2*k] = c2t/a;
        RCoeffs[2*k+1] = s2t*CosPoleAngle/a;
        TCoeffs[2*k] = -2.0*cp*(ct+st*SinPoleAngle)/a;
        TCoeffs[2*k+1] = -2.0*cp*st*CosPoleAngle/a;
    }

    DenomCoeffs = TrinomialMultiply(FilterOrder, TCoeffs, RCoeffs );
    free(TCoeffs);
    free(RCoeffs);

    DenomCoeffs[1] = DenomCoeffs[0];
    DenomCoeffs[0] = 1.0;
    for( k = 3; k <= 2*FilterOrder; ++k )
        DenomCoeffs[k] = DenomCoeffs[2*k-2];

    return DenomCoeffs;
}

void filters::filter(int ord, double *a, double *b, int np, double *x, double *y)
{
    int i,j;
    y[0]=b[0] * x[0];
    for (i=1;i<ord+1;i++)
    {
        y[i]=0.0;
        for (j=0;j<i+1;j++)
            y[i]=y[i]+b[j]*x[i-j];
        for (j=0;j<i;j++)
            y[i]=y[i]-a[j+1]*y[i-j-1];
    }
    for (i=ord+1;i<np+1;i++)
    {
        y[i]=0.0;
        for (j=0;j<ord+1;j++)
            y[i]=y[i]+b[j]*x[i-j];
        for (j=0;j<ord;j++)
            y[i]=y[i]-a[j+1]*y[i-j-1];
    }
}

void filters::butterfiltcoefs(int lcut, int hcut, int order, int sampr)
{
    //Frequency bands is a vector of values - Lower Frequency Band and Higher Frequency Band
    //First value is lower cutoff and second value is higher cutoff
    // 10 Hz = 10/(500/2), 50 Hz = 50/(500/2)

    double lc=(double)lcut/(sampr/2);
    double hc=(double)hcut/(sampr/2);
    double FrequencyBands[2] = {lc,hc};   // these values are as a ratio of f/fs, where fs is sampling rate, and f is cutoff frequency
                                             // and therefore should lie in the range [0 1]

    DenC = ComputeDenCoeffs(order, FrequencyBands[0], FrequencyBands[1]); // is A in matlab function

    NumC = ComputeNumCoeffs(order,lc,hc,DenC); // is B in matlab

    double y[order];
    double x[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    filter(order, DenC, NumC, order, x, y);
}

// = end of butterfiltcoeffs implementation = //

// ==== filtfilt implementation ==== //
void filters::add_index_range(vectori &indices, int beg, int end)
{
    int inc=1;
    for (int i = beg; i <= end; i += inc)
       indices.push_back(i);
}

void filters::add_index_const(vectori &indices, int value, size_t numel)
{
    while (numel--)
        indices.push_back(value);
}

void filters::append_vector(vectord &vec, const vectord &tail)
{
    vec.insert(vec.end(), tail.begin(), tail.end());
}

vectord filters::subvector_reverse(const vectord &vec, int idx_end, int idx_start)
{
    vectord result(&vec[idx_start], &vec[idx_end+1]);
    std::reverse(result.begin(), result.end());
    return result;
}

inline int max_val(const vectori& vec)
{
    return std::max_element(vec.begin(), vec.end())[0];
}

void filters::filter(vectord B, vectord A, const vectord &X, vectord &Y, vectord &Zi)
{
    if (A.empty())
        throw std::domain_error("The feedback filter coefficients are empty.");
    if (std::all_of(A.begin(), A.end(), [](double coef){ return coef == 0; }))
        throw std::domain_error("At least one of the feedback filter coefficients has to be non-zero.");
    if (A[0] == 0)
        throw std::domain_error("First feedback coefficient has to be non-zero.");

    // Normalize feedback coefficients if a[0] != 1;
    auto a0 = A[0];
    if (a0 != 1.0)
    {
        std::transform(A.begin(), A.end(), A.begin(), [a0](double v) { return v / a0; });
        std::transform(B.begin(), B.end(), B.begin(), [a0](double v) { return v / a0; });
    }

    size_t input_size = X.size();
    size_t filter_order = std::max(A.size(), B.size());
    B.resize(filter_order, 0);
    A.resize(filter_order, 0);
    Zi.resize(filter_order, 0);
    Y.resize(input_size);

    const double *x = &X[0];
    const double *b = &B[0];
    const double *a = &A[0];
    double *z = &Zi[0];
    double *y = &Y[0];

    for (size_t i = 0; i < input_size; ++i)
    {
        size_t order = filter_order - 1;
        while (order)
        {
            if (i >= order)
                z[order - 1] = b[order] * x[i - order] - a[order] * y[i - order] + z[order];
            --order;
        }
        y[i] = b[0] * x[i] + z[0];
    }
    Zi.resize(filter_order - 1);
}

void filters::filtfilt(vectord B, vectord A, const vectord &X, vectord &Y)
{

    int len = X.size();     // length of input
    int na = A.size();
    int nb = B.size();
    int nfilt = (nb > na) ? nb : na;
    int nfact = 3 * (nfilt - 1); // length of edge transients

    if (len <= nfact)
        throw std::domain_error("Input data too short! Data must have length more than 3 times filter order.");

    // set up filter's initial conditions to remove DC offset problems at the
    // beginning and end of the sequence
    B.resize(nfilt, 0);
    A.resize(nfilt, 0);

    vectori rows, cols;
    //rows = [1:nfilt-1           2:nfilt-1             1:nfilt-2];
    add_index_range(rows, 0, nfilt - 2);
    if (nfilt > 2)
    {
        add_index_range(rows, 1, nfilt - 2);
        add_index_range(rows, 0, nfilt - 3);
    }
    //cols = [ones(1,nfilt-1)         2:nfilt-1          2:nfilt-1];
    add_index_const(cols, 0, nfilt - 1);
    if (nfilt > 2)
    {
        add_index_range(cols, 1, nfilt - 2);
        add_index_range(cols, 1, nfilt - 2);
    }
    // data = [1+a(2)         a(3:nfilt)        ones(1,nfilt-2)    -ones(1,nfilt-2)];

    auto klen = rows.size();
    vectord data;
    data.resize(klen);
    data[0] = 1 + A[1];  int j = 1;
    if (nfilt > 2)
    {
        for (int i = 2; i < nfilt; i++)
            data[j++] = A[i];
        for (int i = 0; i < nfilt - 2; i++)
            data[j++] = 1.0;
        for (int i = 0; i < nfilt - 2; i++)
            data[j++] = -1.0;
    }

    vectord leftpad = subvector_reverse(X, nfact, 1);
    double _2x0 = 2 * X[0];
    std::transform(leftpad.begin(), leftpad.end(), leftpad.begin(), [_2x0](double val) {return _2x0 - val; });

    vectord rightpad = subvector_reverse(X, len - 2, len - nfact - 1);
    double _2xl = 2 * X[len-1];
    std::transform(rightpad.begin(), rightpad.end(), rightpad.begin(), [_2xl](double val) {return _2xl - val; });

    double y0;
    vectord signal1, signal2, zi;

    signal1.reserve(leftpad.size() + X.size() + rightpad.size());
    append_vector(signal1, leftpad);
    append_vector(signal1, X);
    append_vector(signal1, rightpad);

    // Calculate initial conditions
    MatrixXd sp = MatrixXd::Zero(max_val(rows) + 1, max_val(cols) + 1);
    for (size_t k = 0; k < klen; ++k)
    {
        sp(rows[k], cols[k]) = data[k];
    }
    auto bb = VectorXd::Map(B.data(), B.size());
    auto aa = VectorXd::Map(A.data(), A.size());
    MatrixXd zzi = (sp.inverse() * (bb.segment(1, nfilt - 1) - (bb(0) * aa.segment(1, nfilt - 1))));
    zi.resize(zzi.size());

    // Do the forward and backward filtering
    y0 = signal1[0];
    std::transform(zzi.data(), zzi.data() + zzi.size(), zi.begin(), [y0](double val){ return val*y0; });
    filter(B, A, signal1, signal2, zi);
    std::reverse(signal2.begin(), signal2.end());
    y0 = signal2[0];
    std::transform(zzi.data(), zzi.data() + zzi.size(), zi.begin(), [y0](double val){ return val*y0; });
    filter(B, A, signal2, signal1, zi);
    Y = subvector_reverse(signal1, signal1.size() - nfact - 1, nfact);
}

// = end of filtfilt implementation = //

void filters::zerophaseinit(int lcut, int hcut, int order, int sampr)
{
    butterfiltcoefs(lcut, hcut, order, sampr);
    acoeffs = vector<double>(order*2+1);
    for(int k = 0; k<order*2+1; k++)
        acoeffs[k]=DenC[k];
    bcoeffs = vector<double>(order*2+1);
    for(int k = 0; k<order*2+1; k++)
        bcoeffs[k]=NumC[k];
}

void filters::zerophasefilt(int pos, int length, QVector<double>& inp)
{
    vectord input_signal = vector<double>(length);
    for(int k = 0; k<length; k++)
        input_signal[k] = inp[pos+k];
    vectord y_filter_out, y_filtfilt_out; vectord zi = {0};
    filter(bcoeffs, acoeffs, input_signal, y_filter_out, zi);
    filtfilt(bcoeffs, acoeffs, input_signal, y_filtfilt_out);
    for(int k = 0; k<length; k++)
        inp[pos+k] = y_filtfilt_out[k];
}

void filters::getcoeffs(vectord& acc, vectord& bcc, int ord)
{
    for(int k = 0; k<ord*2+1; k++)
    {
        acc[k]=acoeffs[k];
        bcc[k]=bcoeffs[k];
    }
    for(int k = 0; k<ord*2+1; k++)
        cout<<acc[k]<<" ";
    cout<<endl;
    for(int k = 0; k<ord*2+1; k++)
        cout<<bcc[k]<<" ";
    cout<<endl;
    cout<<endl;
}

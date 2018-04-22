#include <iostream>
#include <cmath>
#include "hls_stream.h"
#include "para.h"
using namespace std;
using namespace para;

extern
void shift(hls::stream<DataType> *tensor, hls::stream<DataType>* act);

int main(){
	DataType input[D][D][C] = {
#include "input"
	};

	float ref[N] = {
#include "t_cifar"
	};

	DataType out[N];

#if defined MUL_NET
hls::stream<DataType> istream[C], ostream[N];
#endif

for(int i=0;i<D;i++)
		for(int j=0;j<D;j++)
			for(int k=0;k<C;k++)
#if defined MUL_NET
				istream[k].write(input[i][j][k]);
#endif

	shift(istream, ostream);


	int err = 0;
	float ave = 0;
			for(int k=0;k<N;k++){
#if defined MUL_NET
				DataType output = ostream[k].read();
#endif
#ifdef FIXED
				float diff = abs(( output - (DataType)ref[k]).to_float());
				ave+=diff;
#else
				float diff = abs(output - ref[k]);// ref[i][j][k]);
				ave+=diff;
#endif
				if (diff > 1e-2){
					err ++;
					cout	<<k<<','
							<<output<<','
							<<ref[k]
							<<endl;

				}
			}
	cout << "there are in total " << err << " errors."<<endl;
	cout << "the ave error is " << ave/D/D/N << " ."<<endl;
	if(err == 0)
		return 0;
	else
		return -1;
}

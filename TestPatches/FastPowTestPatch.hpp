#include "TestPatch.hpp"

#ifdef powf
#undef powf
#endif

class FastPowTestPatch : public TestPatch {
public:
  FastPowTestPatch(){
    {
      TEST("FastPow");
      float maxPerc = 0;
      float threshold = 0.02; // maximum relative error accepted
      int errs = 0;
      int tests = 0;
      for(int n = -1000; n < 1000; n++){
        float base = rand()/(float)RAND_MAX * 10;
        float exponent = n*10/1000.f;
        float approx = fast_pow(base, exponent);
        float exact = powf(base, exponent);
        float err = fabsf(approx - exact);
        float perc = err/exact * 100;
	CHECK(perc<threshold);
	// CHECK_CLOSE(approx, exact, threshold);
        maxPerc = maxPerc > perc ? maxPerc : perc;
        if(fabsf(perc) > threshold)
	  errs++;
	tests++;
      }
      debugMessage("threshold / errors %:", threshold, 100.0f*errs/tests);
      debugMessage("max error %:", maxPerc);
    }
  }
};


#ifndef PeakDetection_h
#define PeakDetection_h
 
#include <math.h>
#include <stdlib.h>

class PeakDetection {
  public:
    PeakDetection();
    ~PeakDetection();
    void begin();
    void begin(int, int, double); //lag, threshold, influence
    void add(double);
    double getFilt();
    double getPeak();

  private:
    int index, lag, threshold, peak;
    double influence, EPSILON, *data, *avg, *std;
    double getAvg(int, int);
    double getPoint(int, int);
    double getStd(int, int);
};

#endif
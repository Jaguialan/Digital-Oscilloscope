#ifndef RC_FILTER_H
#define RC_FILTER_H

typedef struct {

	float coeff[2];
	uint16_t out[2];

} RCFilter;

void RCFilter_Init(RCFilter *filt, float cutoffFreqHz, float sampleTimeS);
float RCFilter_Update(RCFilter *filt, uint16_t inp);

#endif
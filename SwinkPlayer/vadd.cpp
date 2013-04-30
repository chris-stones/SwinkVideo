
#include<stdint.h>
#include<stdlib.h>



void vadd( uint64_t * __restrict__ _b0, uint64_t * __restrict__ _b1, size_t size ) {

  
  
  if(0 && sizeof(size_t) == 4) {
  
    uint32_t *b0 = (uint32_t *)__builtin_assume_aligned(_b0, 16);
    uint32_t *b1 = (uint32_t *)__builtin_assume_aligned(_b1, 16);
  
    while( size > (129600 * 4) ) {

      for(int p=0; p<(129600); p++)
	b0[p] += b1[p];

      size -= (129600 * 4);
      b0   += (129600);
      b1   += (129600);
    }
    
    return;
  }
  
  uint64_t *b0 = (uint64_t *)__builtin_assume_aligned(_b0, 16);
  uint64_t *b1 = (uint64_t *)__builtin_assume_aligned(_b1, 16);
  
  
  while( size > (129600 * 8) ) {
  
    for(int p=0; p<(129600); p++)
      b0[p] += b1[p];
    
    size -= (129600 * 8);
    b0   += (129600);
    b1   += (129600);
  }
  
  
  while( size > (64800 * 8) ) {
  
    for(int p=0; p<(64800); p++)
      b0[p] += b1[p];
    
    size -= (64800 * 8);
    b0   += (64800);
    b1   += (64800);
  }
  
  while( size > (16200 * 8) ) {
  
    for(int p=0; p<(16200); p++)
      b0[p] += b1[p];
    
    size -= (16200 * 8);
    b0   += (16200);
    b1   += (16200);
  }
  
  while( size > (2025 * 8) ) {
  
    for(int p=0; p<(2025); p++)
      b0[p] += b1[p];
    
    size -= (2025 * 8);
    b0   += (2025);
    b1   += (2025);
  }
  
  while( size > 16 * 8 ) {
  
    for(int p=0; p<(16); p++)
      b0[p] += b1[p];
    
    size -= (16 * 8);
    b0   += (16);
    b1   += (16);
  }
  
  size /= 8;
  for(int p=0;p<size;p++)
    b0[p] += b1[p];
  
}


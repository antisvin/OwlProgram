#include <inttypes.h>

#define USE_MIDI_CALLBACK

#if !defined OWL_CLASSIC
#define AUDIO_SATURATE_SAMPLES
#endif

#define DEBUG_MEM
/* #define STARTUP_CODE */

#define AUDIO_BIGEND
/* #define AUDIO_SATURATE_SAMPLES // SATURATE adds almost 500 cycles to 24-bit mode */
#define AUDIO_BITDEPTH               24    /* bits per sample */
#ifndef AUDIO_MAX_BLOCK_SIZE
#define AUDIO_MAX_BLOCK_SIZE         1024
#endif

#define MAX_BUFFERS_PER_PATCH        8
#define MAX_NUMBER_OF_PARAMETERS     42

#ifdef  USE_FULL_ASSERT
#ifdef __cplusplus
 extern "C" {
#endif
   void assert_failed(const char* msg, const char* location, int line);
#ifdef __cplusplus
}
#endif
#define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, "", __LINE__))
#endif

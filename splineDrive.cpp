#include "lv2/core/lv2.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// NOTE: from https://github.com/ttk592/spline/
#include "spline.h"

#define URI "http://lv2plug.in/plugins/splineDrive"

extern "C" typedef enum
{
   GAIN = 0,
   INPUT = 1,
   OUTPUT = 2
} PortIndex;

extern "C" typedef struct
{
   // Port buffers
   const float *gain;
   const float *input;
   float *output;

//   float maxValue;

   uint32_t inputBufferStart;
   uint32_t inputBufferEnd;
   uint32_t inputBufferMaxSize;
   uint32_t inputBufferLastEdgeFlip;
} Distortion;

extern "C" LV2_Handle instantiate(const LV2_Descriptor *descriptor,
            double rate,
            const char *bundle_path,
            const LV2_Feature *const *features)
{
   Distortion *distortion = (Distortion *)calloc(1, sizeof(Distortion));

   // pFile = fopen("dist.log", "w+");
   // fprintf(pFile, "instantiated\n");
   // fclose(pFile);


   return (LV2_Handle)distortion;
}

extern "C" void connect_port(LV2_Handle instance, uint32_t port, void *data)
{
   Distortion *distortion = (Distortion *)instance;

   switch ((PortIndex)port)
   {
   case GAIN:
      distortion->gain = (const float *)data;
      break;
   case INPUT:
      distortion->input = (const float *)data;
      break;
   case OUTPUT:
      distortion->output = (float *)data;
      break;
   }
}

extern "C" void activate(LV2_Handle instance)
{
  Distortion *distortion = (Distortion *)instance;
}

extern "C" float distFunc(float sample, float gain)
{   
   float output = sample * gain;

   if (output > (1.0-1/gain))
      output = (1.0-1/gain) + sample * (1/gain);

   return output;
}

extern "C" void run(LV2_Handle instance, uint32_t n_samples)
{
   Distortion *distortion = (Distortion *)instance;

   const float *const input = distortion->input;
   float *const output = distortion->output;

   // NOTE: read samples from the internal plugin buffer to the output
   float sample;
   float polarity;
   for (uint32_t pos = 0; pos < n_samples; pos++)
   {
      sample = input[pos];
      polarity = sample > 0.0 ? 1.0 : -1.0;
      sample = sample * polarity;

      // if (sample > distortion->maxValue)
      // {
      //    distortion->maxValue = sample;
         
      //    FILE *pFile;
      //    pFile = fopen("dist.log", "a+");
      //    fprintf(pFile, "max value: %.3f\n", sample);
      //    fclose(pFile);
      // }

      output[pos] = distFunc(sample, *(distortion->gain)) * polarity;
   }
}

extern "C" void deactivate(LV2_Handle instance)
{
}

extern "C" void cleanup(LV2_Handle instance)
{
   free(instance);
}

extern "C" const void * extension_data(const char *uri)
{
   return NULL;
}

extern "C" const LV2_Descriptor descriptor = {URI,
                                          instantiate,
                                          connect_port,
                                          activate,
                                          run,
                                          deactivate,
                                          cleanup,
                                          extension_data};

extern "C" LV2_SYMBOL_EXPORT const LV2_Descriptor *
lv2_descriptor(uint32_t index)
{
   return index == 0 ? &descriptor : NULL;
}

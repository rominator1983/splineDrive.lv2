#include "lv2/core/lv2.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// NOTE: Special thanks to for this spline library https://github.com/ttk592/spline/
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
   float oldGain;
   const float *input;
   float *output;

   tk::spline spline;
} Distortion;

#define min(a,b) ((a) < (b) ? (a) : (b))

void setCurve(Distortion *distortion, float gain)
{
   distortion->oldGain = gain;
   
   // NOTE: decent drive
   std::vector<double> x = {0.0, 0.2, 1.0};
   std::vector<double> y = {0.0, min(0.2 * gain, 1.0), 1.0};

   // NOTE: Crazy curve, noisy when plaid loud
   // std::vector<double> x = {0.0, 0.1, 0.2, 0.5, 1.0};
   // std::vector<double> y = {0.0, 0.8, -0.8, 0.7, 1.0};

   // NOTE: Kind of octavey still noisy
   // std::vector<double> x = {0.0, 0.1, 0.2, 1.0};
   // std::vector<double> y = {0.0, -0.9, 0.8, 1.0};

   distortion->spline.set_points(x, y, tk::spline::cspline);

   FILE *pFile;
   pFile = fopen("dist.log", "a+");
   fprintf(pFile, "gain %.2f\n", gain);
   for (float i = 0.0; i <= 1.01; i+=0.05)
   {
      fprintf(pFile, "value %.2f: %.3f\n", i, distortion->spline(i));
   }

   fclose(pFile);
}

extern "C" LV2_Handle instantiate(const LV2_Descriptor *descriptor,
            double rate,
            const char *bundle_path,
            const LV2_Feature *const *features)
{
   Distortion *distortion = (Distortion *)calloc(1, sizeof(Distortion));

   distortion->spline = tk::spline();

   setCurve(distortion, 1.0);

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

extern "C" void run(LV2_Handle instance, uint32_t n_samples)
{
   Distortion *distortion = (Distortion *)instance;

   const float *const input = distortion->input;
   float *const output = distortion->output;

   if (distortion->oldGain != *(distortion->gain))
      setCurve(distortion, *(distortion->gain));

   float sample;
   float polarity;
   for (uint32_t pos = 0; pos < n_samples; pos++)
   {
      sample = input[pos];
      polarity = sample > 0.0 ? 1.0 : -1.0;
      sample = sample * polarity;

      float value = distortion->spline(sample);

      if (value > 1.0)
         value = 1.0;

      if (value < -1.0)
         value = -1.0;

      output[pos] = value * polarity;
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

extern "C" LV2_SYMBOL_EXPORT const LV2_Descriptor *lv2_descriptor(uint32_t index)
{
   return index == 0 ? &descriptor : NULL;
}

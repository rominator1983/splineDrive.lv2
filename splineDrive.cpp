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
   PRESERVE_DYNAMICS = 1,
   INPUT = 2,
   OUTPUT = 3
} PortIndex;

extern "C" typedef struct
{
   // Port buffers
   const float *gain;
   const int *preserveDynamics;
   float oldGain;
   int oldPreserveDynamics;
   const float *input;
   float *output;

   tk::spline spline;
} Distortion;

#define min(a,b) ((a) < (b) ? (a) : (b))

void setCurve(Distortion *distortion, float gain, int preserveDynamics)
{
   distortion->oldGain = gain;
   distortion->oldPreserveDynamics = preserveDynamics;
   
   // NOTE: decent drive
   //std::vector<float> x = {0.0, 0.1, 1.0};
   std::vector<float> x = {-1.0f, -0.1f, 0.0f, 0.1f, 1.0f};
   //std::vector<float> y = {0.0, min(0.1 * gain, 1.0), 1.0};
   std::vector<float> y = {-1.0f, 0.0f - min(0.1f * gain, 1.0f), 0.0f, min(0.1f * gain, 1.0f), 1.0f};

   // NOTE: Crazy curve, noisy when plaid loud
   // std::vector<float> x = {0.0f, 0.1f, 0.2f, 0.5f, 1.0f};
   // std::vector<float> y = {0.0f, 0.8f, -0.8f, 0.7f, 1.0f};

   // NOTE: Kind of octavey still noisy
   // std::vector<float> x = {0.0f, 0.1f, 0.2f, 1.0f};
   // std::vector<float> y = {0.0f, -0.9f, 0.8f, 1.0f};

   distortion->spline.set_points(x, y, tk::spline::cspline);
   
   if (preserveDynamics > 0)
      distortion->spline.make_monotonic();

   // NOTE: trouble shooting output
   // FILE *pFile;
   // pFile = fopen("splineDrive.log", "a+");
   // fprintf(pFile, "gain %.2f\n", gain);
   // fprintf(pFile, "preserveDynamis: %s\n", preserveDynamics > 0 ? "true" : "false");

   // for (float i = -1.0f; i <= 1.01f; i+=0.05f)
   // {
   //    float output = 0.0f;
   //    distortion->spline(&i, &output, 1);
   //    fprintf(pFile, "value %.2f => %.3f\n", i, output);
   // }

   // fclose(pFile);
}

extern "C" LV2_Handle instantiate(const LV2_Descriptor *descriptor,
            double rate,
            const char *bundle_path,
            const LV2_Feature *const *features)
{
   Distortion *distortion = (Distortion *)calloc(1, sizeof(Distortion));

   distortion->spline = tk::spline();

   setCurve(distortion, 1.0, false);

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
   case PRESERVE_DYNAMICS:
      distortion->preserveDynamics = (const int *)data;      
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

   if ((distortion->oldGain != *(distortion->gain)) ||
      (distortion->oldPreserveDynamics != *(distortion->preserveDynamics)))
      setCurve(distortion, *(distortion->gain), *(distortion->preserveDynamics));

   distortion->spline(input, output, n_samples);
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

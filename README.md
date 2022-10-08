Spline Drive
===

This [lv2 plugin](https://en.wikipedia.org/wiki/LV2) uses a spline curve to create a drive.
It does not add any temporal effects to the signal but only does a mapping from input "voltage" to output "voltage" over a defined curve.
This results in a very clean drive/distortion with a lot of dynamics.

Special thanks to @ttk592 for the C++ spline library at https://github.com/ttk592/spline/

Why another drive plugin
===
The intention for this was to create a drive with a LOT of dynamics even for higher gains. Using a spline for the input to output mapping makes sure that every individual input value is mapped to an individual output value without clipping or massive distortion.

Having no temporal effects means that this drive treats all input frequencies equally (of course strictly ignoring psychoacoustics). This distinguishes this plugin from existing drives that are modeled after existing floor pedals via the faust libraray or some other approach.

The pedal should sound like what floor pedal builders always had in mind when using different diodes that would only get them so far in reproducing tube sounds.

Of course tubes have a lot of temporal effects and this plugin will not sound like any of that.

And this has not been done before?
===
I don't know.
But other drive/distortion plugins do some kind of hard clipping where the output level reaaches a maximum above a certain input level. This leads to variances in middle to high inputs to not make any difference in the output any more. Some plugins even do not feature an amplification stage for lower inputs. Which I suppose is mostly to reduce noise for distortion plugins.

I did not like the sound of most of those and tried to do some new stuff instead. And I think it sounds great.

Future things
===
At the moment the input gain controls the output level and thus the amplification of a spline anchor point at an input level of 0.2 (where the range for one polarity is from 0 to 1.0).

I might add a control for varying this hard coded value that could be named "softness" or "dynamics". But I suppose that your amp or cab do add more to the signal than this fine tuning ever could.

I also might try to add some further crazy things where you can control more anchor points via JSON files or something for the complete range of -1 to 1. This would allow for built in noise reduction and even octave sounds (at least more harmonics) when adjusted just right. But this is a quite some work and I also do want to play my guitar in my spare time. You could achieve the same by editing and compiling this plugin yourself. 

Install
===
Install the pre compiled artifacts from https://github.com/rominator1983/splineDrive.lv2/releases
- Create a new folder in your lv2 directory (For example ~/.lv2/ or /usr/lib/lv2 or /usr/local/lib/lv2/ would all work in Ubuntu Studio)
- Copy splineDrive.so, manifest.ttl and splineDrive.ttl to this new folder.
- Restart/refresh your lv2 host

Build on your own
===
-  `./waf configure`
-  `./waf build`
-  `sudo ./waf install`
- Restart/refresh your lv2 host

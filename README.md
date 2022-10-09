Spline Drive
===

This [lv2 plugin](https://en.wikipedia.org/wiki/LV2) uses a spline curve to create a drive.
It does not add any temporal effects to the signal but only does a mapping from input "voltage" to output "voltage" over a defined curve.
This results in a very clean drive/distortion with a lot of dynamics.

Special thanks to [ttk5929](https://github.com/ttk592) (Tino Kluge) for the C++ spline library at https://github.com/ttk592/spline/ that I managed to use in the LV2 plugin.

Likewise as the spline library is not only linked but even compiled into this plugin, this plugin can only be developed under the GPL.

Why another drive plugin
===
The intention for this was to create a drive with a LOT of dynamics even for higher gains. Using a spline for the input to output mapping makes sure that every individual input value is mapped to an individual output value without clipping or massive distortion at least up to some degree of gain (unless you use "preserve dynamics").

Having no temporal effects means that this drive treats all input frequencies equally (of course strictly ignoring psychoacoustics). This distinguishes this plugin from existing drives that are modeled after existing floor pedals via the faust libraray or some other approach.

The pedal should sound like what floor pedal builders always had in mind when using different diodes that would only get them so far in reproducing tube sounds.

Of course tubes have a lot of temporal effects and this plugin will not sound like any of that.

At the moment the input gain controls the output level and thus the amplification of a spline anchor point at an input level of 0.1 (where the range for one polarity is from 0 to 1.0). The gain control in other plugins controls an input amplification that would easily add unwanted distortion to the drive stage. This can be avoided using this approach. Using this achieved the best (at least for me) sounding results.

Be adviced that a very high gain will result in some clipping as the spline curve will cross the 1.0 output level and thus the output has to be clipped. You can use the "preserve dynamics" switch to avoid that behavior which uses monotonic spline curves. There are some limitations for creating monotonic splines listed in https://github.com/ttk592/spline/ which I do not really know if they make a difference for this use case at all. This only gets you so far for gains up to 7ish and I cannot tell if this is audible at all.

And this has not been done before?
===
I don't know.
I have looked into the code of other drive/distortion plugins and saw some kind of hard clipping where the output level reaaches a maximum (1.0) above a certain input level. This approach will remove variances after a certain input level and make no difference in the output level any more which will diverge to what a square signal generator with add harmonics might produce. Some plugins even do not feature an amplification stage for lower inputs at all. I can only suppose that this is done to reduce noise for distortion plugins.

I did not like the sound of most of those plugins and tried to do some new stuff instead. And I think it sounds great.

Future things
===
I might add a control for varying the hard coded input value for the spline anchor point that could be named "softness" or "dynamics". But I suppose that your amp or cab do add more to the signal than this fine tuning ever could.
Also it would be interesting to see if clipping occurs. There might be some output LV2 port to show this.

I also might try to add some further crazy things where you can control more anchor points via JSON files or something for the complete range of -1 to 1. This would allow for built in noise reduction (with anchor points at 0.0 output for some input level) and even octave sounds (at least some added harmonics) when going back to a negative output level for certain input levels when adjusted to your input level just right. But this is a quite some work and I also do want to play my guitar in my spare time. You could achieve the same by compiling this plugin yourself and editing the hard coded anchor points. The effort is managable.

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
